VFD=mtximage

as86 -o  ts.o  ts.s
bcc  -c -ansi  t.c
ld86 -d -o mtx ts.o t.o mtxlib /usr/lib/bcc/libc.a

echo mount $VFD on /mnt
sudo mount -o loop $VFD /mnt

sudo cp mtx /mnt/boot

sudo umount /mnt

rm *.o mtx

(cd USER; ./mku.sh u1)

echo done
