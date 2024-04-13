module "hetzner-server" {
  source = "../../../../terraform/hetzner"

  hcloud_token     = var.hcloud_token
  name             = "pcpc-lab5"
  ssh-pk-save-path = var.ssh-pk-save-path
  machines-count   = 2
  cloud-init-file  = "cloud-init.yaml"
  # run "cloud-init status --wait" in the SSH to check when it is done
  # run "tail -f /var/log/cloud-init-output.log" to see what it is doing
}
