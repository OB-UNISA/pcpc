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

variable "myip" {
  type      = string
  sensitive = true
}

variable "ssh-user" {
  default = "google-ssh"
}