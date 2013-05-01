#!/bin/sh
make clean
make
sudo umount -a -t helo
sudo rmmod helo
sudo insmod ./helo.ko
sudo mount -t helo helo /mnt
sudo cat /mnt/hello
