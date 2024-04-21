/*
 * Copyright (C) 2003  Serge van den Boom
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 * Nota bene: later versions of the GNU General Public License do not apply
 * to this program.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "iointrn.h"
#include "fileblock.h"
#include "uioport.h"

#include <errno.h>

static uio_FileBlock *uio_FileBlock_new(uio_Handle *handle, int flags,
		off_t offset, size_t blockSize, char *buffer, size_t bufSize);
static inline uio_FileBlock *uio_FileBlock_alloc(void);
static void uio_FileBlock_free(uio_FileBlock *block);


uio_FileBlock *
uio_openFileBlock(uio_Handle *handle) {
	// TODO: if mmap support is available, and it is available natively
	//       for the filesystem (make some sort of sysctl for filesystems
	//       to check this?), use mmap.
	//       mmap the entire file if it's small enough.
	//       N.B. Keep in mind streams of which the size is not known in
	//       advance.
	struct stat statBuf;
	if (uio_fstat(handle, &statBuf) == -1) {
		// errno is set
		return NULL;
	}
	uio_Handle_ref(handle);
	return uio_FileBlock_new(handle, 0, 0, 0, NULL, 0);
}

uio_FileBlock *
uio_openFileBlock2(uio_Handle *handle, off_t offset, size_t size) {
	// TODO: mmap (see uio_openFileBlock)

	// TODO: check if offset and size are acceptable.
	//       Need to handle streams of which the size is unknown.
#if 0
	if (uio_stat(handle, &statBuf) == -1) {
		// errno is set
		return NULL;
	}
	if (statBuf.st_size > 
#endif
	uio_Handle_ref(handle);
	return uio_FileBlock_new(handle, 0, offset, size, NULL, 0);
}

// block remains usable until the next call to uio_accessFileBlock
// with the same block as argument, or until uio_closeFileBlock with
// that block as argument.
ssize_t
uio_accessFileBlock(uio_FileBlock *block, off_t offset, size_t length,
		char **buffer) {
	if (block->flags & uio_FB_USE_MMAP) {
		// TODO
		errno = ENOSYS;
		return -1;
	} else {
		// TODO: add read-ahead buffering
		ssize_t numRead;
        size_t allocSize = length;
        int cachehit = 1;
	
        if (allocSize < block->blockSize){
            allocSize = block->blockSize;
        }

        if (offset < block->bufOffset){
            /* Complete miss */
            cachehit = 0;
        } else if (offset + length > block->bufOffset + block->bufSize) {
            /* End of request is beyond end of cache */
            cachehit = 0;
        }

		if (block->buffer != NULL && allocSize > block->blockSize) {
			uio_free(block->buffer);
			block->buffer = NULL;
		}
		if (block->buffer == NULL)
		{
			block->buffer = uio_malloc(allocSize);
			if (block->buffer == NULL)
			{
                errno = ENOSYS;
                return -1;
			}
			block->blockSize = allocSize;
			block->bufSize = 0;
			block->bufOffset = 0;
		}

        if (!cachehit)
        {
		// TODO: lock handle
		if (uio_lseek(block->handle, block->offset + offset, SEEK_SET) ==
				(off_t) -1) {
			// errno is set
			return -1;
		}
		
            numRead = uio_read(block->handle, block->buffer, allocSize);
		if (numRead == -1) {
                block->bufSize = 0;
			// errno is set
			// TODO: unlock handle
			return -1;
		}
            block->bufSize = numRead;
            block->bufOffset = offset;
		// TODO: unlock handle
		*buffer = block->buffer;
        } else {
            *buffer = &block->buffer[offset-block->bufOffset];
            numRead = block->bufSize-(offset-block->bufOffset);
        }
        if (numRead > length)
            numRead = length;
		return numRead;
	}
}

int
uio_copyFileBlock(uio_FileBlock *block, off_t offset, char *buffer,
		size_t length) {
	if (block->flags & uio_FB_USE_MMAP) {
		// TODO
		errno = ENOSYS;
		return -1;
	} else {
		ssize_t numRead;

		// TODO: lock handle
		if (uio_lseek(block->handle, block->offset + offset, SEEK_SET) ==
				(off_t) -1) {
			// errno is set
			return -1;
		}
		
		numRead = uio_read(block->handle, buffer, length);
		if (numRead == -1) {
			// errno is set
			// TODO: unlock handle
			return -1;
		}

		// TODO: unlock handle
		return numRead;
	}
}

int
uio_closeFileBlock(uio_FileBlock *block) {
	if (block->flags & uio_FB_USE_MMAP) {
#if 0
		if (block->buffer != NULL)
			uio_mmunmap(block->buffer);
#endif
	} else {
		if (block->buffer != NULL)
			uio_free(block->buffer);
	}
	uio_Handle_unref(block->handle);
	uio_FileBlock_free(block);
	return 0;
}

#define MAX_FB_SIZE  (65*1024)

// caller should uio_Handle_ref(handle) (unless it doesn't need it's own
// reference anymore).
static uio_FileBlock *
uio_FileBlock_new(uio_Handle *handle, int flags, off_t offset,
		size_t blockSize, char *buffer, size_t bufSize) {
	uio_FileBlock *result;
	
#if 0
	if (blockSize > MAX_FB_SIZE)
        blockSize = MAX_FB_SIZE;
#endif

	result = uio_FileBlock_alloc();
	result->handle = handle;
	result->flags = flags;
	result->offset = offset;
	result->blockSize = blockSize;
	result->buffer = buffer;
	result->bufSize = 0;
	result->bufOffset = 0;
	return result;
}

static inline uio_FileBlock *
uio_FileBlock_alloc(void) {
	return uio_malloc(sizeof (uio_FileBlock));
}

static void
uio_FileBlock_free(uio_FileBlock *block) {
	uio_free(block);
}


