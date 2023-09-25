### Sec_fs
In Memory, every read-write operation encryption-decryption based lightweight file system for lightweight secure files (e.g. secret keys, passwords, public keys etc)

Built for the undergrad thesis at DUET, Gazipur
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
   - gcc secfs.c -o secfs \`pkg-config fuse3 --cflags --libs\`

Mount Local Directory:
  - ./secfs path-to-mountpoint

Example:
 Successful mount of our file system:
 ![Screenshot from 2022-05-31 16-24-49](https://user-images.githubusercontent.com/12424141/171152839-2925e4a3-3440-4995-9692-ffebf43968df.png)

If we run the `findmnt` to find mount points in our system, at the end, we can see our file system
 ![Screenshot from 2022-05-31 16-28-20](https://user-images.githubusercontent.com/12424141/171153602-eff35059-1ba6-4e29-9eec-8435ac61a7e7.png)

To Do:
  - Write data to disks
  - Optimized Encryption

Thanks!
