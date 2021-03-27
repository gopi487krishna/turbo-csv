#ifndef CSV_FILE_READER_HPP
#define CSV_FILE_READER_HPP


#include<fstream>
#include<cstring>
#include<filesystem>
#include<optional>
#include<mutex>
#include<thread>
#include<algorithm>
#include<atomic>


namespace turbo_csv {
    template<std::uint64_t buffer_size = 10>
    class file_reader {

        std::thread producer_thread;

        std::fstream file;
        std::filesystem::path file_path;

        char ibuf1[buffer_size];
        char ibuf2[buffer_size];

        std::mutex ibuf1_resx;
        std::mutex ibuf2_resx;

        std::size_t current_read_count = 0;

        std::atomic_bool ibuf1_consumed = false;
        std::atomic_bool ibuf2_consumed = false;

        char current_active = 'A';
        char* buf_ptr;
        std::atomic_bool stop_producer = false;
    public:
        /**
         * @brief Construct a new file reader object
         *
         * @param path_to_file An absolute or relative path that points to the location of file
         * @throw std::fstream::faliure If an irrecoverable stream error occured or I/O operation failed
         */
        file_reader(const std::string& path_to_file) noexcept(false) :file_path(path_to_file), buf_ptr(ibuf1) {
            //Ensure that exceptions are thrown  if the file fails to open
            file.exceptions(std::fstream::failbit | std::fstream::badbit);
            file.open(path_to_file);

            //Initialize both the buffers with data for the first time and lock ibuf1 for use by get_byte
            initialize();
            ibuf1_resx.lock();

            // Call populate buffer on seperate thread that fills the buffer when it gets consumed
            std::thread populator([this] () {this->populate_buffer();});

            // Transfer ownership to the producer_thred(class object)
            producer_thread = std::move(populator);

        }

        ~file_reader() {
            stop_producer.store(true);
            producer_thread.join();
        }
        /**
         * @brief Checks if the file is open for reading or not
         *
         * @return true File is open for reading
         * @return false File is not open for reading
         */
        bool is_open() {
            return file.is_open();
        }

        /**
         * @brief Gets the file size in bytes
         *
         * @return std::uintmax_t
         */
        auto get_filesize() {
            return std::filesystem::file_size(file_path);
        }

        /**
         * @brief Returns the number of characters read
         *
         * @return std::size_t Number of characters read
         */
        auto get_current_readcount() {
            return current_read_count;
        }

        /**
         * @brief Get the byte at the current location in the file
         *
         * @return std::optional<std::uint8_t> Current byte pointed inside the file/nullopt if reading completed
         */
        std::optional<std::uint8_t> get_byte() {
            if (*buf_ptr != '$' && *buf_ptr != '#') {
                current_read_count++;
                return *buf_ptr++;
            }

            if (*buf_ptr == '#') {
                ibuf1_resx.unlock();
                ibuf2_resx.unlock();
                return {};
            }

            if (current_active == 'A' && *buf_ptr == '$') {
                switch_buffer('B');
                current_read_count++;
                return *buf_ptr++;
            }
            else {
                switch_buffer('A');
                current_read_count++;
                return *buf_ptr++;
            }

        }

    private:
    
        void switch_buffer(char new_active) {

            switch (new_active) {

                case 'A':
                    ibuf2_consumed = true;
                    ibuf2_resx.unlock();
                    while (ibuf1_consumed);
                    buf_ptr = ibuf1;
                    ibuf1_resx.lock();
                    break;

                case 'B':
                    ibuf1_consumed = true;
                    ibuf1_resx.unlock();
                    while (ibuf2_consumed);
                    buf_ptr = ibuf2;
                    ibuf2_resx.lock();
                    break;

            }
            current_active = new_active;
        }
        void populate_buffer() {
            while (true) {
                if (stop_producer.load()) {
                    return;
                }
                if (ibuf1_consumed) {
                    std::lock_guard<std::mutex>lck(ibuf1_resx);
                    auto buf_state = fill_buffer(ibuf1);
                    if (!buf_state) {
                        std::fill(ibuf1, ibuf1 + 1, '#');
                        ibuf1_consumed = false;
                        return;
                    }
                    ibuf1_consumed = false;
                }
                else if (ibuf2_consumed) {
                    std::lock_guard<std::mutex>lck(ibuf2_resx);
                    auto buf_state = fill_buffer(ibuf2);
                    if (!buf_state) {
                        std::fill(ibuf2, ibuf2 + 1, '#');
                        ibuf2_consumed = false;
                        return;
                    }
                    ibuf2_consumed = false;
                }

            }

        }



        /**
         * @brief Fills the buffer with data from file
         *
         * @param buf Buffer to be filled
         * @return true If the data was filled successfully into buffer
         * @return false If No data is left to be read
         */
        bool fill_buffer(char* buf)noexcept {

            auto remaining_bytes = get_filesize() - file.tellg();

            if (remaining_bytes == 0) { return false; } // Nothing left to fill buf

            if (remaining_bytes <= buffer_size - 1) {
                file.read(buf, remaining_bytes);
                buf[remaining_bytes] = '#';
            }
            else {
                file.read(buf, buffer_size - 1);
                buf[buffer_size - 1] = '$';
            }

            return true;

        }
        void initialize()noexcept {
            fill_buffer(ibuf1);
            fill_buffer(ibuf2);
        }

    };

}




#endif