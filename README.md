--------------------------------------------------------------
Twin - a Textmode WINdow environment, by Massimiliano Ghilardi
--------------------------------------------------------------

Version 0.8.0

Twin is a windowing environment with mouse support, window manager,
terminal emulator and networked clients, all inside a text display.

It supports a variety of displays:
* plain text terminals (any termcap/ncurses compatible terminal,
  Linux console, twin's own terminal emulator);
* X11, where it can be used as a multi-window xterm;
* itself (you can display a twin on another twin);
* twdisplay, a general network-transparent display client, used
  to attach/detach more displays on-the-fly.

Currently, twin is tested on Linux (i386, x86_64, ARM, ARM64, PowerPC, Alpha, Sparc),
on Mac OS X (x86_64) and on FreeBSD (i386, x86_64).
I had yet no chance to seriously test it on other systems.

--------------------------------------------------------------

Documentation

[Tutorial](docs/Tutorial)
	A quite complete tour of twin features: the user interface,
	how to use twin clients, compression, attaching/detaching
	displays, fonts. It also contains installation instructions
	and some caveats for system administrators.

[COPYING](COPYING)
	License: twin server and clients are GPL'ed software.

[COPYING.LIB](COPYING.LIB)
	Library license: the libraries libTutf, libTw, libTT
	are LGPL'ed software.

[INSTALL](INSTALL)
	Quick compile/install guide.

[system.twinrc](system.twinrc)
	A detailed example of ~/.twinrc look-n-feel configuration file.

The following documentation is useful mostly to developers:

[Configure](docs/Configure)
	Description of twin configuration options with the meaning
	of every single one.

[README.git](README.git)
	Hints to build twin from GIT repository.

[README.porting](README.porting)
	Tips and warnings to compile twin on unsupported OSes.

[libTw.txt](docs/libTw.txt)
	reference API for programmers who want to write twin clients (INCOMPLETE).
		
[libTw++.txt](docs/libTw++.txt)
	reference API for programmers who want to write	twin C++ clients (INCOMPLETE).

--------------------------------------------------------------
Getting twin


Since you are reading this README, you probably already have it,
anyway twin can be downloaded from

https://github.com/cosmos72/twin

--------------------------------------------------------------
Building and installing twin

For detailed instructions about compiling and installing twin,
see sections 3 and 4 of the file [docs/Tutorial](docs/Tutorial) 

For the impatient, it basically reduces to
```
  ./configure
  make
```
then run as root
```
  make install
```
on Linux, also remember to run as root:
```
  ldconfig
```
on FreeBSD instead, remember to run as root:
```
  ldconfig -R
```

To compile twin you need the following programs installed
on your system:
	
  * a Bourne-shell or compatible (for example bash, dash, ash...)
  
  * make (most variants are supported: GNU make, BSD make...)

  * an ANSI C compiler (for example gcc or clang)


Note: it is STRONGLY recommended to install at least the following packages before compiling twin
(the exact names depend on the operating system or Linux distribution):

  * x11-dev      - may be named x11-devel, libx11-dev ...
  * xpm-dev      - may be named xpm-devel, libxpm-dev ...
  * ncurses-dev  - may be named ncurses-devel, libncurses-dev ...
  * zlib-dev     - may be named zlib1g-dev, zlib-devel, libzlib-dev ...

On Linux, it is STRONGLY recommended to also install the following package before compiling twin:

  * gpm-dev      - may be named gpm-devel, libgpm-dev ...

For a discussion about MANUALLY configuring twin (almost never necessary),
see the file [docs/Configure](docs/Configure).
-- WARNING: if you manually enable options that were disabled by `./configure',
build will almost certainly fail! --

--------------------------------------------------------------
Other topics:

See the rest of the documentation, starting from the [Tutorial](docs/Tutorial)


Greetings,

Massimiliano Ghilardi

