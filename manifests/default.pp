class init {
	exec {  'apt-get-update':
		command => '/usr/bin/apt-get update'
	}

	package { [ "vim", "php5", "php5-curl", "php5-cli", "curl", "make", "g++", "gcc", "libcurl4-openssl-dev", "git", "autoconf", "libtool", "libjansson-dev", "build-essential", "gdb", "clang", "libcunit1-dev", "llvm-3.0", "valgrind" ] :
		ensure => present,
		require => Exec["apt-get-update"]
	}

  file { '/usr/lib/libprofile_rt.a':   
    ensure => 'link',    
    target => '/usr/lib/llvm-3.0/lib/libprofile_rt.a',   
    require => Package['llvm-3.0']   
  }
}

include init
