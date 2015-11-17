#include <list>

#include "../CVirtualDisk/CVirtualDisk.cpp"

//--------------------------< Debug macros >----------------------------------------
#ifdef DEBUG_MODE
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
			cursor_pos       = -1;
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
	/*
	// Copy constructer
	CFile(const CFile& other):
	disk_ptr_(other.disk_ptr_),
	BLOCK_SIZE_IN_BYTES(other.BLOCK_SIZE_IN_BYTES),
	block_ids_(other.block_ids_),
	info_(other.info_) {
	 	LOG("Copying file...")
	}
	*/

	// Create file
	CFile(CVirtualDisk* disk_ptr, unsigned int id, unsigned int block_size):
	disk_ptr_(disk_ptr),
	BLOCK_SIZE_IN_BYTES(block_size),
	info_(id) {
		LOG("Creating file...")
	}

	// Destroy file
	~CFile() {
		LOG("Destroying file...")
		
	}

	// Release used blocks
	std::vector<int> used_blocks() {
		LOG("Releasing blocks...")

		return block_ids_;
	}

//----------------------------------------------------------------------------------
	void add_block(unsigned int new_block_id) {

		info_.number_of_blocks += 1;
		info_.free_space       += BLOCK_SIZE_IN_BYTES;

		block_ids_.push_back(new_block_id);
	}

	void add_line(char* new_line) {
		LOG("Addind new line...")

		char* buffer     = new char[BLOCK_SIZE_IN_BYTES];
		char* final_line = new char[info_.end_pos + sizeof(new_line)];

		// Append characters from end block of the file to the final line
		disk_ptr_->read_block(final_line, info_.end_block);
		std::memcpy(final_line + info_.end_pos, new_line, sizeof(new_line));

		// Writing final line to blocks
		int bytes_to_write = sizeof(final_line);
		int current_pos    = 0;
		for (int index = info_.end_block; bytes_to_write > 0; ++index) {
			disk_ptr_->write_block(final_line + cursor_pos, block_ids_[index]);

			bytes_to_write -= BLOCK_SIZE_IN_BYTES;
			cursor_pos     += BLOCK_SIZE_IN_BYTES;
		}

		info_.end_block = index - 1;
		info_.end_pos   = std::strlen(final_line) % BLOCK_SIZE_IN_BYTES;

		delete [] buffer;
		delete [] final_line;
 	}

//----------------------------------------------------------------------------------
	int free_space() {
		return info_.free_space;
	}

//----------------------------------------------------------------------------------
	void dump() {
		std::cout << "-------------< file info >--------------"    << std::endl;
		std::cout << "file_id          " << info_.file_id          << std::endl;
 		std::cout << "number_of_blocks " << info_.number_of_blocks << std::endl;
		std::cout << "free_space       " << info_.free_space       << std::endl;
		std::cout << "cursor_block     " << info_.cursor_block     << std::endl;
		std::cout << "cursor_pos       " << info_.cursor_pos       << std::endl;
		std::cout << "end_block        " << info_.end_block        << std::endl;
		std::cout << "end_pos          " << info_.end_pos          << std::endl;

		std::cout << "block_ids_:      " << block_ids_.size()      << std::endl;
		for (auto id: block_ids_) {
			std::cout << id << std::endl;
		}
		std::cout << "-------------< end of info >------------"    << std::endl;
	}
//----------------------------------------------------------------------------------
};

#undef LOG
