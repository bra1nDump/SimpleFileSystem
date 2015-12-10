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
	unsigned int block_size_in_bytes_;
	unsigned int disk_size_in_blocks_;

// Memory management
	CVirtualDisk*     disk_ptr_;
	std::vector<bool> free_blocks_ids_;
	unsigned int      next_free_block_id_;

// File managment
	std::map<file_key_t, CFile*> files_;
	std::set<file_key_t>         reserved_file_keys_;
	file_key_t                   next_free_key_;

public:
//----------------------------------------------------------------------------------
	CFileSystem() {
		LOG("Initializing file system...")

		next_free_block_id_ = -1;

		disk_ptr_ = nullptr;

		LOG("File system initialized successfully!\n")
	}

	~CFileSystem() {
		LOG("Destroying file system...")

		for (auto file: files_) {
			delete file.second;
		}

		delete disk_ptr_;

		LOG("File system destroyed successfully!\n")
	}

	// 1
	int set_block_size(unsigned int size) {
		block_size_in_bytes_ = size;

		return 1;
	}

	// 2
	int start_with_disk_size(unsigned int size) {
		disk_size_in_blocks_ = size;

		if (block_size_in_bytes_ <= 0 || disk_size_in_blocks_ <= 0) {
			return 0;
		}

		disk_ptr_ = new CVirtualDisk(block_size_in_bytes_, disk_size_in_blocks_);
		free_blocks_ids_ = std::vector<bool>(disk_size_in_blocks_, true);
		next_free_block_id_ = 0;

		return 1;
	}

//----------------------------------------------------------------------------------
	// 3
	int creatable() {
		return can_reserve_blocks(1);
	}

	// 4
	file_key_t create_file() {
		LOG("Will create new file in file system...")

		file_key_t new_key = create_unique_file_key();
		CFile* new_file = new CFile(disk_ptr_, new_key, block_size_in_bytes_);

		files_.insert(std::pair<file_key_t, CFile*>(new_key, new_file));

		return new_key;
	}

	// 5
	int is_empty(file_key_t key) {
		if (has_file_for_key(key) == 0 || system_ready() == 0) {
			LOG("File not found!")
			return -1;
		}
		CFile* file = files_.at(key);

		return file->is_empty();
	}

	// 6
	int can_add_line(file_key_t key, const char* line) {
		if (has_file_for_key(key) == 0 || system_ready() == 0) {
			return 0;
		}

		int bytes_to_add  = std::strlen(line);
		int blocks_to_add = 0;
		if (bytes_to_add > 0) {
			blocks_to_add = (bytes_to_add / block_size_in_bytes_) + 1;
		}

		return can_reserve_blocks(blocks_to_add);
	}

//----------------------------------------------------------------------------------
	// 7
	int push_back_line(file_key_t key, const char* new_line) {
		LOG("Adding line to file with id: " << key)

		if (has_file_for_key(key) == 0 || system_ready() == 0) {
			LOG("File not found!")
			return -1;
		}
		CFile* dst_file = files_.at(key);

		if (!can_add_line(key, new_line)) {
			return 0;
		}

		int bytes_to_add  = std::strlen(new_line) - dst_file->free_space();
		int blocks_to_add = 0;
		if (bytes_to_add > 0) {
			blocks_to_add = (bytes_to_add / block_size_in_bytes_) + 1;
		}

		for (int i = 0; i < blocks_to_add; ++i) {
			int new_block_id = reserve_block();

			if (new_block_id == -1) {
				return 0;
			}

			dst_file->add_block(new_block_id);
		}

		dst_file->add_line(new_line);

		return true;
	}

