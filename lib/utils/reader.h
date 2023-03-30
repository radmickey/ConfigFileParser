#pragma once

#include "utils.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <cstring>


class Reader {
    private:
        std::stringstream stream;
    public:
        explicit Reader(const std::string&);
        void SkipBlank();
        std::string GetKey();
        std::string GetHeader();
        OMFLvariable::variableType GetArgumentType(char*);
        char* GetArgument(OMFLvariable::variableType type, char*);
        bool eof();
        char Get();
        void Unget();
        char Peek();
        void SkipToNextLine();
        static bool CmpAny (char, const char* del, uint64_t len);
    private:
        char* GetStringArgument();
        char* GetNumericArgument(char*);
        char* GetBoolArgument();
};
