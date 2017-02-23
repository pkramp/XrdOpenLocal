#!/bin/bash
if
[ "$1" == "debug" ]; then
    export XRD_LOGLEVEL=Dump

fi
export XRD_LOGLEVEL=Dump
###Setup the test
cat > test/XrdOpenLocal.conf << EOF
url = root://test.test
lib = $PWD/XrdOpenLocal.so
#redirectlocal = test.test|/tmp/
redirectlocal = a|b
enable = true
EOF
export XRD_PLUGINCONFDIR=$PWD/test
echo "test_NODEFAULT" > /tmp/testfile

echo -e "\e[93m xrdfs a rm on a non existing "test.test" remote and redirecting it to local \e[0m"
#xrdfs root://127.0.0.1:1094// rm /tmp/testfile
xrdfs root://test.test rm /tmp/testfile
#DIFF=$(diff testfile testfile2)
#if  [ $? -eq 0 ] && [ "$DIFF" == "" ]; then
#    echo -e "\e[92m SUCCESS \e[0m"
#else
#    echo -e "\e[91m FAILED \e[0m"
#fi
###Cleanup the test
