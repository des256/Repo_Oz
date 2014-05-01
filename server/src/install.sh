#!/bin/sh

# TODO: make sure this is run as root
# TODO: make sure this is a Robokind R-50 robot

echo -e "Installing Oz Puppeteering System on Robokind R-50 robot.\nPlease be patient, this might take a while.\n"

# create tree
mkdir /usr/oz
mkdir /usr/oz/arch
mkdir /usr/oz/arch/robokind-r50
mkdir /usr/oz/arch/robokind-r50/bin
mkdir /usr/oz/arch/robokind-r50/lib
mkdir /usr/oz/data
mkdir /usr/oz/init.d

# copy switching and init.d scripts
cp init.d/oz.sh /usr/oz
cp init.d/robokind.sh /usr/oz
cp init.d/oz /etc/init.d
cp init.d/robokind /etc/init.d

# copy executable and datafiles
cp arch/robokind-r50/bin/ozserver /usr/oz/arch/robokind-r50/bin
cp arch/robokind-r50/lib/* /usr/oz/arch/robokind-r50/lib
cp -R data/* /usr/oz/data

# copy startup script
cp start.sh /usr/oz

echo -e "Done.\nTo use robot in Oz mode, run /usr/oz/oz.sh.\nTo use robot in Robokind mode, run /usr/oz/robokind.sh.\n"

