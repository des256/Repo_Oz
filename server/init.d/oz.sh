#!/bin/sh

# TODO: make sure this is run as root

echo -e "Switching to Oz mode. Please wait.\n"

# replace services.json
cp oz/services.json /usr/robokind/etc

# kill all robokind services
killall java
killall acapelaspeechserver
killall speechpipelistener
killall python
killall python2

# replace init.d symlinks
rm /etc/rc2.d/S99robokind
rm /etc/rc2.d/S20speech-dispatcher
rm /etc/rc3.d/S99robokind
rm /etc/rc3.d/S20speech-dispatcher
rm /etc/rc4.d/S99robokind
rm /etc/rc4.d/S20speech-dispatcher
rm /etc/rc5.d/S98robokind
rm /etc/rc5.d/S20speech-dispatcher
ln -s /etc/init.d/oz /etc/rc2.d/S99oz
ln -s /etc/init.d/oz /etc/rc3.d/S99oz
ln -s /etc/init.d/oz /etc/rc4.d/S99oz
ln -s /etc/init.d/oz /etc/rc5.d/S98oz

echo -e "Done.\nStarting Oz server...\n"

# start server
/usr/oz/start.sh &
