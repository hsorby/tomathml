
#pragma once

#include <string>

namespace utils {

std::string toUpper(const std::string &input);
std::string left(const std::string &input, std::size_t n);
std::string right(const std::string &input, std::size_t n);
std::string specials(const std::string &pString);
void replaceAll(std::string &str, const std::string &from, const std::string &to);

}
