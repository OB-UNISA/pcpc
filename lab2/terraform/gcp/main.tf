module "compute-instance" {
  source = "../../../terraform/gcp"

  credentials = var.credentials
  project-id  = var.project-id

  name            = "pcpc-lab2"
  ssh-user        = "root"
  machines-count  = 2
  cloud-init-file = "cloud-init.yaml"
  # run "cloud-init status --wait" in the SSH to check when it is done
  # run "tail -f /var/log/cloud-init-output.log" to see what it is doing

  firewall-external = [
    {
      ports    = ["80"]
      protocol = "tcp"
    }
  ]
}
