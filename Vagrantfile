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

  # set ip address to 192.168.12.34
  config.vm.network "private_network", ip: "192.168.12.34", hostsupdater: "skip"

  # The url from where the 'config.vm.box' box will be fetched if it
  # doesn't already exist on the user's system.
  config.vm.box_url = "http://files.vagrantup.com/precise32.box"

  # make /vagrant the working directory for all commands
  config.exec.commands '*', directory: '/vagrant'

	config.vm.provision :puppet do |puppet|
		 puppet.manifests_path = "vagrant/manifests"
		 puppet.manifest_file  = "default.pp"
	end
end
