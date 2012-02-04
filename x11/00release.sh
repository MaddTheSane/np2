#!/bin/sh
echo generating configure...
sh autogen.sh

#echo applying patches...
#for i in 00patch-*
#do
#  ( cd .. && cat x11/$i | patch -p0 -s )
#done
#( cd .. && find . -type f -name "*.orig" | xargs rm -f )

echo copying win9x np21 project files...
cp -p ~/work/xnp2/x11/np21.* ../win9x/

rm -f 00patch-*
rm -f 00release.sh
