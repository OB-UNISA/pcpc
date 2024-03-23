output "ssh-user" {
  value = module.compute-instance.ssh-user
}

output "private-ips" {
  value = module.compute-instance.private-ips
}

output "public-ips" {
  value = module.compute-instance.public-ips
}
