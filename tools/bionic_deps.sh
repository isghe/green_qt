#!/bin/bash
set -eo pipefail

if [ -f /.dockerenv ]; then
    if [ -f /bionic_hash ]; then
        bionic_deps_sha256=$(< /bionic_hash)
        echo "${bionic_deps_sha256} tools/bionic_deps.sh" | sha256sum --check --strict --status && exit 0 || true
    fi
fi

apt-get update -yqq
apt-get upgrade --no-install-recommends -yqq
apt-get install --no-install-recommends -yqq clang curl ca-certificates unzip git automake autoconf pkg-config libtool build-essential ninja-build llvm-dev python3-pip python3-setuptools virtualenv libgl1-mesa-dev python libfontconfig1-dev libfreetype6-dev libx11-dev libxext-dev libxfixes-dev libxi-dev libxrender-dev libxcb1-dev libx11-xcb-dev libxcb-glx0-dev libxkbcommon-x11-dev libd3dadapter9-mesa-dev libegl1-mesa-dev libgles2-mesa-dev software-properties-common

add-apt-repository ppa:cybermax-dexter/mingw-w64-backport
apt-get update

apt-get install --no-install-recommends -yqq g++-mingw-w64-x86-64

update-alternatives --set x86_64-w64-mingw32-g++ /usr/bin/x86_64-w64-mingw32-g++-posix

curl https://sh.rustup.rs -sSf | sh -s -- -y --default-toolchain 1.39.0
source /root/.cargo/env
rustup target add x86_64-pc-windows-gnu


if [ ! -d /qt-everywhere-src-5.14.0 ]; then
   curl -sL -o qt-everywhere-src-5.14.0.tar.xz https://download.qt.io/archive/qt/5.14/5.14.0/single/qt-everywhere-src-5.14.0.tar.xz
   echo "be9a77cd4e1f9d70b58621d0753be19ea498e6b0da0398753e5038426f76a8ba qt-everywhere-src-5.14.0.tar.xz" | sha256sum --check --strict
   tar xf qt-everywhere-src-5.14.0.tar.xz
   rm qt-everywhere-src-5.14.0.tar.xz
fi

if [ -f /.dockerenv ]; then
    sha256sum /bionic_deps.sh | cut -d" " -f1 > /bionic_hash
    apt -yqq autoremove
    apt -yqq clean
    rm -rf /var/lib/apt/lists/* /var/cache/* /tmp/* /usr/share/locale/* /usr/share/man /usr/share/doc /lib/xtables/libip6* /root/.cache
fi
