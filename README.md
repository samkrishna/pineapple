# Pineapple

This project is a standalone Jupyter environment for doing data science
using Python. It aims to include many useful working libraries
and packages, while remaining super easy to install and use.

## Building Prerequisites

General requirements:
* C++11 compiler (e.g. g++-4.9 or later)
* wxWidgets 3.x (source compile)
* lessc (for compiling .less files, get it with npm)

### Mac OS X

For wxWidgets, I downloaded the source then used:

```
mkdir build-release
cd build-release
../configure --enable-shared --enable-monolithic --with-osx_cocoa CXX='clang++ -std=c++11 -stdlib=libc++' CC=clang --with-macosx-version-min=10.8 --disable-debug --without-liblzma
make -j4
sudo make install
```

### Ubuntu 14.04

To get g++-4.9 (required) use the Ubuntu Toolchain PPA.

```
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt-get update
sudo apt-get install g++-4.9
```

You'll need various development files to compile.

```
sudo apt-get install build-essential libz-dev libgtk2.0-dev
    libreadline-dev libssl-dev libncursesw5-dev
    libgdbm-dev libsqlite3-dev libbz2-dev liblzma-dev
    libreadline-gplv2-dev
    libc6-dev
```

Get the wxWidget source and compile:

```
mkdir buildgtk
cd buildgtk
../configure --with-gtk
make -j4
sudo make install
```

## Building for local testing

If prerequisites are met, you should be able to do:

```
mkdir build
cd build
cmake ..
make
```

This builds python and various libraries and builds
the main application.

```
make custom-install
```
This installs pip requirements and then copies custom
files into the notebook static directory. Now you can
try the local application with `make local-test`.

## Distribution

Redistributable packages are built using CPack.

```
make install
make package
```

The final redistributable files will be placed at the top level of the
build directory. Final packages will be compressed tar files for
Linux, DMG images for Mac.

## Notes

### OS X

If you want to change the icon images, you'll need to regenerate the icns files.
I did this using [iconverticons](https://iconverticons.com) because the command-line tools
I found were out of date and I didn't want to hassle with automating it.

## Contact

Pineapple is a project of Nathan Whitehead, copyright 2015.
Let me know what you think at nwhitehe [at] gmail.com.

## Notes on 2018-10-14 20:08:12 -0700 (From Sam Krishna)

This **TOOK SOMETHING** to update. Esp. with the following updates:

- Python 3.7
- iPython 7.x
- Jupyter 1.0

So this is going to be a list of notes about things to do to build this codebase with updated Python / iPython / Jupyter PLUS all the Data Science goodness. Esp. under macOS Mojave. **IN CASE IT'S NOT CLEAR: THESE BUILD INSTRUCTIONS ARE FOR macOS MOJAVE.**

### Installing Command-Line Tools (CLT)

First, you're going to need to install the CLT to deal with the issues at-hand. From [This issue with zlib and Python 3.7](https://github.com/pyenv/pyenv/issues/1219):

'''
sudo installer -pkg /Library/Developer/CommandLineTools/Packages/macOS_SDK_headers_for_macOS_10.14.pkg -target
'''

You'll need this later when installing Python 3.7. The CLT command is required b/c the Mojave SDK doesn't install its headers into /usr/include by default.




