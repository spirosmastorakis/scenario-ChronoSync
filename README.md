Prerequisites
=============

Custom version of NS-3 (branch `ndnSIM-v2.5`) and ndnSIM 2.6 (tagged as `ndnSIM-2.6`)
needs to be installed.

It may or may not work with more recent ndnSIM versions (> 2.6).

    mkdir ndnSIM
    cd ndnSIM

    git clone https://github.com/named-data-ndnSIM/ns-3-dev ns-3
    (cd ns-3; git checkout ndnSIM-v2.5)
    git clone --recursive https://github.com/named-data-ndnSIM/ndnSIM ns-3/src/ndnSIM
    (cd ns-3/src/ndnSIM; git checkout ndnSIM-2.6)

    git clone --recursive https://github.com/spirosmastorakis/scenario-ChronoSync

    cd ns-3
    ./waf configure -d optimized
    ./waf
    sudo ./waf install

    # When using Linux, run
    # sudo ldconfig

    # When using Freebsd, run
    # sudo ldconfig -a

    cd ..
    git clone https://github.com/named-data-ndnSIM/scenario-template.git my-simulations
    cd my-simulations

    ./waf configure
    ./waf --run <scenario>

After which you can proceed to compile and run the code

For more information how to install NS-3 and ndnSIM, please refer to http://ndnsim.net website.

Compiling
=========

To configure in optimized mode without logging **(default)**:

    ./waf configure

To configure in optimized mode with scenario logging enabled (logging in NS-3 and ndnSIM modules will
still be disabled, but you can see output from NS_LOG* calls from your scenarios and extensions):

    ./waf configure --logging

To configure in debug mode with all logging enabled

    ./waf configure --debug

If you have installed NS-3 in a non-standard location, you may need to set up ``PKG_CONFIG_PATH``
variable.

Running
=======

You can run the ping scenario by typing

    ./waf --run chronosync-simple

    # or
    # ./build/chronosync-simple
