#pragma once
#include<filesystem>
#include<string>
#include<fstream>
#include<optional>

namespace turbo_csv {
	class adapted_fstream {

		std::fstream file;
		std::filesystem::path file_path;

	public:
		/**
		 * @brief Construct a new adapted fstream object
		 * 
		 * @param path_to_file path of the file to be opened
		 */
		adapted_fstream(const std::string& path_to_file) :file_path(path_to_file) {
			file.open(path_to_file);
		}

		/**
		 * @brief Checks if the file is open for reading
		 * 
		 * @return true File is open for reading
		 * @return false File is not opened/could not be opened for reading
		 */
		bool is_open() {
			return file.is_open();
		}

		/**
		 * @brief Returns the size of csv file supplied
		 * 
		 * @return std::size_t size of the supplied file in bytes 
		 */
		auto get_filesize() {
			return std::filesystem::file_size(file_path);
		}

		/**
		 * @brief Get the total number of characters read until now
		 * 
		 * @return std::size_t Total number of characters read till now 
		 */
		auto get_current_readcount() {
			return static_cast<std::size_t>(file.tellg()) + 1;
		}

		/**
		 * @brief Get the byte at current location in file
		 * 
		 * @return std::optional<std::uint8_t> Returns the byte read or nullopt if no more data could be read
		 */
		std::optional<std::uint8_t> get_byte() {
			auto element = file.get();
			if (element == EOF) { return {}; }
			else { return element; }

		}

	};
}