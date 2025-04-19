
#pragma once


#include <list>
#include <map>
#include <string>

#include "scanner.h"
#include "utils/xmllite.h"

namespace CellMLText {

class ParserMessage
{
public:
    enum class Type {
        Error,
        Warning
    };

    explicit ParserMessage(Type pType, int pLine, int pColumn,
                                         const std::string &pMessage);

    Type type() const;

    int line() const;
    int column() const;

    std::string message() const;

private:
    Type mType;

    int mLine;
    int mColumn;

    std::string mMessage;
};

using ParserMessages = std::list<ParserMessage>;

class Parser;



using ParseNormalMathematicalExpressionFunction = utils::XmlNodePtr (Parser::*)(utils::XmlNodePtr &pDomNode);



class Parser
{

public:
    enum class Statement {
        Unknown,
        Normal,
        PiecewiseSel,
        PiecewiseCase,
        PiecewiseOtherwise,
        PiecewiseEndSel
    };

    bool execute(const std::string &pCellmlText);
    bool execute(const std::string &pCellmlText, bool pFullParsing, bool cellmlMode);

    utils::XmlNodePtr domDocument() const;
    // utils::XmlNodePtr modelElement() const;

    ParserMessages messages() const;

    Statement statement() const;

    bool cellmlMode() const;
    void setCellmlMode(bool pState);

private:
    bool mCellmlMode = true;
    Scanner mScanner;

    utils::XmlNodePtr mDomDocument;
    utils::XmlNodePtr mMathElement;

    ParserMessages mMessages;

    std::map<std::string, std::string> mNamespaces;

    Statement mStatement = Statement::Unknown;

    void initialize(const std::string &pCellmlText, bool pCellmlMode = true);

    void addUnexpectedTokenErrorMessage(const std::string &pExpectedString,
                                        const std::string &pFoundString);

    utils::XmlNodePtr newDomElement(utils::XmlNodePtr pDomNode, const std::string &pElementName);

    utils::XmlNodePtr newIdentifierElement(const std::string &pValue);
    utils::XmlNodePtr newDerivativeElement(const std::string &pF, const std::string &pX);
    utils::XmlNodePtr newDerivativeElement(const std::string &pF, const std::string &pX,
                                     const std::string &pOrder);
    utils::XmlNodePtr newNumberElement(const std::string &pNumber, const std::string &pUnit);
    utils::XmlNodePtr newMathematicalConstantElement(Scanner::Token pTokenType);
    utils::XmlNodePtr newMathematicalFunctionElement(Scanner::Token pTokenType,
                                                     const std::vector<utils::XmlNodePtr> &pArgumentElements);

    Scanner::Tokens rangeOfTokens(Scanner::Token pFromTokenType,
                                                Scanner::Token pToTokenType);

    bool tokenType(utils::XmlNodePtr &pDomNode, const std::string &pExpectedString,
                   const Scanner::Tokens &pTokens);
    bool tokenType(utils::XmlNodePtr &pDomNode, const std::string &pExpectedString,
                   Scanner::Token pTokenType);
    bool isTokenType(utils::XmlNodePtr &pDomNode,
                     Scanner::Token pTokenType);

    // bool andToken(utils::XmlNodePtr &pDomNode);
    // bool asToken(utils::XmlNodePtr &pDomNode);
    // bool betweenToken(utils::XmlNodePtr &pDomNode);
    bool caseToken(utils::XmlNodePtr &pDomNode);
    bool closingBracketToken(utils::XmlNodePtr &pDomNode);
    bool closingCurlyBracketToken(utils::XmlNodePtr &pDomNode);
    bool colonToken(utils::XmlNodePtr &pDomNode);
    bool commaToken(utils::XmlNodePtr &pDomNode);
    // bool compToken(utils::XmlNodePtr &pDomNode);
    // bool defToken(utils::XmlNodePtr &pDomNode);
    bool dimensionlessToken(utils::XmlNodePtr &pDomNode);
    // bool endcompToken(utils::XmlNodePtr &pDomNode);
    // bool enddefToken(utils::XmlNodePtr &pDomNode);
    bool endselToken(utils::XmlNodePtr &pDomNode);
    bool eqToken(utils::XmlNodePtr &pDomNode);
    // bool forToken(utils::XmlNodePtr &pDomNode);
    bool identifierOrSiUnitToken(utils::XmlNodePtr &pDomNode);
    bool identifierToken(utils::XmlNodePtr &pDomNode);
    bool cmetaIdToken(utils::XmlNodePtr &pDomNode);
    // bool modelToken(utils::XmlNodePtr &pDomNode);
    bool numberToken(utils::XmlNodePtr &pDomNode);
    bool numberValueToken(utils::XmlNodePtr &pDomNode, int &pSign);
    bool openingBracketToken(utils::XmlNodePtr &pDomNode);
    bool openingCurlyBracketToken(utils::XmlNodePtr &pDomNode);
    bool semiColonToken(utils::XmlNodePtr &pDomNode);
    bool strictlyPositiveIntegerNumberToken(utils::XmlNodePtr &pDomNode);
    // bool unitToken(utils::XmlNodePtr &pDomNode);
    // bool usingToken(utils::XmlNodePtr &pDomNode);

