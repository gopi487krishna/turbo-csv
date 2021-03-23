#define BOOST_TEST_MODULE file_reader_test
#include<fstream>
#include<filesystem>
#include<optional>
#include<mutex>
#include<thread>
#include<algorithm>
#include<atomic>

#include <default_file_reader.hpp>
#include <boost/test/unit_test.hpp>


auto examples_dir(){
    return std::string(EXAMPLES);
}


template<std::uint64_t buffer_size=10>
class file_reader{

    std::fstream file;
    std::filesystem::path file_path;

    char ibuf1[buffer_size];
    char ibuf2[buffer_size];

    std::mutex ibuf1_resx;
    std::mutex ibuf2_resx;

    bool ibuf1_consumed=false;
    bool ibuf2_consumed=false;


    char* buf_ptr;
    char current_active='A';

    std::atomic_bool stop_thread_1 = false;

public:
    /**
     * @brief Construct a new file reader object
     *
     * @param path_to_file An absolute or relative path that points to the location of file
     * @throw std::fstream::faliure If an irrecoverable stream error occured or I/O operation failed
     */
    file_reader(const std::string &path_to_file) noexcept(false):file_path(path_to_file),buf_ptr(ibuf1) {
        file.exceptions(std::fstream::failbit|std::fstream::badbit);
        file.open(path_to_file);
       
        initialize();
        ibuf1_resx.lock();
        std::thread populator([this](){this->populate_buffer();});
        populator.detach();
    }

    ~file_reader(){
        //Stop the thread if it was still processing
        stop_thread_1.store(true);
    }
    /**
     * @brief Checks if the file is open for reading or not
     *
     * @return true File is open for reading
     * @return false File is not open for reading
     */
    bool is_open(){
        return file.is_open();
    }

    /**
     * @brief Gets the file size in bytes
     *
     * @return std::uintmax_t
     */
    auto get_filesize(){
        return std::filesystem::file_size(file_path);
    }

    /**
     * @brief Get the byte at the current location in the file
     *
     * @return std::optional<std::uint8_t> Current byte pointed inside the file/nullopt if reading completed
     */
    std::optional<std::uint8_t> get_byte(){
        std::cerr<<"---GK---:"<<"GET_BYTE_ENTERED";
        if(*buf_ptr!='$' && *buf_ptr!='#'){
            return *buf_ptr++;
        }

        if(*buf_ptr=='#'){
            ibuf1_resx.unlock();
            ibuf2_resx.unlock();
            return {};
        }

        if(current_active=='A' && *buf_ptr=='$'){
            ibuf1_consumed=true;
            ibuf1_resx.unlock();
            while(ibuf2_consumed);
            current_active='B';
            buf_ptr=ibuf2;
            ibuf2_resx.lock();
            if(*buf_ptr=='#'){
                return {};
            }
            else{
                return *buf_ptr++;
            }

        } else{
            ibuf2_consumed=true;
            ibuf2_resx.unlock();
            while(ibuf1_consumed);
            current_active='A';
            buf_ptr=ibuf1;
            ibuf1_resx.lock();
           if(*buf_ptr=='#'){
                return {};
            }
            else{
                return *buf_ptr++;
            }
        }

    }

    void populate_buffer(){
        std::cerr<<"---GK---:"<<"POPULATE_BUFFER_ENTERED";
        while(true){
            if(stop_thread_1.load()){
                ibuf1_resx.unlock();
                ibuf2_resx.unlock();
                return;
            }
            if(ibuf1_consumed){
                std::lock_guard<std::mutex>lck(ibuf1_resx);
                 auto buf_state=fill_buffer(ibuf1);
                 if(!buf_state){
                     std::fill(ibuf1,ibuf1+1,'#');
                     ibuf1_consumed=false;
                     return;
                 }
                ibuf1_consumed=false;
            }
            else if(ibuf2_consumed){
                std::lock_guard<std::mutex>lck(ibuf2_resx);
                auto buf_state=fill_buffer(ibuf2);
                if(!buf_state){
                    std::fill(ibuf2,ibuf2+1,'#');
                    ibuf2_consumed=false;
                    return;
                }
                ibuf2_consumed=false;
            }

        }

    }


private:

    /**
     * @brief Fills the buffer with data from file
     *
     * @param buf Buffer to be filled
     * @return true If the data was filled successfully into buffer
     * @return false If No data is left to be read
     */
    bool fill_buffer(char* buf)noexcept{
        std::memset(buf,'#',buffer_size);
        auto remaining_bytes= get_filesize()-file.tellg();

        if(remaining_bytes==0){return false;} // Nothing left to fill buf

        if(remaining_bytes<=buffer_size-1){
            file.read(buf,remaining_bytes);
            buf[remaining_bytes]='$';
        }
        else{
            file.read(buf,buffer_size-1);
            buf[buffer_size-1]='$';
        }

        return true;

    }
    void initialize()noexcept{
        fill_buffer(ibuf1);
        fill_buffer(ibuf2);
    }

};




BOOST_AUTO_TEST_SUITE(file_reader_ctor)

BOOST_AUTO_TEST_CASE(open_file_if_it_exists){
    file_reader<> reader(examples_dir()+"cars.csv");
    BOOST_REQUIRE(reader.is_open());

}
BOOST_AUTO_TEST_CASE(throw_if_file_not_exists){

    BOOST_REQUIRE_THROW(file_reader<>(examples_dir()+"meow.csv"),std::fstream::failure);

}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(file_reader_methods)

BOOST_AUTO_TEST_CASE(is_open){
    file_reader<> my_reader(examples_dir()+"cars.csv");

    BOOST_REQUIRE(my_reader.is_open());
}

BOOST_AUTO_TEST_CASE(file_size){
    file_reader<> my_reader(examples_dir()+"cars.csv");

    BOOST_REQUIRE_EQUAL(my_reader.get_filesize(),58);
}


BOOST_AUTO_TEST_CASE(get_method_small_file){
    std::cerr<<"---GK---:"<<"GET_METHOD_ENTERED";
    file_reader<> my_reader(examples_dir()+"cars.csv");
    std::string cars_data="2014,Ford,Fiesta Classic,1.6\n2020,Maruti Suzuki,Brezza,1.6";
    std::string data;

    while(true){
        auto byte= my_reader.get_byte();
        if(byte.has_value()){data.push_back(byte.value());}
        else{break;}
    }

    BOOST_REQUIRE_EQUAL(cars_data,data);
}


BOOST_AUTO_TEST_SUITE_END()