import math
import antlr4
from LiteExprLexer import LiteExprLexer
from LiteExprParser import LiteExprParser
from LiteExprListener import LiteExprListener


##############################################################################
# CONSTANTS

INTMASK = 0xffffffffffffffff


##############################################################################
# PUBLIC FUNCTIONS

def evalfd(fd, symbols=None):
    code = fd.read()

    return eval(code, symbols)


def eval(code, symbols=None):
    compiled = compile(str(code))

    return compiled.eval(symbols)


def compile(code):
    try:
        lexer = LiteExprLexer(antlr4.InputStream(code))
        parser = LiteExprParser(antlr4.CommonTokenStream(lexer))
        parser._errHandler = antlr4.error.ErrorStrategy.BailErrorStrategy()
        tree = parser.file_()
    except antlr4.error.Errors.ParseCancellationException as e:
        t = e.args[0].offendingToken

        raise LE_SyntaxError(f"Unexpected token `{t.text}`", t.line, t.column) from None

    return LE_Compiled(tree)


def compilefd(fd):
    code = fd.read()

    return compile(code)


##############################################################################
# EXCEPTIONS

class LE_Error(Exception):
    def __init__(self, text, line=None, column=None):
        if   column is None and line is None : super().__init__(f"{text}")
        elif column is None                  : super().__init__(f"[line {line}] {text}")
        else                                 : super().__init__(f"[line {line}, col {column}] {text}")

class LE_SyntaxError(LE_Error): pass
class LE_RuntimeError(LE_Error): pass


##############################################################################
# HELPER CLASSES

class LE_Compiled:
    def __init__(self, tree):
        self.tree = tree
        self.walker = antlr4.ParseTreeWalker()

    def eval(self, symbolTable=None):
        evaluator = LE_Evaluator(symbolTable)

        self.walker.walk(evaluator, self.tree)

        return evaluator.result[self.tree]


class LE_SymbolTable:
    def __init__(self, symbols=dict(), parent=None):
        self.symbols = builtins | symbols if parent is None else symbols
        self.parent = parent

    def __getitem__(self, name):
        name = str(name)

        if name in self.symbols : return self.symbols[name]
        else                    : return self.parent[name]

    def __setitem__(self, name, value):
        name = str(name)

        self.symbols[name] = value

        return self.symbols[name]

    def __str__(self):
        import json

        return json.dumps(self.symbols, indent=2, default=str)


##############################################################################
# TYPES

class LE_Variable:
    def __init__(self, name, container):
        self.name = name
        self.container = container

    @property
    def value(self):
        if   isinstance(self.container, LE_SymbolTable) : return self.container[self.name]
        elif isinstance(self.container, list)           : return self.container[self.name]
        elif isinstance(self.container, dict)           : return self.container[self.name]
        else                                            : return self.container

    @value.setter
    def value(self, value):
        if   isinstance(self.container, LE_SymbolTable) : self.container[self.name] = value
        elif isinstance(self.container, list)           : self.container[self.name] = value
        elif isinstance(self.container, dict)           : self.container[self.name] = value
        else                                            : self.container = value

        return value


class LE_Int(int):
    @property
    def value(self):
        return self


class LE_Double(float):
    @property
    def value(self):
        return self


class LE_String(str):
    @property
    def value(self):
        return self


class LE_Array(list):
    @property
    def value(self):
        return self


class LE_Object(dict):
    @property
    def value(self):
        return self


##############################################################################
# EVALUATOR