    std::string processCommentString(const std::string &pComment);
    void parseComments(utils::XmlNodePtr &pDomNode);
    // bool parseCmetaId(utils::XmlNodePtr &pDomElement);
    // bool parseModelDefinition(utils::XmlNodePtr &pDomNode);
    // bool parseImportDefinition(utils::XmlNodePtr &pDomNode);
    // bool parseUnitsDefinition(utils::XmlNodePtr &pDomNode);
    // bool parseUnitDefinition(utils::XmlNodePtr &pDomNode);
    // bool parseComponentDefinition(utils::XmlNodePtr &pDomNode);
    // bool parseVariableDeclaration(utils::XmlNodePtr &pDomNode);
    bool parseMathematicalExpression(utils::XmlNodePtr &pDomNode,
                                     bool pFullParsing = true);
    // bool parseGroupDefinition(utils::XmlNodePtr &pDomNode);
    // bool parseComponentRefDefinition(utils::XmlNodePtr &pDomNode);
    // bool parseMapDefinition(utils::XmlNodePtr &pDomNode);

    std::string mathmlName(Scanner::Token pTokenType) const;

    utils::XmlNodePtr parseDerivativeIdentifier(utils::XmlNodePtr &pDomNode);
    utils::XmlNodePtr parseNumber(utils::XmlNodePtr &pDomNode);
    utils::XmlNodePtr parseMathematicalFunction(utils::XmlNodePtr &pDomNode, bool pOneArgument,
                                          bool pTwoArguments,
                                          bool pMoreArguments);
    utils::XmlNodePtr parseParenthesizedMathematicalExpression(utils::XmlNodePtr &pDomNode);

    utils::XmlNodePtr parseMathematicalExpressionElement(utils::XmlNodePtr &pDomNode,
                                                   const Scanner::Tokens &pTokens,
                                                   ParseNormalMathematicalExpressionFunction pFunction);
    utils::XmlNodePtr parseNormalMathematicalExpression(utils::XmlNodePtr &pDomNode);
    utils::XmlNodePtr parseNormalMathematicalExpression2(utils::XmlNodePtr &pDomNode);
    utils::XmlNodePtr parseNormalMathematicalExpression3(utils::XmlNodePtr &pDomNode);
    utils::XmlNodePtr parseNormalMathematicalExpression4(utils::XmlNodePtr &pDomNode);
    utils::XmlNodePtr parseNormalMathematicalExpression5(utils::XmlNodePtr &pDomNode);
    utils::XmlNodePtr parseNormalMathematicalExpression6(utils::XmlNodePtr &pDomNode);
    utils::XmlNodePtr parseNormalMathematicalExpression7(utils::XmlNodePtr &pDomNode);
    utils::XmlNodePtr parseNormalMathematicalExpression8(utils::XmlNodePtr &pDomNode);
    utils::XmlNodePtr parseNormalMathematicalExpression9(utils::XmlNodePtr &pDomNode);
    utils::XmlNodePtr parsePiecewiseMathematicalExpression(utils::XmlNodePtr &pDomNode,
                                                     bool pAllowTopPiecewiseStatement = false);

    // void moveTrailingComments(utils::XmlNodePtr &pFromDomNode, utils::XmlNodePtr &pToDomNode);
};

}

