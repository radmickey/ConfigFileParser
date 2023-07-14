#pragma once

#include "reader.h"

#include <filesystem>
#include <fstream>
#include <string>
#include <iostream>
#include <stdexcept>

namespace ConfigParser {
    cparser::Element* GetArgument(const char* del, Reader& reader);

    cparser::Section parse(const std::string& str);

    cparser::Section parse(const std::filesystem::path& path);

    class ArgParser final {
    private:
        std::map<std::string, cparser::Element*> arguments;
    public:

        ArgParser() = default;

        [[nodiscard]] cparser::Element& Get(const std::string& name) const;
    };
}  // namespace
