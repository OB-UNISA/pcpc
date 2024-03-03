variable "credentials" {
  type      = string
  sensitive = true
}

variable "project-id" {
  type      = string
  sensitive = true
}

variable "region" {
  default = "europe-west3"
}

variable "ssh-user" {
  default = "pcpc-ssh"
}