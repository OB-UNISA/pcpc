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

# Network
resource "google_compute_network" "vpc_network" {
  name = "terraform-network"
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
  source_ranges = ["${var.myip}/32"]
  target_tags   = ["ssh"]
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

# Compute Instances
resource "google_compute_instance" "vm_instance" {
  count        = 1
  name         = "terraform-instance-${count.index}"
  machine_type = "e2-micro"
  tags         = ["ssh"]

  metadata_startup_script = "sudo apt-get update;"

  boot_disk {
    initialize_params {
      image = "ubuntu-os-cloud/ubuntu-2204-lts"
    }
  }

  network_interface {
    network = google_compute_network.vpc_network.name
    access_config {
    }
  }

  metadata = {
    ssh-keys = "${var.ssh-user}:${chomp(tls_private_key.ssh.public_key_openssh)}"
  }
}

