variable "credentials" {
  type      = string
  sensitive = true
}

variable "project-id" {
  type      = string
  sensitive = true
}

variable "name" {
  default = "pcpc"
}

variable "machines-count" {
  type = number
}

variable "region" {
  default = "us-west1"
}

variable "machine-type" {
  default = "e2-micro"
}

variable "os-image" {
  default = "ubuntu-os-cloud/ubuntu-2204-lts"
}

variable "spot-instance" {
  default = true
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
  default = "gcp.pem"
}

variable "cloud-init-file" {
  type = string
}
