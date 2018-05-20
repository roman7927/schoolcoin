ROMAN 1 b8:27:eb:12:df:59 (3.45.48.66)

ROMAN 2 b8:27:eb:a6:d0:85 (3.45.48.67)

DIMA  1 b8:27:eb:f6:62:a9 (3.45.48.62)

DIMA  2 ...

Links:
* http://raspnode.com/diyBitcoin.html
* http://elinux.org/RPi_SD_cards
* https://en.bitcoin.it/wiki/Bitcoind#History_of_official_bitcoind_.28and_predecessor.29_releases
* https://github.com/bitcoin/bitcoin/tags?after=v0.3.7
* 3B+ https://raspberrypi.stackexchange.com/questions/81725/raspbian-8-on-raspberry-pi-3-b

Info:
* We are going to use 0.5.0 which is when they switched from Wx to Qt, and I haven't been able to get Wx installed on Raspi
* Buy Raspi 3 B+ with power, case, 256GB SD card
* May 2018 - bitcoin blockchain 170GB
* 0.1.0 Lines 21814, after removing empty lines 18226, after removing comments 14043
* 0.5.0 bitcoind 24764, after removing empty lines 21288, after removing comments 18918
* 0.5.0 bitcoinqt 8357, after removing empty lines 7199, after removing comments 6738

Use Etcher to burn "Raspbian for Robots" image on SD card (need Mini>Micro SD card reader or USB): 2017.10.05-BETA_Dexter_Industries_jessie.img

This SD card will not boot on new hardware Raspi 3B+. You first have to boot this SD card on Raspi 3B, and run this command, which upgrade bootloader on SD card to allow boot on 3B+:

sudo rpi-update ef7621d91cb58ccc856c3c17ddda28685edd23f3

Now you can run on 3B+. NIC will work, but Wifi will not. You now need download the 3B+ wifi drivers from here:

https://archive.raspberrypi.org/debian/pool/main/f/firmware-nonfree/firmware-brcm80211_20161130-3+rpt3_all.deb

Now you can copy through the CAT5 from Macbook to the Raspi 3B+ using this command once you cd into the directory where downloaded file is located. Replace IP with the actual autoconfigured IP address:

scp firmware-brcm80211_20161130-3+rpt3_all.deb pi@169.254.86.181:/home/pi/firmware-brcm80211_20161130-3+rpt3_all.deb

password: robots1234

Now ssh to the Raspi 3B+ and run this command:

sudo dpkg -i firmware-brcm80211_20161130-3+rpt3_all.deb

Reboot. Wifi should work now.

You can connect to VNC dex.local:8001 through direct CAT5 from computer, and connect Raspi to Wifi. Get the IP of Raspi on wifi (ifconfig). Remove CAT5 direct connection. 

ssh pi@dex.local

user: pi
password: robots1234

Change pi's and vnc passwords
>passwd
>vncpasswd

Connect to wireless
>sudo nano /etc/wpa_supplicant/wpa_supplicant.conf
	network={
    	ssid="<your wifi ssid here>"
    	psk="<your wifi password here>"
	}
>ifconfig wlan0

pi@dex:~ $ uname -a
Linux dex 4.4.50-v7+ #970 SMP Mon Feb 20 19:18:29 GMT 2017 armv7l GNU/Linux

Expand file system and reboot
>sudo raspi-config

>sudo apt-get -y update
&&&&SKIPPING THIS FOR 3B+&&&&&>sudo apt-get -y upgrade (answer N to one question)
>reboot

pi@dex:~ $ uname -a
Linux dex 4.9.35-v7+ #1014 SMP Fri Jun 30 14:47:43 BST 2017 armv7l GNU/Linux

Start from /home/pi
>cd
>wget https://github.com/bitcoin/bitcoin/archive/v0.5.3rc4.tar.gz
>tar xzvf v0.5.3rc4.tar.gz
>rm -rf v0.5.3rc4.tar.gz
>mv bitcoin-0.5.3rc4 bitcoin
>cd bitcoin

>sudo apt-get -y install libssl-dev
>sudo apt-get -y install libboost-all-dev
>sudo apt-get -y install libevent-dev
>sudo apt-get -y install libminiupnpc-dev
>sudo apt-get -y install miniupnpc
>sudo apt-get -y install autoconf

>mkdir db4
>cd db4
>wget http://download.oracle.com/berkeley-db/db-4.8.30.NC.tar.gz
>tar xzvf db-4.8.30.NC.tar.gz
>cd db-4.8.30.NC/build_unix/
>../dist/configure --enable-cxx --disable-shared --with-pic --prefix=/home/pi/bitcoin/db4/
>make
>sudo make install
>export BDB_INCLUDE_PATH="/usr/local/BerkeleyDB.4.8/include"
>export BDB_LIB_PATH="/usr/local/BerkeleyDB.4.8/lib"
>sudo ln -s /usr/local/BerkeleyDB.4.8/lib/libdb-4.8.so /usr/lib/libdb-4.8.so
>sudo mkdir /usr/local/BerkeleyDB.4.8
>sudo ln -s /home/pi/bitcoin/db4/lib /usr/local/BerkeleyDB.4.8/lib
>sudo ln -s /home/pi/bitcoin/db4/include /usr/local/BerkeleyDB.4.8/include

>cd ~/bitcoin/src
>nano makefile.unix
  ----> change USE_UPNP:=0 to USE_UPNP:=1
>make -f makefile.unix
>mv bitcoind ~/bitcoin

>cd
>mkdir .bitcoin
>nano .bitcoin/bitcoin.conf
- add
    rpcuser=bitcoinrpc
    rpcpassword=21Hy2d5kycuoLzWxdJjQoVN1jtL7Q5kzqhHz3ZfuYNCU
>cd ~/bitcoin

TEST NET
>./bitcoind -daemon -testnet

MAIN NET
>./bitcoind -daemon

>>>>> TO INSTALL bitcoin-qt

>sudo apt-get -y install qt4-qmake
>sudo apt-get -y install qt4-dev-tools
>sudo apt-get -y install libprotobuf-dev
>sudo apt-get -y install protobuf-compiler
>sudo apt-get -y install libqrencode-dev
///>sudo apt-get -y install qtcreator <<< don't actually need, but let's play with this in the future, this is a GUI for QT creation

>cd ~/bitcoin
>qmake bitcoin-qt.pro
>nano Makefile
>FIND -I AND ADD "-I/home/pi/bitcoin/db4/include/"
>FIND -L AND ADD "-L/home/pi/bitcoin/db4/lib/"
>make
/////// IGNORE WARNINGS

*************************************************************
Extra:

>grep Revision /proc/cpuinfo
Revision	: a02082, Model 3B, Rev1.2, RAM 1 GB
Quad Core 1.2GHz Broadcom BCM2837 64bit CPU
1GB RAM

>lsb_release -a
No LSB modules are available.
Distributor ID:	Raspbian
Description:	Raspbian GNU/Linux 8.0 (jessie)
Release:	8.0
Codename:	jessie

Raspbian is based on Debian
Raspbian tries to stay as close to Debian as reasonably possible.
