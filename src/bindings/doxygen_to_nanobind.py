#!/usr/bin/env python3
"""
Version 0.1.0 of this script to auto-generate nanobind Python bindings from Doxygen
generated XML.
"""

import os
import argparse
import string
import textwrap
import xml.etree.ElementTree as ET

from collections import defaultdict
from difflib import SequenceMatcher
from pathlib import PureWindowsPath


# Common standard library header to nanobind type mapping
std_map = {
    "string": "nanobind/stl/string.h",
    "vector": "nanobind/stl/vector.h",
    "map": "std::map",
    "unordered_map": "std::unordered_map",
    "set": "std::set",
    "memory": "std::shared_ptr, std::unique_ptr",
    "utility": "std::pair, std::make_pair",
    "tuple": "std::tuple",
    "iostream": "std::cin, std::cout, std::cerr",
}


def parse_arguments():
    parser = argparse.ArgumentParser(description="Generate nanobind bindings from Doxygen XML.")
    parser.add_argument('--xml', required=True, help='Path to Doxygen XML directory.')
    parser.add_argument('--out', required=True, help='Output directory for .cpp binding files.')
    parser.add_argument("-v", "--verbose", action="store_true", help="Be verbose when processing.")
    return parser.parse_args()


def get_text(elem):
    if elem is None:
        return ''

    return ''.join(elem.itertext()).strip()


def get_name(elem):
    return elem.find('name').text.strip()


def get_refid(elem):
    return elem.attrib['refid']


def extract_class_details(root, from_kind=None, xml_dir=None):
    methods = []
    enums = []
    includes = []
    header = None
    for member in root.findall('.//memberdef'):
        kind = member.attrib.get('kind')
        prot = member.attrib.get('prot')

        if prot != 'public':
            continue  # only expose public members

        member_header = member.find("location").attrib.get("file", "no-header")
        if header is None:
            header = member_header

        if member_header != header:
            print(f"Warning: class uses more than one header: '{member_header}', '{header}'.")
        elif member_header == "no-header":
            print(f"Warning: class doesn't have a file location defined.")

        # Add any included headers into extracted information.
        header_refid = get_refid_for(header, get_root_element(get_index_file(xml_dir)))
        includes = extract_std_includes(os.path.join(xml_dir, header_refid))

        if kind == 'function':
            name = member.find('name').text
            qualified_name = member.findtext("qualifiedname") or name
            # brief = extract_brief_description(member)
            # print(f'Function: {name}')
            args, doc = extract_docs(member)
            methods.append((kind, name, qualified_name, args, doc))

        elif kind == 'enum':
            enum_name = member.find('name').text
            values = []
            for enum_value in member.findall('enumvalue'):
                ev_name = enum_value.find('name').text
                ev_qualified_name = member.findtext("qualifiedname") or ev_name
                ev_brief = extract_brief_description(enum_value)
                values.append((ev_name, f"{ev_qualified_name}::{ev_name}", ev_brief))
            enums.append((kind, enum_name, values))

    return {header: [methods + enums + includes]}


def extract_std_includes(xml_file):
    includes = []

    root = get_root_element(xml_file)

    for inc in root.findall(".//includes"):
        header = inc.text.strip()
        local = inc.attrib.get("local", "yes")
        if local == "no":  # Likely a system include like <string>
            std_nb_header = std_map.get(header, None)
            if std_nb_header:
                # Add an entry into the list of symbols for the mapped header.
                includes.append(std_nb_header)

    return 'includes', includes, ''


def get_root_element(xml_file):
    tree = ET.parse(xml_file)
    return tree.getroot()


