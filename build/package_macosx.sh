#!/bin/bash
version=030
root=X-IvAp_${version}_mac

rm -rf $root
mkdir $root
cp ../doc/*.txt $root

mkdir $root/introduction
cp ../doc/*.htm $root/introduction
cp ../doc/*.gif $root/introduction

mkdir $root/for_plugins_folder
cp ../src/Linux/Release/Resources/plugins/*.xpl $root/for_plugins_folder
cp ../src/FMOD/FMODMac/api/lib/libfmodex.dylib $root
cp ../src/FMOD/FMODMac/api/lib/libfmodexL.dylib $root
mkdir $root/for_plugins_folder/X-IvAp\ Resources
cp -R ../build/X-IvAp\ Resources/* $root/for_plugins_folder/X-IvAp\ Resources

find $root/for_plugins_folder/X-IvAp\ Resources -name '.svn' | xargs -n1 -I+ rm -rf "+"

rm -f X-IvAp_*_mac.tar.bz2
tar -cjf X-IvAp_${version}_mac.tar.bz2 $root/
