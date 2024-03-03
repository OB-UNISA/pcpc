# public ip
resource "azurerm_public_ip" "vm" {
  name                = var.name
  resource_group_name = var.resource_group_name
  location            = var.location
  allocation_method   = "Dynamic"
}

# network interface
resource "azurerm_network_interface" "vm" {
  name                = var.name
  location            = var.location
  resource_group_name = var.resource_group_name


  ip_configuration {
    name                          = var.name
    subnet_id                     = var.subnet_id
    private_ip_address_allocation = "Dynamic"
    public_ip_address_id          = azurerm_public_ip.vm.id
  }
}

# Connect the security group to the network interface
resource "azurerm_network_interface_security_group_association" "ni_nsg" {
  network_interface_id      = azurerm_network_interface.vm.id
  network_security_group_id = var.network_security_group_id
}

# The VM
resource "azurerm_linux_virtual_machine" "vm" {
  name                = var.name
  resource_group_name = var.resource_group_name
  location            = var.location
  size                = var.size
  admin_username      = var.ssh-user
  network_interface_ids = [
    azurerm_network_interface.vm.id,
  ]

  admin_ssh_key {
    username   = var.ssh-user
    public_key = var.ssh-public_key
  }

  os_disk {
    storage_account_type = "Standard_LRS"
    caching              = "None"
    disk_size_gb         = 64
  }

  source_image_reference {
    publisher = var.source_image_reference.publisher
    offer     = var.source_image_reference.offer
    sku       = var.source_image_reference.sku
    version   = var.source_image_reference.version
  }

  custom_data = var.cloud-init
}
