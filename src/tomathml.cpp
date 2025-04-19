#include "tomathml.h"

#include <iostream>
#include <sstream>

#include "cellmltext/parser.h"

namespace tomathml {

void printMessages(const CellMLText::Parser &parser, std::ostream& os)
{
    os << "Messages from parser (" << parser.messages().size() << ")" << std::endl;
    for (const auto& msg: parser.messages()) {
        os << "[" << msg.line() << ", " << msg.column() << "]: " << msg.message() << std::endl;
    }
}

std::string process(const std::string &text, bool cellml)
{
    auto parser = CellMLText::Parser();

    std::stringstream outstream;
    if (parser.execute(text, true, cellml)) {
        auto doc = parser.domDocument();
        doc->print(outstream);
    } else {
        printMessages(parser, outstream);
    }

    return outstream.str();
}

}
