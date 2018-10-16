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

## Notes on 2018-10-14 20:08:12 -0700 (for Python 3.7 + Jupyter on Mojave)

This **TOOK SOMETHING** to update. Esp. with the following updates:

- Python 3.7
- iPython 7.x
- Jupyter 1.0

So this is going to be a list of notes about things to do to build this codebase with updated Python / iPython / Jupyter PLUS all the Data Science goodness. Esp. under macOS Mojave. **IN CASE IT'S NOT CLEAR: THESE BUILD INSTRUCTIONS ARE FOR macOS MOJAVE.**

### Installing Command-Line Tools (CLT)

First, you're going to need to install the CLT to deal with the issues at-hand. From [this issue with zlib and Python 3.7](https://github.com/pyenv/pyenv/issues/1219):

```
sudo installer -pkg /Library/Developer/CommandLineTools/Packages/macOS_SDK_headers_for_macOS_10.14.pkg -target
```

You'll need this later when installing Python 3.7. The CLT command is required b/c the Mojave SDK doesn't install its headers into `/usr/include` by default.

### Getting the baseline infrastructure up-and-going

Using [Homebrew](https://brew.sh) was a god-send. Seriously.

Install / update to the following kegs (and their dependencies and other packages):

- openssl (OpenSSL 1.0.2p)
- pyenv (to manage multiple versions of Python 3.x)
- npm
- cmake
- ncurses

### Installing and configuring Python 3.7 (the first time)

Use `pyenv` to install Python 3.7 (along with a TON of wheels). Once you've got Python 3.7 installed, use the following command at the root level of the Pineapple codebase directory to setup Python 3.7 as the Python you're going to build against.

To install Python 3.7.0 with `pyenv` and to build it as a framework (which you'll need for browser-based Jupyter):
```
$ env PYTHON_CONFIGURE_OPTS="--enable-framework" pyenv install 3.7.0
```

To set Python 3.7.0 as the "environmental Python" for a given directory tree (like the 'pineapple' directory tree):

```
$ pyenv local 3.7.0
```

Upgrade both `pip` and `wheel`:

```
$ pip install pip wheel -U
````

Once you've done that, you're ready to install all the wheels you'll need to run Jupyter / iPython notebooks through the local webserver. Here's how you do that:

```
$ pip install -r requirements37.txt --no-cache-dir
```

This should install all the wheels necessary and sufficient to get a Jupyter notebook instance running with enough data science goodness.

After you've done this, you may or may not need to install a Jupyter kernel. You can test this out by running `jupyter notebook <notebook name>` and watch the console log. If it gives you a message saying something about "kernel failures", you'll need to add the `ipython` kernel.

You can do this by typing:

```
python3.7 -m ipykernel install
```

Then after trying to run `jupyter notebook <notebook name>`, you may get a kernel error where jupyter is trying to connect to a Python 3.6 kernel. To fix this, do the following:

1. Type `jupyter kernelspec list` to find out where the `python3` kernel is located
2. `cd` to the given directory
3. open `kernel.json` in an editor
4. Change the `3.6` references to `3.7`
5. There is no Step 5.

After that, Jupyter should more-or-less run, but now you'll likely want to disable the authentication requirement.

### Disabling Jupyter authentication under pure Python 3.7

Unfortunately, you'll need to do this twice (once for pure Python 3.7 and once again after you get Pineapple running). (Twice is only for practice and understanding how to make it all work.)

For the first time, go to the `~/.jupyter/` directory, and type the following:

```
$ jupyter notebook --generate-config .
```

Then edit the generated `jupyter_notebook_config.py` file to disable authentication (NOTE: Be clear that you are **ONLY** hosting locally-served Jupyter notebooks). You can do that by following the instructions here: [Disable Jupyter authentication](https://github.com/jupyter/notebook/issues/2254#issuecomment-321189274)

Set the `c.NotebookApp.token` parameter to an empty string in the configuration file created earlier. This will disable authentication.

## Building Pineapple

### Build the wxWidgets source

[Download the wxWidgets source here.](https://www.wxwidgets.org/downloads/)

After decompressing the source tree, run the following commands:

```
mkdir build-release
cd build-release
../configure --enable-shared --enable-monolithic --with-osx_cocoa CXX='clang++ -std=c++11 -stdlib=libc++' CC=clang --with-macosx-version-min=10.8 --disable-debug --without-liblzma
make -j4
sudo make install
```

After installing the wxWidgets, delete its source tree. You won't need it again for the rest of the build process.

### Build Pineapple

Congratulations. You're finally ready to build Pineapple.

Here's what to do next:

```
$ mkdir build
$ cd build
$ npm install -g less
$ export LDFLAGS="-L/usr/local/opt/openssl/lib -L/usr/local/opt/ncurses/lib"
$ export CPPFLAGS="-I/usr/local/opt/openssl/include -I/usr/local/opt/ncurses/include"
$ export PKG_CONFIG_PATH="/usr/local/opt/openssl/lib/pkgconfig"
$ cmake ..
$ make
```

Once the initial pass of building Pineapple is done (along with its own Python 3.7.0), run the following commands from the `build` directory:

```
$ cd python3.7/Python.framework/Versions/3.7/bin
$ ./pip3 install -U pip wheel --no-cache-dir
$ ./pip install -r ../../../../../../requirements37.txt --no-cache-dir
```

There will probably be a few wheels that break on install from the `requirements37.txt`. Just install them individually like so:

```
$ ./pip install <wheel-name> --no-cache-dir
```

After you're finished with all of that, you'll need to run the following commands:

```
$ make custom-install (this will be somewhat redundant b/c you already ran pip against the requirements37.txt)
$ make local-test (this runs the app)
```

### Disabling the Jupyter authentication for Pineapple:

Now you've got to disable authentication a second time. Go to the following directory:

```
cd ~/.pineapple/Jupyter
```

and run the following command:

```
jupyter notebook --generate-config .
```

And set the `c.NotebookApp.token` parameter to an empty string to disable authentication.

After that, run:

```
$ make local-test (to check to make sure it runs correctly without authentication)
$ make install
$ make package
```

And you're done.

NOTE: Tried upgrading `pycurl` and `curl` by putting in the updated .zip files. It failed miserably and I gave up after 7-8 compile attempts.

NOTE: In case something goes wrong with the wheel installations, executing some variation of the following command will remove all wheels from your Python package site:

```
pip freeze | xargs pip uninstall -y
```
