terraform {
  required_providers {
    azurerm = {
      source  = "hashicorp/azurerm"
      version = "3.97.1"
    }
    tls = {
      source  = "hashicorp/tls"
      version = "4.0.5"
    }
  }

  required_version = ">= 1.7.5"
}

provider "azurerm" {
  features {}
}

# RG
resource "azurerm_resource_group" "pcpc" {
  name     = var.name
  location = var.location
}

################ Network #####################################
# vnet
resource "azurerm_virtual_network" "vnet" {
  name                = "${var.name}-network"
  address_space       = ["10.0.0.0/16"]
  location            = var.location
  resource_group_name = azurerm_resource_group.pcpc.name
}

# subnets
resource "azurerm_subnet" "vm" {
  name                 = "vm"
  resource_group_name  = azurerm_resource_group.pcpc.name
  virtual_network_name = azurerm_virtual_network.vnet.name
  address_prefixes     = ["10.0.2.0/24"]
}

# network security group
resource "azurerm_network_security_group" "nsg" {
  name                = "${var.name}-nsg"
  location            = var.location
  resource_group_name = azurerm_resource_group.pcpc.name
}

# NSG SSH
resource "azurerm_network_security_rule" "ssh" {
  count = var.firewall-ssh ? 1 : 0

  name                        = "allow-ssh"
  priority                    = 1000
  direction                   = "Inbound"
  access                      = "Allow"
  protocol                    = "Tcp"
  source_port_range           = "*"
  destination_port_range      = "22"
  source_address_prefix       = "*"
  destination_address_prefix  = "*"
  resource_group_name         = azurerm_resource_group.pcpc.name
  network_security_group_name = azurerm_network_security_group.nsg.name
}

# NSG Internal
resource "azurerm_network_security_rule" "internal" {
  for_each = var.firewall-internal ? {
    for index, protocol in ["Tcp", "Udp", "Icmp"] : index => protocol
  } : {}

  name                         = "allow-internal-${each.value}"
  priority                     = 1001 + each.key
  direction                    = "Inbound"
  access                       = "Allow"
  protocol                     = each.value
  source_port_range            = "0-65535"
  destination_port_range       = "0-65535"
  source_address_prefixes      = azurerm_subnet.vm.address_prefixes
  destination_address_prefixes = azurerm_subnet.vm.address_prefixes
  resource_group_name          = azurerm_resource_group.pcpc.name
  network_security_group_name  = azurerm_network_security_group.nsg.name
}

# NSG External
locals {
  next_priority = var.firewall-internal ? 1001 + length(azurerm_network_security_rule.internal) : 1001
}

resource "azurerm_network_security_rule" "external" {
  for_each = {
    for index, rule in var.firewall-external : index => rule
  }

  name                        = "allow-external-${join("_", each.value.ports)}"
  priority                    = local.next_priority + each.key
  direction                   = "Inbound"
  access                      = "Allow"
  protocol                    = each.value.protocol
  source_port_range           = "*"
  destination_port_ranges     = each.value.ports
  source_address_prefix       = "*"
  destination_address_prefix  = "*"
  resource_group_name         = azurerm_resource_group.pcpc.name
  network_security_group_name = azurerm_network_security_group.nsg.name
}

# Connect the security group to the internal subnet
resource "azurerm_subnet_network_security_group_association" "nsg-vm" {
  subnet_id                 = azurerm_subnet.vm.id
  network_security_group_id = azurerm_network_security_group.nsg.id
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

#################### VM ######################
# cloud-init
# run "cloud-init status --wait" in the SSH to check when it is done
# run "tail -f /var/log/cloud-init-output.log" to see what it is doing
data "cloudinit_config" "conf" {
  gzip          = true
  base64_encode = true

  part {
    content_type = "text/cloud-config"
    content      = file(var.cloud-init-file)
    filename     = "conf.yaml"
  }
}

module "vm" {
  count  = var.machines-count
  source = "./modules/vm"

  name                = "${var.name}-${count.index}"
  resource_group_name = azurerm_resource_group.pcpc.name
  location            = var.location
  size                = var.machine-type

  source_image_reference = {
    publisher = var.os-image.publisher
    offer     = var.os-image.offer
    sku       = var.os-image.sku
    version   = var.os-image.version
  }

  ssh-user       = var.ssh-user
  ssh-public_key = chomp(tls_private_key.ssh.public_key_openssh)

  subnet_id                 = azurerm_subnet.vm.id
  network_security_group_id = azurerm_network_security_group.nsg.id

  cloud-init = data.cloudinit_config.conf.rendered
}
