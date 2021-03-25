#ifndef RECORD_HPP
#define RECORD_HPP

#include<algorithm>
#include<vector>
#include<string_view>
#include<boost/lexical_cast.hpp>
#include<boost/range/adaptor/reversed.hpp>

namespace turbo_csv{
    template<char seperator=','>
    class record{
        std::vector<int>quote_pos;
        std::string raw_record;
        std::vector<std::string_view> fields;
        bool is_cached=false;

        public:
        /**
         * @brief Construct a new record object
         * 
         * @param raw_rec Raw record data
         * @param quotes_position vector of all the quotes position in the record
         */
        record(const std::string& raw_rec,std::vector<int> quotes_position):raw_record(raw_rec),quote_pos(std::move(quotes_position))
        {}

        /**
         * @brief Construct a new record object
         * 
         * @param raw_rec Raw record data
         * @note This constructor is to be used when there are no quotes in the record
         */
        record(const std::string& raw_rec):record(raw_rec,{}){}


        /**
         * @brief Get the raw size object
         * 
         * @return std::size_t size of new record
         */
        auto get_raw_size(){
            return raw_record.size();
        }

        /**
         * @brief returns the number of fields in the record
         * 
         * @return std::size_t count of the fields present in the record
         */
        auto get_field_count(){
       
            if(!is_cached){
                generate_metadata();
                is_cached=true;
            }
            return fields.size();
        }

        /**
         * @brief returns the field value at given field index (in string form)
         * 
         * @param field_index 
         * @return const std::string_view&  view into  the field value
         * @throw std::out_of_range if field_index is out of bounds
         */
        const std::string_view& operator[](int field_index)noexcept(false){
            if(!is_cached){
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
         * @param trim_dbl_quotes indicates whether the leading and trailing quotes must be trimmed or not
         * @return T deserialized value of type T
         */
        template<typename T>
        T get_field(int field_index,bool trim_spaces=false, bool trim_dbl_quotes=false)noexcept(false){
            std::string_view field_view=(*this)[field_index];            
            if(trim_spaces){
                trim(field_view);
            }
            if(trim_dbl_quotes){
                trim(field_view,'\"');
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
                if( *field_end!=seperator ){
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
            if(quote_pos.empty()){return false;}

            auto char_pos= std::distance(raw_record.begin(),current_iter_pos);
            auto nearest_quote =std::lower_bound(quote_pos.begin(),quote_pos.end(),char_pos);
            if(nearest_quote==quote_pos.begin()){return false;}
            auto quote_count= std::distance(quote_pos.begin(),nearest_quote-1)+1;

            //If there are even number of quotes then the current seperator is not escaped
            if(quote_count%2==0){return false;}
            // Non even means its present within some quotes and escaped
            return true;
        }
    };
}



#endif