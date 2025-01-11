#pragma once

#include <vector>
#include <string>

typedef std::vector<std::string> str_array;
class cg_parser {
    str_array tokens;
public:
    cg_parser() {}
    ~cg_parser() {}

    void tokenize(const std::string& line, const std::string& delim, str_array& tok_list);
};

std::string file_extension(const std::string& fname);
std::string change_filename(const std::string& fpath, const std::string& fname);
