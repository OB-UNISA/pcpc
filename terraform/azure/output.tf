output "ssh-user" {
  value = var.ssh-user
}

output "private-ips" {
  value = module.vm[*].private_ips.0
}

output "public-ips" {
  value = module.vm[*].public_ips.0
}
