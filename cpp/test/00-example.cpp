#include <string>
#include <iostream>
#include "liteexpr.h"

int main(int argc, const char* argv[]) {
    liteexpr::SYMBOLS symbols = liteexpr::make_symbols({
        { "grades", liteexpr::make_value({
            { "alice", liteexpr::make_value("A") },
            { "bob"  , liteexpr::make_value("B") },
        })},
    });

    try {
        liteexpr::eval(R"(
            PRINT("I have " + LEN(grades) + " students");
            PRINT("Alice's grade is " + grades.alice);
        )", symbols);
    }
    catch(liteexpr::Error e) {
        std::cerr << std::string(e) << std::endl;
        return 1;
    }

    return 0;
}
