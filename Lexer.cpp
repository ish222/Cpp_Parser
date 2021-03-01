#include "Lexer.h"
#include <stdexcept>
#include <iostream>

namespace lexer {

    using namespace std;

    /*
     * This function just loops over the text in the program
     */
    vector<Token> Lexer::parse(const string &inProgram) {
        vector<Token> tokens;  // List of tokens
        Token currentToken;  // Initialises an empty token object

        currentToken.TLineNumber = 1;

        for (char current_char : inProgram /* For current character in program text */ ) {  // Note this assumes our compiler only handles ASCII, not Unicode
            if (currentToken.TType == STRING_ESCAPE_SEQUENCE) {
                switch (current_char) {
                    case 'n':
                        currentToken.TText.append(1, '\n');
                        break;

                    case 'r':
                        currentToken.TText.append(1, '\r');
                        break;

                    case 't':
                        currentToken.TText.append(1, '\t');
                        break;

                    case '\\':
                        currentToken.TText.append(1, '\\');
                        break;

                    default:  // Throws an exception if there is an invalid escape sequence
                        throw runtime_error(string("Unknown escape sequence: \\") + string(1, current_char) +
                                            string(" in string on line: ") + to_string(currentToken.TLineNumber) +
                                            string("."));
                        break;
                }
                currentToken.TType = STRING_LITERAL;
                continue;
            } else if (currentToken.TType == POTENTIAL_COMMENT && current_char != '/'){
                currentToken.TType = OPERATOR;
                endToken(currentToken, tokens);
                continue;
            }

            switch (current_char) {
                case '0':  // Cases where the current character is a digit
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    if (currentToken.TType == WHITESPACE) {
                        currentToken.TType = INTEGER_LITERAL;  // As we checked for the cases where the input was a number, this changes the token type to Integer
                        currentToken.TText.append(1, current_char);  // Adds the current character to the token string
                    } else if (currentToken.TType == POTENTIAL_DOUBLE) {
                        currentToken.TType = DOUBLE_LITERAL;
                        currentToken.TText.append(1, current_char);
                    } else {
                        currentToken.TText.append(1,
                                                  current_char);  // If its not whitespace, it just appends the character to the token string
                    }
                    break;

                case '.':  // Case of floating point numbers
                    if (currentToken.TType == WHITESPACE ) {
                        currentToken.TType = POTENTIAL_DOUBLE;
                        currentToken.TText.append(1, current_char);
                    } else if (currentToken.TType == INTEGER_LITERAL ) {
                        currentToken.TType = DOUBLE_LITERAL;
                        currentToken.TText.append(1, current_char);
                    } else if (currentToken.TType == STRING_LITERAL ) {
                        currentToken.TText.append(1, current_char);
                    } else {
                        currentToken.TText.append(1,
                                                  current_char);
                        endToken(currentToken, tokens);
                        currentToken.TType = OPERATOR;
                        currentToken.TText.append(1, current_char);
                        endToken(currentToken, tokens);
                    }
                    break;

                case '{':  // Case where the current character is an operator
                case '}':
                case '(':
                case ')':
                case '=':
                case '-':
                case ':':
                case ',':
                    if (currentToken.TType !=
                        STRING_LITERAL) {  // Checks if the token is not string (to not take in operators inside strings)
                        endToken(currentToken, tokens);  // Ends the current token and creates a new one
                        currentToken.TType = OPERATOR;  // As we checked for the cases where the input was a number, this changes the token type to Integer
                        currentToken.TText.append(1, current_char);  // Adds the current character to the token string
                        endToken(currentToken, tokens);
                    } else {
                        currentToken.TText.append(1,
                                                  current_char);  // If its not whitespace, it just appends the character to the token string
                    }
                    break;

                case ' ':  // Case for whitespace
                case '\t':
                    endToken(currentToken, tokens);  // Ends the token when whitespace is encountered
                    break;

                case '\r':  // Case for new line
                case '\n':
                    endToken(currentToken, tokens);
                    ++currentToken.TLineNumber;  // Increments the line number in the case of new lines
                    break;

                case '"':  // Case for strings
                    if (currentToken.TType != STRING_LITERAL) {
                        endToken(currentToken, tokens);
                        currentToken.TType = STRING_LITERAL;
                        currentToken.TText.append(1, current_char);
                    } else if (currentToken.TType == STRING_LITERAL) {
                        endToken(currentToken, tokens);
                    }
                    break;

                case '\\':  // Case for escape characters
                    if (currentToken.TType == STRING_LITERAL) {
                        currentToken.TType = STRING_ESCAPE_SEQUENCE;  // This checks to allow for escape functionality
                    } else {
                        endToken(currentToken, tokens);
                        currentToken.TType = OPERATOR;
                        currentToken.TText.append(1, current_char);
                        endToken(currentToken, tokens);
                    }
                    break;

                case '/':  // Case for comment (or just single slash)
                    if (currentToken.TType == STRING_LITERAL){
                        currentToken.TText.append(1, current_char);
                    } else if (currentToken.TType == POTENTIAL_COMMENT){  // Checks if already hit a slash
                        currentToken.TType = COMMENT;  // Changes token type to comment and clears the two slashes
                        currentToken.TText.erase();
                    } else {  // Checks if we are not in a string
                        endToken(currentToken, tokens);
                        currentToken.TType = POTENTIAL_COMMENT;
                        currentToken.TText.append(1, current_char);
                    }
                    break;

                default:  // Default case is that it either ends up inside a string or as a part of an identifier
                    if(currentToken.TType == WHITESPACE || currentToken.TType == INTEGER_LITERAL || currentToken.TType == DOUBLE_LITERAL) {// Note integers and doubles can have 'f' or 'F' at the end of the double to identify its a float
                        endToken(currentToken, tokens);
                        currentToken.TType = IDENTIFIER;
                        currentToken.TText.append(1, current_char);
                    } else{
                        currentToken.TText.append(1, current_char);
                    }
                    break;
            }
        }

        endToken(currentToken, tokens);  // We might have an identifier that goes up to the end of the document which may need to get closed

        return tokens;
    }

    /*
     * If the token exists or is valid, this function adds it to the tokens list and then resets the token into a 'clean' state
     */
    void Lexer::endToken(Token &token,
                         vector<Token> &tokens) {  // The reason these are references are of course because we plan to modify them
        if (token.TType == COMMENT) {  // We dont want to generate any tokens for comments
            cout << "Ignoring comment " << token.TText << endl;
        } else if (token.TType != WHITESPACE) {  // We dont want to generate any tokens for whitespace
            tokens.push_back(token);
        }
        if (token.TType == POTENTIAL_DOUBLE){
            if (token.TText == "."){
                token.TType = OPERATOR;
            } else {
                token.TType = DOUBLE_LITERAL;  // For the case .5 or 5.
            }
        }
        token.TType = WHITESPACE;
        token.TText.erase();  // We need the token string to be clear because we append to it
    }

    /*
     * Prints the type, value and location (line number) of the token
     */
    void Token::debugPrint() const {
        cout << "Token(" << sTokenTypeStrings[TType] << ", \"" << TText << "\", " << TLineNumber << ")" << endl;
    }
}
