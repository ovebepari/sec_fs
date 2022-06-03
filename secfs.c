/*

  Copyright, Ove Bepari, Md. Sabbir Hossen, Kamrul Islam
  All rights reserved, 2022
  
  Source Code: https://github.com/ovebepari/sec_fs

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
      gcc secfs.c -o secfs `pkg-config fuse3 --cflags --libs`

  Mount Local Directory:
    ./secfs path-to-mountpoint

 */

/* 
 * FUSE version definition
 * Compulsory
 */
#define FUSE_USE_VERSION 31

/* 
 * Standard C Libraries
 * Cross Platform Header files
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

/* 
 * Posix Header Files and Libraries 
 * Only for Linux and Unix Machines 
 */
#include <unistd.h>
#include <sys/types.h>

/* 
 * FUSE API, C endpoint 
 */
#include <fuse.h>


// Keeping the directory lists and
// the current directory indexes
// in memory
char directories[256][256];
int directory_idx = -1;

// Same as directories, keeping them
// in memory
char files[256][256];
int file_idx = -1;

// Keeping file contents in memory
char file_contents[256][256];
int filecontent_idx = -1;


/**
 *
 * Simple Encryptiona and Decryption Algorithms
 * Rot13 Encryption algorithm, we can use any 
 * algorithms here, this is for demonstration 
 * purposes only
 *
 */
char* encrypt(char* str, int n){
	for (int i=0; i<n; i++){
		char c = str[i];
		if       (c >= 'a' && c <= 'm') c += 13;
        else if  (c >= 'A' && c <= 'M') c += 13;
        else if  (c >= 'n' && c <= 'z') c -= 13;
        else if  (c >= 'N' && c <= 'Z') c -= 13;
        str[i] = c;
	}
	return str;
}

char* decrypt(char* str, int n){
	for (int i=0; i<n; i++){
		char c = str[i];
		if       (c >= 'a' && c <= 'm') c += 13;
	    else if  (c >= 'A' && c <= 'M') c += 13;
	    else if  (c >= 'n' && c <= 'z') c -= 13;
	    else if  (c >= 'N' && c <= 'Z') c -= 13;
	    str[i] = c;
	}
	return str;	
}


/**
 *
 * Adding a new directory
 *
 * @param    dir_name: directory name
 * @return   void
 *
 */
void add_dir(const char * dir_name) {
    // Incrementing the directory index
    // so that we make a new directory
    // in a new index next time around
    directory_idx++;

    // directories[directory_idx] = dir_name;
    strcpy(directories[directory_idx], dir_name);
}

/**
 *
 * Checking if a path is a directory
 *
 * @param    path: diretory path
 * @return   int: 0 or 1
 *
 */
int is_dir(const char * path) {
    // Removing the first '/' in path
    path++;

    // Searching if the `path` is a directory
    // in our file system, returning true if
    // that is the case
    for (int curr_idx = 0; curr_idx <= directory_idx; curr_idx++)
        if (strcmp(path, directories[curr_idx]) == 0)
            return 1;

    return 0;
}

/**
 *
 * Adding a new file
 *
 * @param    filename: New file name
 * @return   void
 *
 */
void add_file(const char * filename) {
    // We can add filename naming constraints here
	
    file_idx++;
    strcpy(files[file_idx], filename);

    filecontent_idx++;
    strcpy(file_contents[filecontent_idx], "");
}

/**
 *
 * Checking if a path is a file
 *
 * @param    path: file path
 * @return   int: 0 or 1
 *
 */
int is_file(const char * path) {
    // Removing the first '/' in path
    path++;

    // Searching if the `path` is a file
    // in our file system, returning true:1 if
    // that is the case
    for (int curr_idx = 0; curr_idx <= file_idx; curr_idx++)
        if (strcmp(path, files[curr_idx]) == 0)
            return 1;

    return 0;
}

/**
 *
 * Getting the file index, if exists
 *
 * @param    path: file path
 * @return   int: index or -1
 *
 */
int get_file_index(const char * path) {
    // Removing the first '/' in path
    path++;

    // Searching if the `path` exists in memory
    // in our file system, returning true if
    // that is the case
    for (int curr_idx = 0; curr_idx <= file_idx; curr_idx++)
        if (strcmp(path, files[curr_idx]) == 0)
            return curr_idx;

    return -1;
}

/**
 *
 * Writing to the file
 *
 * @param    path: file path
 * @param    content: file contents
 * @return   void
 *
 */
