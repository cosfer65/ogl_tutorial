#include "utils.h"

void cg_parser::tokenize(const std::string& line, const std::string& delim, str_array& tok_list)
{
    std::string tok;
    int l = (int)line.length();
    bool instr = false;

    for (int i = 0; i < l; ++i)
    {
        if (delim.find(line[i]) != std::string::npos)
        {
            if (instr)
            {
                tok_list.push_back(tok);
                // std::cout << "tok:" << tok << "\n";
                tok = "";
            }
            instr = false;
            continue;
        }
        else
        {
            tok += line[i];
            instr = true;
        }
    }
    if (instr)
    {
        tok_list.push_back(tok);
        tok = "";
    }
    tok_list.push_back(" ");
}

std::string file_extension(const std::string& fname) {
    std::size_t dot = fname.find_last_of(".");
    std::string ret = fname.substr(dot + 1);
    return ret;
}

void bslash_to_slash(std::string& str) {
    int l = str.length();
    for (int i = 0; i < l; ++i) {
        if (str[i] == '\\') {
            str[i] = '/';
        }
    }
}

std::string change_filename(const std::string& fpath, const std::string& fname) {
    std::string res(fpath);
    bslash_to_slash(res);

    auto found = res.find_last_of('/');
    if (found == std::string::npos)
        return fpath;
    res = res.substr(0, found + 1) + fname;
    return res;
}