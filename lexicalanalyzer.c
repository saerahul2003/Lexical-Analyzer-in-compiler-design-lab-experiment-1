#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define MAX_SYMBOLS 100

typedef struct {
    char name[50];
    char type[10];
    int size;
    int address;
    int value;
} Symbol;

Symbol symbolTable[MAX_SYMBOLS];
int symbolCount = 0;

void addSymbol(char* name, char* type, int size, int address, int value) {
    if (symbolCount < MAX_SYMBOLS) {
        strcpy(symbolTable[symbolCount].name, name);
        strcpy(symbolTable[symbolCount].type, type);
        symbolTable[symbolCount].size = size;
        symbolTable[symbolCount].address = address;
        symbolTable[symbolCount].value = value;
        symbolCount++;
    }
}

void printSymbolTable() {
    printf("\nContent of Symbol Table\n");
    printf("Identifier\tName\tType\tNo of bytes\tAddress\tValue\n");
    for (int i = 0; i < symbolCount; i++) {
        printf("%s\t%s\t%s\t%d\t%d\t%d\n", symbolTable[i].name, symbolTable[i].name,
               symbolTable[i].type, symbolTable[i].size, symbolTable[i].address, symbolTable[i].value);
    }
}

void printToken(const char* token, const char* type) {
    printf("%s - %s\n", token, type);
}

int isKeyword(const char* str) {
    static const char* keywords[] = {
        "auto", "break", "case", "char", "const", "continue", "default", "do",
        "double", "else", "enum", "extern", "float", "for", "goto", "if", "int",
        "long", "register", "return", "short", "signed", "sizeof", "static",
        "struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while"
    };
    for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++) {
        if (strcmp(str, keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

void tokenize(const char* source) {
    const char* p = source;
    char ch;
    char buffer[100];
    int bufferIndex = 0;
    int state = 0;

    while ((ch = *p++) != '\0') {
        switch (state) {
            case 0: // Normal State
                if (isalpha(ch) || ch == '_') {
                    buffer[bufferIndex++] = ch;
                    state = 1;
                } else if (isdigit(ch)) {
                    buffer[bufferIndex++] = ch;
                    state = 2;
                } else if (ch == '"') {
                    buffer[bufferIndex++] = ch;
                    state = 3;
                } else if (ch == '/') {
                    char nextCh = *p++;
                    if (nextCh == '/') {
                        state = 4; // Single-line comment
                    } else if (nextCh == '*') {
                        state = 5; // Multi-line comment
                    } else {
                        --p; // Put back the character
                        buffer[0] = ch;
                        buffer[1] = '\0';
                        printToken(buffer, "operator");
                        state = 0;
                    }
                } else if (strchr("+-*/%<>=!&|^", ch)) {
                    buffer[0] = ch;
                    buffer[1] = '\0';
                    printToken(buffer, "operator");
                } else if (strchr(";,.(){}[]", ch)) {
                    buffer[0] = ch;
                    buffer[1] = '\0';
                    printToken(buffer, "special character");
                } else if (isspace(ch)) {
                    // Ignore whitespace
                } else {
                    // Unknown character
                }
                break;

            case 1: // Identifier/Keyword State
                if (isalnum(ch) || ch == '_') {
                    buffer[bufferIndex++] = ch;
                } else {
                    buffer[bufferIndex] = '\0';
                    if (isKeyword(buffer)) {
                        printToken(buffer, "keyword");
                    } else {
                        printToken(buffer, "identifier");
                        // Add identifier to symbol table (example details)
                        addSymbol(buffer, "int", 4, 1000 + symbolCount * 4, 0);
                    }
                    bufferIndex = 0;
                    state = 0;
                    --p; // Put back the character
                }
                break;

            case 2: // Integer Constant State
                if (isdigit(ch)) {
                    buffer[bufferIndex++] = ch;
                } else {
                    buffer[bufferIndex] = '\0';
                    printToken(buffer, "integer constant");
                    bufferIndex = 0;
                    state = 0;
                    --p; // Put back the character
                }
                break;

            case 3: // String Literal State
                if (ch != '"') {
                    buffer[bufferIndex++] = ch;
                } else {
                    buffer[bufferIndex++] = ch;
                    buffer[bufferIndex] = '\0';
                    printToken(buffer, "string constant");
                    bufferIndex = 0;
                    state = 0;
                }
                break;

            case 4: // Single-line Comment State
                while ((ch = *p++) != '\n' && ch != '\0');
                state = 0;
                break;

            case 5: // Multi-line Comment State
                while ((ch = *p++) != '\0') {
                    if (ch == '*' && *p == '/') {
                        p++; // Skip '/'
                        state = 0;
                        break;
                    }
                }
                if (ch == '\0') {
                    printf("Error: Unterminated multi-line comment.\n");
                }
                break;
        }
    }
}

int main() {
    // Example source code
    const char* source = "#include <stdio.h>\n"
                         "int main() {\n"
                         "    int a = 10;\n"
                         "    int b = 20;\n"
                         "    if (a > b) {\n"
                         "        printf(\"a is greater\\n\");\n"
                         "    } else {\n"
                         "        printf(\"b is greater\\n\");\n"
                         "    }\n"
                         "    return 0;\n"
                         "}\n";
    
    tokenize(source);
    printSymbolTable();
    return 0;
}
