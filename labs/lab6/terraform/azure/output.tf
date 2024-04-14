output "ssh-user" {
  value = module.azure-vm.ssh-user
}

output "private-ips" {
  value = module.azure-vm.private-ips
}

output "public-ips" {
  value = module.azure-vm.public-ips
}
