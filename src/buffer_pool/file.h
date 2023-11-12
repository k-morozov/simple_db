//
// Created by focus on 10/18/23.
//

#pragma once

#include <filesystem>
#include <buffer_pool/fwd.h>

namespace sdb::bp {

class File;
using FilePtr = std::shared_ptr<File>;

class File {
public:
	static FilePtr create_file(const std::filesystem::path& path);
	static FilePtr open_file(const std::filesystem::path& path);

	~File();

	FileFD get_fd() const;

	PageIndex alloc_page();

	size_t get_page_count() const;

	void write(PageIndex page, const uint8_t* const data);

	void read(PageIndex page, uint8_t* data);

protected:
	explicit File(const std::filesystem::path& path, int flags);

private:
	FileFD fd = -1;
	long size{};

	long discover_size() const;

	void alloc();
};


} // namespace sdb::bp
