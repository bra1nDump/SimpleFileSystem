#pragma once

#include <iostream>

//--------------------------< Debug macros >----------------------------------------
#ifdef DEBUG_MODE
	#define LOG(TO_PRINT) std::cout << TO_PRINT << std::endl;
#else
	#define LOG(TO_PRINT)
#endif
//----------------------------------------------------------------------------------

class CVirtualDisk {

	const unsigned int BLOCK_SIZE_IN_BYTES;
	const unsigned int DISK_SIZE_IN_BLOCKS;

	char** base_;

public:
//----------------------------------------------------------------------------------
	// Virtual disk initializer (Begin work)
	CVirtualDisk(int block_size, int disk_size):
	BLOCK_SIZE_IN_BYTES(block_size),
	DISK_SIZE_IN_BLOCKS(disk_size),
	base_(new char*[disk_size]) {
		LOG("Creating new virtual disk...")

		for (int i = 0; i < DISK_SIZE_IN_BLOCKS; ++i) {
			base_[i] = new char[BLOCK_SIZE_IN_BYTES];
		}
	}

	// Virtual disk destructor (End work)
	~CVirtualDisk() {
		LOG("Deleting virtual disk...")

		for (int i = 0; i < DISK_SIZE_IN_BLOCKS; ++i) {
			delete [] base_[i];
		}

		delete [] base_;
	}

//----------------------------------------------------------------------------------
	// Write #src_block to disk's block at index #index
	bool write_block(const char* src_block, int index) {
		LOG("Writing a new block to the virtual disk...")

		if (index >= DISK_SIZE_IN_BLOCKS) {
			return false;
		} else {
			std::memcpy(base_[index], src_block, BLOCK_SIZE_IN_BYTES);
			return true;
		}
	}

	// Read disk's block at index #index to #dst_block
	bool read_block(char* dst_block, int index) {
		LOG("Reading a block from the virtual disk...")

		if (index >= DISK_SIZE_IN_BLOCKS) {
			return false;
		} else {
			std::memcpy(dst_block, base_[index], BLOCK_SIZE_IN_BYTES);
			return true;
		}
	}
//----------------------------------------------------------------------------------
};

#undef LOG
