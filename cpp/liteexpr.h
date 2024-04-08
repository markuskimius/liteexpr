#ifndef LITEEXPR_H_
#define LITEEXPR_H_

#include <any>
#include <map>
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <cstdint>
#include <codecvt>
#include <iostream>
#include <initializer_list>
#include "antlr4-runtime.h"
#include "LiteExprLexer.h"
#include "LiteExprParser.h"
#include "LiteExprBaseVisitor.h"


/* ***************************************************************************
* VALUE
*/

namespace liteexpr {
    using std::any;
    using std::map;
    using std::pair;
    using std::vector;
    using std::string;
    using std::type_info;
    using std::shared_ptr;
    using std::initializer_list;

    class Value;
    class Integer;
    class Double;
    class String;
    class Array;
    class Object;
    class SymbolTable;
    class Ident;
    class Function;
    class Evaluator;
    typedef shared_ptr<Value> VALUE;
    typedef shared_ptr<Integer> INTEGER;
    typedef shared_ptr<Double> DOUBLE;
    typedef shared_ptr<String> STRING;
    typedef shared_ptr<Array> ARRAY;
    typedef shared_ptr<Object> OBJECT;
    typedef shared_ptr<SymbolTable> SYMBOLS;
    typedef shared_ptr<Ident> IDENT;
    typedef shared_ptr<Function> FUNCTION;
    extern int64_t MAXARGS;

    class Value {
        public:
            virtual string name() const=0;
            virtual string encoded() const=0;
            virtual const type_info& type() const=0;

            virtual bool istrue() const;
            virtual int64_t length() const;
            virtual int64_t ivalue() const;
            virtual double dvalue() const;
            virtual string svalue() const;
            virtual const vector<VALUE>& avalue() const;
            virtual const map<string,VALUE>& ovalue() const;
            virtual VALUE op_not() const;
            virtual VALUE op_inv() const;
            virtual VALUE op_pos() const;
            virtual VALUE op_neg() const;
            virtual VALUE op_inc() const;
            virtual VALUE op_dec() const;
            virtual VALUE op_lt(const VALUE other) const;
            virtual VALUE op_gt(const VALUE other) const;
            virtual VALUE op_eq(const VALUE other) const;
            virtual VALUE op_ne(const VALUE other) const;
            virtual VALUE op_lte(const VALUE other) const;
            virtual VALUE op_gte(const VALUE other) const;
            virtual VALUE op_pow(const VALUE other) const;
            virtual VALUE op_mul(const VALUE other) const;
            virtual VALUE op_div(const VALUE other) const;
            virtual VALUE op_add(const VALUE other) const;
            virtual VALUE op_sub(const VALUE other) const;
            virtual VALUE op_mod(const VALUE other) const;
            virtual VALUE op_shl(const VALUE other) const;
            virtual VALUE op_asr(const VALUE other) const;
            virtual VALUE op_shr(const VALUE other) const;
            virtual VALUE op_and(const VALUE other) const;
            virtual VALUE op_xor(const VALUE other) const;
            virtual VALUE op_or(const VALUE other) const;
    };

    class Integer: public Value {
        int64_t value;

        public:
            Integer(int64_t v);
            int64_t native() const;
            static string encode(int64_t decoded);
            static int64_t decode(const string& encoded);
            static int64_t decodeHex(const string& encoded);

            string name() const override;
            string encoded() const override;
            const type_info& type() const override;

