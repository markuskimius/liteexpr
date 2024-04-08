#include <codecvt>
#include <cstdio>
#include <cmath>
#include <memory>
#include <utility>
#include "liteexpr.h"

namespace liteexpr {
    using std::any_cast;
    using std::to_string;
    using std::dynamic_pointer_cast;

    int64_t MAXARGS = std::numeric_limits<int>::max();
    int64_t MAXINT = std::numeric_limits<int64_t>::max();
    int64_t MININT = std::numeric_limits<int64_t>::min();
}

namespace liteexpr {
    /* ***************************************************************************
    * VALUE
    */

    bool Value::istrue() const { throw BasicRuntimeError("Unsupported operation `istrue()`: " + this->name()); }
    int64_t Value::length() const { throw BasicRuntimeError("Unsupported operation `length()`: " + this->name()); }
    int64_t Value::ivalue() const { throw BasicRuntimeError("Unsupported operation `ivalue()`: " + this->name()); }
    double Value::dvalue() const { throw BasicRuntimeError("Unsupported operation `dvalue()`: " + this->name()); }
    string Value::svalue() const { throw BasicRuntimeError("Unsupported operation `svalue()`: " + this->name()); }
    const vector<VALUE>& Value::avalue() const { throw BasicRuntimeError("Unsupported operation `avalue()`: " + this->name()); }
    const map<string,VALUE>& Value::ovalue() const { throw BasicRuntimeError("Unsupported operation `ovalue()`: " + this->name()); }
    VALUE Value::op_not() const { return VALUE(new Integer(this->istrue() ? 0 : 1)); }
    VALUE Value::op_inv() const { throw BasicRuntimeError("Unsupported operand type for `~`: (" + this->name() + ")"); }
    VALUE Value::op_pos() const { throw BasicRuntimeError("Unsupported operand type for `+`: (" + this->name() + ")"); }
    VALUE Value::op_neg() const { throw BasicRuntimeError("Unsupported operand type for `-`: (" + this->name() + ")"); }
    VALUE Value::op_inc() const { throw BasicRuntimeError("Unsupported operand type for `++`: (" + this->name() + ")"); }
    VALUE Value::op_dec() const { throw BasicRuntimeError("Unsupported operand type for `--`: (" + this->name() + ")"); }
    VALUE Value::op_lt(const VALUE other) const { throw BasicRuntimeError("Unsupported operand type(s) for `<`: (" + this->name() + ",*)"); }
    VALUE Value::op_gt(const VALUE other) const { throw BasicRuntimeError("Unsupported operand type(s) for `>`: (" + this->name() + ",*)"); }
    VALUE Value::op_eq(const VALUE other) const { throw BasicRuntimeError("Unsupported operand type(s) for `==`: (" + this->name() + ",*)"); }
    VALUE Value::op_ne(const VALUE other) const { throw BasicRuntimeError("Unsupported operand type(s) for `!=`: (" + this->name() + ",*)"); }
    VALUE Value::op_lte(const VALUE other) const { throw BasicRuntimeError("Unsupported operand type(s) for `<=`: (" + this->name() + ",*)"); }
    VALUE Value::op_gte(const VALUE other) const { throw BasicRuntimeError("Unsupported operand type(s) for `>=`: (" + this->name() + ",*)"); }
    VALUE Value::op_pow(const VALUE other) const { throw BasicRuntimeError("Unsupported operand type(s) for `**`: (" + this->name() + ",*)"); }
    VALUE Value::op_mul(const VALUE other) const { throw BasicRuntimeError("Unsupported operand type(s) for `*`: (" + this->name() + ",*)"); }
    VALUE Value::op_div(const VALUE other) const { throw BasicRuntimeError("Unsupported operand type(s) for `/`: (" + this->name() + ",*)"); }
    VALUE Value::op_add(const VALUE other) const { throw BasicRuntimeError("Unsupported operand type(s) for `+`: (" + this->name() + ",*)"); }
    VALUE Value::op_sub(const VALUE other) const { throw BasicRuntimeError("Unsupported operand type(s) for `-`: (" + this->name() + ",*)"); }
    VALUE Value::op_mod(const VALUE other) const { throw BasicRuntimeError("Unsupported operand type(s) for `%`: (" + this->name() + ",*)"); }
    VALUE Value::op_shl(const VALUE other) const { throw BasicRuntimeError("Unsupported operand type(s) for `<<`: (" + this->name() + ",*)"); }
    VALUE Value::op_asr(const VALUE other) const { throw BasicRuntimeError("Unsupported operand type(s) for `>>`: (" + this->name() + ",*)"); }
    VALUE Value::op_shr(const VALUE other) const { throw BasicRuntimeError("Unsupported operand type(s) for `>>>`: (" + this->name() + ",*)"); }
    VALUE Value::op_and(const VALUE other) const { throw BasicRuntimeError("Unsupported operand type(s) for `&`: (" + this->name() + ",*)"); }
    VALUE Value::op_xor(const VALUE other) const { throw BasicRuntimeError("Unsupported operand type(s) for `^`: (" + this->name() + ",*)"); }
    VALUE Value::op_or(const VALUE other) const { throw BasicRuntimeError("Unsupported operand type(s) for `|`: (" + this->name() + ",*)"); }

    /* ***************************************************************************
    * INTEGER
    */

    Integer::Integer(int64_t v) {
        this->value = v;
    }

    int64_t Integer::native() const {
        return this->value;
    }

    string Integer::encode(int64_t decoded) {
        return to_string(decoded);
    }

    int64_t Integer::decode(const string& encoded) {
        int64_t decoded = 0;

        for(int i=0; i<encoded.length(); i++) {
            decoded *= 10;

            switch(encoded[i]) {
                case '0': decoded += 0; break;
                case '1': decoded += 1; break;
                case '2': decoded += 2; break;
                case '3': decoded += 3; break;
                case '4': decoded += 4; break;
                case '5': decoded += 5; break;
                case '6': decoded += 6; break;
                case '7': decoded += 7; break;
                case '8': decoded += 8; break;
                case '9': decoded += 9; break;
                default : throw BasicSyntaxError(string("Invalid decimal character: ") + encoded[i]);
            }
        }

        return decoded;
    }

    int64_t Integer::decodeHex(const string& encoded) {
        int64_t decoded = 0;

        for(int i=0; i<encoded.length(); i++) {
            decoded *= 0x10;

            switch(encoded[i]) {
                case '0': decoded += 0x0; break;
                case '1': decoded += 0x1; break;
                case '2': decoded += 0x2; break;
                case '3': decoded += 0x3; break;
                case '4': decoded += 0x4; break;
                case '5': decoded += 0x5; break;
                case '6': decoded += 0x6; break;
                case '7': decoded += 0x7; break;
                case '8': decoded += 0x8; break;
                case '9': decoded += 0x9; break;
                case 'a': decoded += 0xa; break;
                case 'b': decoded += 0xb; break;
                case 'c': decoded += 0xc; break;
                case 'd': decoded += 0xd; break;
                case 'e': decoded += 0xe; break;
                case 'f': decoded += 0xf; break;
                case 'A': decoded += 0xA; break;
                case 'B': decoded += 0xB; break;
                case 'C': decoded += 0xC; break;
                case 'D': decoded += 0xD; break;
                case 'E': decoded += 0xE; break;
                case 'F': decoded += 0xF; break;
                default : throw BasicSyntaxError(string("Invalid hexadecimal character: ") + encoded[i]);
            }
        }

        return decoded;
    }

    string Integer::name() const {
        return "INTEGER";
    }

    string Integer::encoded() const {
        return this->encode(this->value);
    }

    const type_info& Integer::type() const {
        return typeid(*this);
    }

    bool Integer::istrue() const { return this->value ? true : false; }
    int64_t Integer::ivalue() const { return this->native(); }
    double Integer::dvalue() const { return this->native(); }
    string Integer::svalue() const { return std::to_string(this->native()); }
    VALUE Integer::op_inv() const { return VALUE(new Integer(~this->value)); }
    VALUE Integer::op_pos() const { return VALUE(new Integer(this->value)); }
    VALUE Integer::op_neg() const { return VALUE(new Integer(-this->value)); }
    VALUE Integer::op_inc() const { return VALUE(new Integer(this->value + 1)); }
    VALUE Integer::op_dec() const { return VALUE(new Integer(this->value - 1)); }

    VALUE Integer::op_lt(VALUE other) const {
        if(other->type() == typeid(Integer)) return VALUE(new Integer(this->value < other->ivalue()));
        if(other->type() == typeid(Double)) return VALUE(new Integer(this->value < other->dvalue()));

        throw BasicRuntimeError("Unsupported operand type(s) for `<`: (" + this->name() + "," + other->name() + ")");
    }

