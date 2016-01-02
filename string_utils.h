#pragma once
#include <string>
#include <vector>

// trim from left
inline std::string& ltrim(std::string& s, const char* t = " \t\n\r\f\v")
{
    s.erase(0, s.find_first_not_of(t));
    return s;
}

// trim from right
inline std::string& rtrim(std::string& s, const char* t = " \t\n\r\f\v")
{
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

// trim from left & right
inline std::string& trim(std::string& s, const char* t = " \t\n\r\f\v")
{
    return ltrim(rtrim(s, t), t);
}

template <typename D>
std::vector<std::string> split_string(D delimiter, std::string str)
{
    std::vector<std::string> fields;
    std::string field;
    size_t start = 0;
    size_t end = std::string::npos;
    do {
        end = str.find(delimiter, start);
        field = str.substr(start, end - start);
        fields.push_back(field);
        start = end + 1;
    } while (end != std::string::npos);
    return fields;
}

void find_and_replace(std::string& source, std::string const& find, std::string const& replace);
