#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include "liteexpr.h"

using namespace std;

vector<string> PREOPS = { "!", "~", "+", "-", "++", "--" };
vector<string> POSTOPS = { "++", "--" };
vector<string> BINARYOPS = {
    "**", "*", "/", "%", "+", "-", "<<", ">>", ">>>", "<", "<=", ">", ">=",
    "==", "!=", "&", "^", "|", "&&", "||"
};

const double NaN = 0.0/0.0;
const double Inf = 1.0/0.0;


int main(int argc, const char* argv[]) {
    liteexpr::ARRAY list0 = liteexpr::make_array({
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
    liteexpr::ARRAY list1 = liteexpr::make_array({
        liteexpr::make_value(0),
        liteexpr::make_value(0.0),
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
    liteexpr::ARRAY list2 = liteexpr::make_array({
        liteexpr::make_value(0),
        liteexpr::make_value(3),
        liteexpr::make_value(3.5),
        liteexpr::make_value(-4),
        liteexpr::make_value(-4.5),
        liteexpr::make_value(NaN),
        liteexpr::make_value(Inf),
        liteexpr::make_value(-Inf),
        liteexpr::make_value("\"x\""),
        liteexpr::make_value("\"y\""),
    });

    for(liteexpr::VALUE i : list0->native()) {
        for(string op : PREOPS) {
            string expr = op + "i";
            string result;
            liteexpr::SYMBOLS symbols = liteexpr::make_symbols({
                { "i", i },
            });

            try {
                result = liteexpr::eval(expr, symbols)->svalue();
            }
            catch(liteexpr::Error e) {
                result = std::string(e);
            }

            cout << setw(4) << right << "" << " "
                 << setw(3) << right << op << " "
                 << setw(4) << right << i->svalue() << " => "
                 << result << endl;
        }
    }

    for(liteexpr::VALUE i : list0->native()) {
        for(string op : POSTOPS) {
            string expr = string("i") + op;
            string result;
            liteexpr::SYMBOLS symbols = liteexpr::make_symbols({
                { "i", i },
            });

            try {
                result = liteexpr::eval(expr, symbols)->svalue();
            }
            catch(liteexpr::Error e) {
                result = std::string(e);
            }

            cout << setw(4) << right << i->svalue() << " "
                 << setw(3) << right << op << " "
                 << setw(4) << right << "" << " => "
                 << result << endl;
        }
    }

    for(liteexpr::VALUE i : list1->native()) {
        for(string op : BINARYOPS) {
            for(liteexpr::VALUE j : list2->native()) {
                string expr = string("i") + op + "j";
                string result;
                liteexpr::SYMBOLS symbols = liteexpr::make_symbols({
                    { "i", i },
                    { "j", j },
                });

                try {
                    result = liteexpr::eval(expr, symbols)->svalue();
                }
                catch(liteexpr::Error e) {
                    result = std::string(e);
                }

                cout << setw(4) << right << i->svalue() << " "
                     << setw(3) << right << op << " "
                     << setw(4) << right << j->svalue() << " => "
                     << result << endl;
            }
        }
    }

    return 0;
}
