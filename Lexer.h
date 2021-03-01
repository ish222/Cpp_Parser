#ifndef BASIC_PARSER_C___LEXER_H
#define BASIC_PARSER_C___LEXER_H

#include <vector>
#include <string>

namespace lexer {  // This gives us a scope to use 'using namespace std' without leaking into other files

    using namespace std;

    // Different token types
    enum TokenType{  // Used for the parser itself
        WHITESPACE,  // Whitespace is not something we'll create a token for
        IDENTIFIER,
        INTEGER_LITERAL,
        DOUBLE_LITERAL,
        STRING_LITERAL,
        OPERATOR,  // We could do subtypes for different operators
        STRING_ESCAPE_SEQUENCE,
        POTENTIAL_DOUBLE,
        POTENTIAL_COMMENT,
        COMMENT  // We also wouldn't make tokens of comments
    };

    static const char* sTokenTypeStrings[] = {  // The array index is the same as the token type
        "WHITESPACE",
        "IDENTIFIER",
        "INTEGER_LITERAL",
        "DOUBLE_LITERAL",
        "STRING_LITERAL",
        "OPERATOR",
        "STRING_ESCAPE_SEQUENCE",
        "POTENTIAL_DOUBLE",
        "POTENTIAL_COMMENT",
        "COMMENT"
    };

    class Token{
    public:
        enum TokenType TType{WHITESPACE};  // Starting out with WHITESPACE which is an invalid token
        string TText;  // Copy of our token text, allows for easier debugging, it will show the original text of the token
        size_t TStartOffset{0};  // Size_t is an unsigned integer type and is able to represent the size of any object in bytes
        size_t TEndOffset{0};
        size_t TLineNumber{0};

        void debugPrint() const;
    };

    class Lexer {
    public:
        vector<Token> parse(const string& inProgram);

    private:  // These functions are only used internally
        static void endToken(Token& token, vector<Token>& tokens);
    };
}
#endif //BASIC_PARSER_C___LEXER_H
