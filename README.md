### sec_fs
In Memory, encryption based, every read-write operation encryption-decryption based file system

Copyright, Ove Bepari, Md. Sabbir Hossen, Kamrul Islam
All rights reserved, 2022

This is a C implementation of File System in User Space
Depends on: 
  - GCC
  - Meson
  - Ninja
  - Pytest
  - pkg-config
  - FUSE3 Libraries

Successfuly Tested on:
  - Ubuntu 22.04 LTS, 64 bit OS
  - CPU: AMD® Ryzen 5 3400g with radeon vega graphics × 8 

Compile with:
   - gcc secfs.c -o secfs `pkg-config fuse3 --cflags --libs`

Mount Local Directory:
  - ./secfs path-to-mountpoint