            bool istrue() const override;
            int64_t ivalue() const override;
            double dvalue() const override;
            string svalue() const override;
            VALUE op_inv() const override;
            VALUE op_pos() const override;
            VALUE op_neg() const override;
            VALUE op_inc() const override;
            VALUE op_dec() const override;
            VALUE op_lt(const VALUE other) const override;
            VALUE op_gt(const VALUE other) const override;
            VALUE op_eq(const VALUE other) const override;
            VALUE op_ne(const VALUE other) const override;
            VALUE op_lte(const VALUE other) const override;
            VALUE op_gte(const VALUE other) const override;
            VALUE op_pow(const VALUE other) const override;
            VALUE op_mul(const VALUE other) const override;
            VALUE op_div(const VALUE other) const override;
            VALUE op_add(const VALUE other) const override;
            VALUE op_sub(const VALUE other) const override;
            VALUE op_mod(const VALUE other) const override;
            VALUE op_shl(const VALUE other) const override;
            VALUE op_asr(const VALUE other) const override;
            VALUE op_shr(const VALUE other) const override;
            VALUE op_and(const VALUE other) const override;
            VALUE op_xor(const VALUE other) const override;
            VALUE op_or(const VALUE other) const override;
    };

    class Double: public Value {
        double value;

        public:
            Double(double v);
            double native() const;
            static string encode(double decoded);
            static double decode(const string& encoded);

            string name() const override;
            string encoded() const override;
            const type_info& type() const override;

            bool istrue() const override;
            int64_t ivalue() const override;
            double dvalue() const override;
            string svalue() const override;
            VALUE op_pos() const override;
            VALUE op_neg() const override;
            VALUE op_lt(const VALUE other) const override;
            VALUE op_gt(const VALUE other) const override;
            VALUE op_eq(const VALUE other) const override;
            VALUE op_ne(const VALUE other) const override;
            VALUE op_lte(const VALUE other) const override;
            VALUE op_gte(const VALUE other) const override;
            VALUE op_pow(const VALUE other) const override;
            VALUE op_mul(const VALUE other) const override;
            VALUE op_div(const VALUE other) const override;
            VALUE op_add(const VALUE other) const override;
            VALUE op_sub(const VALUE other) const override;
    };

    class String: public Value {
        string value;

        public:
            String(const string& v);
            const string& native() const;
            static string encode(const string& decoded);
            static string decode(const string& encoded);
            static vector<string> split(const string& text, const string& delim);

            string name() const override;
            string encoded() const override;
            const type_info& type() const override;

            bool istrue() const override;
            int64_t length() const override;
            string svalue() const override;
            VALUE op_lt(const VALUE other) const override;
            VALUE op_gt(const VALUE other) const override;
            VALUE op_eq(const VALUE other) const override;
            VALUE op_ne(const VALUE other) const override;
            VALUE op_lte(const VALUE other) const override;
            VALUE op_gte(const VALUE other) const override;
            VALUE op_add(const VALUE other) const override;
    };

    class Array: public Value {
        vector<VALUE> value;

        public:
            Array();
            Array(const vector<VALUE>& v);
            const vector<VALUE>& native() const;
            static string encode(const vector<VALUE>& decoded);
            VALUE get(int64_t i) const;
            void set(int64_t i, VALUE v);
            void push(VALUE v);

            string name() const override;
            string encoded() const override;
            const type_info& type() const override;

            bool istrue() const override;
            int64_t length() const override;
            const vector<VALUE>& avalue() const override;
            string svalue() const override;
            VALUE op_lt(const VALUE other) const override;
            VALUE op_gt(const VALUE other) const override;
            VALUE op_eq(const VALUE other) const override;
            VALUE op_ne(const VALUE other) const override;
            VALUE op_lte(const VALUE other) const override;
            VALUE op_gte(const VALUE other) const override;
            VALUE op_add(const VALUE other) const override;
    };

    class Object: public Value {
        protected:
            map<string,VALUE> value;
            Object();

        public:
            Object(initializer_list<pair<string,VALUE> > init);
            Object(const map<string,VALUE>& v);
            const map<string,VALUE>& native() const;
            static string encode(const map<string,VALUE>& decoded, SYMBOLS parent=nullptr);
            virtual VALUE get(const string& k);
            virtual void set(const string& k, VALUE v);
            virtual bool has(const string& k);

            virtual string name() const override;
            virtual string encoded() const override;
            virtual const type_info& type() const override;

            virtual bool istrue() const override;
            virtual int64_t length() const override;
            virtual const map<string,VALUE>& ovalue() const override;
            virtual string svalue() const override;
            virtual VALUE op_eq(const VALUE other) const override;
            virtual VALUE op_ne(const VALUE other) const override;
    };