    VALUE Integer::op_gt(const VALUE other) const {
        if(other->type() == typeid(Integer)) return VALUE(new Integer(this->value > other->ivalue()));
        if(other->type() == typeid(Double)) return VALUE(new Integer(this->value > other->dvalue()));

        throw BasicRuntimeError("Unsupported operand type(s) for `>`: (" + this->name() + "," + other->name() + ")");
    }

    VALUE Integer::op_eq(const VALUE other) const {
        if(other->type() == typeid(Integer)) return VALUE(new Integer(this->value == other->ivalue()));
        if(other->type() == typeid(Double)) return VALUE(new Integer(this->value == other->dvalue()));

        return VALUE(new Integer(0));
    }

    VALUE Integer::op_ne(const VALUE other) const {
        if(other->type() == typeid(Integer)) return VALUE(new Integer(this->value != other->ivalue()));
        if(other->type() == typeid(Double)) return VALUE(new Integer(this->value != other->dvalue()));

        return VALUE(new Integer(1));
    }

    VALUE Integer::op_lte(const VALUE other) const {
        if(other->type() == typeid(Integer)) return VALUE(new Integer(this->value <= other->ivalue()));
        if(other->type() == typeid(Double)) return VALUE(new Integer(this->value <= other->dvalue()));

        throw BasicRuntimeError("Unsupported operand type(s) for `<=`: (" + this->name() + "," + other->name() + ")");
    }

    VALUE Integer::op_gte(const VALUE other) const {
        if(other->type() == typeid(Integer)) return VALUE(new Integer(this->value >= other->ivalue()));
        if(other->type() == typeid(Double)) return VALUE(new Integer(this->value >= other->dvalue()));

        throw BasicRuntimeError("Unsupported operand type(s) for `>=`: (" + this->name() + "," + other->name() + ")");
    }

    VALUE Integer::op_pow(const VALUE other) const {
        if(other->type() == typeid(Integer)) {
            int64_t ov = other->ivalue();

            if(this->value == 0 && ov == 0) return VALUE(new Integer(1));
            if(                    ov  < 0) return VALUE(new Double(pow(this->value, ov)));

            return VALUE(new Integer(pow(this->value, ov)));
        }

        if(other->type() == typeid(Double)) {
            return VALUE(new Double(pow(this->value, other->dvalue())));
        }

        throw BasicRuntimeError("Unsupported operand type(s) for `**`: (" + this->name() + "," + other->name() + ")");
    }

    VALUE Integer::op_mul(const VALUE other) const {
        if(other->type() == typeid(Integer)) return VALUE(new Integer(this->value * other->ivalue()));
        if(other->type() == typeid(Double)) return VALUE(new Double(this->value * other->dvalue()));

        throw BasicRuntimeError("Unsupported operand type(s) for `*`: (" + this->name() + "," + other->name() + ")");
    }

    VALUE Integer::op_div(const VALUE other) const {
        if(other->type() == typeid(Double)) return VALUE(new Double(this->value / other->dvalue()));
        if(other->type() == typeid(Integer)) {
            int64_t ov = other->ivalue();

            if(ov == 0) throw BasicRuntimeError("Division by zero: (" + this->svalue() + " / " + other->svalue() + ")");

            return VALUE(new Integer(this->value / other->ivalue()));
        }

        throw BasicRuntimeError("Unsupported operand type(s) for `/`: (" + this->name() + "," + other->name() + ")");
    }

    VALUE Integer::op_add(const VALUE other) const {
        if(other->type() == typeid(Integer)) return VALUE(new Integer(this->value + other->ivalue()));
        if(other->type() == typeid(Double))  return VALUE(new Double(this->value + other->dvalue()));
        if(other->type() == typeid(String))  return VALUE(new String(this->svalue() + other->svalue()));

        throw BasicRuntimeError("Unsupported operand type(s) for `+`: (" + this->name() + "," + other->name() + ")");
    }

    VALUE Integer::op_sub(const VALUE other) const {
        if(other->type() == typeid(Integer)) return VALUE(new Integer(this->value - other->ivalue()));
        if(other->type() == typeid(Double)) return VALUE(new Double(this->value - other->dvalue()));

        throw BasicRuntimeError("Unsupported operand type(s) for `-`: (" + this->name() + "," + other->name() + ")");
    }

    VALUE Integer::op_mod(const VALUE other) const {
        if(other->type() == typeid(Integer)) {
            int64_t ov = other->ivalue();

            if(ov == 0) throw BasicRuntimeError("Modulus by zero: (" + this->svalue() + " % " + other->svalue() + ")");

            return VALUE(new Integer(this->value % other->ivalue()));
        }

        throw BasicRuntimeError("Unsupported operand type(s) for `%`: (" + this->name() + "," + other->name() + ")");
    }

    VALUE Integer::op_shl(const VALUE other) const {
        if(other->type() == typeid(Integer)) {
            int64_t ov = other->ivalue();

            if(ov < 0) throw BasicRuntimeError("Invalid attempt to shift `<<` by a negative amount: " + other->svalue());

            return VALUE(new Integer(this->value << other->ivalue()));
        }

        throw BasicRuntimeError("Unsupported operand type(s) for `<<`: (" + this->name() + "," + other->name() + ")");
    }

    VALUE Integer::op_asr(const VALUE other) const {
        if(other->type() == typeid(Integer)) {
            int64_t value = this->value;
            int64_t shiftby = other->ivalue();

            if(shiftby < 0) {
                throw BasicRuntimeError("Invalid attempt to shift `>>` by a negative amount: " + other->svalue());
            }
            else if(shiftby < 64 && value < 0) {
                value = ~value;
                value >>= shiftby;
                value = ~value;
            }
            else if(shiftby < 64) {
                value >>= shiftby;
            }
            else {
                value = 0;
            }

            return VALUE(new Integer(value));
        }

        throw BasicRuntimeError("Unsupported operand type(s) for `>>`: (" + this->name() + "," + other->name() + ")");
    }

    VALUE Integer::op_shr(const VALUE other) const {
        if(other->type() == typeid(Integer)) {
            int64_t value = this->value;
            int64_t shiftby = other->ivalue();

            if(shiftby < 0) {
                throw BasicRuntimeError("Invalid attempt to shift `>>>` by a negative amount: " + other->svalue());
            }
            else if(shiftby < 64 && value < 0) {
                value ^= INT64_C(0x8000000000000000);
                value >>= shiftby;
                value ^= (INT64_C(0x8000000000000000) >> shiftby);
            }
            else if(shiftby < 64) {
                value >>= shiftby;
            }
            else {
                value = 0;
            }

            return VALUE(new Integer(value));
        }

        throw BasicRuntimeError("Unsupported operand type(s) for `>>>`: (" + this->name() + "," + other->name() + ")");
    }

    VALUE Integer::op_and(const VALUE other) const {
        if(other->type() == typeid(Integer)) return VALUE(new Integer(this->value & other->ivalue()));

        throw BasicRuntimeError("Unsupported operand type(s) for `&`: (" + this->name() + "," + other->name() + ")");
    }

    VALUE Integer::op_xor(const VALUE other) const {
        if(other->type() == typeid(Integer)) return VALUE(new Integer(this->value ^ other->ivalue()));

        throw BasicRuntimeError("Unsupported operand type(s) for `^`: (" + this->name() + "," + other->name() + ")");
    }

    VALUE Integer::op_or(const VALUE other) const {
        if(other->type() == typeid(Integer)) return VALUE(new Integer(this->value | other->ivalue()));

        throw BasicRuntimeError("Unsupported operand type(s) for `|`: (" + this->name() + "," + other->name() + ")");
    }


    /* ***************************************************************************
    * DOUBLE
    */

    Double::Double(double v) {
        this->value = v;
    }

    double Double::native() const {
        return this->value;
    }

    string Double::encode(double decoded) {
        string result = to_string(decoded);
        size_t dpos = result.find(".");

        if     (std::isinf( decoded) && decoded > 0) result = "Inf";
        else if(std::isinf(-decoded) && decoded < 0) result = "-Inf";
        else if(std::isnan( decoded)) result = "NaN";
        else if(dpos < result.length()-2 && result.find("e") == string::npos) {
            size_t i = result.length();

            /* Remove trailing zeros */
            while(result[i-1] == '0' && dpos < i-2) i--;
            result = result.substr(0,i);
        }

        return result;
    }

    double Double::decode(const string& encoded) {
        return std::stod(encoded);
    }

    string Double::name() const {
        return "DOUBLE";
    }

    string Double::encoded() const {
        return this->encode(this->value);
    }

    const type_info& Double::type() const {
        return typeid(*this);
    }

