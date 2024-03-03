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

provider "tls" {}

############### Network ####################
# VPC Network
resource "google_compute_network" "vpc_network" {
  name = "pcpc-network"
}

# Firewall SSH
resource "google_compute_firewall" "ssh" {
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

######################## SSH Keys ##########################
# ssh key
resource "tls_private_key" "ssh" {
  algorithm = "RSA"
  rsa_bits  = "4096"
}

# save ssh private key
resource "local_file" "private_key" {
  content         = tls_private_key.ssh.private_key_pem
  filename        = "gcp.pem"
  file_permission = "0600"
}

############### Compute Engine ####################
# cloud-init. Run "cloud-init status" in the SSH to check when it is done
data "cloudinit_config" "conf" {
  gzip          = false
  base64_encode = false

  part {
    content_type = "text/cloud-config"
    content      = file("../cloud-init.yaml")
    filename     = "conf.yaml"
  }
}

# Compute Instances
resource "google_compute_instance" "vm_instance" {
  count        = 2
  name         = "pcpc-instance-${count.index}"
  machine_type = "e2-micro"

  boot_disk {
    initialize_params {
      image = "ubuntu-os-cloud/ubuntu-2204-lts"
    }
  }

  network_interface {
    network = google_compute_network.vpc_network.name
    access_config {
      // even if it is empty, it is needed to assign a public ip to the VM
    }
  }

  metadata = {
    ssh-keys  = "${var.ssh-user}:${chomp(tls_private_key.ssh.public_key_openssh)}"
    user-data = "${data.cloudinit_config.conf.rendered}"
  }
}

