class init {
	exec {  'apt-get-update':
		command => '/usr/bin/apt-get update'
	}

	package { [ "vim", "php5", "php5-curl", "php5-cli", "curl", "make", "g++", "gcc" ] :
		ensure => present,
		require => Exec["apt-get-update"]
	}
}

include init