class LE_Evaluator(LiteExprListener):
    def __init__(self, symbolTable=None):
        super().__init__()
        self.result = dict()
        self.symbolTable = LE_SymbolTable() if symbolTable is None else symbolTable

    def exitFile(self, ctx):
        lastExpr = ctx.expr()[-1]

        self.result[ctx] = self.result[lastExpr].value

    def exitString(self, ctx):
        try:
            self.result[ctx] = LE_String(_decodeString(ctx.STRING().getText()[1:-1]))
        except LE_SyntaxError as e:
            raise LE_SyntaxError(str(e), ctx.start.line, ctx.start.column) from None

    def exitDouble(self, ctx):
        self.result[ctx] = LE_Double(ctx.DOUBLE().getText())

    def exitHex(self, ctx):
        self.result[ctx] = LE_Int(ctx.HEX().getText()[2:], 16)

    def exitInt(self, ctx):
        self.result[ctx] = LE_Int(ctx.INT().getText())

    def exitCall(self, ctx):
        fn = self.result[ctx.varname()].value
        args = self.result[ctx.list_()].value

        try:
            self.result[ctx] = fn(self.symbolTable, *args)
        except LE_SyntaxError as e:
            raise LE_SyntaxError(f"Syntax error while executing {ctx.getText()}: {str(e)}", ctx.start.line, ctx.start.column) from None
        except LE_RuntimeError as e:
            raise LE_RuntimeError(f"Runtime error while executing {ctx.getText()}: {str(e)}", ctx.start.line, ctx.start.column) from None

    def exitVariable(self, ctx):
        self.result[ctx] = self.result[ctx.varname()]

    def exitObject(self, ctx):
        self.result[ctx] = self.result[ctx.pairlist()]

    def exitArray(self, ctx):
        self.result[ctx] = self.result[ctx.list_()]

    def exitParen(self, ctx):
        self.result[ctx] = self.result[ctx.expr()]

    def exitPostfixOp(self, ctx):
        var = self.result[ctx.varname()]
        op = ctx.op.text
        T = type(var.value)

        if   op == "++" : self.result[ctx] = var.value; var.value = T(var.value + 1)
        elif op == "--" : self.result[ctx] = var.value; var.value = T(var.value - 1)
        else            : raise LE_SyntaxError("Unknown postfix operator `{op}`", ctx.start.line, ctx.start.column)

    def exitPrefixOp(self, ctx):
        var = self.result[ctx.varname()]
        op = ctx.op.text
        T = type(var.value)

        if   op == "++" : var = T(var.value + 1); self.result[ctx] = var.value
        elif op == "--" : var = T(var.value - 1); self.result[ctx] = var.value
        else            : raise LE_SyntaxError("Unknown prefix operator `{op}`", ctx.start.line, ctx.start.column)

    def exitUnaryOp(self, ctx):
        value = self.result[ctx.expr()].value
        op = ctx.op.text
        T = type(value)

        if   op == "!"  : self.result[ctx] = LE_Int(not value)
        elif op == "~"  : self.result[ctx] = LE_Int(~value)
        elif op == "+"  : self.result[ctx] = value
        elif op == "-"  : self.result[ctx] = T(-value)
        else            : raise LE_SyntaxError("Unknown unary operator `{op}`", ctx.start.line, ctx.start.column)

    def exitBinaryOp(self, ctx):
        op = ctx.op.text
        value = (
            self.result[ctx.expr(0)].value,
            self.result[ctx.expr(1)].value,
        )
        T = LE_Int if isinstance(value[0],int) and isinstance(value[1],int) else LE_Double

        if   op == "**"  : self.result[ctx] = (LE_Double if value[1] < 0 else T)(value[0] **  value[1])
        elif op == "*"   : self.result[ctx] = T(value[0] *   value[1])
        elif op == "/"   : self.result[ctx] = T(value[0] /   value[1])
        elif op == "%"   : self.result[ctx] = LE_Int(value[0] %   value[1])
        elif op == "+"   : self.result[ctx] = LE_String(str(value[0]) + str(value[1])) if isinstance(value[0],str) or isinstance(value[1],str) else T(value[0] + value[1])
        elif op == "-"   : self.result[ctx] = T(value[0] -   value[1])
        elif op == "<<"  : self.result[ctx] = LE_Int(value[0] <<  value[1])
        elif op == ">>"  : self.result[ctx] = LE_Int(value[0] >>  value[1])
        elif op == ">>>" : self.result[ctx] = LE_Int((value[0] & INTMASK) >> value[1])
        elif op == "<"   : self.result[ctx] = LE_Int(value[0] <   value[1])
        elif op == "<="  : self.result[ctx] = LE_Int(value[0] <=  value[1])
        elif op == ">"   : self.result[ctx] = LE_Int(value[0] >   value[1])
        elif op == ">="  : self.result[ctx] = LE_Int(value[0] >=  value[1])
        elif op == "=="  : self.result[ctx] = LE_Int(value[0] ==  value[1])
        elif op == "!="  : self.result[ctx] = LE_Int(value[0] !=  value[1])
        elif op == "&"   : self.result[ctx] = LE_Int(value[0] &   value[1])
        elif op == "^"   : self.result[ctx] = LE_Int(value[0] ^   value[1])
        elif op == "|"   : self.result[ctx] = LE_Int(value[0] |   value[1])
        elif op == "&&"  : self.result[ctx] = LE_Int(value[0] and value[1])
        elif op == "||"  : self.result[ctx] = LE_Int(value[0] or  value[1])
        else             : raise LE_SyntaxError("Unknown binary operator `{op}`", ctx.start.line, ctx.start.column)

    def exitAssignOp(self, ctx):
        op = ctx.op.text
        var = self.result[ctx.varname()]
        value2 = self.result[ctx.expr()].value

        if op == "=" : self.result[ctx] = value2
        else         :
            T = LE_Int if isinstance(var.value,int) and isinstance(var.value,int) else LE_Double

            if   op == "**="  : self.result[ctx] = (LE_Double if value2 < 0 else T)(var.value **  value2)
            elif op == "*="   : self.result[ctx] = T(var.value *   value2)
            elif op == "/="   : self.result[ctx] = T(var.value /   value2)
            elif op == "%="   : self.result[ctx] = LE_Int(var.value %   value2)
            elif op == "+="   : self.result[ctx] = LE_String(str(var.value) + str(value2)) if isinstance(var.value,str) or isinstance(value2,str) else T(var.value + value2)
            elif op == "-="   : self.result[ctx] = T(var.value -   value2)
            elif op == "<<="  : self.result[ctx] = LE_Int(var.value <<  value2)
            elif op == ">>="  : self.result[ctx] = LE_Int(var.value >>  value2)
            elif op == ">>>=" : self.result[ctx] = LE_Int((var.value & INTMASK) >> value2)
            elif op == "&="   : self.result[ctx] = LE_Int(var.value &   value2)
            elif op == "^="   : self.result[ctx] = LE_Int(var.value ^   value2)
            elif op == "|="   : self.result[ctx] = LE_Int(var.value |   value2)
            elif op == "&&="  : self.result[ctx] = LE_Int(var.value and value2)
            elif op == "||="  : self.result[ctx] = LE_Int(var.value or  value2)
            else              : raise LE_SyntaxError("Unknown assign operator `{op}`", ctx.start.line, ctx.start.column)

        var.value = self.result[ctx]

    def exitTertiaryOp(self, ctx):
        op = (
            ctx.op1.text,
            ctx.op2.text,
        )
        value = (
            self.result[ctx.expr(0)].value,
            self.result[ctx.expr(1)].value,
            self.result[ctx.expr(2)].value,
        )

        if op[0] == "?" and op[1] == ":" : self.result[ctx] = LiteExprDatum(value[1] if value[0] else value[2])
        else                             : raise LE_SyntaxError("Unknown tertiary operator `{op[0]} {op[1]}`", ctx.start.line, ctx.start.column)

    def exitIndexedVar(self, ctx):
        array = self.result[ctx.varname()].value
        index = self.result[ctx.expr()].value

        self.result[ctx] = LE_Variable(index, array)

    def exitMemberVar(self, ctx):
        base = self.result[ctx.varname(0)].value
        member = self.result[ctx.varname(1)].name

        self.result[ctx] = LE_Variable(member, base)

    def exitSimpleVar(self, ctx):
        varname = ctx.ID().getText()

        self.result[ctx] = LE_Variable(varname, self.symbolTable)

    def exitPairlist(self, ctx):
        self.result[ctx] = LE_Object()

        for pair in ctx.pair():
            for k,v in self.result[pair].items():
                self.result[ctx][k] = v

    def exitPair(self, ctx):
        name = ctx.ID().getText()
        value = self.result[ctx.expr()]

        self.result[ctx] = { LE_String(name) : value }

    def exitList(self, ctx):
        self.result[ctx] = LE_Array()

        for item in ctx.expr():
            self.result[ctx] += [self.result[item].value]


