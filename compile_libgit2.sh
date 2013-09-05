#!/bin/bash
cd deps/libgit2
cmake . > /dev/null 2>&1
make > /dev/null 2>&1
make install > /dev/null 2>&1
cd ../../
