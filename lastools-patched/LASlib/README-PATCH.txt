This README file is here to indicate that this version of LASlib has been patched with unofficial modifications to its Makefiles. These modifications fall under the same licensing conditions as the rest of the library, and are in no way intended to fork or discredit LASlib.

Building and installing LASlib in this way is required for other software to be built on LASlib and subsequently distributed to users who do not have the time or knowledge to compile and install LASlib on the level of compiling and linking individual files

The changes are as follows:
* LASlib now builds dynamically linkable libraries aswell as the static ones
* Libraries are now named liblaslib, to avoid conflict with the libLAS library on linux systems which need to support both (-llas refers to libLAS, -llaslib refers to this (LASlib))
* Versions of the library are notated by date, in the form YYYYMMDD, as LASlib has no official versioning system
* make now supports an install command for the laslib subdirectory, installing LASlib to a location specified in the Makefile, /usr/local by default.

For queries, please contact: arsf-processing@pml.ac.uk

LASlib (with LASzip) is a C++ programming API for reading / writing LIDAR
data stored in standard LAS or in compressed LAZ format (1.0 - 1.3). Both
libraries - LASlib with LASzip - are released together under the terms of
the GNU Lesser General Public Licence also known as LGPL. See LICENSE.txt
for details.

Internally LASlib includes my stand-alone LASzip compression library that
provides lossless compression for LAS data. This allows you to read or
write *.laz (LASzip compressed *.las files) directly like you would an
uncompressed .las file using the same LASreader and LASwriter classes.

This distribution comes with a small tool called "laszip" or "laszip.exe"
compresses LAS files into much more compact LAZ files and vice versa. The
resulting files are around 7 - 20 percent of their original size without
a single bit being different (i.e. absolutely lossless compression).

LASlib (with LASzip) is easy-to-use, light-weight, yet extremely fast. Just
download it here and you are ready to go. For Windows users "laszip.exe" and
other binary LAStools and a MSVC6.0 project environment are included. For
Linux users there are simple Makefiles.

The best way is get LASlib is to download LAStools, which contains the
full LASlib (with LASzip) distribution:

wget http://www.lastools.org/download/LAStools.zip
unzip LAStools.zip
cd LAStools/
make

--------------------

Here is how simple the programming API is:

#include "lasreader.hpp"
#include "laswriter.hpp"

int main(int argc, char *argv[])
{
  LASreadOpener lasreadopener;
  lasreadopener.set_file_name("original.las");
  LASreader* lasreader = lasreadopener.open();

  LASwriteOpener laswriteopener;
  laswriteopener.set_file_name("compressed.laz");
  LASwriter* laswriter = laswriteopener.open(&lasreader->header);

  while (lasreader->read_point()) laswriter->write_point(&lasreader->point);

  laswriter->close();
  delete laswriter;

  lasreader->close();
  delete lasreader;

  return 0;
}

--------------------

Many LAStools are "free" open source (LGPL). But some of the more powerful
LAStools (i.e. for contour extraction, boundary polygon computation, TIN
generation, DEM creation, thinning, clipping, tiling, sorting, ...) need
a license for commercial and government use. 

--------------------

(c) 2007-2014 martin.isenburg@rapidlasso.com
