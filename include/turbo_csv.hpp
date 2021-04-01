#ifndef TURBO_CSV_HPP
#define TURBO_CSV_HPP

#include<deque>
#include<future>
#include<record.hpp>
#include<boost/range/iterator_range.hpp>
#include<unordered_map>
#include<turbo_parser.hpp>
#include<csv_file_reader.hpp>
#include<fstream_adaptor.hpp>
#include<dialect.hpp>

namespace turbo_csv {
    template<template<class,class>class Parser,class FileReader,class Dialect>
    class basic_reader {
        Parser<FileReader,Dialect> csv_parser;
        std::deque<basic_record<Dialect>> records;
       
        bool treat_first_record_as_header;
        std::unordered_map<std::string,std::size_t> header_record;

        inline static basic_record<Dialect> empty_record{};

    public:

        /**
         * @brief Construct a new basic reader object
         * 
         * @param fp path to the csv file
         * @param treat_first_record_as_header exclude first record from processing? 
         */
        basic_reader(const std::string& fp, bool treat_first_record_as_header=false) :
        csv_parser(fp),
        treat_first_record_as_header(treat_first_record_as_header) {
            if(treat_first_record_as_header){
                read_next_record();
                index_first_record();
            }
        }

        /**
         * @brief Returns the next record from file
         * 
         * @return  basic_record<Dialect>&  lazy record containing the data
         */
         basic_record<Dialect>& next() {
            if (read_next_record()) {
                return records.back();
            }
            else {
                return empty_record;
            }
        }

        /**
         * @brief Iterator support for range-based for loops
         * 
         */
        class iterator {
            std::int64_t index = 0;
            bool reached_end;
            basic_reader& parent_reader;
        public:
            /**
             * @brief Construct a new iterator object
             * 
             * @param index index of record
             * @param parent_reader reader to which this iterator belongs.
             * @param reached_end Used for generating end iterator
             */
            iterator(std::int64_t index, basic_reader& parent_reader, bool reached_end = false) :
                index(index),
                parent_reader(parent_reader),
                reached_end(reached_end) {

            }

            /**
             * @brief Dereferences the iterator and gets the underlying record
             * 
             * @return basic_record<Dialect>& Record at the given iterator
             */
            basic_record<Dialect>& operator *() {
                if (parent_reader.records.empty()) {
                    parent_reader.read_next_record();
                }
                return parent_reader.records.at(index);
            }

            /**
             * @brief Points iterator to the next record
             * 
             */
            void operator++() {
                ++index;
                if (index >= parent_reader.records.size()) {
                    if (!parent_reader.read_next_record()) { reached_end = true; }
                }
            }

            /**
             * @brief Checks whether lhs !=rhs  iterator
             * 
             * @param rhs Iterator to compare to
             * @return true lhs!=rhs
             * @return false lhs=rhs
             */
            bool operator !=(const iterator& rhs) {

                if (std::addressof(parent_reader) == std::addressof(rhs.parent_reader)) {
                    if (reached_end && rhs.reached_end || index == rhs.index)
                        return false;
                }

                return true;

            }

        };

        /**
         * @brief Returns an iterator pointing to the beginning of records
         * 
         * @return iterator
         */
        auto begin() {
            return iterator{ 0,*this };
        }

        /**
         * @brief Returns an iterator depicting the end of records
         * 
         * @return iterator 
         */
        auto end() {
            return iterator{ -1,*this,true };
        }

        /**
         * @brief Returns the record at location 'index' in file
         * 
         * @param index location of the record in file
         * @return basic_record<Dialect>& 
         * @throw std::out_of_range if index> number of records available in file
         */
        basic_record<Dialect>& operator[](std::size_t index){
            //Get all the records till the index if not present
            while(index>=records.size()&&read_next_record());

            return records.at(index);
            
        }

        /**
         * @brief Gets the total number of records in a csv file
         * 
         * @return std::size_t total number of records in a csv file 
         */
        std::size_t get_totalrecords(){
            // Bring all the rows into memory ( This is not optimal on memory at all)
            while(read_next_record());
            return records.size();
        }


        /**
         * @brief Get the active records in the reader object
         * 
         * @return std::size_t Count indicating the number of active records
         */
        std::size_t get_active_recordcount(){
            return records.size();
        }

        /**
         * @brief gets the column values of specified column index
         * 
         * @tparam T Type to deseralize and return the values in
         * @param column_index column whose data needs to be obtained
         * @return std::vector<T> vector containing all the values in given column
         */

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
                deserialized_fields.push_back(std::async([&rec,column_index](){return rec. template get_field<T>(column_index,true,true);}));
            }

            column_items.reserve(records.size());

            for(auto& deserialized_field:deserialized_fields){
                column_items.push_back(deserialized_field.get());
            }

            return column_items;

        }

        /**
         * @brief Returns the index of the associated column
         * 
         * @param column_name name of the column whose index needs to be found
         * @return std::size_t index of the associated column
         */

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

            auto next_record= csv_parser.next();

            if(!next_record.is_empty()){
                records.push_back(next_record);
                return true;
            }

            return false;
        }

    };

    using reader=basic_reader<parser,adapted_fstream,dialect>;
    using experimental_reader=basic_reader<parser,file_reader<1000000>,dialect>;
}


#endif