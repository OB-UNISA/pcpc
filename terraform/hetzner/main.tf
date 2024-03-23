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

############### Network ###############à
# Private Network
resource "hcloud_network" "pcpc" {
  name     = "${var.name}-network"
  ip_range = "10.0.0.0/8"
}

# subnet
resource "hcloud_network_subnet" "vm" {
  network_id   = hcloud_network.pcpc.id
  type         = "cloud"
  network_zone = var.network-zone
  ip_range     = "10.0.2.0/24"
}

####### Firewall #########
# SSH
resource "hcloud_firewall" "ssh" {
  count = var.firewall-ssh ? 1 : 0

  name = "allow-ssh"
  rule {
    direction = "in"
    protocol  = "tcp"
    port      = "22"
    source_ips = [
      "0.0.0.0/0",
      "::/0"
    ]
  }
}

# Internal
resource "hcloud_firewall" "internal" {
  count = var.firewall-internal ? 1 : 0

  name = "allow-internal"
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

# External
resource "hcloud_firewall" "external" {
  count = length(var.firewall-external) == 0 ? 0 : 1

  name = "allow-external"

  dynamic "rule" {
    for_each = var.firewall-external

    content {
      direction = "in"
      port      = rule.value.port
      protocol  = rule.value.protocol
      source_ips = [
        "0.0.0.0/0",
        "::/0"
      ]
    }
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
  filename        = var.ssh-pk-save-path
  file_permission = "0600"
}

# Save SSH key to Hetzner
resource "hcloud_ssh_key" "default" {
  name       = "default"
  public_key = chomp(tls_private_key.ssh.public_key_openssh)
}

########## VM ####################
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

# VM
resource "hcloud_server" "vm" {
  count = var.machines-count

  name        = "${var.name}-${count.index}"
  server_type = var.machine-type
  image       = var.os-image
  location    = var.location

  public_net {
    ipv4_enabled = true
    ipv6_enabled = false
  }

  network {
    network_id = hcloud_network.pcpc.id
  }
  ssh_keys  = [hcloud_ssh_key.default.id]
  user_data = data.cloudinit_config.conf.rendered

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

# Firewall attach to servers
resource "hcloud_firewall_attachment" "fw_att_ssh" {
  count = var.firewall-ssh ? 1 : 0

  firewall_id = hcloud_firewall.ssh[0].id
  server_ids  = hcloud_server.vm[*].id
}

resource "hcloud_firewall_attachment" "fw_att_internal" {
  count = var.firewall-internal ? 1 : 0

  firewall_id = hcloud_firewall.internal[0].id
  server_ids  = hcloud_server.vm[*].id
}

resource "hcloud_firewall_attachment" "fw_att_external" {
  count = length(var.firewall-external) == 0 ? 0 : 1

  firewall_id = hcloud_firewall.external[0].id
  server_ids  = hcloud_server.vm[*].id
}
