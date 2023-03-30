#include "reader.h"

bool isLetter(char x)
{
    return (x >= 'A' && x <= 'Z') || (x >= 'a' && x <= 'z');
}

bool Reader::CmpAny(char ch, const char* del, uint64_t len) {
    for (int i = 0; i < len; ++i) {
        if (ch == del[i]) {
            return true;
        }
    }
    return false;
}

bool ValidKey(char ch) {
    return (isLetter(ch) || std::isdigit(ch) || ch == '_' || ch == '-');
}

char* Reader::GetStringArgument() {
    stream.ignore(1);
    std::streampos start = stream.tellg();
    char ch;

    do {
        stream.get(ch);
    } while(ch != '\"');

    long long finish = stream.tellg();
    
    char* arg = new char[finish - start];
    arg[finish - start - 1] = '\0';
    stream.seekg(start);

    stream.read(arg, finish - start - 1);
    stream.ignore(1);
    return arg;
}

char* Reader::GetNumericArgument(char* del) {
    std::streampos start = stream.tellg();
    char ch;
    size_t len = strlen(del);

    do {
        stream.get(ch);
    } while(!isspace(ch) && !CmpAny(ch, del, len));

    size_t finish = stream.tellg();

    char* arg = new char[finish - start];
    arg[(int64_t) finish - start - 1LL] = '\0';
    stream.seekg(start);

    stream.read(arg, (int64_t) finish - start - 1LL);
    return arg;
}

char* Reader::GetBoolArgument() {
    int start = stream.tellg();
    char ch;
    stream.get(ch);

    int finish;

    if (ch == 'f') {
        finish = start + 6;
    } else {
        finish = start + 5;
    }
    
    char* arg = new char[finish - start];
    arg[finish - start - 1] = '\0';
    stream.seekg(start);

    stream.read(arg, finish - start - 1);
    return arg;
}

Reader::Reader(const std::string& str) {
    stream = std::stringstream(str + "\n");
}

void Reader::SkipBlank() {
    char ch;
    stream.get(ch);
    while(ch == ' ' || ch == '\v' || ch == '\t' || ch == '\r') stream.get(ch);
    stream.unget();
}

std::string Reader::GetKey() {
    char ch;
    std::streamsize n = stream.tellg();
    do {
        stream.get(ch);
    } while(ValidKey(ch));
    n = stream.tellg() - n;
    stream.seekg(stream.tellg() - n);
    --n;

    std::string out(n, '\0');
    stream.read(&out[0], n);
    return out;
}

std::string Reader::GetHeader() {
    char ch;
    
    std::streamsize n = stream.tellg();

    stream.get(ch);

    if (ch == '.') {
        return {};
    }

    while(ValidKey(ch) || ch == '.') {
        stream.get(ch);
    }

    stream.unget();
    stream.unget();
    stream.get(ch);
    if (ch == '.' || ch == '[') {
        return {};
    }

    n = stream.tellg() - n;
    stream.seekg(stream.tellg() - n);

    std::string out(n, '\0');
    stream.read(&out[0], n);
    return out;
}

OMFLvariable::variableType Reader::GetArgumentType(char* del) {
    OMFLvariable::variableType type = OMFLvariable::variableType::INVALID;
    std::streampos start = stream.tellg();
    bool is_float = false;
    size_t len = strlen(del);
    char ch;
    stream.get(ch);

    const int kFalseSize = 5;
    const int kTrueSize = 4;

    if (ch == '\"') {
        do {
            stream.get(ch);
            if (ch == '\n') {
                return type;
            }
        } while(ch != '\"');

        SkipBlank();

        stream.get(ch);

        
        if (CmpAny(ch, del, len)) {
            type = OMFLvariable::variableType::STRING;
        }
    } else if (isdigit(ch) || ch == '+' || ch == '-') {
        if (stream.peek() == '.' && (ch == '+' || ch == '-')) {
            stream.seekg(start);
            return type;
        }

        do {
            if (ch == '.') {
                if (is_float) {
                    stream.seekg(start);
                    return type;
                } else {
                    is_float = true;
                }
            }
            stream.get(ch);
        } while (isdigit(ch) || ch == '.');

        stream.unget();
        stream.unget();
        stream.get(ch);

        if (!isdigit(ch)) {
            stream.seekg(start);
            return type;
        }

        SkipBlank();

        stream.get(ch);

        if (!CmpAny(ch, del, len)) {
            stream.seekg(start);
            return type;
        }
        type = is_float ? OMFLvariable::variableType::FLOAT : OMFLvariable::variableType::INT;
    } else if (ch == 'f') {
        char* arg = new char[kFalseSize];
        arg[kFalseSize - 1] = '\0';
        stream.read(arg, kFalseSize - 1);

        if (strcmp(arg, "alse") == 0) {
            SkipBlank();
            stream.get(ch);
            delete []arg;

            if (!CmpAny(ch, del, len)) {
                stream.seekg(start);
                return type;
            }
            type = OMFLvariable::variableType::BOOL;
        }
    } else if (ch == 't') {
        char* arg = new char[kTrueSize];
        arg[kTrueSize - 1] = '\0';
        stream.read(arg, kTrueSize - 1);

        if (strcmp(arg, "rue") == 0) {
            SkipBlank();
            stream.get(ch);
            delete []arg;
            
            if (!CmpAny(ch, del, len)) {
                stream.seekg(start);
                return type;
            }
            type = OMFLvariable::variableType::BOOL;
        }
    }

    stream.seekg(start);
    return type;
}

char* Reader::GetArgument(OMFLvariable::variableType type, char* del) {
    switch (type) {
        case OMFLvariable::variableType::STRING: {
            return GetStringArgument();
        }
        case OMFLvariable::variableType::FLOAT: {
            return GetNumericArgument(del);
        }
        case OMFLvariable::variableType::INT: {
            return GetNumericArgument(del);
        }
        case OMFLvariable::variableType::BOOL: {
            return GetBoolArgument();
        }
        default: {
            return nullptr;
        }
    }
}

bool Reader::eof() {
    return stream.peek() == EOF;
}
char Reader::Get() {
    char ch;
    stream.get(ch);
    return ch;
}
void Reader::Unget() {
    stream.unget();
}
char Reader::Peek() {
    return stream.peek();
}

void Reader::SkipToNextLine() {
    char ch;
    stream.get(ch);
    while (ch != '\n') stream.get(ch);
}