    bool Double::istrue() const { return this->value ? true : false; }
    int64_t Double::ivalue() const { return this->native(); }
    double Double::dvalue() const { return this->native(); }
    string Double::svalue() const { return this->encoded(); }
    VALUE Double::op_pos() const { return VALUE(new Double(this->value)); }
    VALUE Double::op_neg() const { return VALUE(new Double(-this->value)); }

    VALUE Double::op_lt(const VALUE other) const {
        if(other->type() == typeid(Integer)) return VALUE(new Integer(this->value < other->ivalue()));
        if(other->type() == typeid(Double)) return VALUE(new Integer(this->value < other->dvalue()));

        throw BasicRuntimeError("Unsupported operand type(s) for `<`: (" + this->name() + "," + other->name() + ")");
    }

    VALUE Double::op_gt(const VALUE other) const {
        if(other->type() == typeid(Integer)) return VALUE(new Integer(this->value > other->ivalue()));
        if(other->type() == typeid(Double)) return VALUE(new Integer(this->value > other->dvalue()));

        throw BasicRuntimeError("Unsupported operand type(s) for `>`: (" + this->name() + "," + other->name() + ")");
    }

    VALUE Double::op_eq(const VALUE other) const {
        if(other->type() == typeid(Integer)) return VALUE(new Integer(this->value == other->ivalue()));
        if(other->type() == typeid(Double)) return VALUE(new Integer(this->value == other->dvalue()));

        return VALUE(new Integer(0));
    }

    VALUE Double::op_ne(const VALUE other) const {
        if(other->type() == typeid(Integer)) return VALUE(new Integer(this->value != other->ivalue()));
        if(other->type() == typeid(Double)) return VALUE(new Integer(this->value != other->dvalue()));

        return VALUE(new Integer(1));
    }

    VALUE Double::op_lte(const VALUE other) const {
        if(other->type() == typeid(Integer)) return VALUE(new Integer(this->value <= other->ivalue()));
        if(other->type() == typeid(Double)) return VALUE(new Integer(this->value <= other->dvalue()));

        throw BasicRuntimeError("Unsupported operand type(s) for `<=`: (" + this->name() + "," + other->name() + ")");
    }

    VALUE Double::op_gte(const VALUE other) const {
        if(other->type() == typeid(Integer)) return VALUE(new Integer(this->value >= other->ivalue()));
        if(other->type() == typeid(Double)) return VALUE(new Integer(this->value >= other->dvalue()));

        throw BasicRuntimeError("Unsupported operand type(s) for `>=`: (" + this->name() + "," + other->name() + ")");
    }

    VALUE Double::op_pow(const VALUE other) const {
        if(other->type() == typeid(Integer)) return VALUE(new Double(std::pow(this->value, other->ivalue())));
        if(other->type() == typeid(Double)) return VALUE(new Double(std::pow(this->value, other->dvalue())));

        throw BasicRuntimeError("Unsupported operand type(s) for `**`: (" + this->name() + "," + other->name() + ")");
    }

    VALUE Double::op_mul(const VALUE other) const {
        if(other->type() == typeid(Integer)) return VALUE(new Double(this->value * other->ivalue()));
        if(other->type() == typeid(Double)) return VALUE(new Double(this->value * other->dvalue()));

        throw BasicRuntimeError("Unsupported operand type(s) for `*`: (" + this->name() + "," + other->name() + ")");
    }

    VALUE Double::op_div(const VALUE other) const {
        if(other->type() == typeid(Integer)) return VALUE(new Double(this->value / other->ivalue()));
        if(other->type() == typeid(Double)) return VALUE(new Double(this->value / other->dvalue()));

        throw BasicRuntimeError("Unsupported operand type(s) for `/`: (" + this->name() + "," + other->name() + ")");
    }

    VALUE Double::op_add(const VALUE other) const {
        if(other->type() == typeid(Integer)) return VALUE(new Double(this->value + other->ivalue()));
        if(other->type() == typeid(Double))  return VALUE(new Double(this->value + other->dvalue()));
        if(other->type() == typeid(String))  return VALUE(new String(this->svalue() + other->svalue()));

        throw BasicRuntimeError("Unsupported operand type(s) for `+`: (" + this->name() + "," + other->name() + ")");
    }

    VALUE Double::op_sub(const VALUE other) const {
        if(other->type() == typeid(Integer)) return VALUE(new Double(this->value - other->ivalue()));
        if(other->type() == typeid(Double)) return VALUE(new Double(this->value - other->dvalue()));

        throw BasicRuntimeError("Unsupported operand type(s) for `-`: (" + this->name() + "," + other->name() + ")");
    }


    /* ***************************************************************************
    * STRING
    */

    String::String(const string& v) {
        this->value = v;
    }

    const string& String::native() const {
        return this->value;
    }

    string String::encode(const string& decoded) {
        string encoded = "\"";

        for(int i=0; i<decoded.length(); i++) {
            const char c = decoded[i];

            switch(c) {
                case '\\' : encoded += "\\\\"; break;
                case '"'  : encoded += "\\\""; break;
                case '\r' : encoded += "\\r"; break;
                case '\n' : encoded += "\\n"; break;
                case '\t' : encoded += "\\t"; break;
                default   : encoded += c; break;
            }
        }

        encoded += "\"";

        return encoded;
    }

    string String::decode(const string& encoded0) {
        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> utf32to8;
        string encoded = encoded0;
        string decoded;
        int i = 0;
        int max = encoded.length();

        if(max>=2 && encoded[i]=='"' && encoded[max-1]=='"') {
            encoded = encoded.substr(1, max-2);
            max -= 2;
        }

        while(i < max) {
            if     (encoded.substr(i,2) == "\\\\")   { decoded += "\\"; i+=2; }
            else if(encoded.substr(i,2) == "\\\"")   { decoded += "\""; i+=2; }
            else if(encoded.substr(i,3) == "\\\r\n") { i+=3; }
            else if(encoded.substr(i,2) == "\\\r")   { i+=2; }
            else if(encoded.substr(i,2) == "\\\n")   { i+=2; }
            else if(encoded.substr(i,2) == "\\t")    { decoded += "\t"; i+=2; }
            else if(encoded.substr(i,2) == "\\r")    { decoded += "\r"; i+=2; }
            else if(encoded.substr(i,2) == "\\n")    { decoded += "\n"; i+=2; }
            else if(encoded.substr(i,2) == "\\x")    { decoded += utf32to8.to_bytes(Integer::decodeHex(encoded.substr(i+2,2))); i+=4; }
            else if(encoded.substr(i,2) == "\\u")    { decoded += utf32to8.to_bytes(Integer::decodeHex(encoded.substr(i+2,4))); i+=6; }
            else if(encoded.substr(i,2) == "\\U")    { decoded += utf32to8.to_bytes(Integer::decodeHex(encoded.substr(i+2,8))); i+=10; }
            else if(encoded.substr(i,1) == "\\")     { throw BasicSyntaxError("Invalid backslash sequence in string at position " + to_string(i)); }
            else                                     { decoded += encoded[i]; i+=1; }
        }

        return decoded;
    }

    vector<string> String::split(const string& text, const string& delim) {
        vector<string> lines;
        size_t pos = 0;

        while(true) {
            size_t next = text.find(delim, pos);

            if(next == string::npos) {
                lines.push_back(text.substr(pos));
                break;
            }

            lines.push_back(text.substr(pos, next-pos));
            pos = next+1;
        }

        return lines;
    }

    string String::name() const {
        return "STRING";
    }

    string String::encoded() const {
        return this->encode(this->value);
    }

    const type_info& String::type() const {
        return typeid(*this);
    }

    bool String::istrue() const {
        return this->value.length() == 0 ? false : true;
    }

    int64_t String::length() const {
        return this->value.length();
    }

    string String::svalue() const {
        return this->native();
    }

    VALUE String::op_lt(const VALUE other) const {
        if(other->type() == typeid(String)) return VALUE(new Integer(this->value < other->svalue()));

        throw BasicRuntimeError("Unsupported operand type(s) for `<`: (" + this->name() + "," + other->name() + ")");
    }

    VALUE String::op_gt(const VALUE other) const {
        if(other->type() == typeid(String)) return VALUE(new Integer(this->value > other->svalue()));

        throw BasicRuntimeError("Unsupported operand type(s) for `>`: (" + this->name() + "," + other->name() + ")");
    }

    VALUE String::op_eq(const VALUE other) const {
        if(other->type() == typeid(String)) return VALUE(new Integer(this->value == other->svalue()));

        return VALUE(new Integer(0));
    }

    VALUE String::op_ne(const VALUE other) const {
        if(other->type() == typeid(String)) return VALUE(new Integer(this->value != other->svalue()));

        return VALUE(new Integer(1));
    }

