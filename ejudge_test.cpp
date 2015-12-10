#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <assert.h>

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


//--------------------------< Debug macros >----------------------------------------
#ifdef FILE_DEBUG
	#define LOG(TO_PRINT) std::cout << TO_PRINT << std::endl;
#else
	#define LOG(TO_PRINT)
#endif
//----------------------------------------------------------------------------------

class CFile {

	struct FileInfo {
		unsigned int file_id;

		int number_of_blocks;
		int free_space;

		int cursor_block;
		int cursor_pos;

		int end_block;
		int end_pos;

		FileInfo(unsigned int id) {
			file_id = id;

			number_of_blocks = 0;
			free_space       = 0;
			cursor_block     = 0;
			cursor_pos       = 0;
			end_block        = 0;
			end_pos          = 0;
		}
	};

	FileInfo info_;

	CVirtualDisk* disk_ptr_;
	unsigned int  BLOCK_SIZE_IN_BYTES;

	std::vector<int> block_ids_;

public:
//----------------------------------------------------------------------------------
	// Create file
	CFile(CVirtualDisk* disk_ptr, unsigned int id, unsigned int block_size):
	info_(id),
	disk_ptr_(disk_ptr),
	BLOCK_SIZE_IN_BYTES(block_size) {
		LOG("File created id: " << info_.file_id << std::endl);
	}

	// Destroy file
	~CFile() {
		LOG("File deleted id: " << info_.file_id << std::endl);
	}

	// Release used blocks
	std::vector<int> used_blocks() {
		LOG("Releasing blocks...");

		return block_ids_;
	}

//----------------------------------------------------------------------------------
	void add_block(int new_block_id) {
		LOG("+ block with id: " << new_block_id);

		info_.number_of_blocks += 1;
		info_.free_space       += BLOCK_SIZE_IN_BYTES;

		block_ids_.push_back(new_block_id);
	}

	void add_line(const char* new_line) {
		LOG("+ line: " << new_line);

		char* final_line = new char[info_.end_pos + std::strlen(new_line) + BLOCK_SIZE_IN_BYTES];

		// Append characters from end block of the file to the final line
		disk_ptr_->read_block(final_line, block_ids_[info_.end_block]);
		std::memcpy(final_line + info_.end_pos, new_line, std::strlen(new_line) + 1);

		LOG("Final line to write: " << final_line)

		// Writing final line to blocks
		int bytes_to_write = std::strlen(final_line);
		int current_pos    = 0;
		int index = info_.end_block;
		while (bytes_to_write > 0) {
			disk_ptr_->write_block(final_line + current_pos, block_ids_[index++]);

			bytes_to_write -= BLOCK_SIZE_IN_BYTES;
			current_pos    += BLOCK_SIZE_IN_BYTES;
		}

		info_.end_pos   = std::strlen(final_line) % BLOCK_SIZE_IN_BYTES;
		info_.end_block = (info_.end_pos == 0)? index : index - 1;
		info_.free_space -= std::strlen(new_line);

		delete [] final_line;
 	}

//----------------------------------------------------------------------------------
 	int is_empty() {
 		return (info_.end_block == 0 && info_.end_pos == 0);
 	}

 	int at_begin() {
 		return (info_.cursor_block == 0 && info_.cursor_pos == 0);
 	}

 	int at_end() {
 		return (info_.cursor_block == info_.end_block &&
 			info_.cursor_pos == info_.end_pos);
 	}

	int set_cursor_begin() {
		LOG("Setting cursor to the begining of the file");
		if (info_.number_of_blocks == 0) {
			return 0;
		}

		info_.cursor_block = 0;
		info_.cursor_pos   = 0;

		return 1;
 	}

