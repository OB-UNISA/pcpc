variable "name" {
  type = string
}

variable "resource_group_name" {
  type = string
}

variable "location" {
  type = string
}

variable "size" {
  type = string
}

variable "source_image_reference" {
  type = object({
    publisher = string
    offer     = string
    sku       = string
    version   = string
  })
}

variable "ssh-user" {
  type = string
}

variable "ssh-public_key" {
  type = string
}

variable "subnet_id" {
  type = string
}

variable "network_security_group_id" {
  type = string
}

variable "cloud-init" {
  type = string
}