    VALUE String::op_lte(const VALUE other) const {
        if(other->type() == typeid(String)) return VALUE(new Integer(this->value <= other->svalue()));

        throw BasicRuntimeError("Unsupported operand type(s) for `<=`: (" + this->name() + "," + other->name() + ")");
    }

    VALUE String::op_gte(const VALUE other) const {
        if(other->type() == typeid(String)) return VALUE(new Integer(this->value >= other->svalue()));

        throw BasicRuntimeError("Unsupported operand type(s) for `>=`: (" + this->name() + "," + other->name() + ")");
    }

    VALUE String::op_add(const VALUE other) const {
        if(other->type() == typeid(String))  return VALUE(new String(this->value + other->svalue()));
        if(other->type() == typeid(Integer)) return VALUE(new String(this->value + other->svalue()));
        if(other->type() == typeid(Double))  return VALUE(new String(this->value + other->svalue()));

        throw BasicRuntimeError("Unsupported operand type(s) for `+`: (" + this->name() + "," + other->name() + ")");
    }


    /* ***************************************************************************
    * ARRAY
    */

    Array::Array() {
        /* Intentionally left blank */
    }

    Array::Array(const vector<VALUE>& v) {
        this->value = v;
    }

    const vector<VALUE>& Array::native() const {
        return this->value;
    }

    string Array::encode(const vector<VALUE>& decoded) {
        string encoded = "[";

        for(int i=0; i<decoded.size(); i++) {
            vector<string> childLines = String::split(decoded[i]->encoded(), "\n");

            if(i) encoded += ",";

            for(int j=0; j<childLines.size(); j++) {
                encoded += "\n  " + childLines[j];
            }
        }

        encoded += "\n]";

        return encoded;
    }

    VALUE Array::get(int64_t i) const {
        try {
            return this->value.at(i);
        }
        catch(std::out_of_range e) {
            throw BasicRuntimeError(string("Array index `") + std::to_string(i) + "` out of range, expected < " + std::to_string(this->value.size()));
        }
    }

    void Array::set(int64_t i, VALUE v) {
        if(this->value.size() < i) {
            this->value[i] = v;
        }
        else if(this->value.size() == i) {
            this->value.push_back(v);
        }
        else {
            throw BasicRuntimeError(string("Array index `") + std::to_string(i) + "` out of range, expected <= " + std::to_string(this->value.size()));
        }
    }

    void Array::push(VALUE v) {
        this->value.push_back(v);
    }

    string Array::name() const {
        return "ARRAY";
    }

    string Array::encoded() const {
        return this->encode(this->value);
    }

    const type_info& Array::type() const {
        return typeid(*this);
    }

    bool Array::istrue() const {
        return this->value.size() ? true : false;
    }

    int64_t Array::length() const {
        return this->value.size();
    }

    const vector<VALUE>& Array::avalue() const {
        return this->native();
    }

    string Array::svalue() const {
        return this->encoded();
    }

    VALUE Array::op_lt(const VALUE other) const {
        if(other->type() == typeid(Array)) return VALUE(new Integer(this->value < other->avalue()));

        throw BasicRuntimeError("Unsupported operand type(s) for `<`: (" + this->name() + "," + other->name() + ")");
    }

    VALUE Array::op_gt(const VALUE other) const {
        if(other->type() == typeid(Array)) return VALUE(new Integer(this->value > other->avalue()));

        throw BasicRuntimeError("Unsupported operand type(s) for `>`: (" + this->name() + "," + other->name() + ")");
    }

    VALUE Array::op_eq(const VALUE other) const {
        if(other->type() == typeid(Array)) return VALUE(new Integer(this->value == other->avalue()));

        return VALUE(new Integer(0));
    }

    VALUE Array::op_ne(const VALUE other) const {
        if(other->type() == typeid(Array)) return VALUE(new Integer(this->value != other->avalue()));

        return VALUE(new Integer(1));
    }

    VALUE Array::op_lte(const VALUE other) const {
        if(other->type() == typeid(Array)) return VALUE(new Integer(this->value <= other->avalue()));

        throw BasicRuntimeError("Unsupported operand type(s) for `<=`: (" + this->name() + "," + other->name() + ")");
    }

    VALUE Array::op_gte(const VALUE other) const {
        if(other->type() == typeid(Array)) return VALUE(new Integer(this->value >= other->avalue()));

        throw BasicRuntimeError("Unsupported operand type(s) for `>=`: (" + this->name() + "," + other->name() + ")");
    }

    VALUE Array::op_add(const VALUE other) const {
        if(other->type() == typeid(Array)) {
            const vector<VALUE>& ov = other->avalue();
            vector<VALUE> sum;

            sum.insert(sum.end(), this->value.begin(), this->value.end());
            sum.insert(sum.end(), ov.begin(), ov.end());

            return VALUE(new Array(sum));
        }

        throw BasicRuntimeError("Unsupported operand type(s) for `+`: (" + this->name() + "," + other->name() + ")");
    }


    /* ***************************************************************************
    * OBJECT
    */

    Object::Object() {
        /* Intentionally left blank */
    }

    Object::Object(initializer_list<pair<string,VALUE> > init) {
        for(auto rec : init) {
            this->value[rec.first] = rec.second;
        }
    }

    Object::Object(const map<string,VALUE>& v) {
        this->value = v;
    }

    const map<string,VALUE>& Object::native() const {
        return this->value;
    }

    string Object::encode(const map<string,VALUE>& decoded, SYMBOLS parent) {
        string encoded = "{";
        int i = 0;

        if(parent) {
            vector<string> childLines = String::split(parent->encoded(), "\n");
            encoded += "\n  __PARENT__ : ";

            for(int j=0; j<childLines.size(); j++) {
                if(j) encoded += "\n  ";
                encoded += childLines[j];
            }

            i++;
        }

        for(const pair<string,VALUE >& pi: decoded) {
            string name = pi.first;
            VALUE value = pi.second;

            if(i++) encoded += ",";
            encoded += "\n  " + name + " : ";

            if(pi.first == "GLOBAL" || pi.first == "UPSCOPE") {
                encoded += string("<") + pi.first + ">";
            }
            else {
                vector<string> childLines = String::split(value->encoded(), "\n");

                for(int j=0; j<childLines.size(); j++) {
                    if(j) encoded += "\n  ";
                    encoded += childLines[j];
                }
            }
        }

        encoded += "\n}";

        return encoded;
    }

    VALUE Object::get(const string& k) {
        try {
            return this->value.at(k);
        }
        catch(std::out_of_range e) {
            throw BasicRuntimeError(k + " is not a valid symbol");
        }
    }

    void Object::set(const string& k, VALUE v) {
        this->value[k] = v;
    }

    bool Object::has(const string& k) {
        return this->value.count(k) > 0;
    }

    string Object::name() const {
        return "OBJECT";
    }

    string Object::encoded() const {
        return this->encode(this->value);
    }

    const type_info& Object::type() const {
        return typeid(*this);
    }

    bool Object::istrue() const {
        return this->value.size() ? true : false;
    }

    int64_t Object::length() const {
        return this->value.size();
    }

    const map<string,VALUE>& Object::ovalue() const {
        return this->native();
    }

    string Object::svalue() const {
        return this->encoded();
    }

    VALUE Object::op_eq(const VALUE other) const {
        if(other->type() == typeid(Object)) return VALUE(new Integer(this->value == other->ovalue()));

        return VALUE(new Integer(0));
    }

    VALUE Object::op_ne(const VALUE other) const {
        if(other->type() == typeid(Object)) return VALUE(new Integer(this->value != other->ovalue()));

        return VALUE(new Integer(1));
    }


    /* ***************************************************************************
    * SYMBOL TABLE
    */

    SymbolTable::SymbolTable(initializer_list<pair<string,VALUE> > init): Object(init) {
        this->value.insert(builtins.begin(), builtins.end());
    }

    SymbolTable::SymbolTable(SYMBOLS parent) {
        this->parent = parent;
        this->root = parent;

        if(this->parent == nullptr) {
            this->value.insert(builtins.begin(), builtins.end());
        }
        else {
            this->root = parent->root ? parent->root : parent;
            this->set("UPSCOPE", parent);
            this->set("GLOBAL", this->root);
        }
    }

    VALUE SymbolTable::get(const string& k) {
        if(Object::has(k) || !this->parent) {
            return Object::get(k);
        }

        return this->parent->get(k);
    }

    void SymbolTable::set(const string& k, VALUE v) {
        if(this->parent && this->parent->has(k)) {
            this->parent->set(k, v);
        }

        return Object::set(k, v);
    }

    bool SymbolTable::has(const string& k) {
        bool hasit = Object::has(k);

        if(this->parent) {
            hasit = hasit || this->parent->has(k);
        }

        return hasit;
    }

    string SymbolTable::encoded() const {
        return this->encode(this->value, this->parent);
    }


    /* ***************************************************************************
    * FUNCTION
    */

