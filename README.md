# CornerFix

Fork of the [CornerFix][cornerfix] source to support running as an OSX command line utility and as an Aperture 3.5 plugin.

## Usage

### Install

```
git clone http://github.com/mattbierner/cornerfix cornerfix
cd cornerfix
git submodule update --init
```

### Other Dependancies 

* [DNG SDK][dng_sdk] - Uses a weird modified 1.3 version included as source.
  * [XMP SDK][xmp_sdk] - CS6 version, with one minor modification for missing API call. also included as source for now.
    * [zlib][zlib]
    * [expat][expat]  



# Original Readme

This contains the source code for CornerFix

(C) Copyright 2007-2012, by Sandy McGuffog and Contributors.
The DNG SDK and XMP SDK are (C) Copyright 2007 Adobe Corporation

All Rights reserved

This program is free software; you can redistribute it and/or modify it under the terms
of the GNU General Public License as published by the Free Software Foundation;
either version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this
library; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA 02111-1307, USA.


Notes:

1. This was built under VC Express 2008 for Windows, and XCode 4.4 for the Mac. The respective project files are included

2. In order to compile this, you need to download the Adobe DNG SDK 1.3, and recompile it

3. In order to recompile the DNG SDK, you will need to:

a) Download the Adobe XMP SDK 4.4, as well as the approporiate version of Expat. How to include Expat into the 
XMP SDK is documented by Adobe on www.adobe.com, and in the XMP SDK

b) You will need to modify both the DNG SDK and the XMP SDK. WinMerge DIFF files are included that show exact mods. 
However, broadly, the mods are as follows:

i. The XMP SDK is modified to compile somewhat cleanly under VC2008 -  Note however that when compiled for release, 
the following warnings will show:

dng_xmp_sdk.obj : warning LNK4248: unresolved typeref token (0100001B) for '__XMPMeta__'; image may not run
dng_xmp_sdk.obj : warning LNK4248: unresolved typeref token (01000020) for '__XMPIterator__'; image may not run

These do not show under debug mode, and are not harmful. They relate to VC2005 attempting to make sense of 
the XMP toolkit's use of declarations that are legal, but obscure. And not good programming practice.

In addition, various "Visibility different" warming may show. These relate to the XMP toolkit's use of
inline functions is a non-standard way, and can be ignored.

ii. The DNK SDK is modified to expose some previously protected image related data structures, and also such that 
printf's can be redirected to something more useful.

4. For Windows, the XMP SDK must be compiled to a static library, without common runtime support.

5. For Windows, CornerFix and the DNG SDK are compiled together, with common runtime support (/clr), aka in mixed mode. 
Then linked to the XMP static library

Enjoy..................



[cornerfix]: https://sites.google.com/site/cornerfix/
[dng_sdk]: https://www.adobe.com/support/downloads/dng/dng_sdk.html
[xmp_sdk]: http://www.adobe.com/devnet/xmp.html
[zlib]: http://www.zlib.net
[expat]: http://expat.sourceforge.net