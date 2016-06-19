#! /bin/bash

UNAME_S=`uname -s`

if [ ${UNAME_S} = Darwin ]; then
  sudo brew update
  sudo brew install osxfuse
else
  sudo apt-get update -qq
  sudo apt-get install -qq libfuse-dev
fi
