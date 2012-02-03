#!/bin/sh
sh autogen.sh
for i in 00patch-*
do
  ( cd .. && cat x11/$i | patch -p0 )
done
( cd .. && find . -type f -name "*.orig" | xargs rm -f )
cp -p ~/work/xnp2/x11/np21.* ../win9x/
rm -f 00patch-*
rm -f 00release.sh