void write_to_file(const char * path, const char * content, size_t size) {
    int file_idx = get_file_index(path);

    if (file_idx == -1) // No such file
        return;

    char arr[size+1];
    strcpy(arr, content);

    // We do encryption before writing
    strcpy(file_contents[file_idx], encrypt(arr, size));
}

/**
 *
 * Get file/direcotry attributes
 *
 * @param    path: file/diretory path
 * @param    st: file/diretory system stat structure
 * @param    fi: fuse_file_info structure
 * @return   int: 0 or 1
 *
 */
static int serve_getattr(const char * path, struct stat * st,
    					 struct fuse_file_info * fi) {
	/**
	 *
	 * File Attributes
	 *
	 * getuid = get user id
	 * getgid = get group id
	 * atime = file access time
	 * mtime = file modification time
	 */
    st -> st_uid = getuid(); 
    st -> st_gid = getgid(); 
    st -> st_atime = time(NULL); 
    st -> st_mtime = time(NULL); 

    if (strcmp(path, "/") == 0 || is_dir(path) == 1) {
        st -> st_mode = S_IFDIR | 0755;
        // Two hardlinks: http://unix.stackexchange.com/a/101536
        st -> st_nlink = 2; 
    } else if (is_file(path) == 1) {
    	//  u   g   o
    	// rwx rwx rwx
    	//  6   4   4
    	// 110 100 100
        st -> st_mode = S_IFREG | 0644;
        st -> st_nlink = 1;
        st -> st_size = 1024;
    } else {
        return -ENOENT;
    }

    return 0;
}

/**
 *
 * Readdir implementaion
 *
 * @param    path: file path
 * @param    buffer: stores dir content llists
 * @return   int
 *
 */
static int serve_readdir(const char * path, void * buffer, fuse_fill_dir_t filler,
    					 off_t offset, struct fuse_file_info * fi, 
    					 enum fuse_readdir_flags flags) {

	 // Current Directory
    filler(buffer, ".", NULL, 0, 0);
    // Parent Directory
    filler(buffer, "..", NULL, 0, 0);

    // If the user is trying to show the files/directories of the root directory
    // show the following
    if (strcmp(path, "/") == 0)
    {
        for (int curr_idx = 0; curr_idx <= directory_idx; curr_idx++)
            filler(buffer, directories[curr_idx], NULL, 0, 0);

        for (int curr_idx = 0; curr_idx <= file_idx; curr_idx++)
            filler(buffer, files[curr_idx], NULL, 0, 0);
    }

    return 0;
}

/**
 *
 * Read file implementaion
 *
 * @param    path: file path
 * @param    buffer: stores dir content llists
 * @return   int
 *
 */
static int serve_read(const char * path, char * buffer, size_t size, 
					  off_t offset, struct fuse_file_info * fi) {

    int file_idx = get_file_index(path);

    if (file_idx == -1)
        return -1;

    char * content = decrypt(file_contents[file_idx], size);

    memcpy(buffer, content + offset, size);

    return strlen(content) - offset;
}

/**
 *
 * Make Directory implementaion
 *
 * @param    path: file path
 * @param    buffer: stores dir content llists
 * @return   int
 *
 */
static int serve_mkdir(const char * path, mode_t mode) {
    path++;
    add_dir(path);

    return 0;
}

/**
 *
 * Make new file implementaion
 *
 * @param    path: file path
 * @param    buffer: stores dir content llists
 * @return   int
 *
 */
static int serve_mknod(const char * path, mode_t mode, dev_t rdev) {
    path++;
    add_file(path);

    return 0;
}

/**
 *
 * Write new file implementaion
 *
 * @param    path: file path
 * @param    buffer: stores dir content llists
 * @return   int
 *
 */
static int serve_write(const char * path,
    const char * buffer, size_t size, off_t offset, struct fuse_file_info * info) {
    write_to_file(path, buffer, size);

    return size;
}


/*
 * FUSE API mapping
 */
static struct fuse_operations operations = {
    .getattr = serve_getattr,
    .readdir = serve_readdir,
    .read = serve_read,
    .mkdir = serve_mkdir,
    .mknod = serve_mknod,
    .write = serve_write,
};

int main(int argc, char * argv[]) {
    /*
     * FUSE API calling, argc = argumnet count
     * argv = argument values, an array of char*
     * address of operations used by FUSE
     */
    return fuse_main(argc, argv, & operations, NULL);
}
