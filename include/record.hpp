#ifndef RECORD_HPP
#define RECORD_HPP

#include<algorithm>
#include<vector>
#include<string_view>
#include<boost/lexical_cast.hpp>
#include<dialect.hpp>
#include<boost/range/adaptor/reversed.hpp>

namespace turbo_csv{
    template<typename Dialect>
    class basic_record{
        std::vector<int>escape_char_pos;
        std::string raw_record;
        std::vector<std::string_view> fields;
        bool is_cached=false;

        public:

        /**
         * @brief Construct a new record object
         * 
         */
        basic_record():basic_record(""){

        }
        /**
         * @brief Construct a new record object
         * 
         * @param raw_rec Raw record data
         * @param escape_char_pos vector of all the escape character position in the record
         */
        basic_record(const std::string& raw_rec,std::vector<int> escape_char_pos):raw_record(raw_rec),escape_char_pos(std::move(escape_char_pos))
        {}

        /**
         * @brief Construct a new record object
         * 
         * @param raw_rec Raw record data
         * @note This constructor is to be used when there are no quotes in the record
         */
        basic_record(const std::string& raw_rec):basic_record(raw_rec,{}){}


        /**
         * @brief Get the raw size object
         * 
         * @return std::size_t size of new record
         */
        auto get_raw_size(){
            return raw_record.size();
        }

        /**
         * @brief Checks whether the record is empty or not
         * 
         * @return bool boolean value indicating whether the record is empty or not
         */
        auto is_empty(){
            return raw_record.empty();
        }
        /**
         * @brief returns the number of fields in the record
         * 
         * @return std::size_t count of the fields present in the record
         */
        auto get_field_count(){
       
            if(!is_cached && !raw_record.empty()){
                generate_metadata();
                is_cached=true;
            }
            return fields.size();
        }

        /**
         * @brief Returns the fields collection
         * 
         * @return const std::vector<std::string_view>& fields collection
         */
        const std::vector<std::string_view>& get_fields(){
            if(!is_cached && !raw_record.empty()){generate_metadata();is_cached=true;}
            return fields;
        }

        /**
         * @brief returns the field value at given field index (in string form)
         * 
         * @param field_index 
         * @return const std::string_view&  view into  the field value
         * @throw std::out_of_range if field_index is out of bounds
         */
        const std::string_view& operator[](int field_index)noexcept(false){
            if(!is_cached&& !raw_record.empty()){
                generate_metadata();
                is_cached=true;
            }
            return fields.at(field_index);

        }

        /**
         * @brief returns the field value deserialized to the appropriate type
         * 
         * @tparam T type to deserialize to
         * @param field_index index of field
         * @param trim_spaces indicates whether the leading and trailing spaces must be trimmed or not
         * @param trim_escape_char indicates whether the leading and trailing quotes must be trimmed or not
         * @return T deserialized value of type T
         */
        template<typename T>
        T get_field(int field_index,bool trim_spaces=false, bool trim_escape_char=false)noexcept(false){
            std::string_view field_view=(*this)[field_index];            
            if(trim_spaces){
                trim(field_view);
            }
            if(trim_escape_char){
                trim(field_view,Dialect::get_escapecharacter());
            }
            return boost::lexical_cast<T>(field_view);
        }

        private:

        void trim(std::string_view& str_view, char trim_char=' '){
            // Remove leading spaces
                str_view.remove_prefix(std::min(str_view.find_first_not_of(trim_char),str_view.size()));
                //Remove trailing spaces
                int count=0;
                for(auto character:boost::adaptors::reverse(str_view)){
                    if(character==trim_char)
                        count++;
                    else
                        break;
                }
                str_view.remove_suffix(count);
        }


        void generate_metadata(){
           std::string::iterator field_begin= raw_record.begin();
            std::string::iterator field_end= raw_record.begin();

            while(true){
                if(field_end==raw_record.end()){
                    fields.emplace_back(std::string_view(field_begin,field_end));
                    break;
                }
                if( !Dialect::is_fieldseperator(*field_end)){
                    *field_end++;
                }
                else{
                     if(seperator_escaped(field_end)){
                         *field_end++;
                     }
                     else{
                         fields.emplace_back(std::string_view(field_begin,field_end));
                         field_begin=++field_end;
                     }
                }
            }
        }
        bool seperator_escaped(std::string::iterator current_iter_pos){
            // No quotes means parse it normally
            if(escape_char_pos.empty()){return false;}

            auto char_pos= std::distance(raw_record.begin(),current_iter_pos);
            auto nearest_quote =std::lower_bound(escape_char_pos.begin(),escape_char_pos.end(),char_pos);
            if(nearest_quote==escape_char_pos.begin()){return false;}
            auto quote_count= std::distance(escape_char_pos.begin(),nearest_quote-1)+1;

            //If there are even number of quotes then the current seperator is not escaped
            if(quote_count%2==0){return false;}
            // Non even means its present within some quotes and escaped
            return true;
        }
    };
}



#endif