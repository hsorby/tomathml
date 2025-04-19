#pragma once

#include <memory>
#include <string>
#include <vector>

namespace utils {

class XmlNode;
using XmlNodePtr = std::shared_ptr<XmlNode>;

enum class XmlNodeType {
    Comment,
    Declaration,
    Element,
    Text,
    Root
};

XmlNodePtr createNode(XmlNodeType type, const std::string& name,
                      const std::string& nsPrefix = "");


class XmlAttribute {
public:
    XmlAttribute(const std::string& name, const std::string& value,
                 const std::string& nsPrefix = "");

    std::string toString() const;

private:
    std::string mName;
    std::string mValue;
    std::string mNamespacePrefix;
};


class XmlNode {
public:
    XmlNode(XmlNodeType type, const std::string& name,
            const std::string& nsPrefix = "");

    void addAttribute(const std::string& name, const std::string& value,
                      const std::string& nsPrefix = "");

    void declareNamespace(const std::string& prefix, const std::string& uri);

    void addChild(XmlNodePtr child);

    void print(std::ostream& os, int indent = 0) const;

private:
    XmlNodeType mType;
    std::string mName;
    std::string mNamespacePrefix;
    std::vector<XmlAttribute> mAttributes;
    std::vector<XmlNodePtr> mChildren;
};

}
