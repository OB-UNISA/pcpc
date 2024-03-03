terraform {
  required_providers {
    hcloud = {
      source  = "hetznercloud/hcloud"
      version = "1.45.0"
    }
    tls = {
      source  = "hashicorp/tls"
      version = "4.0.5"
    }
  }

  required_version = ">= 1.7.3"
}

provider "hcloud" {
  token = var.hcloud_token
}

provider "tls" {}

############### Network ###############à
# Private Network
resource "hcloud_network" "pcpc" {
  name     = "pcpc"
  ip_range = "10.0.0.0/8"
}

# subnet
resource "hcloud_network_subnet" "vm" {
  network_id   = hcloud_network.pcpc.id
  type         = "cloud"
  network_zone = "eu-central"
  ip_range     = "10.0.2.0/24"
}

# Firewall
resource "hcloud_firewall" "pcpc" {
  name = "pcpc"

  # SSH
  rule {
    direction = "in"
    protocol  = "tcp"
    port      = "22"
    source_ips = [
      "0.0.0.0/0",
      "::/0"
    ]
  }

  # Internal TCP
  rule {
    direction  = "in"
    protocol   = "tcp"
    source_ips = [hcloud_network_subnet.vm.ip_range]
    port       = "any"
  }

  # Internal UDP
  rule {
    direction  = "in"
    protocol   = "udp"
    source_ips = [hcloud_network_subnet.vm.ip_range]
    port       = "any"
  }

  # Internal ICMP
  rule {
    direction  = "in"
    protocol   = "icmp"
    source_ips = [hcloud_network_subnet.vm.ip_range]
  }

}

################# SSH Keys #######################
# ssh key
resource "tls_private_key" "ssh" {
  algorithm = "RSA"
  rsa_bits  = "4096"
}

# save ssh private key
resource "local_file" "private_key" {
  content         = tls_private_key.ssh.private_key_pem
  filename        = "hetzner.pem"
  file_permission = "0600"
}

# Save SSH key to Hetzner
resource "hcloud_ssh_key" "default" {
  name       = "default"
  public_key = chomp(tls_private_key.ssh.public_key_openssh)
}

########## VM ####################
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

# VM
resource "hcloud_server" "vm" {
  count       = 2
  name        = "pcpc-${count.index}"
  server_type = "cx11"
  image       = "ubuntu-22.04"
  location    = var.location

  public_net {
    ipv4_enabled = true
    ipv6_enabled = false
  }

  network {
    network_id = hcloud_network.pcpc.id
  }

  firewall_ids = [hcloud_firewall.pcpc.id]
  ssh_keys     = [hcloud_ssh_key.default.id]
  user_data    = data.cloudinit_config.conf.rendered

  lifecycle {
    ignore_changes = [network]
  }

  # **Note**: the depends_on is important when directly attaching the
  # server to a network. Otherwise Terraform will attempt to create
  # server and sub-network in parallel. This may result in the server
  # creation failing randomly.
  depends_on = [
    hcloud_network_subnet.vm,
  ]
}
