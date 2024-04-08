#include <vector>
#include <string>
#include <iostream>
#include "liteexpr.h"

using namespace std;

vector<string> BUILTINS = {
    "CEIL",
    "EVAL",
    "FLOOR",
    "FOR",
    "FOREACH",
    "FUNCTION",
    "IF",
    "LEN",
    "PRINT",
    "ROUND",
    "SQRT",
    "WHILE",
};


const double NaN = 0.0/0.0;
const double Inf = 1.0/0.0;


int main(int argc, const char* argv[]) {
    string result;
    liteexpr::SYMBOLS symbols = liteexpr::make_symbols({
        { "grades", liteexpr::make_value({
            { "Alice"   , liteexpr::make_value("A") },
            { "Bob"     , liteexpr::make_value("B") },
            { "Charlie" , liteexpr::make_value("C") },
        })},
    });
    liteexpr::ARRAY list = liteexpr::make_array({
        liteexpr::make_value(0),
        liteexpr::make_value(7),
        liteexpr::make_value(3.5),
        liteexpr::make_value(-7),
        liteexpr::make_value(-3.5),
        liteexpr::make_value(NaN),
        liteexpr::make_value(Inf),
        liteexpr::make_value(-Inf),
        liteexpr::make_value("\"\""),
        liteexpr::make_value("\"x\""),
    });

    for(liteexpr::VALUE i : list->native()) {
        for(string builtin : BUILTINS) {
            string expr = builtin + "(i)";
            string pexpr = builtin + "(" + i->svalue() + ")";
            liteexpr::SYMBOLS symbols = liteexpr::make_symbols({
                { "i", i },
            });

            try {
                result = liteexpr::eval(expr, symbols)->svalue();
            }
            catch(liteexpr::Error e) {
                result = string(e);
                replace(result.begin(), result.end(), '\n', '\t');
            }

            cout << setw(15) << left << pexpr << " => " << result << endl;
        }
    }

    try {
        result = liteexpr::eval(R"(
            FOREACH(record, grades,
                name = record[0];
                grade = record[1];

                PRINT(name + "'s grade is " + grade);
            )
        )", symbols)->svalue();
    }
    catch(liteexpr::Error e) {
        result = string(e);
        replace(result.begin(), result.end(), '\n', '\t');
    }

    cout << "=> " << result << endl;

    return 0;
}
