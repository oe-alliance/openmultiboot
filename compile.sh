#!/bin/bash
/home/skaman/Documenti/build-enviroment/builds/openmips/gbquadplus/tmp/sysroots/x86_64-linux/usr/bin/mipsel-oe-linux/mipsel-oe-linux-gcc -o open-multiboot omb_main.c omb_framebuffer.c omb_freetype.c omb_log.c omb_menu.c omb_input.c omb_utils.c omb_lcd.c -I/home/skaman/Documenti/build-enviroment/builds/openmips/gbquadplus/tmp/sysroots/gbquadplus/usr/include/freetype2 -lfreetype -lm -Wall
ncftpput -u root -p root 172.16.1.147 /sbin open-multiboot
