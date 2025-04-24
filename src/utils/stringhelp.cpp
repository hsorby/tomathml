
#include "stringhelp.h"

#include <algorithm>

namespace utils {

std::string toUpper(const std::string &input)
{
    std::string result = input;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
    return result;
}

std::string left(const std::string &input, std::size_t n)
{
    return input.substr(0, n);
}

std::string right(const std::string &input, std::size_t n)
{
    if (n >= input.size()) return input;
    return input.substr(input.size() - n);
}

std::string specials(const std::string &pString)
{
    // Check whether the given string is one of the Specials (Unicode block)
    // Note: see the customised representation of the Specials (Unicode block)
    //       in the constructor...

    static const std::string IAA = "\xef\xbf\xb9";
    static const std::string IAS = "\xef\xbf\xba";
    static const std::string IAT = "\xef\xbf\xbb";
    static const std::string OBJ = "\xef\xbf\xbc";

    if (pString == IAA) {
        return "IAA";
    }

    if (pString == IAS) {
        return "IAS";
    }

    if (pString == IAT) {
        return "IAT";
    }

    if (pString == OBJ) {
        return "OBJ";
    }

    return pString;
}

void replaceAll(std::string &str, const std::string &from, const std::string &to)
{
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Advance past the replacement
    }
}


}
