#!/bin/bash

os=`uname`
if [ $os == "Darwin" ] ; then
	cp -v Release/Resources/plugins/X-IvAp.xpl /X-Plane\ 9.00\ Beta-11/Resources/plugins/X-IvAp.xpl/Contents/MacOS/X-IvAp 
else
	cp -v Release/Resources/plugins/X-IvAp.xpl ~/XPlane/Resources/plugins
fi
