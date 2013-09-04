#!/bin/bash
wget http://sourceforge.net/projects/boost/files/boost/1.54.0/boost_1_54_0.tar.bz2
tar --bzip2 -xf boost_1_54_0.tar.bz2
cd boost_1_54_0
./bootstrap.sh --help
sudo ./bootstrap.sh
sudo ./b2 install
cd ..
bash ./compile_libgit2.sh
cd deps/libgit2
cmake .
make
cd ../../
node-gyp configure