##############################################################################
# HELPER FUNCTIONS

def _decodeString(s):
    decoded = ""
    i = 0

    while i < len(s):
        if   s[i:i+2] == "\\\\"   : decoded += "\\"; i+=2
        elif s[i:i+2] == "\\\""   : decoded += "\""; i+=2
        elif s[i:i+3] == "\\\r\n" : i+=2
        elif s[i:i+2] == "\\\r"   : i+=2
        elif s[i:i+2] == "\\\n"   : i+=2
        elif s[i:i+2] == "\\t"    : decoded += "\t"; i+=2
        elif s[i:i+2] == "\\r"    : decoded += "\r"; i+=2
        elif s[i:i+2] == "\\n"    : decoded += "\n"; i+=2
        elif s[i:i+2] == "\\x"    : decoded += chr(int(s[i+2:i+4], 16)); i+=4
        elif s[i:i+2] == "\\u"    : decoded += chr(int(s[i+2:i+6], 16)); i+=6
        elif s[i:i+2] == "\\U"    : decoded += chr(int(s[i+2:i+10], 16)); i+=10
        elif s[i:i+1] == "\\"     : raise LE_SyntaxError(f"Invalid backslash sequence in string at position {i}")
        else                      : decoded += s[i]; i+=1

    return decoded