	int set_cursor_end() {
		LOG("Setting cursor to the end of the file");
		if (info_.number_of_blocks == 0) {
			return 0;
		}

		info_.cursor_block = info_.end_block;
		info_.cursor_pos   = info_.end_pos;

		return 1;
	}

//---------------------------------------------------------------------------------
	int read_forward() {
		if (info_.number_of_blocks == 0) {
			LOG("Empty file!");
			return 0;
		} else if ((info_.cursor_block == info_.end_block) &&
			(info_.cursor_pos == info_.end_pos)) {
				LOG("EOF!");
				return 0;
		}

		std::string line;
		char* block_buffer = new char[BLOCK_SIZE_IN_BYTES];

		int block = info_.cursor_block;
		int pos   = info_.cursor_pos;
		for (; block <= info_.end_block; ++block) {
			disk_ptr_->read_block(block_buffer, block_ids_[block]);

			for (; pos < BLOCK_SIZE_IN_BYTES; ++pos) {
				if (block_buffer[pos] == '\n') {
					goto exit_loops;
				} else if (block_buffer[pos] == '\0') {
					assert(0);
				} else {
					line += block_buffer[pos];
				}
			}
			pos = 0;
		}
		exit_loops:

		if (pos == BLOCK_SIZE_IN_BYTES - 1) {
			info_.cursor_block = block + 1;
			info_.cursor_pos   = 0;
		} else {
			info_.cursor_block = block;
			info_.cursor_pos   = pos + 1;
		}

 		delete [] block_buffer;

 		return line.length() - 1;
	}

	int read_backward() {
		if (info_.number_of_blocks == 0) {
			LOG("Empty file!");
			return 0;
		} else if ((info_.cursor_block == 0) &&
			(info_.cursor_pos == 0)) {
				LOG("Can not read forward! Already in the begining of the file");
				return 0;
		}

		std::string line;
		char* block_buffer = new char[BLOCK_SIZE_IN_BYTES];

		int block = info_.cursor_block;
		int pos   = info_.cursor_pos;
		switch (pos) {
		case (0): {
			if (BLOCK_SIZE_IN_BYTES != 1) {
				block -= 1;
				pos   = BLOCK_SIZE_IN_BYTES - 2;
			} else {
				block -= 2;
				pos   = BLOCK_SIZE_IN_BYTES - 1;
			}
			break;
		}
		case (1): {
			block -= 1;
			pos   = BLOCK_SIZE_IN_BYTES - 1;
			break;
		}
		default: {
			pos -= 2;
			break;
		}
		}

		for (; block >= 0; --block) {
			disk_ptr_->read_block(block_buffer, block_ids_[block]);

			for (; pos >= 0; --pos) {
				if (block_buffer[pos] == '\n') {
					goto break_loops;
				} else {
					line += block_buffer[pos];
				}
			}
			pos = BLOCK_SIZE_IN_BYTES - 1;
		}
		break_loops:

		if (pos == BLOCK_SIZE_IN_BYTES - 1) {
			info_.cursor_block = block + 1;
			info_.cursor_pos   = 0;
		} else {
			info_.cursor_block = block;
			info_.cursor_pos   = pos + 1;
		}

 		delete [] block_buffer;

 		return line.length() - 1;
	}

//----------------------------------------------------------------------------------
	int free_space() {
		return info_.free_space;
	}
};

