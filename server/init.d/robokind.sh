#!/bin/sh

# TODO: make sure this is run as root

echo -e "Switching to Robokind mode. Please wait.\n"

# replace services.json
cp robokind/services.json /usr/robokind/etc

# kill ozserver
killall ozserver

# replace init.d symlinks
rm /etc/rc2.d/S99oz
rm /etc/rc3.d/S99oz
rm /etc/rc4.d/S99oz
rm /etc/rc5.d/S98oz

ln -s /etc/init.d/robokind /etc/rc2.d/S99robokind
ln -s /etc/init.d/speech-dispatcher /etc/rc2.d/S20speech-dispatcher
ln -s /etc/init.d/robokind /etc/rc3.d/S99robokind
ln -s /etc/init.d/speech-dispatcher /etc/rc3.d/S20speech-dispatcher
ln -s /etc/init.d/robokind /etc/rc4.d/S99robokind
ln -s /etc/init.d/speech-dispatcher /etc/rc4.d/S20speech-dispatcher
ln -s /etc/init.d/robokind /etc/rc5.d/S98robokind
ln -s /etc/init.d/speech-dispatcher /etc/rc5.d/S20speech-dispatcher

echo -e "Done. Rebooting.\n"

# reboot
reboot
