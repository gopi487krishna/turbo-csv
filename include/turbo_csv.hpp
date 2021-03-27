#ifndef TURBO_CSV_HPP
#define TURBO_CSV_HPP

#include<deque>
#include<future>
#include<csv_file_reader.hpp>
#include<record.hpp>
#include<boost/range/iterator_range.hpp>
#include<unordered_map>

namespace turbo_csv {

    class reader {
        file_reader<1024> f_reader;
        std::deque<record<>> records;
        bool treat_first_record_as_header;
        std::unordered_map<std::string,std::size_t> header_record;

    private:

        inline static record<> empty_record{};

    public:

        reader(const std::string& fp, bool treat_first_record_as_header=false) :
        f_reader(fp),
        treat_first_record_as_header(treat_first_record_as_header) {
            if(treat_first_record_as_header){
                read_next_record();
                index_first_record();
            }
        }

        const record<>& next() {
            if (read_next_record()) {
                return records.back();
            }
            else {
                return empty_record;
            }
        }

        class iterator {
            std::int64_t index = 0;
            bool reached_end;
            reader& parent_reader;
        public:
            iterator(std::int64_t index, reader& parent_reader, bool reached_end = false) :
                index(index),
                parent_reader(parent_reader),
                reached_end(reached_end) {

            }

            record<>& operator *() {
                if (parent_reader.records.empty()) {
                    parent_reader.read_next_record();
                }
                return parent_reader.records.at(index);
            }

            void operator++() {
                ++index;
                if (index >= parent_reader.records.size()) {
                    if (!parent_reader.read_next_record()) { reached_end = true; }
                }
            }

            bool operator !=(const iterator& rhs) {

                if (std::addressof(parent_reader) == std::addressof(rhs.parent_reader)) {
                    if (reached_end && rhs.reached_end || index == rhs.index)
                        return false;
                }

                return true;

            }

        };

        auto begin() {
            return iterator{ 0,*this };
        }
        auto end() {
            return iterator{ -1,*this,true };
        }

        record<>& operator[](std::size_t index){
            //Get all the records till the index if not present
            while(index>=records.size()&&read_next_record());

            return records.at(index);
            
        }

        std::size_t get_active_recordcount(){
            return records.size();
        }

        template<typename T>
        std::vector<T> get_column(std::size_t column_index){
            // Read all the records as we need it
            while(read_next_record());

            // Should we skip over the first record?
            int offset=0;
            if(treat_first_record_as_header){offset=1;}

            std::vector<std::future<T>> deserialized_fields;
            std::vector<T>column_items;

            for(auto& rec: boost::make_iterator_range(records.begin()+offset,records.end())){
                deserialized_fields.push_back(std::async([&rec,column_index](){return rec.get_field<T>(column_index);}));
            }

            column_items.reserve(records.size());

            for(auto& deserialized_field:deserialized_fields){
                column_items.push_back(deserialized_field.get());
            }

            return column_items;

        }

        std::size_t get_indexof(const std::string& column_name){
            return header_record.at(column_name);
        }


    private:

        void index_first_record(){
            std::size_t index=0;
            for(auto& field: records.front().get_fields()){
                header_record.insert(std::make_pair(field,index));
                index++;
            }
        }

        bool read_next_record() {

            std::vector<int>dbl_quote_pos;
            std::string raw_rec;

            while (true) {

                auto byte = f_reader.get_byte();

                if (!byte.has_value() && raw_rec.empty()) { return false; }

                if (!byte.has_value()) {
                    records.push_back(record<>(raw_rec, dbl_quote_pos));
                    return true;
                }

                if (byte.value() == '\"') {
                    raw_rec.push_back(byte.value());
                    dbl_quote_pos.push_back(f_reader.get_current_readcount() - 1); // 0 based
                }

                else if (byte.value() == '\n') {
                    if (dbl_quote_pos.size() % 2 == 0) {
                        records.push_back(record<>(raw_rec, dbl_quote_pos));
                        return true;
                    }
                    else {
                        raw_rec.push_back(byte.value());
                    }
                }
                else {
                    raw_rec.push_back(byte.value());
                }
            }
        }

    };
}


#endif