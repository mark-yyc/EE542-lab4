#!/bin/sh
restext=$(sudo md5sum $2 | head -n1 | awk '{print $1;}')
origtext=$(sudo md5sum $1 | head -n1 | awk '{print $1;}')
if [ "$restext" = "$origtext" ]; then
	echo "md5sum check passed"
else
	echo "md5sum check failed"
	echo $restext
	echo $origtext
fi
