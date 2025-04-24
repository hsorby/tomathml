
#include "cellmltext/parser.h"
#include "utils/stringhelp.h"

#include <format>

namespace CellMLText {

static const auto MathmlNamespace = "http://www.w3.org/1998/Math/MathML";

template<typename Key, typename Value>
std::vector<Key> getKeys(const std::map<Key, Value>inMap)
{
    std::vector<Key> mapKeys;
    for (const auto& pair : inMap) {
        mapKeys.push_back(pair.first);
    }
    return mapKeys;
}

bool toInt(const std::string& input, int& output) {
    try {
        size_t idx;
        output = std::stoi(input, &idx);
        return idx == input.size(); // Ensure entire string was parsed
    } catch (...) {
        return false;
    }
}

bool containsToken(const std::list<Scanner::Token> &l, Scanner::Token item)
{
    return std::find(l.begin(), l.end(), item) != l.end();
}

ParserMessage::ParserMessage(Type pType, int pLine,
                                                         int pColumn,
                                                         const std::string &pMessage) :
    mType(pType),
    mLine(pLine),
    mColumn(pColumn),
    mMessage(pMessage)
{
}



ParserMessage::Type ParserMessage::type() const
{
    // Return our type

    return mType;
}



int ParserMessage::line() const
{
    // Return our line number

    return mLine;
}



int ParserMessage::column() const
{
    // Return our column number

    return mColumn;
}



std::string ParserMessage::message() const
{
    // Return our message

    return mMessage;
}



bool Parser::execute(const std::string &pCellmlText)
{
    // Get ready for the parsing of a model definition

    initialize(pCellmlText);

    return true;
}



bool Parser::execute(const std::string &pCellmlText,
                                   bool pFullParsing,
                                   bool cellmlMode)
{
    // Get ready for the parsing of a mathematical expression

    initialize(pCellmlText, cellmlMode);

    // Either fully parse or partially parse a mathematical expression

    if (pFullParsing) {
        // Parse a mathematical expression

        static const Scanner::Tokens Tokens = { Scanner::Token::IdentifierOrCmetaId,
                                                              Scanner::Token::Ode };

        while (mScanner.token() != Scanner::Token::Eof) {
            if (tokenType(mMathElement, "An identifier or 'ode'",
                          Tokens)) {
                if (!parseMathematicalExpression(mMathElement)) {
                    return false;
                }
            } else {
                return false;
            }

            // Expect the end of the mathematical expression

            mScanner.getNextToken();
        }

        return true;
    }

    // Partially parse a mathematical expression

    static const Scanner::Tokens Tokens = { Scanner::Token::IdentifierOrCmetaId,
                                                          Scanner::Token::Ode,
                                                          Scanner::Token::Case,
                                                          Scanner::Token::Otherwise,
                                                          Scanner::Token::EndSel };

    if (tokenType(mDomDocument, "An identifier, 'ode', 'case', 'otherwise' or 'endsel'",
                  Tokens)) {
        if (mScanner.token() == Scanner::Token::Case) {
            mStatement = Statement::PiecewiseCase;

            return true;
        }

        if (mScanner.token() == Scanner::Token::Otherwise) {
            mStatement = Statement::PiecewiseOtherwise;

            return true;
        }

        if (mScanner.token() == Scanner::Token::EndSel) {
            mStatement = Statement::PiecewiseEndSel;

            return true;
        }

        return parseMathematicalExpression(mDomDocument, pFullParsing);
    }

    return false;
}



utils::XmlNodePtr Parser::domDocument() const
{
    // Return our DOM document

    return mDomDocument;
}

ParserMessages Parser::messages() const
{
    // Return our messages

    return mMessages;
}



Parser::Statement Parser::statement() const
{
    // Return our statement type

    return mStatement;
}



void Parser::initialize(const std::string &pCellmlText, bool pCellmlMode)
{
    // Initialize ourselves with the given CellML Text string

    mScanner.setText(pCellmlText);
    mCellmlMode = pCellmlMode;

    mDomDocument = utils::createNode(utils::XmlNodeType::Root, "");
    mDomDocument->addChild(utils::createNode(utils::XmlNodeType::Declaration, "xml version=\"1.0\" encoding=\"UTF-8\""));
    mMathElement = utils::createNode(utils::XmlNodeType::Element, "math");
    mMathElement->declareNamespace("", MathmlNamespace);
    mDomDocument->addChild(mMathElement);

    mMessages = ParserMessages();

    mNamespaces.clear();

    mStatement = Statement::Unknown;
}



void Parser::addUnexpectedTokenErrorMessage(const std::string &pExpectedString,
                                                          const std::string &pFoundString)
{
    // Add an error message for the given unexpected token

    mMessages.push_back(ParserMessage(ParserMessage::Type::Error,
                                             mScanner.line(),
                                             mScanner.column(),
                                             std::format("{} is expected, but {} was found instead.", pExpectedString, pFoundString)));
}



utils::XmlNodePtr Parser::newDomElement(utils::XmlNodePtr pDomNode,
                                                const std::string &pElementName)
{
    // Create a new DOM element with the given name and append it to the given
    // DOM node before returning it

    utils::XmlNodePtr domElement = utils::createNode(utils::XmlNodeType::Element, pElementName);

    pDomNode->addChild(domElement);

    return domElement;
}



utils::XmlNodePtr Parser::newIdentifierElement(const std::string &pValue)
{
    // Create and return a new identifier element with the given value

    utils::XmlNodePtr identifierElement = utils::createNode(utils::XmlNodeType::Element, "ci");

    identifierElement->addChild(utils::createNode(utils::XmlNodeType::Text, pValue));

    return identifierElement;
}



utils::XmlNodePtr Parser::newDerivativeElement(const std::string &pF,
                                                       const std::string &pX)
{
    // Create and return a new derivative element with the given parameters

    utils::XmlNodePtr derivativeElement = utils::createNode(utils::XmlNodeType::Element, "apply");
    utils::XmlNodePtr bvarElement = utils::createNode(utils::XmlNodeType::Element, "bvar");

    derivativeElement->addChild(utils::createNode(utils::XmlNodeType::Element, "diff"));
    derivativeElement->addChild(bvarElement);
    derivativeElement->addChild(newIdentifierElement(pF));

    bvarElement->addChild(newIdentifierElement(pX));

    return derivativeElement;
}



utils::XmlNodePtr Parser::newDerivativeElement(const std::string &pF,
                                                       const std::string &pX,
                                                       const std::string &pOrder)
{
    // Create and return a new derivative element with the given parameters

    utils::XmlNodePtr derivativeElement = utils::createNode(utils::XmlNodeType::Element, "apply");
    utils::XmlNodePtr bvarElement = utils::createNode(utils::XmlNodeType::Element, "bvar");
    utils::XmlNodePtr degreeElement = utils::createNode(utils::XmlNodeType::Element, "degree");
    utils::XmlNodePtr cnElement = utils::createNode(utils::XmlNodeType::Element, "cn");

    derivativeElement->addChild(utils::createNode(utils::XmlNodeType::Element, "diff"));
    derivativeElement->addChild(bvarElement);
    derivativeElement->addChild(newIdentifierElement(pF));

    bvarElement->addChild(newIdentifierElement(pX));
    bvarElement->addChild(degreeElement);

    degreeElement->addChild(cnElement);

    cnElement->addChild(utils::createNode(utils::XmlNodeType::Text, pOrder));
    if (mCellmlMode) {
        cnElement->addAttribute("units", "dimensionless", "cellml");
        cnElement->declareNamespace("cellml", "http://www.cellml.org/cellml/2.0#");
    }

    return derivativeElement;
}



utils::XmlNodePtr Parser::newNumberElement(const std::string &pNumber,
                                                   const std::string &pUnit)
{
    // Create and return a new number element with the given value

    utils::XmlNodePtr numberElement = utils::createNode(utils::XmlNodeType::Element, "cn");
    auto ePos = utils::toUpper(pNumber).find("E");

    if (ePos == std::string::npos) {
        numberElement->addChild(utils::createNode(utils::XmlNodeType::Text, pNumber));
    } else {
        numberElement->addAttribute("type", "e-notation");

        numberElement->addChild(utils::createNode(utils::XmlNodeType::Text, utils::left(pNumber, ePos)));
        numberElement->addChild(utils::createNode(utils::XmlNodeType::Element, "sep"));
        numberElement->addChild(utils::createNode(utils::XmlNodeType::Text, utils::right(pNumber, pNumber.length() - ePos - 1)));
    }

    if (mCellmlMode) {
        numberElement->addAttribute("units", pUnit, "cellml");
        numberElement->declareNamespace("cellml", "http://www.cellml.org/cellml/2.0#");
    }

    return numberElement;
}



utils::XmlNodePtr Parser::newMathematicalConstantElement(Scanner::Token pTokenType)
{
    // Create and return a new mathematical constant element for the given token
    // typewith the given value

    return utils::createNode(utils::XmlNodeType::Element, mathmlName(pTokenType));
}



utils::XmlNodePtr Parser::newMathematicalFunctionElement(Scanner::Token pTokenType,
                                                         const std::vector<utils::XmlNodePtr> &pArgumentElements)
{
    // Create and return a new mathematical function element for the given token
    // and arguments

    utils::XmlNodePtr mathematicalFunctionElement = utils::createNode(utils::XmlNodeType::Element, "apply");

    mathematicalFunctionElement->addChild(utils::createNode(utils::XmlNodeType::Element, mathmlName(pTokenType)));

    if (pArgumentElements.size() == 2) {
        if (pTokenType == Scanner::Token::Log) {
            utils::XmlNodePtr logBaseElement = utils::createNode(utils::XmlNodeType::Element, "logbase");

            logBaseElement->addChild(pArgumentElements[1]);
            mathematicalFunctionElement->addChild(logBaseElement);
        } else if (pTokenType == Scanner::Token::Root) {
            utils::XmlNodePtr degreeElement = utils::createNode(utils::XmlNodeType::Element, "degree");

            degreeElement->addChild(pArgumentElements[1]);
            mathematicalFunctionElement->addChild(degreeElement);
        }
    }

    mathematicalFunctionElement->addChild(pArgumentElements[0]);

    if (pArgumentElements.size() == 1) {
        if (pTokenType == Scanner::Token::Sqr) {
            mathematicalFunctionElement->addChild(newNumberElement("2", "dimensionless"));
        }
    } else if (   (pTokenType >= Scanner::Token::FirstTwoOrMoreArgumentMathematicalFunction)
               && (pTokenType <= Scanner::Token::LastTwoOrMoreArgumentMathematicalFunction)) {
        for (int i = 1, iMax = pArgumentElements.size(); i < iMax; ++i) {
            mathematicalFunctionElement->addChild(pArgumentElements[i]);
        }
    } else if (   (pTokenType != Scanner::Token::Log)
               && (pTokenType != Scanner::Token::Root)) {
        mathematicalFunctionElement->addChild(pArgumentElements[1]);
    }

    return mathematicalFunctionElement;
}



Scanner::Tokens Parser::rangeOfTokens(Scanner::Token pFromTokenType,
                                                                  Scanner::Token pToTokenType)
{
    // Return a range of token types

    Scanner::Tokens tokens;

    for (Scanner::Token tokenType = pFromTokenType;
         tokenType <= pToTokenType;
         tokenType = Scanner::Token(int(tokenType)+1)) {
        tokens.push_back(tokenType);
    }

    return tokens;
}



bool Parser::tokenType(utils::XmlNodePtr &pDomNode,
                       const std::string &pExpectedString,
                       const Scanner::Tokens &pTokens)
{
    // Try to parse comments, if any

    parseComments(pDomNode);

    // Check whether the current token type is the one we are after

    bool tokenFound = (std::find(pTokens.begin(), pTokens.end(), mScanner.token()) != pTokens.end());
    if (tokenFound) {
        // We have the correct token, so check whether a comment exists and, if
        // so, generate a warning for it

        if (!mScanner.comment().empty()) {
            mMessages.push_back(ParserMessage(ParserMessage::Type::Warning,
                                                     mScanner.line(),
                                                     mScanner.column(),
                                                     mScanner.comment()));
        }

        return true;
    }

    if (mScanner.token() == Scanner::Token::Invalid) {
        // This is the token we were expecting, but it is invalid

        mMessages.push_back(ParserMessage(ParserMessage::Type::Error,
                                                 mScanner.line(),
                                                 mScanner.column(),
                                                 mScanner.comment()));

        return false;
    }

    // This is not the token we were expecting, so let the user know about it

    std::string foundString = mScanner.string();

    if (mScanner.token() != Scanner::Token::Eof) {
        foundString = std::format("'{}'", utils::specials(foundString));
    }

    addUnexpectedTokenErrorMessage(pExpectedString, foundString);

    return false;
}



bool Parser::tokenType(utils::XmlNodePtr &pDomNode,
                       const std::string &pExpectedString,
                       Scanner::Token pTokenType)
{
    // Expect the given token

    auto tokens = Scanner::Tokens();
    tokens.push_back(pTokenType);

    return tokenType(pDomNode, pExpectedString, tokens);
}



bool Parser::isTokenType(utils::XmlNodePtr &pDomNode,
                         Scanner::Token pTokenType)
{
    // Try to parse comments, if any

    parseComments(pDomNode);

    // Return whether the current token type is the one we are after

    return mScanner.token() == pTokenType;
}



bool Parser::caseToken(utils::XmlNodePtr &pDomNode)
{
    // Expect "case"

    return tokenType(pDomNode, "'case'",
                     Scanner::Token::Case);
}



bool Parser::closingBracketToken(utils::XmlNodePtr &pDomNode)
{
    // Expect ")"

    return tokenType(pDomNode, "')'",
                     Scanner::Token::ClosingBracket);
}



bool Parser::closingCurlyBracketToken(utils::XmlNodePtr &pDomNode)
{
    // Expect "}"

    return tokenType(pDomNode, "'}'",
                     Scanner::Token::ClosingCurlyBracket);
}



bool Parser::colonToken(utils::XmlNodePtr &pDomNode)
{
    // Expect ":"

    return tokenType(pDomNode, "':'",
                     Scanner::Token::Colon);
}



bool Parser::commaToken(utils::XmlNodePtr &pDomNode)
{
    // Expect ","

    return tokenType(pDomNode, "','",
                     Scanner::Token::Comma);
}



bool Parser::dimensionlessToken(utils::XmlNodePtr &pDomNode)
{
    // Expect "dimensionless"

    return tokenType(pDomNode, "'dimensionless'",
                     Scanner::Token::Dimensionless);
}



bool Parser::endselToken(utils::XmlNodePtr &pDomNode)
{
    // Expect "endsel"

    return tokenType(pDomNode, "'endsel'",
                     Scanner::Token::EndSel);
}



bool Parser::eqToken(utils::XmlNodePtr &pDomNode)
{
    // Expect "="

    return tokenType(pDomNode, "'='",
                     Scanner::Token::Eq);
}



bool Parser::identifierOrSiUnitToken(utils::XmlNodePtr &pDomNode)
{
    // Expect an identifier or an SI unit

    static Scanner::Tokens tokens;
    static bool needInitializeTokens = true;

    if (needInitializeTokens) {
        auto tokenRange = rangeOfTokens(Scanner::Token::FirstUnit,
                               Scanner::Token::LastUnit);
        tokenRange.push_back(Scanner::Token::IdentifierOrCmetaId);
        tokens = tokenRange;

        needInitializeTokens = false;
    }

    return tokenType(pDomNode, "An identifier or an SI unit (e.g. 'second')",
                     tokens);
}



bool Parser::identifierToken(utils::XmlNodePtr &pDomNode)
{
    // Expect an identifier

    return tokenType(pDomNode, "An identifier",
                     Scanner::Token::IdentifierOrCmetaId);
}



bool Parser::cmetaIdToken(utils::XmlNodePtr &pDomNode)
{
    // Expect a cmeta:id

    static const Scanner::Tokens Tokens = { Scanner::Token::IdentifierOrCmetaId,
                                                          Scanner::Token::ProperCmetaId };

    return tokenType(pDomNode, "A cmeta:id",
                     Tokens);
}



bool Parser::numberToken(utils::XmlNodePtr &pDomNode)
{
    // Expect a number

    return tokenType(pDomNode, "A number",
                     Scanner::Token::Number);
}



bool Parser::numberValueToken(utils::XmlNodePtr &pDomNode, int &pSign)
{
    // Check whether we have "+" or "-"

    pSign = 0;

    if (isTokenType(pDomNode, Scanner::Token::Plus)) {
        pSign = 1;

        mScanner.getNextToken();
    } else if (isTokenType(pDomNode, Scanner::Token::Minus)) {
        pSign = -1;

        mScanner.getNextToken();
    }

    // Expect a number

    return numberToken(pDomNode);
}



bool Parser::openingBracketToken(utils::XmlNodePtr &pDomNode)
{
    // Expect "("

    return tokenType(pDomNode, "'('",
                     Scanner::Token::OpeningBracket);
}



bool Parser::openingCurlyBracketToken(utils::XmlNodePtr &pDomNode)
{
    // Expect "{"

    return tokenType(pDomNode, "'{'",
                     Scanner::Token::OpeningCurlyBracket);
}



bool Parser::semiColonToken(utils::XmlNodePtr &pDomNode)
{
    // Expect ";"

    return tokenType(pDomNode, "';'",
                     Scanner::Token::SemiColon);
}



bool Parser::strictlyPositiveIntegerNumberToken(utils::XmlNodePtr &pDomNode)
{
    // Check whether we have "+" or "-"

    int sign = 0;

    if (isTokenType(pDomNode, Scanner::Token::Plus)) {
        sign = 1;

        mScanner.getNextToken();
    } else if (isTokenType(pDomNode, Scanner::Token::Minus)) {
        sign = -1;

        mScanner.getNextToken();
    }

    // Expect a strictly positive integer number

    static const std::string ExpectedString = "A strictly positive integer number";

    if (!tokenType(pDomNode, ExpectedString,
                   Scanner::Token::Number)) {
        return false;
    }

    // We have got a number, but now the question is whether it is a strictly
    // positive integer one

    int number;
    toInt(mScanner.string(), number);

    if (sign == -1) {
        number = -number;
    }

    if (number <= 0) {
        std::string foundString = mScanner.string();

        if (sign == 1) {
            foundString = "+"+foundString;
        } else if (sign == -1) {
            foundString = "-"+foundString;
        }

        addUnexpectedTokenErrorMessage(ExpectedString, std::format("'{}'", foundString));

        return false;
    }

    return true;
}



std::string Parser::processCommentString(const std::string &pComment)
{
    // From https://w3.org/TR/xml/#sec-comments, we can see that XML comments
    // cannot have "--" within them, yet we want to allow them in a comment and
    // one way to allow this is by replacing all occurrences of "--" with its
    // corresponding decimal HTML entity

    std::string realComment = pComment;

    utils::replaceAll(realComment, "--", "&#45;&#45;");

    return realComment;
}



void Parser::parseComments(utils::XmlNodePtr &pDomNode)
{
    // Check whether there are some comments

    int prevLineCommentLine = 0;
    std::string singleLineComments;

    while (true) {
        if (mScanner.token() == Scanner::Token::SingleLineComment) {
            // Keep track of the line comment, if no previous line comments are
            // being tracked

            if (prevLineCommentLine == 0) {
                singleLineComments = processCommentString(mScanner.string());
            } else {
                // There is at least one other line comment that is being
                // tracked, so compare line numbers

                if (mScanner.line() == prevLineCommentLine+1) {
                    // The line comment is directly on the line following the
                    // previous line comment, so add it to the list of tracked
                    // line comments

                    singleLineComments += "\n"+processCommentString(mScanner.string());
                } else {
                    // The line comment is not directly on the line following
                    // the previous line comment, so add the previous line
                    // comment(s) to the current node and keep track of the new
                    // line comment
                    // Note #1: we add a space to the comment in case it is
                    //          empty so that we can still have an "empty"
                    //          comment in the resulting XML document...
                    // Note #2: we check whether pDomNode is null in case we
                    //          have a comment before the first element of the
                    //          document (i.e. the model element)...

                    auto comment = utils::createNode(utils::XmlNodeType::Comment, singleLineComments.empty() ? " " : singleLineComments);

                    if (pDomNode) {
                        pDomNode->addChild(comment);
                    }

                    singleLineComments = processCommentString(mScanner.string());
                }
            }

            prevLineCommentLine = mScanner.line();
        } else if (mScanner.token() == Scanner::Token::MultilineComment) {
            // We simply ignore the multiline comment

            ;
        } else {
            // No (more) comment(s left), so add the tracked line comment(s) to
            // the current node, if any, and leave

            if (prevLineCommentLine != 0) {
                // Note: see the two notes above...

                auto comment = utils::createNode(utils::XmlNodeType::Comment, singleLineComments.empty() ? " " : singleLineComments);

                if (pDomNode) {
                    pDomNode->addChild(comment);
                }
            }

            return;
        }

        // Fetch the next token

        mScanner.getNextToken();
    }
}


bool Parser::parseMathematicalExpression(utils::XmlNodePtr &pDomNode,
                                                       bool pFullParsing)
{
    // Check whether we have got an identifier or "ode"

    utils::XmlNodePtr lhsElement;

    if (mScanner.token() == Scanner::Token::IdentifierOrCmetaId) {
        lhsElement = newIdentifierElement(mScanner.string());
    } else if (mScanner.token() == Scanner::Token::Ode) {
        lhsElement = parseDerivativeIdentifier(pDomNode);
    }

    // Check whether we have got an LHS element

    if (lhsElement == nullptr) {
        return false;
    }

    // Expect "="

    mScanner.getNextToken();

    if (!eqToken(pDomNode)) {
        return false;
    }

    // Create our apply element

    utils::XmlNodePtr applyElement = newDomElement(pDomNode, "apply");

    // Try to parse for a cmeta:id

    mScanner.getNextToken();

    // if (!parseCmetaId(applyElement)) {
    //     return !pFullParsing;
    // }

    // At this stage, we are done when it comes to partial parsing

    if (!pFullParsing) {
        if (mScanner.token() == Scanner::Token::Sel) {
            // We have "sel", but is that for a sel() function or a sel...endsel
            // statement?

            utils::XmlNodePtr dummyNode;

            mScanner.getNextToken();

            mStatement = isTokenType(dummyNode, Scanner::Token::OpeningBracket)?
                             Statement::Normal:
                             Statement::PiecewiseSel;
        } else {
            mStatement = Statement::Normal;
        }

        return true;
    }

    // Expect either a normal or a piecewise mathematical expression

    utils::XmlNodePtr rhsElement;

    if (mScanner.token() == Scanner::Token::Sel) {
        // We have "sel", but is that for a sel() function or a sel...endsel
        // statement?

        utils::XmlNodePtr dummyNode;
        Scanner origScanner(mScanner);

        mScanner.getNextToken();

        bool selFunction = isTokenType(dummyNode, Scanner::Token::OpeningBracket);

        mScanner = origScanner;

        rhsElement = selFunction?
                         parseNormalMathematicalExpression(pDomNode):
                         parsePiecewiseMathematicalExpression(pDomNode, true);
    } else {
        rhsElement = parseNormalMathematicalExpression(pDomNode);
    }

    if (rhsElement == nullptr) {
        return false;
    }

    // Expect ";"

    if (!semiColonToken(pDomNode)) {
        return false;
    }

    // Populate our apply element

    newDomElement(applyElement, "eq");

    applyElement->addChild(lhsElement);
    applyElement->addChild(rhsElement);

    return true;
}



std::string Parser::mathmlName(Scanner::Token pTokenType) const
{
    // Return the MathML name of the given token type

    if (pTokenType == Scanner::Token::And) {
        return "and";
    }

    if (pTokenType == Scanner::Token::Or) {
        return "or";
    }

    if (pTokenType == Scanner::Token::Xor) {
        return "xor";
    }

    if (pTokenType == Scanner::Token::Not) {
        return "not";
    }

    if (pTokenType == Scanner::Token::Abs) {
        return "abs";
    }

    if (pTokenType == Scanner::Token::Ceil) {
        return "ceiling";
    }

    if (pTokenType == Scanner::Token::Exp) {
        return "exp";
    }

    if (pTokenType == Scanner::Token::Fact) {
        return "factorial";
    }

    if (pTokenType == Scanner::Token::Floor) {
        return "floor";
    }

    if (pTokenType == Scanner::Token::Ln) {
        return "ln";
    }

    if (pTokenType == Scanner::Token::Sqr) {
        return "power";
    }

    if (pTokenType == Scanner::Token::Sqrt) {
        return "root";
    }

    if (pTokenType == Scanner::Token::Min) {
        return "min";
    }

    if (pTokenType == Scanner::Token::Max) {
        return "max";
    }

    if (pTokenType == Scanner::Token::Gcd) {
        return "gcd";
    }

    if (pTokenType == Scanner::Token::Lcm) {
        return "lcm";
    }

    if (pTokenType == Scanner::Token::Sin) {
        return "sin";
    }

    if (pTokenType == Scanner::Token::Cos) {
        return "cos";
    }

    if (pTokenType == Scanner::Token::Tan) {
        return "tan";
    }

    if (pTokenType == Scanner::Token::Sec) {
        return "sec";
    }

    if (pTokenType == Scanner::Token::Csc) {
        return "csc";
    }

    if (pTokenType == Scanner::Token::Cot) {
        return "cot";
    }

    if (pTokenType == Scanner::Token::Sinh) {
        return "sinh";
    }

    if (pTokenType == Scanner::Token::Cosh) {
        return "cosh";
    }

    if (pTokenType == Scanner::Token::Tanh) {
        return "tanh";
    }

    if (pTokenType == Scanner::Token::Sech) {
        return "sech";
    }

    if (pTokenType == Scanner::Token::Csch) {
        return "csch";
    }

    if (pTokenType == Scanner::Token::Coth) {
        return "coth";
    }

    if (pTokenType == Scanner::Token::Asin) {
        return "arcsin";
    }

    if (pTokenType == Scanner::Token::Acos) {
        return "arccos";
    }

    if (pTokenType == Scanner::Token::Atan) {
        return "arctan";
    }

    if (pTokenType == Scanner::Token::Asec) {
        return "arcsec";
    }

    if (pTokenType == Scanner::Token::Acsc) {
        return "arccsc";
    }

    if (pTokenType == Scanner::Token::Acot) {
        return "arccot";
    }

    if (pTokenType == Scanner::Token::Asinh) {
        return "arcsinh";
    }

    if (pTokenType == Scanner::Token::Acosh) {
        return "arccosh";
    }

    if (pTokenType == Scanner::Token::Atanh) {
        return "arctanh";
    }

    if (pTokenType == Scanner::Token::Asech) {
        return "arcsech";
    }

    if (pTokenType == Scanner::Token::Acsch) {
        return "arccsch";
    }

    if (pTokenType == Scanner::Token::Acoth) {
        return "arccoth";
    }

    if (pTokenType == Scanner::Token::Log) {
        return "log";
    }

    if (pTokenType == Scanner::Token::Pow) {
        return "power";
    }

    if (pTokenType == Scanner::Token::Rem) {
        return "rem";
    }

    if (pTokenType == Scanner::Token::Root) {
        return "root";
    }

    if (pTokenType == Scanner::Token::True) {
        return "true";
    }

    if (pTokenType == Scanner::Token::False) {
        return "false";
    }

    if (pTokenType == Scanner::Token::Nan) {
        return "notanumber";
    }

    if (pTokenType == Scanner::Token::Pi) {
        return "pi";
    }

    if (pTokenType == Scanner::Token::Inf) {
        return "infinity";
    }

    if (pTokenType == Scanner::Token::E) {
        return "exponentiale";
    }

    if (pTokenType == Scanner::Token::EqEq) {
        return "eq";
    }

    if (pTokenType == Scanner::Token::Neq) {
        return "neq";
    }

    if (pTokenType == Scanner::Token::Lt) {
        return "lt";
    }

    if (pTokenType == Scanner::Token::Leq) {
        return "leq";
    }

    if (pTokenType == Scanner::Token::Gt) {
        return "gt";
    }

    if (pTokenType == Scanner::Token::Geq) {
        return "geq";
    }

    if (pTokenType == Scanner::Token::Plus) {
        return "plus";
    }

    if (pTokenType == Scanner::Token::Minus) {
        return "minus";
    }

    if (pTokenType == Scanner::Token::Times) {
        return "times";
    }

    if (pTokenType == Scanner::Token::Divide) {
        return "divide";
    }

    return "???";
}



utils::XmlNodePtr Parser::parseDerivativeIdentifier(utils::XmlNodePtr &pDomNode)
{
    // At this stage, we have already come across "ode", so now expect "("

    mScanner.getNextToken();

    if (!openingBracketToken(pDomNode)) {
        return {};
    }

    // Expect an identifier

    mScanner.getNextToken();

    if (!identifierToken(pDomNode)) {
        return {};
    }

    // Keep track of our f

    std::string f = mScanner.string();

    // Expect ","

    mScanner.getNextToken();

    if (!commaToken(pDomNode)) {
        return {};
    }

    // Expect an identifier

    mScanner.getNextToken();

    if (!identifierToken(pDomNode)) {
        return {};
    }

    // Keep track of our x

    std::string x = mScanner.string();

    // Expect "," or ")"

    static const Scanner::Tokens Tokens = { Scanner::Token::Comma,
                                                          Scanner::Token::ClosingBracket };

    mScanner.getNextToken();

    if (!tokenType(pDomNode, "',' or ')'",
                   Tokens)) {
        return {};
    }

    // Check what we got exactly

    if (mScanner.token() == Scanner::Token::Comma) {
        // Expect a strictly positive integer number

        mScanner.getNextToken();

        if (!strictlyPositiveIntegerNumberToken(pDomNode)) {
            return {};
        }

        // Keep track of our order

        std::string order = mScanner.string();

        if (mCellmlMode) {
            // Expect "{"

            mScanner.getNextToken();

            if (!openingCurlyBracketToken(pDomNode)) {
                return {};
            }

            // Expect "dimensionless"

            mScanner.getNextToken();

            if (!dimensionlessToken(pDomNode)) {
                return {};
            }

            // Expect "}"

            mScanner.getNextToken();

            if (!closingCurlyBracketToken(pDomNode)) {
                return {};
            }
        }

        // Expect ")"

        mScanner.getNextToken();

        if (!closingBracketToken(pDomNode)) {
            return {};
        }

        // Return a derivative element with an order

        return newDerivativeElement(f, x, order);
    }

    // Return a derivative element with no order

    return newDerivativeElement(f, x);
}



utils::XmlNodePtr Parser::parseNumber(utils::XmlNodePtr &pDomNode)
{
    // Keep track of the number
    // Note: this is useful to do in case the number is not valid (e.g. too big,
    //       too small)...

    std::string number = mScanner.string();
    std::string unit = "";

    if (mCellmlMode) {
        // Expect "{"

        mScanner.getNextToken();

        if (!openingCurlyBracketToken(pDomNode)) {
            return {};
        }

        // Expect an identifier or an SI unit

        mScanner.getNextToken();

        if (!identifierOrSiUnitToken(pDomNode)) {
            return {};
        }

        // Keep track of the unit for our number

        unit = mScanner.string();

        // Expect "}"

        mScanner.getNextToken();

        if (!closingCurlyBracketToken(pDomNode)) {
            return {};
        }
    }

    // Return a number element

    return newNumberElement(number, unit);
}



utils::XmlNodePtr Parser::parseMathematicalFunction(utils::XmlNodePtr &pDomNode,
                                                            bool pOneArgument,
                                                            bool pTwoArguments,
                                                            bool pMoreArguments)
{
    // Keep track of the mathematical function

    Scanner::Token tokenType = mScanner.token();

    // Expect "("

    mScanner.getNextToken();

    if (!openingBracketToken(pDomNode)) {
        return {};
    }

    // Try to parse the first argument as a normal mathematical expression

    mScanner.getNextToken();

    std::vector<utils::XmlNodePtr> argumentElements;
    utils::XmlNodePtr argumentElement = parseNormalMathematicalExpression(pDomNode);

    if (argumentElement == nullptr) {
        return {};
    }

    argumentElements.push_back(argumentElement);

    // Check whether we expect or might expect a second argument

    if (   (   pOneArgument && pTwoArguments
            && isTokenType(pDomNode, Scanner::Token::Comma))
        || (!pOneArgument && pTwoArguments)) {
        // Expect ",", should we always be expecting a second argument

        if (!pOneArgument && pTwoArguments && !commaToken(pDomNode)) {
            return {};
        }

        // Try to parse the second argument as a normal mathematical expression

        mScanner.getNextToken();

        argumentElement = parseNormalMathematicalExpression(pDomNode);

        if (argumentElement == nullptr) {
            return {};
        }

        argumentElements.push_back(argumentElement);
    }

    // Check whether we might expect more arguments

    while (   pTwoArguments && pMoreArguments
           && isTokenType(pDomNode, Scanner::Token::Comma)) {
        // Try to parse the nth argument as a normal mathematical expression

        mScanner.getNextToken();

        argumentElement = parseNormalMathematicalExpression(pDomNode);

        if (argumentElement == nullptr) {
            return {};
        }

        argumentElements.push_back(argumentElement);
    }

    // Expect ")"

    if (!closingBracketToken(pDomNode)) {
        return {};
    }

    // Return a mathematical function element

    return newMathematicalFunctionElement(tokenType, argumentElements);
}



utils::XmlNodePtr Parser::parseParenthesizedMathematicalExpression(utils::XmlNodePtr &pDomNode)
{
    // Try to parse a normal mathematical expression

    mScanner.getNextToken();

    utils::XmlNodePtr res = parseNormalMathematicalExpression(pDomNode);

    if (res == nullptr) {
        return {};
    }

    // Expect ")"

    if (!closingBracketToken(pDomNode)) {
        return {};
    }

    // Return our normal mathematical expression

    return res;
}



utils::XmlNodePtr Parser::parseMathematicalExpressionElement(utils::XmlNodePtr &pDomNode,
                                                                     const Scanner::Tokens &pTokens,
                                                                     ParseNormalMathematicalExpressionFunction pFunction)
{
    // Try to parse something of the form:
    //
    //     <operand1> [<operator> <operand2> <operator> <operand3> ...]
    //
    // with <operandn> the result of a call to pFunction

    // Expect an operand

    utils::XmlNodePtr res = (this->*pFunction)(pDomNode);

    if (res == nullptr) {
        return {};
    }

    // Loop while we have a valid operator and operand

    Scanner::Token prevOperator = Scanner::Token::Unknown;

    while(true) {
        // Try to parse comments, if any

        parseComments(pDomNode);

        // Expect an operator

        Scanner::Token crtOperator = mScanner.token();

        if (!containsToken(pTokens, crtOperator)) {
            return res;
        }

        // Expect an operand

        mScanner.getNextToken();

        utils::XmlNodePtr otherOperand = (this->*pFunction)(pDomNode);

        if (otherOperand == nullptr) {
            return {};
        }

        // Update our DOM tree with our operator and operand
        // Note #1: the check against the previous operator only makes sense for
        //          n-ary operators, i.e. "plus", "times", "and", "or" and
        //          "xor"...
        // Note #2: see https://w3.org/TR/MathML2/chapter4.html#contm.funopqual
        //          for more information...

        static const Scanner::Tokens NaryOperators = { Scanner::Token::Plus,
                                                                     Scanner::Token::Times,
                                                                     Scanner::Token::And,
                                                                     Scanner::Token::Or,
                                                                     Scanner::Token::Xor };

        if ((crtOperator == prevOperator) && containsToken(NaryOperators, crtOperator)) {
            res->addChild(otherOperand);
        } else {
            // Create an apply element and populate it with our operator and two
            // operands

            utils::XmlNodePtr applyElement = utils::createNode(utils::XmlNodeType::Element, "apply");

            applyElement->addChild(utils::createNode(utils::XmlNodeType::Element, mathmlName(crtOperator)));
            applyElement->addChild(res);
            applyElement->addChild(otherOperand);

            // Make our apply element our new result element

            res = applyElement;
        }

        // Keep track of our operator

        prevOperator = crtOperator;
    }
}



utils::XmlNodePtr Parser::parseNormalMathematicalExpression(utils::XmlNodePtr &pDomNode)
{
    // Look for "or"

    return parseMathematicalExpressionElement(pDomNode,
                                              { Scanner::Token::Or },
                                              &Parser::parseNormalMathematicalExpression2);
}



utils::XmlNodePtr Parser::parseNormalMathematicalExpression2(utils::XmlNodePtr &pDomNode)
{
    // Look for "and"

    return parseMathematicalExpressionElement(pDomNode,
                                              { Scanner::Token::And },
                                              &Parser::parseNormalMathematicalExpression3);
}



utils::XmlNodePtr Parser::parseNormalMathematicalExpression3(utils::XmlNodePtr &pDomNode)
{
    // Look for "xor"

    return parseMathematicalExpressionElement(pDomNode,
                                              { Scanner::Token::Xor },
                                              &Parser::parseNormalMathematicalExpression4);
}



utils::XmlNodePtr Parser::parseNormalMathematicalExpression4(utils::XmlNodePtr &pDomNode)
{
    // Look for "==" or "<>"

    return parseMathematicalExpressionElement(pDomNode,
                                              { Scanner::Token::EqEq,
                                                Scanner::Token::Neq },
                                              &Parser::parseNormalMathematicalExpression5);
}



utils::XmlNodePtr Parser::parseNormalMathematicalExpression5(utils::XmlNodePtr &pDomNode)
{
    // Look for "<", ">", "<=" or ">="

    return parseMathematicalExpressionElement(pDomNode,
                                              { Scanner::Token::Lt,
                                                Scanner::Token::Gt,
                                                Scanner::Token::Leq,
                                                Scanner::Token::Geq },
                                              &Parser::parseNormalMathematicalExpression6);
}



utils::XmlNodePtr Parser::parseNormalMathematicalExpression6(utils::XmlNodePtr &pDomNode)
{
    // Look for "+" or "-"

    return parseMathematicalExpressionElement(pDomNode,
                                              { Scanner::Token::Plus,
                                                Scanner::Token::Minus },
                                              &Parser::parseNormalMathematicalExpression7);
}



utils::XmlNodePtr Parser::parseNormalMathematicalExpression7(utils::XmlNodePtr &pDomNode)
{
    // Look for "*" or "/"

    return parseMathematicalExpressionElement(pDomNode,
                                              { Scanner::Token::Times,
                                                Scanner::Token::Divide },
                                              &Parser::parseNormalMathematicalExpression8);
}



utils::XmlNodePtr Parser::parseNormalMathematicalExpression8(utils::XmlNodePtr &pDomNode)
{
    // Try to parse comments, if any

    parseComments(pDomNode);

    // Look for "not", unary "+" or unary "-"

    static const Scanner::Tokens Tokens = { Scanner::Token::Not,
                                                          Scanner::Token::Plus,
                                                          Scanner::Token::Minus };

    Scanner::Token crtOperator = mScanner.token();

    if (containsToken(Tokens, crtOperator)) {
        utils::XmlNodePtr operand;

        if (mScanner.token() == Scanner::Token::Not) {
            mScanner.getNextToken();

            operand = parseNormalMathematicalExpression(pDomNode);
        } else {
            mScanner.getNextToken();

            operand = parseNormalMathematicalExpression8(pDomNode);
        }

        if (operand == nullptr) {
            return {};
        }

        // Create and return an apply element that has been populated with our
        // operator and operand

        utils::XmlNodePtr res = utils::createNode(utils::XmlNodeType::Element, "apply");

        res->addChild(utils::createNode(utils::XmlNodeType::Element, mathmlName(crtOperator)));
        res->addChild(operand);

        return res;
    }

    return parseNormalMathematicalExpression9(pDomNode);
}



utils::XmlNodePtr Parser::parseNormalMathematicalExpression9(utils::XmlNodePtr &pDomNode)
{
    // Look for an identifier, "ode", a number, a mathematical constant, a
    // mathematical function or an opening bracket

    utils::XmlNodePtr res;

    static Scanner::Tokens mahematicalConstantTokens;
    static Scanner::Tokens oneArgumentMathematicalFunctionTokens;
    static Scanner::Tokens oneOrTwoArgumentMathematicalFunctionTokens;
    static Scanner::Tokens twoArgumentMathematicalFunctionTokens;
    static Scanner::Tokens twoOrMoreArgumentMathematicalFunctionTokens;
    static bool needInitializeTokens = true;

    if (needInitializeTokens) {
        mahematicalConstantTokens = rangeOfTokens(Scanner::Token::FirstMathematicalConstant,
                                                  Scanner::Token::LastMathematicalConstant);
        oneArgumentMathematicalFunctionTokens = rangeOfTokens(Scanner::Token::FirstOneArgumentMathematicalFunction,
                                                              Scanner::Token::LastOneArgumentMathematicalFunction);
        oneOrTwoArgumentMathematicalFunctionTokens = rangeOfTokens(Scanner::Token::FirstOneOrTwoArgumentMathematicalFunction,
                                                                   Scanner::Token::LastOneOrTwoArgumentMathematicalFunction);
        twoArgumentMathematicalFunctionTokens = rangeOfTokens(Scanner::Token::FirstTwoArgumentMathematicalFunction,
                                                              Scanner::Token::LastTwoArgumentMathematicalFunction);
        twoOrMoreArgumentMathematicalFunctionTokens = rangeOfTokens(Scanner::Token::FirstTwoOrMoreArgumentMathematicalFunction,
                                                                    Scanner::Token::LastTwoOrMoreArgumentMathematicalFunction);

        needInitializeTokens = false;
    }

    if (mScanner.token() == Scanner::Token::IdentifierOrCmetaId) {
        // Create an identifier element

        res = newIdentifierElement(mScanner.string());
    } else if (mScanner.token() == Scanner::Token::Ode) {
        // Try to parse a derivative identifier

        res = parseDerivativeIdentifier(pDomNode);
    } else if (mScanner.token() == Scanner::Token::Number) {
        // Try to parse a number

        res = parseNumber(pDomNode);
    } else if (containsToken(mahematicalConstantTokens, mScanner.token())) {
        // Create a mathematical constant element

        res = newMathematicalConstantElement(mScanner.token());
    } else if (containsToken(oneArgumentMathematicalFunctionTokens, mScanner.token())) {
        // Try to parse a one-argument mathematical function

        res = parseMathematicalFunction(pDomNode, true, false, false);
    } else if (mScanner.token() == Scanner::Token::Sel) {
        // Try to parse a piecewise statement using the sel() function

        res = parsePiecewiseMathematicalExpression(pDomNode);
    } else if (containsToken(oneOrTwoArgumentMathematicalFunctionTokens, mScanner.token())) {
        // Try to parse a one- or two-argument mathematical function

        res = parseMathematicalFunction(pDomNode, true, true, false);
    } else if (containsToken(twoArgumentMathematicalFunctionTokens, mScanner.token())) {
        // Try to parse a two-argument mathematical function

        res = parseMathematicalFunction(pDomNode, false, true, false);
    } else if (containsToken(twoOrMoreArgumentMathematicalFunctionTokens, mScanner.token())) {
        // Try to parse a two-or-more argument mathematical function

        res = parseMathematicalFunction(pDomNode, false, true, true);
    } else if (mScanner.token() == Scanner::Token::OpeningBracket) {
        // Try to parse a parenthesised mathematical expression

        res = parseParenthesizedMathematicalExpression(pDomNode);
    } else {
        std::string foundString = mScanner.string();

        if (mScanner.token() != Scanner::Token::Eof) {
            foundString = std::format("'{}'", foundString);
        }

        addUnexpectedTokenErrorMessage("An identifier, 'ode', a number, a mathematical function, a mathematical constant or '('", foundString);

        return {};
    }

    // Fetch the next token and return our result element

    mScanner.getNextToken();

    return res;
}



utils::XmlNodePtr Parser::parsePiecewiseMathematicalExpression(utils::XmlNodePtr &pDomNode,
                                                                       bool pAllowTopPiecewiseStatement)
{
    // If a top piecewise statement is allowed then check whether we have "(",
    // in which case it will mean that we are using the sel() function rather
    // than the sel...endsel statement, or expect "(" if a top piecewise
    // statement is not allowed

    mScanner.getNextToken();

    bool selFunction = true;

    if (pAllowTopPiecewiseStatement) {
        selFunction = isTokenType(pDomNode, Scanner::Token::OpeningBracket);
    } else if (!openingBracketToken(pDomNode)) {
        return {};
    }

    // Loop while we have "case" or "otherwise", or leave if we get ")" in the
    // case of the sel() function or "endsel" otherwise

    static const Scanner::Tokens OpeningBracketCaseOtherwiseTokens = { Scanner::Token::OpeningBracket,
                                                                                     Scanner::Token::Case,
                                                                                     Scanner::Token::Otherwise };
    static const Scanner::Tokens CaseOtherwiseTokens = { Scanner::Token::Case,
                                                                       Scanner::Token::Otherwise };
    static const Scanner::Tokens CommaClosingBracketTokens = { Scanner::Token::Comma,
                                                                             Scanner::Token::ClosingBracket };
    static const Scanner::Tokens CaseOtherwiseEndSelTokens = { Scanner::Token::Case,
                                                                             Scanner::Token::Otherwise,
                                                                             Scanner::Token::EndSel };

    utils::XmlNodePtr piecewiseElement = newDomElement(mDomDocument, "piecewise");
    bool hasOtherwiseClause = false;

    if (selFunction) {
        mScanner.getNextToken();

        if (!tokenType(piecewiseElement, "'case' or 'otherwise'", CaseOtherwiseTokens)) {
            return {};
        }
    } else if (!tokenType(piecewiseElement, "'(', 'case' or 'otherwise'", OpeningBracketCaseOtherwiseTokens)) {
        return {};
    }

    do {
        bool caseClause = mScanner.token() == Scanner::Token::Case;
        utils::XmlNodePtr conditionElement;

        if (caseClause) {
            // Expect a condition in the form of a normal mathematical equation

            mScanner.getNextToken();

            conditionElement = parseNormalMathematicalExpression(piecewiseElement);

            if (conditionElement == nullptr) {
                return {};
            }
        } else if (hasOtherwiseClause) {
            mMessages.push_back(ParserMessage(ParserMessage::Type::Error,
                                                     mScanner.line(),
                                                     mScanner.column(),
                                                     "There can only be one 'otherwise' clause."));

            return {};
        } else {
            hasOtherwiseClause = true;

            // Fetch the next token

            mScanner.getNextToken();
        }

        // Expect ":"

        if (!colonToken(piecewiseElement)) {
            return {};
        }

        // Expect an expression in the form of a normal mathematical equation

        mScanner.getNextToken();

        utils::XmlNodePtr expressionElement = parseNormalMathematicalExpression(piecewiseElement);

        if (expressionElement == nullptr) {
            return {};
        }

        // Expect "," in the case of the sel() function or ";" otherwise

        if (   ( selFunction && !tokenType(piecewiseElement, "',' or ')'", CommaClosingBracketTokens))
            || (!selFunction && !semiColonToken(piecewiseElement))) {
            return {};
        }

        // Create and populate our piece/otherwise element, and add it to our
        // piecewise element

        utils::XmlNodePtr pieceOrOtherwiseElement = newDomElement(mDomDocument, caseClause ? "piece" : "otherwise");

        pieceOrOtherwiseElement->addChild(expressionElement);

        if (caseClause) {
            pieceOrOtherwiseElement->addChild(conditionElement);
        }

        piecewiseElement->addChild(pieceOrOtherwiseElement);

        // Fetch the next token and consider ourselves done if we have ")" in
        // the case of the sel() function or "endsel" otherwise

        if (    (selFunction && (mScanner.token() == Scanner::Token::Comma))
            || !selFunction) {
            mScanner.getNextToken();
        }
    } while (   !(   ( selFunction && isTokenType(pDomNode, Scanner::Token::ClosingBracket))
                  || (!selFunction && isTokenType(pDomNode, Scanner::Token::EndSel)))
             &&  (   ( selFunction && tokenType(piecewiseElement, "'case' or 'otherwise'", CaseOtherwiseTokens))
                  || (!selFunction && tokenType(piecewiseElement, "'case', 'otherwise' or 'endsel'", CaseOtherwiseEndSelTokens))));

    // Expect ")" in the case of the sel() function or "endsel" otherwise

    if (   ( selFunction && !closingBracketToken(pDomNode))
        || (!selFunction && !endselToken(pDomNode))) {
        return {};
    }

    // Fetch the next token and return our piecewise element

    if (pAllowTopPiecewiseStatement) {
        mScanner.getNextToken();
    }

    return piecewiseElement;
}


bool Parser::cellmlMode() const
{
    return mCellmlMode;
}


void Parser::setCellmlMode(bool pState)
{
    mCellmlMode = pState;
}

}