    class SymbolTable: public Object {
        SYMBOLS parent;
        SYMBOLS root;

        public:
            SymbolTable(initializer_list<pair<string,VALUE> > init);
            SymbolTable(SYMBOLS parent=nullptr);
            VALUE get(const string& k) override;
            void set(const string& k, VALUE v) override;
            bool has(const string& k) override;

            string encoded() const override;
    };

    class Function: public Value {
        VALUE (*func)(const vector<VALUE>&);
        VALUE (*dfunc)(vector<LiteExprParser::ExprContext*>& vexpr, Evaluator* visitor);
        VALUE (*xfunc)(vector<LiteExprParser::ExprContext*>& vexpr, Evaluator* visitor, SYMBOLS scope);
        LiteExprParser::ExprContext* staticExpr;
        int64_t minargs;
        int64_t maxargs;
        SYMBOLS scope;

        public:
            Function(VALUE (*func)(const vector<VALUE>& vv), int64_t minargs=0, int64_t maxargs=MAXARGS);
            Function(VALUE (*func)(vector<LiteExprParser::ExprContext*>& vexpr, Evaluator* visitor), int64_t minargs=0, int64_t maxargs=MAXARGS);
            Function(VALUE (*func)(vector<LiteExprParser::ExprContext*>& vexpr, Evaluator* visitor, SYMBOLS scope), SYMBOLS scope, int64_t minargs=0, int64_t maxargs=MAXARGS);

            VALUE call(vector<LiteExprParser::ExprContext*>& vexpr, Evaluator* visitor) const;
            SYMBOLS getScope() const;

            string name() const override;
            string encoded() const override;
            const type_info& type() const override;

            void setStaticExpr(LiteExprParser::ExprContext*);
    };
}


/* ***************************************************************************
* VARIABLE
*/

namespace liteexpr {
    class Ident: public Value {
        VALUE container;
        VALUE key;

        public:
            Ident(VALUE container, VALUE key);
            virtual ~Ident();
            void set(VALUE other);
            VALUE get() const;
            VALUE getkey() const;

            string name() const override;
            string encoded() const override;
            const type_info& type() const override;

            bool istrue() const override;
            int64_t length() const override;
            int64_t ivalue() const override;
            double dvalue() const override;
            string svalue() const override;
            const vector<VALUE>& avalue() const override;
            const map<string,VALUE>& ovalue() const override;

            VALUE op_not() const override;
            VALUE op_inv() const override;
            VALUE op_pos() const override;
            VALUE op_neg() const override;
            VALUE op_inc() const override;
            VALUE op_dec() const override;
            VALUE op_lt(const VALUE other) const override;
            VALUE op_gt(const VALUE other) const override;
            VALUE op_eq(const VALUE other) const override;
            VALUE op_ne(const VALUE other) const override;
            VALUE op_lte(const VALUE other) const override;
            VALUE op_gte(const VALUE other) const override;
            VALUE op_pow(const VALUE other) const override;
            VALUE op_mul(const VALUE other) const override;
            VALUE op_div(const VALUE other) const override;
            VALUE op_add(const VALUE other) const override;
            VALUE op_sub(const VALUE other) const override;
            VALUE op_mod(const VALUE other) const override;
            VALUE op_shl(const VALUE other) const override;
            VALUE op_asr(const VALUE other) const override;
            VALUE op_shr(const VALUE other) const override;
            VALUE op_and(const VALUE other) const override;
            VALUE op_xor(const VALUE other) const override;
            VALUE op_or(const VALUE other) const override;
    };
}


/* ***************************************************************************
* EVALUATOR
*/

namespace liteexpr {
    class Evaluator: public LiteExprBaseVisitor {
        SYMBOLS symbols;
        map<LiteExprParser::ExprContext*,VALUE> cache;

        public:
            Evaluator(SYMBOLS s);
            SYMBOLS getSymbols();

