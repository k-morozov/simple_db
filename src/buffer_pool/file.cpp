//
// Created by focus on 10/18/23.
//

#include "file.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <utils/utils.h>
#include <cassert>

namespace sdb::bp {

File::File(const std::filesystem::path &path, const int flags) {
	const int mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	/*
	 * O_DIRECT
	 * Try to minimize cache effects of the I/O to and from this file.
	 * This completely bypasses any kernel space buffers, but requires that the writes are
	 * the same size as the underlying storage block size (usually 512 bytes or 4k). By itself,
	 * it does not guarantee that the data is completely transferred to the device when the call returns.
	 * O_SYNC + O_DIRECT
	 * As stated above, we would need to use both options together guarantee true synchronous IO.
	 */
	fd = open(path.c_str(), O_DIRECT | O_SYNC | O_RDWR | flags, mode);
	if (fd == -1) {
		throw std::runtime_error("failed to create file");
	}
	size = discover_size();
}

File::~File() {
	if (fd>0) {
		close(fd);
	}
}

long File::discover_size() const {
	struct stat st{};
	if (fstat(fd, &st) == - 1) {
		throw std::invalid_argument("broken descriptor for fstat");
	}
	return static_cast<long>(st.st_size);
}


int File::get_fd() const {
	return fd;
}

PageIndex File::alloc_page() {
	const size_t page_index = size / PageSize;
	alloc();
	return page_index;
}

void File::alloc() {
	if (fallocate(fd, FALLOC_FL_ZERO_RANGE, size, PageSize) == 1) {
		throw std::invalid_argument("broken alloc for new page in file");
	}
	size += PageSize;
}

void File::write(sdb::bp::PageIndex index, const uint8_t *const data) {
	const long offset = index * PageSize;
	const size_t r = pwrite(fd, data, PageSize, offset);
	assert(r == PageSize);
	if (r == -1) {
		throw std::runtime_error("failed pwrite");
	}
}

void File::read(PageIndex index, uint8_t * data) {
	const long offset = index * PageSize;
	const size_t r = pread(fd, data, PageSize, offset);
	assert(r == PageSize);
	if (r == -1) {
		throw std::runtime_error("failed pread");
	}
}

FilePtr File::create_file(const std::filesystem::path &path) {
	return std::make_shared<utils::ConstructFromProtected<File>>(path, O_CREAT | O_EXCL);
}

FilePtr File::open_file(const std::filesystem::path &path) {
	return std::make_shared<utils::ConstructFromProtected<File>>(path, 0);
}

size_t File::get_page_count() const {
	// @TODO thinks
	return (size + PageSize - 1) / PageSize;
}

} // namespace sdb::bp