    Function::Function(VALUE (*func)(const vector<VALUE>&), int64_t minargs, int64_t maxargs) {
        this->func = func;
        this->dfunc = nullptr;
        this->xfunc = nullptr;
        this->staticExpr = nullptr;
        this->minargs = minargs;
        this->maxargs = maxargs;
        this->scope = nullptr;
    }

    Function::Function(VALUE (*dfunc)(vector<LiteExprParser::ExprContext*>& vexpr, Evaluator* visitor), int64_t minargs, int64_t maxargs) {
        this->func = nullptr;
        this->dfunc = dfunc;
        this->xfunc = nullptr;
        this->staticExpr = nullptr;
        this->minargs = minargs;
        this->maxargs = maxargs;
        this->scope = nullptr;
    }

    Function::Function(VALUE (*xfunc)(vector<LiteExprParser::ExprContext*>& vexpr, Evaluator* visitor, SYMBOLS scope), SYMBOLS scope, int64_t minargs, int64_t maxargs) {
        this->func = nullptr;
        this->dfunc = nullptr;
        this->xfunc = xfunc;
        this->staticExpr = nullptr;
        this->minargs = minargs;
        this->maxargs = maxargs;
        this->scope = scope;
    }

    VALUE Function::call(vector<LiteExprParser::ExprContext*>& vexpr, Evaluator* visitor) const {
        if(vexpr.size() < this->minargs || this->maxargs < vexpr.size()) {
            throw BasicSyntaxError(string("Invalid argument count. min=")
                + std::to_string(this->minargs) + ", max="
                + std::to_string(this->maxargs) + ", got="
                + std::to_string(vexpr.size())
            );
        }

        if(this->func) {
            vector<VALUE> args;

            for(LiteExprParser::ExprContext* expr : vexpr) {
                args.push_back(any_cast<VALUE>(visitor->visit(expr)));
            }

            return this->func(args);
        }
        else if(this->xfunc) {
           vector<LiteExprParser::ExprContext*> sexpr;

           sexpr.push_back(this->staticExpr);
           sexpr.insert(sexpr.end(), vexpr.begin(), vexpr.end());

           return this->xfunc(sexpr, visitor, this->scope);
        }
        else if(this->staticExpr) {
           vector<LiteExprParser::ExprContext*> sexpr;

           sexpr.push_back(this->staticExpr);
           sexpr.insert(sexpr.end(), vexpr.begin(), vexpr.end());

           return this->dfunc(sexpr, visitor);
        }
        else {
            return this->dfunc(vexpr, visitor);
        }
    }

    SYMBOLS Function::getScope() const {
        return this->scope;
    }

    string Function::name() const {
        return "Function";
    }

    string Function::encoded() const {
        return "<Function>";
    }

    const type_info& Function::type() const {
        return typeid(*this);
    }

    void Function::setStaticExpr(LiteExprParser::ExprContext* staticExpr) {
        this->staticExpr = staticExpr;
    }
}


/* ***************************************************************************
* VARIABLE
*/

namespace liteexpr {
    Ident::Ident(VALUE container, VALUE key) {
        this->container = container;
        this->key = key;
    }

    Ident::~Ident() {
        this->container = nullptr;
        this->key = nullptr;
    }

    void Ident::set(VALUE other) {
        if(this->container->type() == typeid(Array)) {
            ARRAY array = dynamic_pointer_cast<Array>(this->container);

            array->set(this->key->ivalue(), other);
            return;
        }

        if(this->container->type() == typeid(Object) || this->container->type() == typeid(SymbolTable)) {
            OBJECT object = dynamic_pointer_cast<Object>(this->container);

            object->set(this->key->svalue(), other);
            return;
        }

        throw BasicRuntimeError(string("Invalid identifier set type: ") + this->container->type().name());
    }

    VALUE Ident::get() const {
        if(this->container->type() == typeid(Array)) {
            ARRAY array = dynamic_pointer_cast<Array>(this->container);
            VALUE value = array->get(this->key->ivalue());

            return value;
        }

        if(this->container->type() == typeid(Object) || this->container->type() == typeid(SymbolTable)) {
            OBJECT object = dynamic_pointer_cast<Object>(this->container);
            VALUE value = object->get(this->key->svalue());

            return value;
        }

        throw BasicRuntimeError(string("Invalid identifier get type: ") + this->container->type().name());
    }

    VALUE Ident::getkey() const {
        return this->key;
    }

    string Ident::name() const { return this->get()->name(); }
    string Ident::encoded() const { return this->get()->encoded(); }
    const type_info& Ident::type() const { return this->get()->type(); }
    bool Ident::istrue() const { return this->get()->istrue(); }
    int64_t Ident::length() const { return this->get()->length(); }
    int64_t Ident::ivalue() const { return this->get()->ivalue(); }
    double Ident::dvalue() const { return this->get()->dvalue(); }
    string Ident::svalue() const { return this->get()->svalue(); }
    const vector<VALUE>& Ident::avalue() const { return this->get()->avalue(); }
    const map<string,VALUE>& Ident::ovalue() const { return this->get()->ovalue(); }
    VALUE Ident::op_not() const { return this->get()->op_not(); }
    VALUE Ident::op_inv() const { return this->get()->op_inv(); }
    VALUE Ident::op_pos() const { return this->get()->op_pos(); }
    VALUE Ident::op_neg() const { return this->get()->op_neg(); }
    VALUE Ident::op_inc() const { return this->get()->op_inc(); }
    VALUE Ident::op_dec() const { return this->get()->op_dec(); }
    VALUE Ident::op_lt(const VALUE other) const { return this->get()->op_lt(other); }
    VALUE Ident::op_gt(const VALUE other) const { return this->get()->op_gt(other); }
    VALUE Ident::op_eq(const VALUE other) const { return this->get()->op_eq(other); }
    VALUE Ident::op_ne(const VALUE other) const { return this->get()->op_ne(other); }
    VALUE Ident::op_lte(const VALUE other) const { return this->get()->op_lte(other); }
    VALUE Ident::op_gte(const VALUE other) const { return this->get()->op_gte(other); }
    VALUE Ident::op_pow(const VALUE other) const { return this->get()->op_pow(other); }
    VALUE Ident::op_mul(const VALUE other) const { return this->get()->op_mul(other); }
    VALUE Ident::op_div(const VALUE other) const { return this->get()->op_div(other); }
    VALUE Ident::op_add(const VALUE other) const { return this->get()->op_add(other); }
    VALUE Ident::op_sub(const VALUE other) const { return this->get()->op_sub(other); }
    VALUE Ident::op_mod(const VALUE other) const { return this->get()->op_mod(other); }
    VALUE Ident::op_shl(const VALUE other) const { return this->get()->op_shl(other); }
    VALUE Ident::op_asr(const VALUE other) const { return this->get()->op_asr(other); }
    VALUE Ident::op_shr(const VALUE other) const { return this->get()->op_shr(other); }
    VALUE Ident::op_and(const VALUE other) const { return this->get()->op_and(other); }
    VALUE Ident::op_xor(const VALUE other) const { return this->get()->op_xor(other); }
    VALUE Ident::op_or(const VALUE other) const { return this->get()->op_or(other); }
}


/* ***************************************************************************
* EVALUATOR
*/

namespace liteexpr {
    typedef pair<string,VALUE> Pair;
    typedef vector<Pair> PairList;
    typedef vector<VALUE> List;

    Evaluator::Evaluator(SYMBOLS s) {
        this->symbols = s;
    }

    SYMBOLS Evaluator::getSymbols() {
        return this->symbols;
    }

    any Evaluator::visitFile(LiteExprParser::FileContext *ctx) {
        any result = VALUE(new Integer(0));

        if(ctx->expr()) {
            result = this->visit(ctx->expr());
        }

        return result;
    }

    any Evaluator::visitString(LiteExprParser::StringContext *ctx) {
        if(!this->cache.count(ctx)) {
            try {
                this->cache[ctx] = VALUE(new String(String::decode(ctx->STRING()->getText())));
            }
            catch(BasicSyntaxError e) {
                throw SyntaxError(string(e), ctx->start->getLine(), ctx->start->getCharPositionInLine());
            }
        }

        return this->cache[ctx];
    }

    any Evaluator::visitDouble(LiteExprParser::DoubleContext *ctx) {
        if(!this->cache.count(ctx)) {
            try {
                this->cache[ctx] = VALUE(new Double(Double::decode(ctx->DOUBLE()->getText())));
            }
            catch(BasicSyntaxError e) {
                throw SyntaxError(string(e), ctx->start->getLine(), ctx->start->getCharPositionInLine());
            }
        }

        return this->cache[ctx];
    }