            any visitFile(LiteExprParser::FileContext *ctx) override;
            any visitString(LiteExprParser::StringContext *ctx) override;
            any visitDouble(LiteExprParser::DoubleContext *ctx) override;
            any visitHex(LiteExprParser::HexContext *ctx) override;
            any visitInt(LiteExprParser::IntContext *ctx) override;
            any visitArray(LiteExprParser::ArrayContext *ctx) override;
            any visitObject(LiteExprParser::ObjectContext *ctx) override;
            any visitPairlist(LiteExprParser::PairlistContext *ctx) override;
            any visitPair(LiteExprParser::PairContext *ctx) override;
            any visitList(LiteExprParser::ListContext *ctx) override;
            any visitCall(LiteExprParser::CallContext *ctx) override;
            any visitParen(LiteExprParser::ParenContext *ctx) override;
            any visitPrefixOp(LiteExprParser::PrefixOpContext *ctx) override;
            any visitPostfixOp(LiteExprParser::PostfixOpContext *ctx) override;
            any visitUnaryOp(LiteExprParser::UnaryOpContext *ctx) override;
            any visitBinaryOp(LiteExprParser::BinaryOpContext *ctx) override;
            any visitTernaryOp(LiteExprParser::TernaryOpContext *ctx) override;
            any visitAssignOp(LiteExprParser::AssignOpContext *ctx) override;
            any visitVariable(LiteExprParser::VariableContext *ctx) override;
            any visitMemberVar(LiteExprParser::MemberVarContext *ctx) override;
            any visitIndexedVar(LiteExprParser::IndexedVarContext *ctx) override;
            any visitSimpleVar(LiteExprParser::SimpleVarContext *ctx) override;
            any visitTerm(LiteExprParser::TermContext *ctx) override;
    };

    class Compiled {
        antlr4::ANTLRInputStream* input;
        antlr4::CommonTokenStream* tokens;
        antlr4::ParserRuleContext* parseTree;
        LiteExprLexer* lexer;
        LiteExprParser* parser;

        public:
            Compiled(string);
            ~Compiled();
            VALUE eval(SYMBOLS symbols);
    };

    Compiled compile(const string& expr);
    VALUE eval(const string& expr, SYMBOLS symbols);
}


/* ***************************************************************************
* EXCEPTIONS
*/

namespace liteexpr {
    class Error {
        string text;

        public:
            Error(const string& t);
            Error(const string& t, int line);
            Error(const string& t, int line, int col);
            operator string() const { return this->text; }
    };

    class SyntaxError: public Error {
        public:
            SyntaxError(const string& t, int line): Error(t, line) {};
            SyntaxError(const string& t, int line, int col): Error(t, line, col) {};
    };

    class RuntimeError: public Error {
        public:
            RuntimeError(const string& t): Error(t) {};
            RuntimeError(const string& t, int line): Error(t, line) {};
            RuntimeError(const string& t, int line, int col): Error(t, line, col) {};
    };

    class BasicSyntaxError: public Error {
        public:
            BasicSyntaxError(const string& t): Error(t) {};
    };

    class BasicRuntimeError: public Error {
        public:
            BasicRuntimeError(const string& t): Error(t) {};
    };
}


/* ***************************************************************************
* BUILTINS
*/

namespace liteexpr {
    extern map<string,VALUE> builtins;
}


/* ***************************************************************************
* HELPER FUNCTIONS
*/

namespace liteexpr {
    VALUE make_value(int);
    VALUE make_value(int64_t);
    VALUE make_value(double);
    VALUE make_value(const string &);
    VALUE make_value(initializer_list<VALUE>);
    VALUE make_value(initializer_list<pair<string,VALUE> >);
    INTEGER make_integer(int64_t);
    DOUBLE make_double(double);
    STRING make_string(const string&);
    ARRAY make_array(initializer_list<VALUE>);
    OBJECT make_object(initializer_list<pair<string,VALUE> >);
    SYMBOLS make_symbols(initializer_list<pair<string,VALUE> >);
}


#endif /* LITEEXPR_H_ */
