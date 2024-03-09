terraform {
  required_providers {
    google = {
      source  = "hashicorp/google"
      version = "5.18.0"
    }
    tls = {
      source  = "hashicorp/tls"
      version = "4.0.5"
    }
  }
  required_version = ">= 1.7.3"
}

provider "google" {
  credentials = file(var.credentials)

  project = var.project-id
  region  = var.region
  zone    = "${var.region}-a"
}

############### Network ####################
# VPC Network
resource "google_compute_network" "vpc_network" {
  name = "${var.name}-network"
}

# Firewall SSH
resource "google_compute_firewall" "ssh" {
  count = var.firewall-ssh ? 1 : 0

  name = "allow-ssh"
  allow {
    ports    = ["22"]
    protocol = "tcp"
  }
  direction     = "INGRESS"
  network       = google_compute_network.vpc_network.id
  priority      = 1000
  source_ranges = ["0.0.0.0/0"]
}

# Firewall internal. Allow VMs to communicate between them in the LAN
resource "google_compute_firewall" "internal" {
  count = var.firewall-internal ? 1 : 0

  name = "allow-internal"
  allow {
    ports    = ["0-65535"]
    protocol = "tcp"
  }
  allow {
    ports    = ["0-65535"]
    protocol = "udp"
  }
  allow {
    protocol = "icmp"
  }

  direction     = "INGRESS"
  network       = google_compute_network.vpc_network.id
  priority      = 1000
  source_ranges = ["10.128.0.0/9"]
}

# Firewall external
resource "google_compute_firewall" "external" {
  count = length(var.firewall-external) == 0 ? 0 : 1

  name = "allow-external"

  dynamic "allow" {
    for_each = var.firewall-external

    content {
      ports    = allow.value.ports
      protocol = allow.value.protocol
    }
  }

  direction     = "INGRESS"
  network       = google_compute_network.vpc_network.id
  priority      = 1000
  source_ranges = ["0.0.0.0/0"]
}

######################## SSH Keys ##########################
# ssh key
resource "tls_private_key" "ssh" {
  algorithm = "RSA"
  rsa_bits  = "4096"
}

# save ssh private key
resource "local_file" "private_key" {
  content         = tls_private_key.ssh.private_key_pem
  filename        = var.ssh-pk-save-path
  file_permission = "0600"
}

############### Compute Engine ####################
# cloud-init
# run "cloud-init status --wait" in the SSH to check when it is done
# run "tail -f /var/log/cloud-init-output.log" to see what it is doing
data "cloudinit_config" "conf" {
  gzip          = false
  base64_encode = false

  part {
    content_type = "text/cloud-config"
    content      = file(var.cloud-init-file)
    filename     = "conf.yaml"
  }
}

# Compute Instances
resource "google_compute_instance" "vm_instance" {
  count = var.machines-count

  name         = "${var.name}-${count.index}"
  machine_type = var.machine-type

  scheduling {
    preemptible                 = var.spot-instance
    automatic_restart           = !var.spot-instance
    provisioning_model          = var.spot-instance ? "SPOT" : "STANDARD"
    instance_termination_action = var.spot-instance ? "STOP" : null
  }


  boot_disk {
    initialize_params {
      image = var.os-image
    }
  }

  network_interface {
    network = google_compute_network.vpc_network.name
    access_config {
      network_tier = "STANDARD"
    }
  }

  metadata = {
    ssh-keys  = "${var.ssh-user}:${chomp(tls_private_key.ssh.public_key_openssh)}"
    user-data = data.cloudinit_config.conf.rendered
  }
}