    any Evaluator::visitHex(LiteExprParser::HexContext *ctx) {
        if(!this->cache.count(ctx)) {
            try {
                string encoded = ctx->HEX()->getText();
                this->cache[ctx] = VALUE(new Integer(Integer::decodeHex(encoded.substr(2))));
            }
            catch(BasicSyntaxError e) {
                throw SyntaxError(string(e), ctx->start->getLine(), ctx->start->getCharPositionInLine());
            }
        }

        return this->cache[ctx];
    }

    any Evaluator::visitInt(LiteExprParser::IntContext *ctx) {
        if(!this->cache.count(ctx)) {
            try {
                this->cache[ctx] = VALUE(new Integer(Integer::decode(ctx->INT()->getText())));
            }
            catch(BasicSyntaxError e) {
                throw SyntaxError(string(e), ctx->start->getLine(), ctx->start->getCharPositionInLine());
            }
        }

        return this->cache[ctx];
    }

    any Evaluator::visitArray(LiteExprParser::ArrayContext *ctx) {
        List result = any_cast<List>(this->visit(ctx->list()));

        return VALUE(new Array(result));
    }

    any Evaluator::visitObject(LiteExprParser::ObjectContext *ctx) {
        PairList plist = any_cast<PairList>(this->visit(ctx->pairlist()));
        map<string,VALUE> result;

        for(const Pair& pi: plist) {
            string k = pi.first;
            VALUE v = pi.second;

            result[k] = v;
        }

        return VALUE(new Object(result));
    }

    any Evaluator::visitPairlist(LiteExprParser::PairlistContext *ctx) {
        PairList plist;

        for(LiteExprParser::PairContext* pc: ctx->pair()) {
            Pair p = any_cast<Pair>(this->visit(pc));

            plist.push_back(p);
        }

        return plist;
    }

    any Evaluator::visitPair(LiteExprParser::PairContext *ctx) {
        string k = ctx->ID()->getText();
        VALUE v = any_cast<VALUE>(this->visit(ctx->expr()));

        return Pair(k,v);
    }

    any Evaluator::visitList(LiteExprParser::ListContext *ctx) {
        List result;

        for(LiteExprParser::ExprContext* expr: ctx->expr()) {
            result.push_back(any_cast<VALUE>(this->visit(expr)));
        }

        return result;
    }

    any Evaluator::visitCall(LiteExprParser::CallContext *ctx) {
        VALUE varname = any_cast<VALUE>(this->visit(ctx->varname()));
        IDENT ident = dynamic_pointer_cast<Ident>(varname);
        VALUE value;

        try {
            value = ident->get();
        }
        catch(std::out_of_range e) {
            throw RuntimeError(ident->getkey()->svalue() + " is not a valid symbol", ctx->start->getLine(), ctx->start->getCharPositionInLine());
        }

        FUNCTION fnname = dynamic_pointer_cast<Function>(value);
        vector<LiteExprParser::ExprContext*> vexpr = ctx->list()->expr();

        try {
            VALUE result = fnname->call(vexpr, this);

            return result;
        }
        catch(BasicRuntimeError e) {
            throw RuntimeError("Runtime error while executing `" + ctx->getText()  + "`:\n" + string(e), ctx->start->getLine(), ctx->start->getCharPositionInLine());
        }
        catch(BasicSyntaxError e) {
            throw SyntaxError("Syntax error while executing `" + ctx->getText()  + "`:\n" + string(e), ctx->start->getLine(), ctx->start->getCharPositionInLine());
        }
    }

    any Evaluator::visitParen(LiteExprParser::ParenContext *ctx) {
        return this->visit(ctx->expr());
    }

    any Evaluator::visitPrefixOp(LiteExprParser::PrefixOpContext *ctx) {
        string op = ctx->op->getText();
        VALUE left = any_cast<VALUE>(this->visit(ctx->varname()));
        IDENT lvalue = dynamic_pointer_cast<Ident>(left);
        VALUE result;

        try {
            if     (op == "++") result = left->op_inc();
            else if(op == "--") result = left->op_dec();
            else throw SyntaxError("Unknown prefix operator `" + op + "`", ctx->op->getLine(), ctx->op->getCharPositionInLine());
        }
        catch(BasicRuntimeError e) {
            throw RuntimeError(string(e), ctx->op->getLine(), ctx->op->getCharPositionInLine());
        }

        lvalue->set(result);

        return left;
    }

    any Evaluator::visitPostfixOp(LiteExprParser::PostfixOpContext *ctx) {
        string op = ctx->op->getText();
        VALUE left = any_cast<VALUE>(this->visit(ctx->varname()));
        IDENT lvalue = dynamic_pointer_cast<Ident>(left);
        VALUE last = lvalue->get();
        VALUE result;

        try {
            if     (op == "++") result = left->op_inc();
            else if(op == "--") result = left->op_dec();
            else throw SyntaxError("Unknown postfix operator `" + op + "`", ctx->op->getLine(), ctx->op->getCharPositionInLine());
        }
        catch(BasicRuntimeError e) {
            throw RuntimeError(string(e), ctx->op->getLine(), ctx->op->getCharPositionInLine());
        }

        lvalue->set(result);

        return last;
    }

    any Evaluator::visitUnaryOp(LiteExprParser::UnaryOpContext *ctx) {
        string op = ctx->op->getText();
        VALUE value = any_cast<VALUE>(this->visit(ctx->expr()));
        VALUE result;

        try {
            if     (op == "!") result = value->op_not();
            else if(op == "~") result = value->op_inv();
            else if(op == "+") result = value->op_pos();
            else if(op == "-") result = value->op_neg();
            else throw SyntaxError(string("Unknown unary operator `") + op + "`", ctx->op->getLine(), ctx->op->getCharPositionInLine());
        }
        catch(BasicRuntimeError e) {
            throw RuntimeError(string(e), ctx->op->getLine(), ctx->op->getCharPositionInLine());
        }

        return result;
    }

    any Evaluator::visitBinaryOp(LiteExprParser::BinaryOpContext *ctx) {
        string op = ctx->op->getText();
        VALUE left = any_cast<VALUE>(this->visit(ctx->expr(0)));
        antlr4::tree::ParseTree* rexpr = ctx->expr(1);
        VALUE result;

        try {
            if     (op == "**")  result = left->op_pow(any_cast<VALUE>(this->visit(rexpr)));
            else if(op == "*")   result = left->op_mul(any_cast<VALUE>(this->visit(rexpr)));
            else if(op == "/")   result = left->op_div(any_cast<VALUE>(this->visit(rexpr)));
            else if(op == "%")   result = left->op_mod(any_cast<VALUE>(this->visit(rexpr)));
            else if(op == "+")   result = left->op_add(any_cast<VALUE>(this->visit(rexpr)));
            else if(op == "-")   result = left->op_sub(any_cast<VALUE>(this->visit(rexpr)));
            else if(op == "<<")  result = left->op_shl(any_cast<VALUE>(this->visit(rexpr)));
            else if(op == ">>")  result = left->op_asr(any_cast<VALUE>(this->visit(rexpr)));
            else if(op == ">>>") result = left->op_shr(any_cast<VALUE>(this->visit(rexpr)));
            else if(op == "<")   result = left->op_lt(any_cast<VALUE>(this->visit(rexpr)));
            else if(op == ">")   result = left->op_gt(any_cast<VALUE>(this->visit(rexpr)));
            else if(op == "==")  result = left->op_eq(any_cast<VALUE>(this->visit(rexpr)));
            else if(op == "!=")  result = left->op_ne(any_cast<VALUE>(this->visit(rexpr)));
            else if(op == "<=")  result = left->op_lte(any_cast<VALUE>(this->visit(rexpr)));
            else if(op == ">=")  result = left->op_gte(any_cast<VALUE>(this->visit(rexpr)));
            else if(op == "|")   result = left->op_or(any_cast<VALUE>(this->visit(rexpr)));
            else if(op == "^")   result = left->op_xor(any_cast<VALUE>(this->visit(rexpr)));
            else if(op == "&")   result = left->op_and(any_cast<VALUE>(this->visit(rexpr)));
            else if(op == "||")  result = left->istrue() ? left : any_cast<VALUE>(this->visit(rexpr));
            else if(op == "&&")  result = left->istrue() ? any_cast<VALUE>(this->visit(rexpr)) : left;
            else if(op == ";")   result = any_cast<VALUE>(this->visit(rexpr));
            else throw SyntaxError("Unknown binary operator `" + op + "`", ctx->op->getLine(), ctx->op->getCharPositionInLine());
        }
        catch(BasicRuntimeError e) {
            throw RuntimeError(string(e), ctx->op->getLine(), ctx->op->getCharPositionInLine());
        }

        return result;
    }

