#include "../CVirtualDisk/CVirtualDisk.cpp"

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

		std::cout << "Line from file: " << line << std::endl;
		std::cout << line.length() << std::endl;

 		delete [] block_buffer;

 		return line.length();
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

		std::cout << "Line from file: " << line << std::endl;
		std::cout << line.length() << std::endl;

 		delete [] block_buffer;

 		return line.length();
	}

//----------------------------------------------------------------------------------
	int free_space() {
		return info_.free_space;
	}

//----------------------------------------------------------------------------------
	void dump() {
		std::cout << std::endl;
		std::cout << "-------------< file DUMP >--------------"    << std::endl;
		std::cout << "-------------< file info >--------------"    << std::endl;
		std::cout << "file_id          " << info_.file_id          << std::endl;
 		std::cout << "number_of_blocks " << info_.number_of_blocks << std::endl;
		std::cout << "free_space       " << info_.free_space       << std::endl;
		std::cout << "cursor_block     " << info_.cursor_block     << std::endl;
		std::cout << "cursor_pos       " << info_.cursor_pos       << std::endl;
		std::cout << "end_block        " << info_.end_block        << std::endl;
		std::cout << "end_pos          " << info_.end_pos          << std::endl;

		std::cout << "block_ids_:      " << block_ids_.size()      << std::endl;
		for (int& id: block_ids_) {
			std::cout << id << " ";
		}
		std::cout << std::endl;
		std::cout << "-------------< end of info >------------"    << std::endl;
		std::cout << "-------------< file content >-----------"    << std::endl;

		char* buffer = new char[BLOCK_SIZE_IN_BYTES];
		for (int i = 0; i < info_.number_of_blocks; ++i) {
			disk_ptr_->read_block(buffer, block_ids_[i]);
			for (int j = 0; j < BLOCK_SIZE_IN_BYTES; ++j) {
				printf("%c", buffer[j]);
			}
			std::cout << std::endl;
		}
		delete [] buffer;
		std::cout << std::endl;
		std::cout << "-------------< end of content >---------"    << std::endl;
		std::cout << "-------------< end of file DUMP >-------"    << std::endl;
		std::cout << std::endl;
	}
};

#undef LOG
