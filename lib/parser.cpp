#include "parser.h"

cparser::Element* ConfigParser::GetArgument(const char* del, Reader& reader) {
    char ch;
    char* arg;

    reader.SkipBlank();
    if (reader.Peek() == '[') { // array
        auto* array = new OMFLvariable::ArrayValue();
        while (reader.Get() != ']') {
            cparser::Element* child = ConfigParser::GetArgument(",]", reader);
            if (child == nullptr) {
                return nullptr;
            }
            reader.SkipBlank();
            array->AddElement(child);
        }

        return array;

    } else { // another arg
        OMFLvariable::variableType type = reader.GetArgumentType(const_cast<char*>(del));
        if (type == OMFLvariable::variableType::INVALID) {
            return nullptr;
        }

        arg = reader.GetArgument(type, const_cast<char*>(del));
        if (arg == nullptr) {
            return nullptr;
        }

        std::string arg_string = arg;
        delete arg;

        switch (type) {
            case OMFLvariable::variableType::INT:
                return new OMFLvariable::IntValue("", std::atoi(arg));
            case OMFLvariable::variableType::FLOAT:
                return new OMFLvariable::FloatValue("", std::atof(arg));
            case OMFLvariable::variableType::STRING:
                return new OMFLvariable::StringValue("", arg_string);
            case OMFLvariable::variableType::BOOL:
                return new OMFLvariable::BoolValue("", std::strcmp(arg_string.c_str(),"true") == 0);
            case OMFLvariable::ARRAY:
            case OMFLvariable::INVALID:
                break;
        }

        return new OMFLvariable::IntValue("", std::atoi(arg));
    }
}

cparser::Section ConfigParser::parse(const std::string& str) {
    cparser::Section root = cparser::Section();
    cparser::Section* current = &root;
    char ch;
    std::string key;
    Reader reader(str);

    while (!reader.eof()) {
        reader.SkipBlank();
        ch = reader.Get();

        if (ch == '[') { //section
            reader.SkipBlank();
            key = reader.GetHeader();
            if (key.empty()) {
                root.setNotValid();
                break;
            }

            current = root.AddSection(key);

            reader.SkipBlank();
            ch = reader.Get();
            if (ch != ']') {
                root.setNotValid();
                break;
            }
            reader.SkipBlank();
            ch = reader.Get();
            if (ch != '\n') {
                root.setNotValid();
                break;
            }


        } else if (ch == '#') { //comment
            reader.SkipToNextLine();


        } else if (ch != '\n') { //key arg pair
            reader.Unget();
            key = reader.GetKey();
            reader.SkipBlank();
            ch = reader.Get();
            if (ch != '=' || key.empty() || current->InSection(key)) {
                root.setNotValid();
                break;
            }

            cparser::Element* arg = ConfigParser::GetArgument("\n#", reader);
            reader.SkipBlank();
            ch = reader.Get();
            if (ch == '#') {
                reader.SkipToNextLine();
            } else if (ch != '\n') {
                delete arg;
                arg = nullptr;
            }

            if (arg == nullptr) {
                root.setNotValid();
                break;
            }
            current->AddArgument(key, arg);
        }
    }
    return root;
}

cparser::Section ConfigParser::parse(const std::filesystem::path& path) {
    std::ifstream file(path);
    std::stringstream buffer;

    if (!file.is_open()) {
        throw std::invalid_argument("File can't be opened. Probably, file is not exist");
    }
    buffer << file.rdbuf();

    return parse(buffer.str());
}

cparser::Element& ConfigParser::ArgParser::Get(const std::string& name) const {
    if (arguments.at(name)->getElementType() == cparser::elementType::SECTION) {
        return dynamic_cast<cparser::Section&>(*arguments.at(name)).Get(name);
    } else if (arguments.at(name)->getElementType() == cparser::elementType::VARIABLE) {

        switch (dynamic_cast<OMFLvariable::Variable&>(*arguments.at(name)).getVariableType())  {
            case OMFLvariable::variableType::INT:
                return dynamic_cast<OMFLvariable::IntValue&>(*arguments.at(name));
            case OMFLvariable::variableType::FLOAT:
                return dynamic_cast<OMFLvariable::FloatValue&>(*arguments.at(name));
            case OMFLvariable::variableType::STRING:
                return dynamic_cast<OMFLvariable::StringValue&>(*arguments.at(name));
            case OMFLvariable::variableType::BOOL:
                return dynamic_cast<OMFLvariable::BoolValue&>(*arguments.at(name));
            case OMFLvariable::variableType::ARRAY:
                return dynamic_cast<OMFLvariable::ArrayValue&>(*arguments.at(name));
            case OMFLvariable::variableType::INVALID:
                throw std::invalid_argument("The cparser result is invalid");
        }
    }
    throw std::invalid_argument("Something went wrong");
}
