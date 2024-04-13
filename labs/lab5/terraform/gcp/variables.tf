variable "credentials" {
  type      = string
  sensitive = true
}

variable "project-id" {
  type      = string
  sensitive = true
}

variable "ssh-pk-save-path" {
  type = string
}