def parse_doxygen_detailed_description(element):
    doc_lines = []

    detailed = ""
    # Extract paragraphs and parameter lists
    for para in element.findall("./para"):
        skip = False
        # Handle parameter list
        paramlist = para.find("parameterlist")
        if paramlist is not None:
            doc_lines.append("Parameters:")
            for item in paramlist.findall("parameteritem"):
                name = item.findtext("parameternamelist/parametername", default="(unknown)")
                desc = item.findtext("parameterdescription/para", default="(no description)")
                doc_lines.append(f"    {name}: {desc.strip()}")
            skip = True  # Don't include the raw para if it's just a wrapper for params

        # Handle return section
        return_section = para.find("simplesect[@kind='return']")
        if return_section is not None:
            return_desc = return_section.findtext("para", default="(no return description)")
            doc_lines.append(f"\nReturns:\n    {return_desc.strip()}")
            skip = True

        if skip:
            continue

        # Otherwise, treat as normal paragraph text
        text = get_text(para) if para.text else ""
        if text:
            detailed = textwrap.fill(text, width=100)

    return detailed, doc_lines  # "\n".join(doc_lines)


def normalize_advanced(text):
    text = text.lower()
    text = text.translate(str.maketrans("", "", string.punctuation))
    return ' '.join(text.split())


def extract_docs(element):
    args = []
    brief_description = get_text(element.find("briefdescription"))
    detailed_description, doc_lines = parse_doxygen_detailed_description(element.find("detaileddescription"))
    similarity = SequenceMatcher(None, normalize_advanced(brief_description), normalize_advanced(detailed_description)).ratio()
    doc = brief_description + "\n\n"
    if similarity > 0.9:
        print(f"Descriptions are similar ({similarity:.2f}).")
    else:
        doc += f"{detailed_description}\n\n"

    doc += "\n".join(doc_lines)
    # doc = get_text(element.find("briefdescription")) + "\n\n" + parse_doxygen_detailed_description(element.find("detaileddescription"), brief_description)
    for param in element.findall("param"):
        pname = param.find("declname")
        ptype = get_text(param.find("type"))
        def_value = get_text(param.find('defval'))
        args.append((pname.text if pname is not None else "arg", ptype, def_value))

    return args, doc.strip()


# Extracts the filename from a full path
def get_module_name(file_path):
    return os.path.splitext(os.path.basename(file_path))[0]


# Utility to safely create filenames
def safe_name(name):
    return name.replace('.', '_').replace('-', '_')


def extract_info_from(root, from_kind, xml_dir):
    symbols = defaultdict(list)

    for compounddef in root.findall(".//compounddef"):
        kind = compounddef.attrib.get("kind")
        if kind == "class":
            print(f"Shouldn't be extracting info from {from_kind} with class information in it.")

        for member in compounddef.findall("sectiondef/memberdef"):
            header = member.findtext("location[@file]") or member.find("location").attrib.get("file")
            if not header or not header.endswith(".h"):
                print(f"Extracting info from {from_kind} but could not detect header member information is drawn from '{header}'.")
                continue
                
            # Add any included headers into extracted information.
            header_refid = get_refid_for(header, get_root_element(get_index_file(xml_dir)))
            includes = extract_std_includes(os.path.join(xml_dir, f"{header_refid}.xml"))
            symbols[header].append(includes)
            
            kind = member.attrib.get("kind")
            visibility = member.attrib.get("prot")
            if visibility == "public":
                name = member.findtext("name")
                qname = member.findtext("qualifiedname") or name  # use full qualified name
                if kind == "function":
                    args, doc = extract_docs(member)
                    symbols[header].append((kind, name, qname, args, doc))
                elif kind == "enum":
                    values = []
                    for enum_value in member.findall('enumvalue'):
                        val_name = get_text(enum_value.find('name'))
                        value_doc = get_text(enum_value.find('.//briefdescription/para'))
                        values.append((val_name, value_doc))

                    symbols[header].append((kind, name, qname, values))

    return symbols


