output "ssh-user" {
  value = module.hetzner-server.ssh-user
}

output "private-ips" {
  value = module.hetzner-server.private-ips
}

output "public-ips" {
  value = module.hetzner-server.public-ips
}
