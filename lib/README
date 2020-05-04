# libarchive-qt
A Qt based archiving solution with libarchive backend

This is a simple archiving library for Qt mainly based on libarchive.
Currently it supports the following archive/compression formats

 - Gzip
 - BZip2
 - LZMA2
 - LZip (needs lzlib or lzip binary)
 - LZ4
 - Cpio
 - AR
 - ISO9660
 - PAX
 - Shar
 - Zip
 - 7Zip
 - Tar

Following formats have read (extraction) support have limited support using extrnal binary
 - LZip
 - LZop
 - LrZip

Please open an `Issue` in `github` if you find bugs. If you know the fix, please contribute.

Several other filters such as compress, grzip, rpm, zstd are supported by the libarchive, but not by this library. Also, several archive formats
like XAR, LHA, CAB, RAR and a few more at not yet supported by this library, though the backend exists in libarchive. If and when I find time
and knowledge to add those filters, I shall do so.

Those who know to add these and other filters to this library, are welcome to do so. :)

## Dependencies (Version in my system: Debian Sid)
 - Qt5                  (Qt5 5.12.5)
 - libarchive           (3.4.0)
 - liblzma              (5.2.4)
 - libbz2               (1.0.8)
 - zlib                 (1.2.11)
 - lzlib (liblz1)       (1.11)
 - lzop (binary)        (1.04)
 - lzip (binary)        (1.21)
 - lrzip (binary)       (0.631)

An minimal illustration of how to use this library can be found in example.cpp.
It compiles to a tiny stand-alone archiver capable of creating, extracting and listing archives.

A simple implementation of this library in a GUI project can be found [here](https://gitlab.com/cubocore/corearchiver)