# Scan compound information XML information and extract symbols by header
def extract_symbols_by_header(xml_dir, compound_map):
    symbols = defaultdict(list)

    kind_handler_function = {
        "class": extract_class_details,
        "namespace": extract_info_from,
        "file": extract_info_from,
    }

    for ref_id, ref_kind in compound_map.items():
        root = get_root_element(os.path.join(xml_dir, f"{ref_id}.xml"))
        current_keys = set(symbols.keys())
        new_symbols = kind_handler_function[ref_kind](root, ref_kind, xml_dir)
        new_keys = set(new_symbols.keys())
        intersection_of_keys = current_keys & new_keys
        if intersection_of_keys:
            print("Overwriting symbols: ", intersection_of_keys)
            
        symbols |= new_symbols

    return symbols


def create_arg_string(args):
    return ", ".join([f'"{a[0]}"_a{" = " + a[2] if a[2] else ""}' for a in args])


# Write nanobind skeleton file per header
def write_binding_files(symbol_map, output_dir):
    os.makedirs(output_dir, exist_ok=True)

    written_files = []
    for header, items in symbol_map.items():
        module_name = safe_name(get_module_name(header))
        filename = os.path.join(output_dir, f"{module_name}_bindings.cpp")
        written_files.append(PureWindowsPath(filename).as_posix())

        mapped_includes = [item for item in items if item[0] == 'includes']

        with open(filename, "w") as f:
            f.write(f'#include <nanobind/nanobind.h>\n')
            f.write('\n'.join([f'#include <{i}>' for inc in mapped_includes for i in inc[1]]))
            f.write('\n\n')
            f.write(f'#include "{header}"\n\n')
            f.write(f'namespace nb = nanobind;\nusing namespace nb::literals;\n\n')
            f.write(f'NB_MODULE({module_name}, m) {{\n')
            for data in items:
                kind = data[0]
                name = data[1]
                qname = data[2]
                if kind == "function":
                    arg_string = create_arg_string(data[3])
                    doc = data[4]
                    f.write(f'    m.def("{name}", &{qname}, {arg_string}, R"pbdoc(\n{doc}\n)pbdoc");\n')
                elif kind == "class":
                    f.write(f'    nb::class_<{qname}>(m, "{name}");\n')
            # for kind, name in items:
            #     if kind == "function":
            #         f.write(f'    m.def("{name}", &{name});\n')
            #     elif kind == "class":
            #         f.write(f'    nb::class_<{name}>(m, "{name}");\n')
            f.write(f'}}\n')

        # print(f"Generated: {filename}")
    return written_files


def get_refid_for(name, index):
    print("get refid for:", name, index)
    for compound in index.findall("compound"):
        compound_name = compound.find("name")
        print(compound_name, get_text(compound_name))
        if get_text(compound_name) == name:
            print('returning:', compound.attrib["refid"])
            return compound.attrib["refid"]

    print('returning None.')
    return None


def create_compound_map(index):
    compound_map = {}

    for compound in index.findall("compound"):
        if len(compound) > 1:
            ref_id = compound.attrib["refid"]
            kind = compound.attrib["kind"]
            print(kind, ref_id)
            if kind in ["class", "namespace", "file"]:
                compound_map[ref_id] = kind

    return compound_map


def get_index_file(xml_dir):
    return os.path.join(xml_dir, "index.xml")


def main():
    args = parse_arguments()
    print("-------------------")
    print(args.xml)
    print(args.out)

    for f in os.listdir(args.xml):
        print(f)
    print('cat')
    os.system(f'cat {os.path.dirname(args.xml)}/Doxyfile')
    print('cat index file:')
    os.system(f'cat {get_index_file(args.xml)}')
    index = get_root_element(get_index_file(args.xml))
    compound_map = create_compound_map(index)
    symbols = extract_symbols_by_header(args.xml, compound_map)
    files_written = write_binding_files(symbols, args.out)

    print(f'Wrote bindings to: {args.out}')
    print('\n * '.join(files_written))
    with open(os.path.join(args.out, 'generated_files.txt'), 'w') as fh:
        fh.write('\n'.join(files_written))
        fh.write('\n')


if __name__ == '__main__':
    main()
