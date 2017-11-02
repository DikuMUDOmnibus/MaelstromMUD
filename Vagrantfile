# -*- mode: ruby -*-
# vi: set ft=ruby :

require File.dirname(__FILE__) + "/vagrant/addons/dependency_manager"

check_plugins ["vagrant-exec"]

Vagrant.configure("2") do |config|
  # All Vagrant configuration is done here. The most common configuration
  # options are documented and commented below. For a complete reference,
  # please see the online documentation at vagrantup.com.

  # Every Vagrant virtual environment requires a box to build off of.
  config.vm.box = "precise32"

  # The url from where the 'config.vm.box' box will be fetched if it
  # doesn't already exist on the user's system.
  config.vm.box_url = "http://files.vagrantup.com/precise32.box"

  # make /vagrant the working directory for all commands
  config.exec.commands '*', directory: '/vagrant'

  # Create a forwarded port mapping which allows access to a specific port
  # within the machine from a port on the host machine. In the example below,
  # accessing "localhost:8080" will access port 80 on the guest machine.
  # config.vm.network :forwarded_port, guest: 80, host: 8080
  config.vm.network :forwarded_port, guest: 1234, host: 1234

	config.vm.provision :puppet do |puppet|
		 puppet.manifests_path = "vagrant/manifests"
		 puppet.manifest_file  = "default.pp"
	end
end
