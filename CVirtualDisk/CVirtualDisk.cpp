#pragma once

#include <iostream>

//--------------------------< Debug macros >----------------------------------------
#ifdef VIRTUAL_DISK_DEBUG
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
	CVirtualDisk(int block_size, int disk_size):
	BLOCK_SIZE_IN_BYTES(block_size),
	DISK_SIZE_IN_BLOCKS(disk_size),
	base_(new char*[disk_size]) {
		LOG("Creating new virtual disk...");

		for (int i = 0; i < DISK_SIZE_IN_BLOCKS; ++i) {
			base_[i] = new char[BLOCK_SIZE_IN_BYTES];
		}
	}

	~CVirtualDisk() {
		LOG("Deleting virtual disk...");

		for (int i = 0; i < DISK_SIZE_IN_BLOCKS; ++i) {
			delete [] base_[i];
		}

		delete [] base_;
	}

//----------------------------------------------------------------------------------
	bool write_block(const char* src_block, int id) {
		LOG("Writing to block: " << id);

		if (id >= DISK_SIZE_IN_BLOCKS) {
			return false;
		} else {
			std::memcpy(base_[id], src_block, BLOCK_SIZE_IN_BYTES);
			return true;
		}
	}

	bool read_block(char* dst_block, int id) const {
		LOG("Reading from block: " << id);

		if (id >= DISK_SIZE_IN_BLOCKS) {
			return false;
		} else {
			std::memcpy(dst_block, base_[id], BLOCK_SIZE_IN_BYTES);
			return true;
		}
	}
//----------------------------------------------------------------------------------
};

#undef LOG
