module "azure-vm" {
  source = "../../../../terraform/azure"

  name             = "pcpc-lab6"
  ssh-user         = "azureuser"
  ssh-pk-save-path = var.ssh-pk-save-path
  machines-count   = 3
  cloud-init-file  = "cloud-init.yaml"
  # run "cloud-init status --wait" in the SSH to check when it is done
  # run "tail -f /var/log/cloud-init-output.log" to see what it is doing
}
