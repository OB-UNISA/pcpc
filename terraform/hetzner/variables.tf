variable "hcloud_token" {
  type      = string
  sensitive = true
}

variable "location" {
  default = "fsn1"
}

variable "network-zone" {
  default = "eu-central"
}

variable "name" {
  default = "pcpc"
}

variable "machines-count" {
  type = number
}

variable "machine-type" {
  default = "cx11"
}

variable "os-image" {
  default = "ubuntu-22.04"
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
    port     = string
    protocol = string
  }))

  default = []

  description = "Add additional ports to be opened on the WLAN"
}

variable "ssh-pk-save-path" {
  default = "hetzner.pem"
}

variable "cloud-init-file" {
  type = string
}
