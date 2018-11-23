# libarchive-qt
A Qt based archiving solution with libarchive backend

This is a simple archiving library for Qt.
Currently it supports the following archive/compression formats

 - Gzip
 - BZip2
 - LZMA2
 - LZip
 - LZ4
 - Cpio
 - AR
 - ISO9660
 - PAX
 - Shar
 - Zip
 - 7Zip
 - Tar

This is a library that I whipped up in a few hours and hence may contain bugs. Please open an `Issue` in `github` if you find one.
If you know the fix, please contribute.

Several other filters such as grzip, lrzip are supported by the libarchive, but not by this library.
If and when I find time and knowledge to add those filters I shall do so.

Those who know to add these and other filters to this library, are welcome to do so. :)

## My System Info
 - OS:					Debian Sid
 - Qt:					Qt5 5.11.2
 - LibArchive:			3.2.2
 - LibLzma: 			5.2.2
 - LibBz2:				1.0.6
 - zlib:				1.2.11
 - liblz (lzip):		1.10
 - lzop (binary):		1.03