#undef LOG


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
		next_free_key_      = -1;

		disk_ptr_ = nullptr;

		LOG("File system initialized successfully!\n")
	}

	~CFileSystem() {
		LOG("Destroying file system...")

		if (disk_ptr_ != nullptr) {
			delete disk_ptr_;
		}

		for (auto file: files_) {
			delete file.second;
		}

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

		if (disk_ptr_ != nullptr) {
			delete disk_ptr_;
		}

		disk_ptr_ = new CVirtualDisk(block_size_in_bytes_, disk_size_in_blocks_);
		free_blocks_ids_ = std::vector<bool>(disk_size_in_blocks_, true);
		next_free_block_id_ = 0;
		next_free_key_      = 0;


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
	int read_backward(file_key_t key) {
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

		for (auto file: files_) {
			delete file.second;
		}
		files_.clear();

		if (disk_ptr_ != nullptr) {
			delete disk_ptr_;
			disk_ptr_ = nullptr;
		}

		block_size_in_bytes_ = -1;
		disk_size_in_blocks_ = -1;

		return 1;
	}

//----------------------------------------------------------------------------------
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



int   get_command_number(const char* base_);
int   to_int(const char* base_);
char* to_ptr(const char* base_);

void read_file_str_to_buffer(char* dest);

int main() {

	char buf[1024];

	int command_number;
	int result;

	CFileSystem test_system;

	do {
		scanf("%s", buf);
		command_number = get_command_number(buf);

		switch(command_number) {
			case (1): {
				scanf("%s", buf);
				int block_size = to_int(buf);

				result = test_system.set_block_size(block_size);
				break;
			}
			case (2): {
				scanf("%s", buf);
				int disk_size = to_int(buf);

				result = test_system.start_with_disk_size(disk_size);
				break;
			}
			case (3): {
				result = test_system.creatable();
				break;
			}
			case (4): {
				result = test_system.create_file();
				break;
			}
			case (5): {
				scanf("%s", buf);
				int file_key = to_int(buf);

				result = test_system.is_empty(file_key);
				break;
			}
			case (6): {
				scanf("%s", buf);
				int file_key = to_int(buf);
				read_file_str_to_buffer(buf);

				result = test_system.can_add_line(file_key, buf);
				break;
			}
			case (7): {
				scanf("%s", buf);
				int file_key = to_int(buf);
				read_file_str_to_buffer(buf);

				result = test_system.push_back_line(file_key, buf);
				break;
			}
			case (8): {
				scanf("%s", buf);
				int file_key = to_int(buf);
				scanf("%s", buf);
				char* ptr = to_ptr(buf);

				result = test_system.get_last_line(file_key);
				break;
			}
			case (9): {
				scanf("%s", buf);
				int file_key = to_int(buf);

				result = test_system.at_begin(file_key);
				break;
			}
			case (10): {
				scanf("%s", buf);
				int file_key = to_int(buf);

				result = test_system.at_end(file_key);
				break;
			}
			case (11): {
				scanf("%s", buf);
				int file_key = to_int(buf);

				result = test_system.set_cursor_to_begin(file_key);
				break;
			}
			case (12): {
				scanf("%s", buf);
				int file_key = to_int(buf);

				result = test_system.set_cursor_to_end(file_key);
				break;
			}
			case (13): {
				scanf("%s", buf);
				int file_key = to_int(buf);
				scanf("%s", buf);
				char* ptr = to_ptr(buf);

				result = test_system.read_forward(file_key);
				break;
			}
			case (14): {
				scanf("%s", buf);
				int file_key = to_int(buf);
				scanf("%s", buf);
				char* ptr = to_ptr(buf);

				result = test_system.read_backward(file_key);
				break;

				break;
			}
			case (15): {
				scanf("%s", buf);
				int file_key = to_int(buf);

				result = test_system.delete_file(file_key);
				break;
			}
			case (16): {
				result = test_system.end_work();
				goto exit_main_loop;
			}
			default: {
				std::cout << "Unknown command" << std::endl;
				goto exit_main_loop;
			}
		}

		std::cout.flush();

		//std::cout << "Command_number: " << command_number << std::endl;
		std::cout << result << std::endl;

	} while (*buf != '\0');
	exit_main_loop:

	return 0;
}

// heplp functions
int get_command_number(const char* base_) {
	if (std::strcmp(base_, "init") == 0) {
		return 1;
	} else if (std::strcmp(base_, "start") == 0) {
		return 2;
	} else if (std::strcmp(base_, "creatable") == 0) {
		return 3;
	} else if (std::strcmp(base_, "create") == 0) {
		return 4;
	} else if (std::strcmp(base_, "empty") == 0) {
		return 5;
	} else if (std::strcmp(base_, "addable") == 0) {
		return 6;
	} else if (std::strcmp(base_, "add") == 0) {
		return 7;
	} else if (std::strcmp(base_, "get") == 0) {
		return 8;
	} else if (std::strcmp(base_, "atbegin") == 0) {
		return 9;
	} else if (std::strcmp(base_, "atend") == 0) {
		return 10;
	} else if (std::strcmp(base_, "tobegin") == 0) {
		return 11;
	} else if (std::strcmp(base_, "toend") == 0) {
		return 12;
	} else if (std::strcmp(base_, "forward") == 0) {
		return 13;
	} else if (std::strcmp(base_, "backward") == 0) {
		return 14;
	} else if (std::strcmp(base_, "del") == 0) {
		return 15;
	} else if (std::strcmp(base_, "end") == 0) {
		return 16;
	}

	return -1;
}

int to_int(const char* base_) {

	int number = 0;
	int it = 0;
	while (*base_ != '\0') {
		number *= 10;
		number += base_[0] - '0';

		base_++;
	}

	return number;
}

char* to_ptr(const char* base_) {

	unsigned long ptr = 0;
	while (*base_ != '\0') {
		ptr *= 10;
		ptr += base_[0] - '0';

		base_++;
	}

	return ((char*) ptr);
}

void read_file_str_to_buffer(char* dest) {

	std::string input_str;
	std::getline(std::cin, input_str);

	int len = input_str.length();
	std::memcpy(dest, input_str.c_str(), len);
	dest[len] = '\n';
	dest[len + 1] = '\0';
}
