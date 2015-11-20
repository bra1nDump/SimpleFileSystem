#include <stdlib.h>
#include <assert.h>

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>

#include "../CVirtualDisk/CVirtualDisk.cpp"
#include "../CFile/CFile.cpp"

//--------------------------< Debug macros >----------------------------------------
#ifdef FILE_SYSTEM_DEBUG
	#define LOG(TO_PRINT) std::cout << TO_PRINT << std::endl;
#else
	#define LOG(TO_PRINT)
#endif
//----------------------------------------------------------------------------------

class CFileSystem {

	typedef unsigned int file_key_t;

// Virtual disk configurations
	const unsigned int BLOCK_SIZE_IN_BYTES;
	const unsigned int DISK_SIZE_IN_BLOCKS;

// Memory management
	CVirtualDisk       disk_;
	std::vector<bool>  free_blocks_ids_;
	unsigned int       next_free_block_id_;

// File managment
	std::map<file_key_t, CFile*> files_;
	std::set<file_key_t>         reserved_file_keys_;
	unsigned int                 next_free_key_;

public:
//----------------------------------------------------------------------------------
	CFileSystem(int block_size = 256, int disk_size = 1024):
	BLOCK_SIZE_IN_BYTES(block_size),
	DISK_SIZE_IN_BLOCKS(disk_size),
	free_blocks_ids_(disk_size, true),
	disk_(block_size, disk_size) {
		LOG("Initializing file system...")

		next_free_block_id_ = 0;
		next_free_key_      = 0;

		LOG("File system initialized successfully!\n")
	}

	~CFileSystem() {
		LOG("Destroying file system...")

		for (auto file: files_) {
			delete file.second;
		}

		LOG("File system destroyed successfully!\n")
	}

//----------------------------------------------------------------------------------
	file_key_t create_file() {
		LOG("Will create new file in file system...")

		file_key_t new_key = create_unique_file_key();
		CFile* new_file = new CFile(&disk_, new_key, BLOCK_SIZE_IN_BYTES);

		files_.insert(std::pair<file_key_t, CFile*>(new_key, new_file));

		return new_key;
	}

	bool delete_file(file_key_t key) {
		LOG("Will delete file from file system...")

		if (has_file_for_key(key) == false) {
			LOG("File not found!")
			return false;
		}

		CFile* file_to_delete = files_.at(key);

		for (auto block_id: file_to_delete->used_blocks()) {
			LOG("- block " << block_id)
			assert(free_blocks_ids_[block_id] == false);

			if (block_id < next_free_block_id_) {
				next_free_block_id_ = block_id;
			}

			free_blocks_ids_[block_id] = true;
		}

		delete file_to_delete;
		files_.erase(key);

		if (key < next_free_key_) {
			next_free_key_ = key;
		}
		reserved_file_keys_.erase(key);

		return true;
	}

//----------------------------------------------------------------------------------
	bool cursor_begin(file_key_t key) {
		if (has_file_for_key(key) == false) {
			LOG("File not found!")
			return false;
		}
		CFile* file = files_.at(key);

		file->set_cursor_begin();

		return true;
	}

	bool cursor_end(file_key_t key) {
		if (has_file_for_key(key) == false) {
			LOG("File not found!")
			return false;
		}
		CFile* file = files_.at(key);

		file->set_cursor_end();

		return true;
	}

//----------------------------------------------------------------------------------
	bool read_line(file_key_t key) {
		if (has_file_for_key(key) == false) {
			LOG("File not found!")
			return false;
		}
		CFile* file = files_.at(key);

		file->read_line();

		return true;
	}

//----------------------------------------------------------------------------------
	bool can_add_line_to_file(file_key_t key) {
		return has_file_for_key(key);
	}

	bool add_line_to_file(file_key_t key, char* new_line) {
		LOG("Adding line to file with id: " << key)

		if (has_file_for_key(key) == false) {
			LOG("File not found!")
			return false;
		}
		CFile* dst_file = files_.at(key);

		int bytes_to_add  = std::strlen(new_line) - dst_file->free_space();
		int blocks_to_add = 0;
		if (bytes_to_add > 0) {
			blocks_to_add = (bytes_to_add / BLOCK_SIZE_IN_BYTES) + 1;
		}

		for (int i = 0; i < blocks_to_add; ++i) {
			int new_block_id = reserve_block();
			dst_file->add_block(new_block_id);
		}

		dst_file->add_line(new_line);

		return true;
	}

//----------------------------------------------------------------------------------
	void dump() {
		std::cout << std::endl;
		std::cout << "----------< file system DUMP >----------"    << std::endl;
		std::cout << "Virtual disk configurations:"                << std::endl;
		std::cout << "BLOCK_SIZE_IN_BYTES " << BLOCK_SIZE_IN_BYTES << std::endl;
		std::cout << "DISK_SIZE_IN_BLOCKS " << DISK_SIZE_IN_BLOCKS << std::endl;
		std::cout << std::endl;
		std::cout << "Memory management:"                          << std::endl;
		std::cout << "Free blocks ids:"                            << std::endl;
		for (auto free: free_blocks_ids_) {
			std::cout << free;
		}
		std::cout << std::endl;
		std::cout << "Next free block id: " << next_free_block_id_ << std::endl;
		std::cout << std::endl;
		std::cout << "File managment:"      << std::endl           << std::endl;
		for (auto map_el: files_) {
			std::cout << map_el.first;
		}
		for (auto file: files_) {
			file.second->dump();
		}
		std::cout << "-------< end of file system DUMP >------"    << std::endl;
		std::cout << std::endl;
	}

	bool file_dump(file_key_t key) {
		if (has_file_for_key(key) == false) {
			LOG("File not found!")
			return false;
		}

		CFile* file = files_.at(key);
		file->dump();

		return true;
	}

private:
//----------------------------------------------------------------------------------
	int reserve_block() {
		int block_id = next_free_block_id_;

		int id = next_free_block_id_ + 1;
		for (; id < DISK_SIZE_IN_BLOCKS; ++id) {
			if (free_blocks_ids_[id] == true) {
				next_free_block_id_ = id;
				break;
			}
		}
		if (id == DISK_SIZE_IN_BLOCKS) {
			LOG("Out of memory! Shutting down the file system")
			assert(0);
		}

		free_blocks_ids_[block_id] = false;

		return block_id;
	}

	bool has_file_for_key(file_key_t key) {
		if (files_.find(key) != files_.end()) {
			return true;
		} else {
			return false;
		}
	}

	file_key_t create_unique_file_key() {
		LOG("Getting a unique key for file...")

		while (true) {
			if (reserved_file_keys_.find(next_free_key_) == reserved_file_keys_.end()) {
				reserved_file_keys_.insert(next_free_key_);

				return next_free_key_++;
			} else {
				next_free_key_++;
			}
		}
	}
//----------------------------------------------------------------------------------
};

#undef LOG
