#include <memory>
#include <string>
#include <iostream>
#include <cstdlib>
#include "liteexpr.h"

const std::string readfile(const char* filename) {
    std::ifstream file(filename);
    std::string data;

    if(!file.is_open()) {
        std::cerr << std::string(filename) << ": " << std::strerror(errno) << std::endl;
        exit(1);
    }

    while(!file.eof()) {
        std::string line;

        getline(file, line);
        if(file.eof()) break;

        if(data.length()) data += "\n";
        data += line;
    }

    return data;
}

const std::string feval(const char* filename, std::shared_ptr<liteexpr::SymbolTable> symbols) {
    std::string data = readfile(filename);

    try {
        return liteexpr::eval(data, symbols)->svalue();
    }
    catch(liteexpr::Error e) {
        std::cerr << std::string(e) << std::endl;
    }

    return std::string();
}

int main(int argc, const char* argv[]) {
    liteexpr::SYMBOLS symbols(new liteexpr::SymbolTable());
    const char** cp = argv;

    while(*++cp) {
        feval(*cp, symbols);
    }

    return 0;
}
