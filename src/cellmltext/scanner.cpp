
#include "scanner.h"

#include <regex>

namespace CellMLText {

Scanner::Scanner()
{
}



Scanner::Scanner(const Scanner &pScanner)
{
    // Initialise ourselves using the given scanner

    *this = pScanner;
}



void Scanner::operator=(const Scanner &pScanner)
{
    // Initialise ourselves using the given scanner

    mText = pScanner.mText;

    mChar = pScanner.mChar;

    mCharType = pScanner.mCharType;
    mCharLine = pScanner.mCharLine;
    mCharColumn = pScanner.mCharColumn;

    mToken = pScanner.mToken;
    mLine = pScanner.mLine;
    mColumn = pScanner.mColumn;
    mString = pScanner.mString;
    mComment = pScanner.mComment;

    mWithinParameterBlock = pScanner.mWithinParameterBlock;
}



void Scanner::setText(const std::string &pText)
{
    // Initialise ourselves with the text to scan

    mText = pText;

    mChar = &mText.at(size_t(0));

    mCharType = Char::Eof;
    mCharLine = 1;
    mCharColumn = 0;

    mToken = Token::Unknown;
    mLine = 0;
    mColumn = 0;
    mString = std::string();

    mWithinParameterBlock = false;

    getNextChar(false);
    getNextToken();
}



Scanner::Token Scanner::token() const
{
    // Return our token type

    return mToken;
}



int Scanner::line() const
{
    // Return our token line

    return mLine;
}



int Scanner::column() const
{
    // Return our token column

    return mColumn;
}



std::string Scanner::string() const
{
    // Return our token as a string

    return mString;
}



std::string Scanner::comment() const
{
    // Return our token comment

    return mComment;
}



void Scanner::getNextChar(bool increment)
{
    // Determine the type of our next character
    const char nextChar = increment ? *(++mChar) : *mChar;

    switch (static_cast<uint32_t>(nextChar)) {
    case 0:
        mCharType = Char::Eof;

        break;
    case 9:
        mCharType = Char::Tab;

        break;
    case 10:
        mCharType = Char::Lf;

        break;
    case 13:
        mCharType = Char::Cr;

        break;
    case 32:
        mCharType = Char::Space;

        break;
    case 34:
        mCharType = Char::DoubleQuote;

        break;
    case 39:
        mCharType = Char::Quote;

        break;
    case 40:
        mCharType = Char::OpeningBracket;

        break;
    case 41:
        mCharType = Char::ClosingBracket;

        break;
    case 42:
        mCharType = Char::Times;

        break;
    case 43:
        mCharType = Char::Plus;

        break;
    case 44:
        mCharType = Char::Comma;

        break;
    case 45:
        mCharType = Char::Minus;

        break;
    case 46:
        mCharType = Char::FullStop;

        break;
    case 47:
        mCharType = Char::Divide;

        break;
    case 48: case 49: case 50: case 51: case 52: case 53: case 54: case 55:
    case 56: case 57:
        mCharType = Char::Digit;

        break;
    case 58:
        mCharType = Char::Colon;

        break;
    case 59:
        mCharType = Char::SemiColon;

        break;
    case 60:
        mCharType = Char::Lt;

        break;
    case 61:
        mCharType = Char::Eq;

        break;
    case 62:
        mCharType = Char::Gt;

        break;
    case 65: case 66: case 67: case 68: case 69: case 70: case 71: case 72:
    case 73: case 74: case 75: case 76: case 77: case 78: case 79: case 80:
    case 81: case 82: case 83: case 84: case 85: case 86: case 87: case 88:
    case 89: case 90:
        mCharType = Char::Letter;

        break;
    case 95:
        mCharType = Char::Underscore;

        break;
    case 97: case 98: case 99: case 100: case 101: case 102: case 103:
    case 104: case 105: case 106: case 107: case 108: case 109: case 110:
    case 111: case 112: case 113: case 114: case 115: case 116: case 117:
    case 118: case 119: case 120: case 121: case 122:
        mCharType = Char::Letter;

        break;
    case 123:
        mCharType = Char::OpeningCurlyBracket;

        break;
    case 125:
        mCharType = Char::ClosingCurlyBracket;

        break;
    default:
        mCharType = Char::Other;
    }

    // Update our token line and/or column numbers

    if (mCharType == Char::Lf) {
        mCharColumn = 0;

        ++mCharLine;
    } else {
        ++mCharColumn;
    }
}



void Scanner::getSingleLineComment()
{
    // Retrieve a single line comment by looking for the end of the current line

    mToken = Token::SingleLineComment;
    mString = std::string();

    while(true) {
        getNextChar();

        if (   (mCharType == Char::Cr) || (mCharType == Char::Lf)
            || (mCharType == Char::Eof)) {
            break;
        }

        mString += *mChar;
    }
}



void Scanner::getMultilineComment()
{
    // Retrieve a multiline comment from our text by looking for the first
    // occurrence of "*/"

    mString = std::string();

    while(true) {
        getNextChar();

        if (mCharType == Char::Times) {
            const char timesChar = *mChar;

            getNextChar();

            if (mCharType == Char::Divide) {
                getNextChar();

                mToken = Token::MultilineComment;

                return;
            }

            mString += timesChar;

            if (mCharType == Char::Eof) {
                break;
            }

            mString += *mChar;
        } else if (mCharType == Char::Eof) {
            break;
        } else {
            mString += *mChar;
        }
    }

    mToken = Token::Invalid;
    mComment = "The comment is incomplete.";
}

template<typename Map, typename Key>
typename Map::mapped_type getOrDefault(const Map& m, const Key& key, const typename Map::mapped_type& defaultValue) {
    auto it = m.find(key);
    return (it != m.end()) ? it->second : defaultValue;
}

void Scanner::getWord()
{
    // Retrieve a word from our text

    while(true) {
        getNextChar();

        if (   (mCharType == Char::Letter) || (mCharType == Char::Digit) || (mCharType == Char::Underscore)
            || (mWithinParameterBlock && ((mCharType == Char::Minus) || (mCharType == Char::FullStop)))) {
            mString += *mChar;
        } else {
            break;
        }
    }

    // Check what kind of word we are dealing with, i.e. a keyword, an SI unit
    // keyword, a parameter keyword, an identifier or something else

    if (mWithinParameterBlock) {
        mToken = getOrDefault(mParameterKeywords, mString, Token::Unknown);
    } else {
        mToken = getOrDefault(mKeywords, mString, Token::Unknown);
    }

    if (mToken == Token::Unknown) {
        mToken = getOrDefault(mSiUnitKeywords, mString, Token::Unknown);
    }

    if (mToken == Token::Unknown) {
        // We are not dealing with a keyword, but it might still be an
        // identifier or cmeta:id, as long as it doesn't only consist of
        // underscores, hyphens and periods

        static const std::regex underscoresHyphensOrPeriodsRegEx("[_.-]");
        static const std::regex hyphensOrPeriodsRegEx("[.-]");

        std::string stripped = std::regex_replace(mString, underscoresHyphensOrPeriodsRegEx, "");

        if (!stripped.empty()) {
            if (std::regex_search(mString, hyphensOrPeriodsRegEx)) {
                mToken = Token::ProperCmetaId;
            } else {
                mToken = Token::IdentifierOrCmetaId;
            }
        }
    }
}

bool toDouble(const std::string& input, double& output) {
    try {
        size_t idx;
        output = std::stod(input, &idx);
        return idx == input.size(); // Ensure entire string was parsed
    } catch (...) {
        return false;
    }
}

void Scanner::getNumber()
{
    // Retrieve a number from our text

    bool fullStopFirstChar = mCharType == Char::FullStop;

    if (fullStopFirstChar) {
        // We started a number with a full stop, so reset mTokenString since it
        // is going to be updated with our full stop

        mString = std::string();
    } else {
        // We started a number with a digit, so look for additional ones

        while(true) {
            getNextChar();

            if (mCharType == Char::Digit) {
                mString += *mChar;
            } else {
                break;
            }
        }
    }

    // Look for the fractional part, if any

    if (mCharType == Char::FullStop) {
        mString += *mChar;

        getNextChar();

        // Check whether the full stop is followed by some digits

        if (mCharType == Char::Digit) {
            mString += *mChar;

            while (true) {
                getNextChar();

                if (mCharType == Char::Digit) {
                    mString += *mChar;
                } else {
                    break;
                }
            }
        } else if (fullStopFirstChar) {
            // We started a number with a full stop, but it's not followed by
            // digits, so it's not a number after all

            mToken = Token::Unknown;

            return;
        }
    }

    // Check whether we have an exponent part

    if (   (mCharType == Char::Letter)
        && ((*mChar == 'e') || (*mChar == 'E'))) {
        mString += *mChar;

        getNextChar();

        // Check whether we have a + or - sign

        if ((mCharType == Char::Plus) || (mCharType == Char::Minus)) {
            mString += *mChar;

            getNextChar();
        }

        // Check whether we have some digits

        if (mCharType == Char::Digit) {
            mString += *mChar;

            while (true) {
                getNextChar();

                if (mCharType == Char::Digit) {
                    mString += *mChar;
                } else {
                    break;
                }
            }
        } else {
            // We started an exponent part, but it isn't followed by digits

            mToken = Token::Invalid;
            mComment = "The exponent has no digits.";

            return;
        }
    }

    // At this stage, we have a number, but it may be invalid

    double number;
    bool validNumber = toDouble(mString, number);

    mToken = Token::Number;

    if (!validNumber) {
        mComment = "The number is not valid (e.g. too big, too small).";
    }
}



void Scanner::getString()
{
    // Retrieve a string from our text by looking for a double quote

    mString = std::string();

    while (true) {
        getNextChar();

        if (   (mCharType == Char::DoubleQuote)
            || (mCharType == Char::Cr) || (mCharType == Char::Lf)
            || (mCharType == Char::Eof)) {
            break;
        }

        mString += *mChar;
    }

    if (mCharType == Char::DoubleQuote) {
        mToken = Token::String;

        getNextChar();
    } else {
        mToken = Token::Invalid;
        mComment = "The string is incomplete.";
    }
}



void Scanner::getNextToken()
{
    // Get the next token in our text by first skipping all the spaces and
    // special characters

    while (   (mCharType == Char::Space) || (mCharType == Char::Tab)
           || (mCharType == Char::Cr)    || (mCharType == Char::Lf)) {
        getNextChar();
    }

    // Determine the type of our next token

    mLine = mCharLine;
    mColumn = mCharColumn;

    mString = *mChar;

    mComment = std::string();

    if (   (mCharType == Char::Letter)
        || (mCharType == Char::Underscore)) {
        getWord();
    } else if (   (mCharType == Char::Digit)
               || (mCharType == Char::FullStop)) {
        getNumber();
    } else if (mCharType == Char::DoubleQuote) {
        getString();
    } else if (mCharType == Char::Eq) {
        mToken = Token::Eq;

        getNextChar();

        if (mCharType == Char::Eq) {
            mString += *mChar;

            mToken = Token::EqEq;

            getNextChar();
        }
    } else if (mCharType == Char::Lt) {
        mToken = Token::Lt;

        getNextChar();

        if (mCharType == Char::Eq) {
            mString += *mChar;

            mToken = Token::Leq;

            getNextChar();
        } else if (mCharType == Char::Gt) {
            mString += *mChar;

            mToken = Token::Neq;

            getNextChar();
        }
    } else if (mCharType == Char::Gt) {
        mToken = Token::Gt;

        getNextChar();

        if (mCharType == Char::Eq) {
            mString += *mChar;

            mToken = Token::Geq;

            getNextChar();
        }
    } else if (mCharType == Char::Divide) {
        mToken = Token::Divide;

        getNextChar();

        if (mCharType == Char::Divide) {
            getSingleLineComment();
        } else if (mCharType == Char::Times) {
            getMultilineComment();
        }
    } else if (   (mCharType == Char::OpeningCurlyBracket)
               || (mCharType == Char::ClosingCurlyBracket)) {
        mToken = (mCharType == Char::OpeningCurlyBracket)?
                     Token::OpeningCurlyBracket:
                     Token::ClosingCurlyBracket;
        mWithinParameterBlock = mCharType == Char::OpeningCurlyBracket;

        getNextChar();
    } else if (mCharType == Char::Eof) {
        mToken = Token::Eof;
        mString = "the end of the file";
    } else {
        if (mCharType == Char::Quote) {
            mToken = Token::Quote;
        } else if (mCharType == Char::Comma) {
            mToken = Token::Comma;
        } else if (mCharType == Char::Plus) {
            mToken = Token::Plus;
        } else if (mCharType == Char::Minus) {
            mToken = Token::Minus;
        } else if (mCharType == Char::Times) {
            mToken = Token::Times;
        } else if (mCharType == Char::Colon) {
            mToken = Token::Colon;
        } else if (mCharType == Char::SemiColon) {
            mToken = Token::SemiColon;
        } else if (mCharType == Char::OpeningBracket) {
            mToken = Token::OpeningBracket;
        } else if (mCharType == Char::ClosingBracket) {
            mToken = Token::ClosingBracket;
        } else {
            mToken = Token::Unknown;
        }

        getNextChar();
    }
}

}
