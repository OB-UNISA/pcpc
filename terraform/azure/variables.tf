variable "location" {
  default = "germanywestcentral"
}

variable "name" {
  default = "pcpc"
}

variable "machines-count" {
  type = number
}

variable "machine-type" {
  default = "Standard_B1s"
}

variable "os-image" {
  default = {
    publisher = "Canonical"
    offer     = "0001-com-ubuntu-server-jammy"
    sku       = "22_04-lts-gen2"
    version   = "latest"
  }
}

variable "firewall-ssh" {
  default = true
}

variable "firewall-internal" {
  default     = true
  description = "Allow the machines to communicate between them in the LAN"
}

variable "firewall-external" {
  type = list(object({
    ports    = list(string)
    protocol = string
  }))

  default = []

  description = "Add additional ports to be opened on the WLAN"
}

variable "ssh-user" {
  default = "pcpc-ssh"
}

variable "ssh-pk-save-path" {
  default = "azure.pem"
}

variable "cloud-init-file" {
  type = string
}
