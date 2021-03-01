#include "Lexer.h"
#include <iostream>

using namespace std;
using namespace lexer;

int main() {
    std::cout << "BASIC >> \n" << std::endl;

    FILE * fh = fopen(R"(C:\Users\ishfa\CLionProjects\BASIC_Parser_C++\test.myc)", "r");
    if (!fh) { cerr << "Can't find file." << endl;}
    fseek(fh, 0, SEEK_END);
    size_t fileSize = ftell(fh);
    fseek(fh, 0, SEEK_SET);
    string fileContents(fileSize, ' ');
    fread(fileContents.data(), 1, fileSize, fh);

    cout << fileContents << endl << endl;

    Lexer lexer;
    vector<Token> tokens = lexer.parse(fileContents);

    for (const Token& currentToken : tokens){
        currentToken.debugPrint();
    }

    return 0;
}
