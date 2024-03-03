output "vm" {
  value = {
    private-ip = google_compute_instance.vm_instance.*.network_interface.0.network_ip
    public-ip  = google_compute_instance.vm_instance.*.network_interface.0.access_config.0.nat_ip
    ssh-user   = var.ssh-user
  }
}
