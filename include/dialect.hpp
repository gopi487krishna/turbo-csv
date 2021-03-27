#ifndef DIALECT_HPP
#define DIALECT_HPP

#include<vector>
#include<algorithm>

namespace turbo_csv {
    class dialect {
        inline static char record_seperator = '\n';
        inline static char field_seperator = ',';
        inline static char escape_character = '\"';
        inline static std::vector<char> ignore_characters = { '\t' };
    public:

        static auto get_recordseperator() {
            return record_seperator;
        }
        static auto get_fieldseperator() {
            return field_seperator;
        }
        static const auto& get_ignore_characters() {
            return ignore_characters;
        }
        static auto get_escapecharacter() {
            return escape_character;
        }

        static auto is_recordseperator(char character) {
            return character == record_seperator;
        }
        static auto is_fieldseperator(char character) {
            return character == field_seperator;
        }

        static auto is_ignorecharacter(char character) {
            auto member_status = std::find(ignore_characters.begin(), ignore_characters.end(), character);
            return member_status != ignore_characters.end();
        }

        static auto is_escapecharacter(char character) {
            return escape_character == character;
        }

    };
}

#endif