    any Evaluator::visitTernaryOp(LiteExprParser::TernaryOpContext *ctx) {
        string op1 = ctx->op1->getText();
        string op2 = ctx->op2->getText();
        VALUE left = any_cast<VALUE>(this->visit(ctx->expr(0)));
        antlr4::tree::ParseTree* expr1 = ctx->expr(1);
        antlr4::tree::ParseTree* expr2 = ctx->expr(2);
        VALUE result;

        try {
            if (op1 == "?" && op2 == ":") {
                result = left->istrue() ? any_cast<VALUE>(this->visit(expr1)) : any_cast<VALUE>(this->visit(expr2));
            }
            else throw SyntaxError("Unknown ternary operator `" + op1 + " " + op2 + "`", ctx->op1->getLine(), ctx->op1->getCharPositionInLine());
        }
        catch(BasicRuntimeError e) {
            throw RuntimeError(string(e), ctx->op1->getLine(), ctx->op1->getCharPositionInLine());
        }

        return result;
    }

    any Evaluator::visitAssignOp(LiteExprParser::AssignOpContext *ctx) {
        string op = ctx->op->getText();
        VALUE left = any_cast<VALUE>(this->visit(ctx->varname()));
        IDENT lvalue = dynamic_pointer_cast<Ident>(left);
        antlr4::tree::ParseTree* rexpr = ctx->expr();
        VALUE result;

        try {
            if     (op == "=")    result = any_cast<VALUE>(this->visit(rexpr));
            else if(op == "**=")  result = left->op_pow(any_cast<VALUE>(this->visit(rexpr)));
            else if(op == "*=")   result = left->op_mul(any_cast<VALUE>(this->visit(rexpr)));
            else if(op == "/=")   result = left->op_div(any_cast<VALUE>(this->visit(rexpr)));
            else if(op == "%=")   result = left->op_mod(any_cast<VALUE>(this->visit(rexpr)));
            else if(op == "+=")   result = left->op_add(any_cast<VALUE>(this->visit(rexpr)));
            else if(op == "-=")   result = left->op_sub(any_cast<VALUE>(this->visit(rexpr)));
            else if(op == "<<=")  result = left->op_shl(any_cast<VALUE>(this->visit(rexpr)));
            else if(op == ">>=")  result = left->op_asr(any_cast<VALUE>(this->visit(rexpr)));
            else if(op == ">>>=") result = left->op_shr(any_cast<VALUE>(this->visit(rexpr)));
            else if(op == "|=")   result = left->op_or(any_cast<VALUE>(this->visit(rexpr)));
            else if(op == "^=")   result = left->op_xor(any_cast<VALUE>(this->visit(rexpr)));
            else if(op == "&=")   result = left->op_and(any_cast<VALUE>(this->visit(rexpr)));
            else if(op == "||=")  result = left->istrue() ? left : any_cast<VALUE>(this->visit(rexpr));
            else if(op == "&&=")  result = left->istrue() ? any_cast<VALUE>(this->visit(rexpr)) : left;
            else throw SyntaxError("Unknown assignment operator `" + op + "`", ctx->op->getLine(), ctx->op->getCharPositionInLine());
        }
        catch(BasicRuntimeError e) {
            throw RuntimeError(string(e), ctx->op->getLine(), ctx->op->getCharPositionInLine());
        }

        lvalue->set(result);

        return left;
    }

    any Evaluator::visitVariable(LiteExprParser::VariableContext *ctx) {
        return this->visit(ctx->varname());
    }

    any Evaluator::visitMemberVar(LiteExprParser::MemberVarContext *ctx) {
        IDENT name = dynamic_pointer_cast<Ident>(any_cast<VALUE>(this->visit(ctx->varname(0))));
        IDENT value = dynamic_pointer_cast<Ident>(any_cast<VALUE>(this->visit(ctx->varname(1))));

        return VALUE(new Ident(name->get(), value->getkey()));
    }

    any Evaluator::visitIndexedVar(LiteExprParser::IndexedVarContext *ctx) {
        IDENT name = dynamic_pointer_cast<Ident>(any_cast<VALUE>(this->visit(ctx->varname())));
        VALUE value = any_cast<VALUE>(this->visit(ctx->expr()));

        return VALUE(new Ident(name->get(), value));
    }

    any Evaluator::visitSimpleVar(LiteExprParser::SimpleVarContext *ctx) {
        string name = ctx->ID()->getText();

        return VALUE(new Ident(this->symbols, VALUE(new String(name))));
    }

    any Evaluator::visitTerm(LiteExprParser::TermContext *ctx) {
        return this->visit(ctx->expr());
    }


    /* ***************************************************************************
    * COMPILED
    */

    Compiled::Compiled(string expr) {
        this->input = new antlr4::ANTLRInputStream(expr);
        this->lexer = new LiteExprLexer(this->input);

        this->tokens = new antlr4::CommonTokenStream(this->lexer);
        this->parser = new LiteExprParser(this->tokens);

        this->parser->setErrorHandler(std::make_shared<antlr4::BailErrorStrategy>());

        try {
            this->parseTree = this->parser->file();
        }
        catch(antlr4::ParseCancellationException e) {
            const antlr4::Token* token = this->parser->getCurrentToken();
            string message = string("Unexpected token `") + token->getText() + "`";
            int line = token->getLine();
            int col = token->getCharPositionInLine();

            throw SyntaxError(message, line, col);
        }
    }

    Compiled::~Compiled() {
        if(this->parser) delete(this->parser);
        if(this->tokens) delete(this->tokens);
        if(this->lexer) delete(this->lexer);
        if(this->input) delete(this->input);

        this->parser = nullptr;
        this->tokens = nullptr;
        this->lexer = nullptr;
        this->input = nullptr;
    }

    VALUE Compiled::eval(SYMBOLS symbols) {
        Evaluator evaluator(symbols);
        any result = evaluator.visit(this->parseTree);

        return any_cast<VALUE>(result);
    }

    /* ***************************************************************************
    * PUBLIC FUNCTIONS
    */

    Compiled compile(const string& expr) {
        return Compiled(expr);
    }

    VALUE eval(const string& expr, SYMBOLS symbols) {
        Compiled compiled = compile(expr);
        VALUE result = compiled.eval(symbols);

        return result;
    }
}


/* ***************************************************************************
* EXCEPTIONS
*/

namespace liteexpr {
    Error::Error(const string& t) {
        this->text = t;
    }

    Error::Error(const string& t, int line) {
        this->text = "[line " + to_string(line) + "] " + t;
    }

    Error::Error(const string& t, int line, int col) {
        this->text = "[line " + to_string(line) + ", col " + to_string(col+1) + "] " + t;
    };
}


/* ***************************************************************************
* BUILT-INS
*/

namespace liteexpr {
    static VALUE builtin_ceil(const vector<VALUE>& vv) {
        VALUE v = vv[0];

        if(v->type() == typeid(Integer))                           return VALUE(new Integer(ceil(v->dvalue())));
        if(v->type() == typeid(Double) && std::isnan(v->dvalue())) return VALUE(new Double(0.0/0.0));
        if(v->type() == typeid(Double) && std::isinf(v->dvalue())) return v->dvalue() < 0 ? VALUE(new Double(-1.0/0.0)) : VALUE(new Double(1.0/0.0));
        if(v->type() == typeid(Double) && v->dvalue() > MAXINT)    return VALUE(new Double(ceil(v->dvalue())));
        if(v->type() == typeid(Double) && v->dvalue() < MININT)    return VALUE(new Double(ceil(v->dvalue())));
        if(v->type() == typeid(Double))                            return VALUE(new Integer(ceil(v->dvalue())));

        throw BasicRuntimeError("Unsupported argument to `CEIL()`: (" + v->name() + ")");
    }

    static VALUE builtin_eval(vector<LiteExprParser::ExprContext*>& vexpr, Evaluator* visitor) {
        VALUE v = any_cast<VALUE>(visitor->visit(vexpr[0]));

        if(v->type() == typeid(String)) {
            return eval(v->svalue(), visitor->getSymbols());
        }

        throw BasicRuntimeError("Unsupported argument to `EVAL()`: (" + v->name() + ")");
    }

    static VALUE builtin_floor(const vector<VALUE>& vv) {
        VALUE v = vv[0];

        if(v->type() == typeid(Integer))                           return VALUE(new Integer(floor(v->dvalue())));
        if(v->type() == typeid(Double) && std::isnan(v->dvalue())) return VALUE(new Double(0.0/0.0));
        if(v->type() == typeid(Double) && std::isinf(v->dvalue())) return v->dvalue() < 0 ? VALUE(new Double(-1.0/0.0)) : VALUE(new Double(1.0/0.0));
        if(v->type() == typeid(Double) && v->dvalue() > MAXINT)    return VALUE(new Double(floor(v->dvalue())));
        if(v->type() == typeid(Double) && v->dvalue() < MININT)    return VALUE(new Double(floor(v->dvalue())));
        if(v->type() == typeid(Double))                            return VALUE(new Integer(floor(v->dvalue())));

        throw BasicRuntimeError("Unsupported argument to `FLOOR()`: (" + v->name() + ")");
    }

