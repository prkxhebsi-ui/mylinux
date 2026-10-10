MyLinux Clang 21 Integration 

This directory contains source code and documentation for integrating Clang 21 into the MyLinux development toolchain.

Directories include/ — compiler builtin and architecture intrinsic header integration. parsers/ — source code for parsing MyLinux configuration formats, including RC, mount configuration, and Blueprint (.bp) files. linker/ — linker-related source code and integration interfaces. arch/ — architecture-specific utilities and interfaces. tools/ — compiler-support utilities and source-analysis tools. Source-only policy 

Keep authored C, C++, assembly, header, configuration, and documentation files in version control. Generated binaries, object files, kernel modules, and compiled device-tree blobs do not belong in this source tree.

Compatibility 

Clang builtin headers are not a replacement for a C library, Linux kernel headers, or a complete kernel build system. Architecture-specific features must be guarded and compiled for the correct target.

Status 

Experimental. Components are added and validated incrementally.

