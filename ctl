#!/bin/bash

# NAME
#
#   ctl - Control project's building
#
# SYNOPSIS
#
#   ctl build   build this project
#   ctl clear   clear build files
# 
# AUTHORS
#
#   Kevin Leptons <kevin.leptons@gmail.com>

# bash options
set -e

ROOT=$(realpath .)
DEST="$ROOT/dest"

# help menu
HELP="USAGE\n
    $(basename $0) build        build repo
    $(basename $0) clear        clear build files
    $(basename $0) -h           show help menu

NOTES

    It requires privilege to perform some operation related with
    permission which require by exercises

    You should work in sandbox environemnt to protect your system from
    wrong actions.\n"

ctl_build()
{
    # prepare
    mkdir -vp $DEST
    cd $DEST

    # use cmake, make to build
    cmake ..
    make

    # modify executalbe file
    sudo chown root:root bin/35-02
    sudo chmod u+s,g+s,o+s bin/35-02
    sudo chown root:root bin/35-03
    sudo chmod u+s,g+s,o+s bin/35-03
    sudo chown root:root bin/36-03
    sudo chmod u+s,g+s,o+s bin/36-03
    sudo chown root:root bin/38-02
    sudo chmod u+s,g+s,o+s bin/38-02
    
    sudo chown root:root bin/40-01
    sudo chmod u+s,g+s,o+s bin/40-01
    sudo chown root:root bin/40-02
    sudo chmod u+s,g+s,o+s bin/40-02
    sudo chown root:root bin/40-03
    sudo chmod u+s,g+s,o+s bin/40-03
}

ctl_clear()
{
    rm -rf $DEST
    rm -rf cmake-build-debug
    if [ -f core ]; then
        rm core
    fi
}

ctl_help()
{
    printf "$HELP"
}

# parse arguments
case "$1" in
    build) ctl_build; exit 0;;
    clear) ctl_clear; exit 0;;
    -h) ctl_help; exit 0;;
    *) ctl_help; exit 1;;
esac
