### require 'berkshelf/vagrant'

Vagrant.configure("2") do |config|

  # name host
  config.vm.hostname = "nd-geohash-local"

  # deploy using Ubuntu 14.04 LTS (Trusty Tahr)
  config.vm.box = "ubuntu/trusty64"

  # expose machine as private network (host access only)
  config.vm.network "private_network", ip: "192.168.50.11"

  # increase RAM to 3GB
  config.vm.provider "virtualbox" do |vb|
    vb.memory = "3072"
  end

  # provision shell scripts
  config.vm.provision "shell", path: "vagrant/provision.sh"
  config.vm.provision "shell", path: "vagrant/nodejs/provision.sh"

end
