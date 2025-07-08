//
// Created by Bradley Pearce on 20/02/2024.
//

#ifndef BLIBS_READ_LINES_H
#define BLIBS_READ_LINES_H

#include <vector>
#include <iostream>
#include <fstream>

namespace TextFileParser {

    enum ParserStatus {
        NO_PARSER_STATUS,
        SUCCESSFUL,
        FILE_NOT_READ_CORRECTLY,
    };

    struct ParserOutput {
        ParserStatus status = NO_PARSER_STATUS;
        std::shared_ptr<std::vector<std::string>> output{};
    };

    ParserOutput read_lines(std::string &filename) {
        // Prepare the output
        ParserOutput out;
        // Setup the output string.
        // INFO: we could not allocate anything here, but allocating the empty vector
        // stops a segv in the event you dereference the pointer.
        out.output = std::make_shared<std::vector<std::string>>();

        std::ifstream file(filename);

        if (file.fail() || !file.is_open() || !file) {
            out.status = FILE_NOT_READ_CORRECTLY;
            return out;
        }

        std::string buffer;
        while (std::getline(file, buffer)) {
            out.output->push_back(buffer);
        }
        out.status = SUCCESSFUL;
        return out;
    }

    const char* ws = " \t\n\r\f\v";
    // trim from end of string (right)
    inline std::string& rtrim(std::string& s, const char* t = ws)
    {
        s.erase(s.find_last_not_of(t) + 1);
        return s;
    }

    // trim from beginning of string (left)
    inline std::string& ltrim(std::string& s, const char* t = ws)
    {
        s.erase(0, s.find_first_not_of(t));
        return s;
    }

    // trim from both ends of string (right then left)
    inline std::string& trim(std::string& s, const char* t = ws)
    {
        return ltrim(rtrim(s, t), t);
    }

}




#endif //BLIBS_READ_LINES_H
