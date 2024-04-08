#include <memory>
#include <iostream>
#include "liteexpr.h"

int main(int argc, const char* argv[]) {
    liteexpr::SYMBOLS symbols(new liteexpr::SymbolTable());

    try {
        std::cout << liteexpr::eval(R"(
            "Hello, world!";
            "안녕\n하세\U0001d7a8\x21\u2757";
            [ 1, "Hello, world!", [2.0, 3.0] ];
            { hello: "world!", bye: [ "cruel", "world!" ] };
            3 ** 4;
            0 ? (1+2) : (3*4);
            "Hello, " + "world!";
            three = 3;
            four = 4;
            three ** four;
        )", symbols)->encoded() << std::endl;
    }
    catch(liteexpr::Error e) {
        std::cerr << std::string(e) << std::endl;
        return 1;
    }

    return 0;
}
