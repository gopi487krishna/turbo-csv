#define BOOST_TEST_MODULE reader_test

#include<deque>
#include<csv_file_reader.hpp>
#include<record.hpp>

#include<boost/test/unit_test.hpp>

namespace turbo_csv{

    class reader{
        file_reader<1024> f_reader;
        std::deque<record<>> records;

        private:

        inline static record<> empty_record{};

        public:

        reader(const std::string& fp):f_reader(fp){}
        
        const record<>& next(){
            if(read_next_record()){
                return records.back();
            }
            else{
                return empty_record;
            }
        }
        
        class iterator{
            std::int64_t index=0;
            bool reached_end;
            reader& parent_reader;
            public:
            iterator(std::int64_t index,reader& parent_reader, bool reached_end=false):
                index(index),
                parent_reader(parent_reader),
                reached_end(reached_end)
                {
                    parent_reader.read_next_record();
                }
    
            record<>& operator *(){
               return parent_reader.records[index];
            }

            void operator++(){
                ++index;
                if(index>=parent_reader.records.size()){
                    if(!parent_reader.read_next_record()){reached_end=true;}
                }
            }

            bool operator !=(const iterator& rhs){

                if(std::addressof(parent_reader)==std::addressof(rhs.parent_reader)){
                    if(reached_end && rhs.reached_end || index==rhs.index)
                        return false;
                }

                return true;

            }

        };

        auto begin(){
            return iterator{0,*this};
        }
        auto end(){
            return iterator{-1,*this,true};
        }

        private:
        bool read_next_record(){

            std::vector<int>dbl_quote_pos;
            std::string raw_rec;

            while(true){
    
                auto byte= f_reader.get_byte();

                if(!byte.has_value()&&raw_rec.empty()){return false;}

                if(!byte.has_value()){
                    records.push_back(record<>(raw_rec,dbl_quote_pos));
                    return true;
                    }
        
                if(byte.value()=='\"'){
                    raw_rec.push_back(byte.value());
                    dbl_quote_pos.push_back(f_reader.get_current_readcount()-1); // 0 based
                }

                else if(byte.value()=='\n'){
                    if(dbl_quote_pos.size()%2==0){
                        records.push_back(record<>(raw_rec,dbl_quote_pos));
                        return true;
                    }
                    else{
                        raw_rec.push_back(byte.value());
                    }
                }
                else{
                    raw_rec.push_back(byte.value());
                }
            }
        }

    };
}

auto get_examples_dir(){
    return std::string(EXAMPLES);
}

BOOST_AUTO_TEST_SUITE(methods)


BOOST_AUTO_TEST_CASE(return_next_row){
    turbo_csv::reader csv_reader(get_examples_dir()+"cars.csv");

    auto first_record= csv_reader.next();

    BOOST_REQUIRE_EQUAL(2014,first_record.get_field<int>(0));
    BOOST_REQUIRE_EQUAL("Ford",first_record[1]);
    BOOST_REQUIRE_EQUAL("Fiesta Classic",first_record[2]);
    BOOST_REQUIRE_EQUAL("1.6",first_record[3]);  

}

BOOST_AUTO_TEST_CASE(range_based_for){
    turbo_csv::reader csv_reader(get_examples_dir()+"cars.csv");
    std::vector<int>expected_raw_size{28,29};
    std::vector<int>actual_raw_sizes;

    for(auto record:csv_reader){
        actual_raw_sizes.push_back(record.get_raw_size());
    }

    BOOST_REQUIRE_EQUAL_COLLECTIONS(
        expected_raw_size.begin(),expected_raw_size.end(),
        actual_raw_sizes.begin(),actual_raw_sizes.end()
    );
}


BOOST_AUTO_TEST_SUITE_END()