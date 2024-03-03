terraform {
  required_providers {
    azurerm = {
      source  = "hashicorp/azurerm"
      version = "3.94.0"
    }
    tls = {
      source  = "hashicorp/tls"
      version = "4.0.5"
    }
  }

  required_version = ">= 1.7.3"
}

provider "azurerm" {
  features {}
}

provider "tls" {}


# RG
resource "azurerm_resource_group" "pcpc" {
  name     = "pcpc"
  location = var.location
}

################ Network #####################################
# vnet
resource "azurerm_virtual_network" "vnet" {
  name                = "pcpc"
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
  name                = "nsg"
  location            = var.location
  resource_group_name = azurerm_resource_group.pcpc.name

  # SSH
  security_rule {
    name                       = "allow-ssh"
    priority                   = 1000
    direction                  = "Inbound"
    access                     = "Allow"
    protocol                   = "Tcp"
    source_port_range          = "*"
    destination_port_range     = "22"
    source_address_prefix      = "*"
    destination_address_prefix = "*"
  }

  ######## Internal #########
  # TCP
  security_rule {
    name                         = "allow-internal-tcp"
    priority                     = 1001
    direction                    = "Inbound"
    access                       = "Allow"
    protocol                     = "Tcp"
    source_port_range            = "0-65535"
    destination_port_range       = "0-65535"
    source_address_prefixes      = azurerm_subnet.vm.address_prefixes
    destination_address_prefixes = azurerm_subnet.vm.address_prefixes
  }
  # UDP
  security_rule {
    name                         = "allow-internal-udp"
    priority                     = 1002
    direction                    = "Inbound"
    access                       = "Allow"
    protocol                     = "Udp"
    source_port_range            = "0-65535"
    destination_port_range       = "0-65535"
    source_address_prefixes      = azurerm_subnet.vm.address_prefixes
    destination_address_prefixes = azurerm_subnet.vm.address_prefixes
  }
  # Icmp
  security_rule {
    name                         = "allow-internal-icmp"
    priority                     = 1003
    direction                    = "Inbound"
    access                       = "Allow"
    protocol                     = "Icmp"
    source_port_range            = "*"
    destination_port_range       = "*"
    source_address_prefixes      = azurerm_subnet.vm.address_prefixes
    destination_address_prefixes = azurerm_subnet.vm.address_prefixes
  }
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
  filename        = "azure.pem"
  file_permission = "0600"
}

#################### VM ######################
# cloud-init. Run "cloud-init status" in the SSH to check when it is done
data "cloudinit_config" "conf" {
  gzip          = true
  base64_encode = true

  part {
    content_type = "text/cloud-config"
    content      = file("../cloud-init.yaml")
    filename     = "conf.yaml"
  }
}


module "vm" {
  count  = 2
  source = "./modules/vm"

  name                = "pcpc-${count.index}"
  resource_group_name = azurerm_resource_group.pcpc.name
  location            = var.location
  size                = "Standard_B1s"

  source_image_reference = {
    publisher = "Canonical"
    offer     = "0001-com-ubuntu-server-jammy"
    sku       = "22_04-lts-gen2"
    version   = "latest"
  }

  ssh-user       = var.ssh-user
  ssh-public_key = chomp(tls_private_key.ssh.public_key_openssh)

  subnet_id                 = azurerm_subnet.vm.id
  network_security_group_id = azurerm_network_security_group.nsg.id

  cloud-init = data.cloudinit_config.conf.rendered
}