def _encodeString(s):
    encoded = ""

    for c in s:
        if   c == "\\" : encoded += "\\\\"
        elif c == "\"" : encoded += "\\\""
        else           : encoded += c

    return encoded


##############################################################################
# BUILTIN FUNCTIONS

builtins = {
    "CEIL"  : lambda s,  x : LE_Int(math.ceil(x)),
    "EVAL"  : lambda s,  x : eval(x, s),
    "FLOOR" : lambda s,  x : LE_Int(math.floor(x)),
    "FOR"   : lambda s,  a, b, c, d : __builtin_for(s, a, b, c, d),
    "IF"    : lambda s, *x : __builtin_if(s, *x),
    "LEN"   : lambda s,  x : LE_Int(len(x)),
    "PRINT" : lambda s, *x : __builtin_print(s, *x),
    "ROUND" : lambda s, *x : LE_Int(round(*x)),
    "SQRT"  : lambda s,  x : LE_Double(math.sqrt(x)),
    "WHILE" : lambda s,  a, b: __builtin_while(s, a, b),
}

def __builtin_for(s, a, b, c, d):
    eval(a, s)

    while(eval(b, s)):
        eval(d, s)
        eval(c, s)

    return LE_Int(0)

def __builtin_if(s, *x):
    v = x[-1]
    i = 0

    while i+1 < len(x):
        if eval(x[i], s): v = x[i+1]
        i += 2

    return eval(v, s)

def __builtin_print(s, *x):
    o = []

    for y in x:
        o += [y.value]

    print(*o)

    return LE_Int(0)

def __builtin_while(s, a, b):
    while(eval(a, s)):
        eval(b, s)

    return LE_Int(0)

