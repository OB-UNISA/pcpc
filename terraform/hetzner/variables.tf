variable "hcloud_token" {
  type      = string
  sensitive = true
}

variable "location" {
  default = "fsn1"
}