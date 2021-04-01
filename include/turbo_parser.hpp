#ifndef TURBO_PARSER_HPP
#define TURBO_PARSER_HPP

#include<record.hpp>

namespace turbo_csv{
    template<typename FileReader, typename Dialect>
    class parser{
        // Handles the disk reading part
        FileReader file_reader;
        public:

        /**
         * @brief Constructs a new parser instance that will parse from the beginning of the file
         * 
         * @param filename  name of the file to be opened
         */
        parser(const std::string&  filename):file_reader(filename){}

        /**
         * @brief Constructs a new parser instance that will parse from the position specified in file reader
         * 
         * @param file_reader Reference to an file reader object
         */
        parser(FileReader& file_reader):file_reader(file_reader){}

        /**
         * @brief Returns the next record from the file
         * 
         * @return basic_record<Dialect> Record containing the current csv row
         */
        basic_record<Dialect> next(){

            // Represents the position of double quotes. This is used here as well as used in records
            // for parsing the record properly 
            std::vector<int>escape_pos;

            // Raw string representation of record
            std::string raw_record;

            while (true) {
                
                auto byte = file_reader.get_byte();

                // We are trying to read records even after reaching end of file
                if (!byte.has_value() && raw_record.empty()) { return basic_record<Dialect>(); }

                //We are at the last record ( it is not seperated by record seperator)
                if (!byte.has_value()) {
                    return basic_record<Dialect>(raw_record,escape_pos);
                }

                // If the current byte is an espace character push it simply but 
                // note down its position as well. This will be useful later for
                // record to find membership of a seperator in log(logN)
                if (Dialect::is_escapecharacter(byte.value())) {
                    raw_record.push_back(byte.value());
                    escape_pos.push_back(file_reader.get_current_readcount() - 1); // 0 based
                }

                // If the current byte is a record seperator example (\n,\r)
                // check its membership in escaped set i.e check whether it is inside double
                // quotes simply. This is constant time O(1) here because current state of escape
                // pos tells the number of escape_chars read. If those are 'even' it simply means
                // we are at the end of a csv record so we just return it otherwise if odd then 
                // the recordseperator is not a record seperator but a part of field of a csv record :)
                else if (Dialect::is_recordseperator(byte.value())) {
                    if (escape_pos.size() % 2 == 0) {
                        return basic_record<Dialect>(raw_record, escape_pos);
                    }
                    else {
                        raw_record.push_back(byte.value());
                    }
                }

                // Store the ignore characters(line feed, carriage return) if it belongs to the membership of
                //  escape set otherwise simply dump we dont need them
                else if(dialect::is_ignorecharacter(byte.value())){
                    if(escape_pos.size()%2!=0){
                        raw_record.push_back(byte.value());
                    }
                }

                // Normal character [a-z][0-9][everything not present in dialect set]
                else {
                    raw_record.push_back(byte.value());
                }
            }
        }


    };
}




#endif