    static VALUE builtin_for(vector<LiteExprParser::ExprContext*>& vexpr, Evaluator* visitor) {
        VALUE result(new Integer(0));

        visitor->visit(vexpr[0]);

        while(any_cast<VALUE>(visitor->visit(vexpr[1]))->istrue()) {
            result = any_cast<VALUE>(visitor->visit(vexpr[3]));

            visitor->visit(vexpr[2]);
        }

        return result;
    }

    static VALUE builtin_foreach(vector<LiteExprParser::ExprContext*>& vexpr, Evaluator* visitor) {
        VALUE result(new Integer(0));
        IDENT ident = dynamic_pointer_cast<Ident>(any_cast<VALUE>(visitor->visit(vexpr[0])));
        VALUE iterable = any_cast<VALUE>(visitor->visit(vexpr[1]));

        if(iterable->type() == typeid(Array)) {
            for(auto v: iterable->avalue()) {
                ident->set(v);
                result = any_cast<VALUE>(visitor->visit(vexpr[2]));
            }
        }
        else if(iterable->type() == typeid(Object) || iterable->type() == typeid(SymbolTable)) {
            for(auto v: iterable->ovalue()) {
                VALUE name(new String(v.first));
                VALUE value(v.second);
                vector<VALUE> pair = { name, value };

                ident->set(VALUE(new Array(pair)));
                result = any_cast<VALUE>(visitor->visit(vexpr[2]));
            }
        }
        else throw BasicRuntimeError("Argument 2 to `FOREACH` must be an iterable, got (" + iterable->name() + ")");

        return result;
    }

    static VALUE builtin_function(vector<LiteExprParser::ExprContext*>& vexpr, Evaluator* visitor) {
        STRING argfmt = dynamic_pointer_cast<String>(any_cast<VALUE>(visitor->visit(vexpr[0])));
        LiteExprParser::ExprContext* parseTree = vexpr[1];
        int64_t minargs = 0;
        int64_t maxargs = 0;

        for(char c: argfmt->svalue()) {
            switch(c) {
                case '?' : minargs++;         break;
                case '*' : maxargs = MAXARGS; break;
                default:
                    throw RuntimeError(string() + c + " is an invalid function signature",
                        vexpr[0]->start->getLine(),
                        vexpr[0]->start->getCharPositionInLine()
                    );
            }

            if(maxargs < minargs) maxargs = minargs;
        }

        Function* func = new Function(
            [](vector<LiteExprParser::ExprContext*>& ivexpr, Evaluator* ivisitor, SYMBOLS upscope) {
                SYMBOLS scope = SYMBOLS(new SymbolTable(upscope));
                Evaluator evaluator(scope);
                ARRAY args(new Array());

                for(auto expr=ivexpr.begin()+1; expr!=ivexpr.end(); expr++) {
                    args->push(any_cast<VALUE>(ivisitor->visit(*expr)));
                }

                scope->set("ARG", args);

                return any_cast<VALUE>(evaluator.visit(ivexpr[0]));
            }, visitor->getSymbols(), minargs, maxargs
        );

        func->setStaticExpr(vexpr[1]);

        return VALUE(func);
    }

    static VALUE builtin_if(vector<LiteExprParser::ExprContext*>& vexpr, Evaluator* visitor) {
        VALUE result(new Integer(0));
        int i = 0;

        /* IF + ELIF */
        for(i=0; i+1<vexpr.size(); i+=2) {
            if(any_cast<VALUE>(visitor->visit(vexpr[i]))->istrue()) {
                result = any_cast<VALUE>(visitor->visit(vexpr[i+1]));
                break;
            }
        }

        /* ELSE */
        if(i == vexpr.size()-1) {
            result = any_cast<VALUE>(visitor->visit(vexpr[i]));
        }

        return result;
    }

    static VALUE builtin_len(const vector<VALUE>& vv) {
        try {
            return VALUE(new Integer(vv[0]->length()));
        }
        catch(BasicRuntimeError e) {
            throw BasicRuntimeError("Unsupported argument to `LEN()`: (" + vv[0]->name() + ")");
        }
    }

    static VALUE builtin_print(const vector<VALUE>& vv) {
        for(int i = 0; i < vv.size(); i++) {
            VALUE v = vv[i];

            if(i) std::cout << " ";
            std::cout << v->svalue();
        }

        std::cout << std::endl;

        return VALUE(new Integer(vv.size()));
    }

    static VALUE builtin_round(const vector<VALUE>& vv) {
        VALUE v = vv[0];

        if(v->type() == typeid(Integer))                           return VALUE(new Integer(round(v->dvalue())));
        if(v->type() == typeid(Double) && std::isnan(v->dvalue())) return VALUE(new Double(0.0/0.0));
        if(v->type() == typeid(Double) && std::isinf(v->dvalue())) return v->dvalue() < 0 ? VALUE(new Double(-1.0/0.0)) : VALUE(new Double(1.0/0.0));
        if(v->type() == typeid(Double) && v->dvalue() > MAXINT)    return VALUE(new Double(round(v->dvalue())));
        if(v->type() == typeid(Double) && v->dvalue() < MININT)    return VALUE(new Double(round(v->dvalue())));
        if(v->type() == typeid(Double))                            return VALUE(new Integer(round(v->dvalue())));

        throw BasicRuntimeError("Unsupported argument to `ROUND()`: (" + v->name() + ")");
    }

    static VALUE builtin_sqrt(const vector<VALUE>& vv) {
        VALUE v = vv[0];

        if(v->type() == typeid(Integer) || v->type() == typeid(Double)) {
            double d = v->dvalue();

            return VALUE(new Double(sqrt(d)));
        }

        throw BasicRuntimeError("Unsupported argument to `SQRT()`: (" + v->name() + ")");
    }

    static VALUE builtin_while(vector<LiteExprParser::ExprContext*>& vexpr, Evaluator* visitor) {
        VALUE result(new Integer(0));

        while(any_cast<VALUE>(visitor->visit(vexpr[0]))->istrue()) {
            result = any_cast<VALUE>(visitor->visit(vexpr[1]));
        }

        return result;
    }

    map<string,VALUE> builtins = {
        { "CEIL"     , VALUE(new Function(builtin_ceil     , 1,       1)) },
        { "EVAL"     , VALUE(new Function(builtin_eval     , 1,       1)) },
        { "FLOOR"    , VALUE(new Function(builtin_floor    , 1,       1)) },
        { "FOR"      , VALUE(new Function(builtin_for      , 4,       4)) },
        { "FOREACH"  , VALUE(new Function(builtin_foreach  , 3,       3)) },
        { "FUNCTION" , VALUE(new Function(builtin_function , 2,       2)) },
        { "IF"       , VALUE(new Function(builtin_if       , 2, MAXARGS)) },
        { "LEN"      , VALUE(new Function(builtin_len      , 1         )) },
        { "PRINT"    , VALUE(new Function(builtin_print                )) },
        { "ROUND"    , VALUE(new Function(builtin_round    , 1         )) },
        { "SQRT"     , VALUE(new Function(builtin_sqrt     , 1         )) },
        { "WHILE"    , VALUE(new Function(builtin_while    , 2,       2)) },
    };
}


/* ***************************************************************************
* HELPER FUNCTIONS
*/

namespace liteexpr {
    VALUE make_value(int v) {
        return VALUE(new Integer(v));
    }

    VALUE make_value(int64_t v) {
        return VALUE(new Integer(v));
    }

    VALUE make_value(double v) {
        return VALUE(new Double(v));
    }

    VALUE make_value(const string& v) {
        return VALUE(new String(v));
    }

    VALUE make_value(initializer_list<VALUE> vv) {
        return VALUE(new Array(vv));
    }

    VALUE make_value(initializer_list<pair<string,VALUE> > vv) {
        return VALUE(new Object(vv));
    }

    INTEGER make_integer(int64_t v) {
        return INTEGER(new Integer(v));
    }

    DOUBLE make_double(double v) {
        return DOUBLE(new Double(v));
    }

    STRING make_string(const string& v) {
        return STRING(new String(v));
    }

    ARRAY make_array(initializer_list<VALUE> vv) {
        return ARRAY(new Array(vv));
    }

    OBJECT make_object(initializer_list<pair<string,VALUE> > vv) {
        return OBJECT(new Object(vv));
    }

    SYMBOLS make_symbols(initializer_list<pair<string,VALUE> > vv) {
        return SYMBOLS(new SymbolTable(vv));
    }
}

