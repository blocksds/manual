# Chapter 3
## How do I create programs?

### All About devkitPro

devkitPro is a brand, like Microsoft or Adobe. You don't use Nintendo DS
software anymore than you write your letters with Microsoft or edit photos with
Adobe. devkitPro produces a collection of toolchains for homebrew developers.
Toolchains are available for Game Boy Advance, GP32, Playstation Portable,
GameCube, and the Nintendo DS. The toolchain we are most interested in is known
as devkitARM.

devkitARM is a specific toolchain of devkitPro. It allows the compiling of ARM
binaries from most all computers. It is based on gcc, the GNU Compiler
Collection. devkitARM includes everything you'll need to create software for
the Nintendo DS, GBA, and GP32; all of which are run by the ARM processor.
However, we will be using something to make our job much easier in addition to
just devkitARM.

### All About BlocksDS

BlocksDS is an SDK created as a fork of most NDS devkitARM libraries as they
were at the beginning of 2023. It only consists of libraries used to build NDS
programs (libnds, DSWiFi, Maxmod, etc) and tools to convert graphics and audio
into formats understood by the NDS. The toolchain (compiler, C and C++ standard
libraries) are provided by Wonderful Toolchain: <https://wonderful.asie.pl/>

### The Wonderful World of libnds

libnds, the library for Nintendo DS, started out its life as NDSLIB. NDSLIB was
a simple library created by joat (Michael Noland) and dovoto (Jason Rogers).
The name was changed to libnds over the course of a few months and the
maintainer has been changed to WinterMute (Dave Murphy).

NDSLIB started out as a collection of defines for common memory locations in
the DS. This is useful, as you can simply reference `BG_BMP_RAM` instead of
`0x06000000`. Eventually, the library began to include structs and unions and
other useful constructs that help to simplify the programmers job and abstract
certain portions of the hardware from the programmer.

Later, even more high level utilities were added to simplify the interaction
with the Nintendo DS hardware. For example, `bgInit()` can now be used to setup
backgrounds instead of accessing low level registers.

Today, libnds is an incredibly useful library that most of the Nintendo DS
homebrew community uses.

### Installing BlocksDS

Installing BlocksDS is quite simple. Directions are already documented on
their website. Visit <https://blocksds.github.io/docs/setup/options/> for
directions. Although more geared towards Linux, the installation is fairly
straight forward. It's possible to install it natively on Windows and Linux, but
it can be run in other operating systems by using their Docker images.

It is also possible to find instructions on how to build and install the
libraries in the official setup instructions.

### The Next Step

Now that you have installed this on your computer, you have everything you need
to start coding, excepting perhaps a bit of knowledge on how to code
specifically for the unique hardware of the Nintendo DS. In the next chapter,
we'll cover the basics of displaying a bitmap on the screen.