//----------------------------------------------------------------------------------
	// 8
	int get_last_line(file_key_t key) {
		if (has_file_for_key(key) == 0 || system_ready() == 0) {
			LOG("File not found!")
			return -1;
		}
		CFile* file = files_.at(key);

		file->set_cursor_end();
		return file->read_backward();
	}

	// 9
	int at_begin(file_key_t key) {
		if (has_file_for_key(key) == 0 || system_ready() == 0) {
			LOG("File not found!")
			return -1;
		}
		CFile* file = files_.at(key);

		return file->at_begin();
	}

	// 10
	int at_end(file_key_t key) {
		if (has_file_for_key(key) == 0 || system_ready() == 0) {
			LOG("File not found!")
			return -1;
		}
		CFile* file = files_.at(key);

		return file->at_end();
	}

	// 11
	int set_cursor_to_begin(file_key_t key) {
		if (has_file_for_key(key) == 0 || system_ready() == 0) {
			LOG("File not found!")
			return 0;
		}
		CFile* file = files_.at(key);

		file->set_cursor_begin();

		return 1;
	}

	// 12
	int set_cursor_to_end(file_key_t key) {
		if (has_file_for_key(key) == 0 || system_ready() == 0) {
			LOG("File not found!")
			return 0;
		}
		CFile* file = files_.at(key);

		file->set_cursor_end();

		return 1;
	}

//----------------------------------------------------------------------------------
	// 13
	int read_forward(file_key_t key) {
		if (has_file_for_key(key) == 0 || system_ready() == 0) {
			LOG("File not found!")
			return 0;
		}
		CFile* file = files_.at(key);

		return file->read_forward();
	}

	// 14
	int read_bakward(file_key_t key) {
		if (has_file_for_key(key) == 0 || system_ready() == 0) {
			LOG("File not found!")
			return 0;
		}
		CFile* file = files_.at(key);

		return file->read_backward();
	}

	// 15
	int delete_file(file_key_t key) {
		LOG("Will delete file from file system...")

		if (has_file_for_key(key) == 0 || system_ready() == 0) {
			LOG("File not found!")
			return 0;
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

		return 1;
	}

	// 16
	int end_work() {
		block_size_in_bytes_ = -1;
		disk_size_in_blocks_ = -1;

		return 1;
	}

//----------------------------------------------------------------------------------
	void dump() {
		std::cout << std::endl;
		std::cout << "----------< file system DUMP >----------"    << std::endl;
		std::cout << "Virtual disk configurations:"                << std::endl;
		std::cout << "block_size_in_bytes_ " << block_size_in_bytes_ << std::endl;
		std::cout << "disk_size_in_blocks_ " << disk_size_in_blocks_ << std::endl;
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
		for (auto map_el: files_) {
			map_el.second->dump();
		}
		std::cout << "-------< end of file system DUMP >------"    << std::endl;
		std::cout << std::endl;
	}

	bool file_dump(file_key_t key) {
		if (has_file_for_key(key) == 0 || system_ready() == 0) {
			LOG("File not found!")
			return false;
		}

		CFile* file = files_.at(key);
		file->dump();

		return true;
	}

	int get_first_line(file_key_t key) {
		if (has_file_for_key(key) == 0 || system_ready() == 0) {
			LOG("File not found!")
			return 0;
		}
		CFile* file = files_.at(key);

		file->set_cursor_begin();
		return file->read_forward();
	}

private:
//----------------------------------------------------------------------------------
	int reserve_block() {
		int block_id = next_free_block_id_;

		int id = next_free_block_id_ + 1;
		for (; id < disk_size_in_blocks_; ++id) {
			if (free_blocks_ids_[id] == true) {
				next_free_block_id_ = id;
				break;
			}
			next_free_block_id_ = -1;
		}
		if (block_id == -1) {
			LOG("Out of memory!")
			return -1;
		}

		free_blocks_ids_[block_id] = false;

		return block_id;
	}

	int can_reserve_blocks(unsigned int number_to_reserve) {
		
		int total_free = 0;
		for (auto flag: free_blocks_ids_) {
			if (flag == true) {
				total_free++;
			}
		}

		return total_free >= number_to_reserve;
	}

	int system_ready() {
		if (disk_ptr_ == nullptr ||
			block_size_in_bytes_ == -1 ||
			disk_size_in_blocks_ == -1) {

				LOG("System is not ready!")
				return 0;
		}

		return 1;
	}

	int has_file_for_key(file_key_t key) {
		if (files_.find(key) != files_.end()) {
			return 1;
		} else {
			return 0;
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
