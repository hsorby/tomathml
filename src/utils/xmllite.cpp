#include <iostream>
#include <memory>

#include "xmllite.h"

namespace utils {

XmlAttribute::XmlAttribute(const std::string& name, const std::string& value,
                 const std::string& nsPrefix)
        : mName(name), mValue(value), mNamespacePrefix(nsPrefix)
{
}

std::string XmlAttribute::toString() const
{
    std::string fullName = mNamespacePrefix.empty() ? mName : mNamespacePrefix + ":" + mName;
    return fullName + "=\"" + mValue + "\"";
}


XmlNode::XmlNode(XmlNodeType type, const std::string& name,
            const std::string& nsPrefix)
            : mType(type), mName(name), mNamespacePrefix(nsPrefix)
{
}

void XmlNode::addAttribute(const std::string& name, const std::string& value,
                  const std::string& nsPrefix) {
    mAttributes.emplace_back(name, value, nsPrefix);
}

void XmlNode::declareNamespace(const std::string& prefix, const std::string& uri) {
    std::string key = prefix.empty() ? "xmlns" : "xmlns:" + prefix;
    mAttributes.emplace_back(key, uri);
}

void XmlNode::addChild(XmlNodePtr child) {
    mChildren.push_back(child);
}

void XmlNode::print(std::ostream& os, int indent) const {
    std::string indentStr(indent, ' ');
    std::string tagName = mNamespacePrefix.empty() ? mName : mNamespacePrefix + ":" + mName;

    switch (mType) {
        case XmlNodeType::Root:
            for (const auto& child : mChildren) {
                child->print(os);
            }
            break;
        case XmlNodeType::Element:
            os << indentStr << "<" << tagName;
            for (const auto& attr : mAttributes) {
                os << " " << attr.toString();
            }
            if (mChildren.empty()) {
                os << " />\n";
            } else {
                os << ">\n";
                for (const auto& child : mChildren) {
                    child->print(os, indent + 2);
                }
                os << indentStr << "</" << tagName << ">\n";
            }
            break;
        case XmlNodeType::Text:
            os << indentStr << mName << "\n";
            break;
        case XmlNodeType::Comment:
            os << indentStr << "<!-- " << mName << " -->\n";
            break;
        case XmlNodeType::Declaration:
            os << indentStr << "<?" << mName << "?>\n";
            break;
    }
}


// Helper
XmlNodePtr createNode(XmlNodeType type, const std::string& name,
                      const std::string& nsPrefix) {
    return std::make_shared<XmlNode>(type, name, nsPrefix);
}

}
