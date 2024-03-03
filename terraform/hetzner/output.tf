output "ssh-user" {
  value = "root"
}

output "private-ips" {
  value = flatten(hcloud_server.vm[*].network[*].ip)
}

output "public-ips" {
  value = hcloud_server.vm[*].ipv4_address
}
