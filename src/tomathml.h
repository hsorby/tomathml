
#pragma once

#include <string>

#include "tomathml_export.h"

namespace tomathml {

/**
 * @brief Process a text string into content MathML.
 *
 * Processes a test string from CellML text into MathML.
 * The optional cellml flag (default true) is used to turn on or off the CellML specific output.
 * If the processing of the input text fails, the output will be a print out of error messages.
 *
 * @param text A string of mathematical equations.
 * @param cellml Optional flag to indicate if output should be CellML aware [default: true].
 * @return Content MathML string if successful.
 */
std::string TOMATHML_API process(const std::string &text, bool cellml = true);

}
