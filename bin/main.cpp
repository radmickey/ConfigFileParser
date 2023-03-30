#include "lib/parser.h"

int main(int argc, char** argv) {
    std::string data = R"(
        key = true
        key1 = ["1", "2", "3"])";

    auto root = ConfigParser::parse(data);
    std::cout << root.Get("key").AsBool();
}
