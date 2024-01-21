__copyright__ = "Copyright 2023-2024 Mark Kim"
__license__ = "Apache 2.0"
__version__ = "0.0.6"
__author__ = "Mark Kim"

import math
import antlr4
from .LiteExprLexer import LiteExprLexer
from .LiteExprParser import LiteExprParser
from .LiteExprVisitor import LiteExprVisitor


##############################################################################
# CONSTANTS

INTMASK = 0xffffffffffffffff
INTMAX  = (INTMASK >> 1)


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

        raise SyntaxError(f"Unexpected token `{t.text}`", t.line, t.column) from None

    return Compiled(tree)


def compilefd(fd):
    code = fd.read()

    return compile(code)


##############################################################################
# EXCEPTIONS

class Error(Exception):
    def __init__(self, text, line=None, column=None):
        if   column is None and line is None : super().__init__(f"{text}")
        elif column is None                  : super().__init__(f"[line {line}] {text}")
        else                                 : super().__init__(f"[line {line}, col {column}] {text}")

class SyntaxError(Error): pass
class RuntimeError(Error): pass


##############################################################################
# HELPER CLASSES

class Compiled:
    def __init__(self, tree):
        self.tree = tree

    def eval(self, symbolTable=None):
        evaluator = Evaluator(symbolTable)
        evaluator.visit(self.tree)

        return evaluator.result[self.tree]


class SymbolTable:
    def __init__(self, symbols=dict(), parent=None):
        self.symbols = dict()
        self.parent = parent
        self.root = self if parent is None else parent.root

        if self.parent is None:
            self.symbols |= builtins

        for k,v in symbols.items():
            self.__setitem__(k,v)

    def __getitem__(self, name):
        name = str(name)

        if   name in self.symbols : return self.symbols[name]
        elif self.parent is None  : raise RuntimeError(f"{name} is not a valid symbol")
        else                      : return self.parent[name]

    def __setitem__(self, name, value):
        name = str(name)

        self.symbols[name] = _to_levalue(value)

        return self.symbols[name]

    def __all(self):
        symbols = dict()

        if self.parent:
            symbols["__parent__"] = self.parent.__all()

        for k,v in self.symbols.items():
            if   k in ("GLOBAL", "UPSCOPE") : symbols[k] = f"<{k}>"
            elif isinstance(v,SymbolTable)  : symbols[k] = v.__all()
            else                            : symbols[k] = v

        return symbols

    def __str__(self):
        import json

        return json.dumps(self.__all(), indent=2, default=str)

    @property
    def value(self):
        return self


##############################################################################
# TYPES

class Variable:
    def __init__(self, name, container):
        self.name = name
        self.container = container

    @property
    def value(self):
        try:
            if   isinstance(self.container, SymbolTable) : return self.container[self.name]
            elif isinstance(self.container, list)        : return self.container[self.name]
            elif isinstance(self.container, dict)        : return self.container[self.name]
            else                                         : return self.container
        except IndexError as e:
            raise RuntimeError(str(e)) from None

    @value.setter
    def value(self, value):
        if   isinstance(self.container, SymbolTable) : self.container[self.name] = value
        elif isinstance(self.container, list)        : self.container[self.name] = value
        elif isinstance(self.container, dict)        : self.container[self.name] = value
        else                                         : self.container = value

        return value


class Int(int):
    def __new__(cls, value, *args, **kwargs):
        #
        # Integer has no limit in Python, but liteexpr's integer is a signed
        # 64-bit.  To simulate a signed 64-bit in Python, we reduce the
        # precision every time liteexpr integer type is instantiated.  And we
        # have to do this in __new__ instead of __init__ because the int
        # built-in type we're subclassing is immutable.
        #

        value = int(value, *args, **kwargs) & INTMASK

        if value > INTMAX:
            value = -((value ^ INTMASK) + 1)

        return super(Int, cls).__new__(cls, value)

    @property
    def value(self):
        return self


class Double(float):
    def __new__(cls, value, *args, **kwargs):
        #
        # liteexpr doesn't support complex numbers.  A complex result can occur
        # by the ** operator, for example `-1 ** 0.5` (for some strange reason,
        # Python resolves -1 ** 0.5 to a complex number only if -1 and 0.5 are
        # stored in variables, not literals.)
        #
        if isinstance(value,complex) : value = "NaN"

        return super(Double, cls).__new__(cls, value, *args, **kwargs)

    @property
    def value(self):
        return self

    def __truediv__(self, other):
        if   other == 0 and self == 0                  : return Double("NaN")
        if   other == 0 and math.isnan(self)           : return Double("NaN")
        elif other == 0 and math.copysign(1, self) > 0 : return Double("Inf")
        elif other == 0 and math.copysign(1, self) < 1 : return Double("-Inf")
        else                                           : return super().__truediv__(other)

    def __str__(self):
        if   math.isnan(self)              : return "NaN"
        elif math.isinf(self) and self > 0 : return "Inf"
        elif math.isinf(self) and self < 0 : return "-Inf"

        return super().__str__()


class String(str):
    @property
    def value(self):
        return self


class Array(list):
    @property
    def value(self):
        return self

    def __setitem__(self, index, value):
        value = _to_levalue(value)

        if   index < len(self)  : super().__setitem__(index, value)
        elif index == len(self) : super().append(value)
        else                    : raise RuntimeError(f"Index `{index}` is out of array range (<{len(self)})")

        return value


class Object(dict):
    @property
    def value(self):
        return self

    def __setitem__(self, key, value):
        value = _to_levalue(value)

        super().__setitem__(key, value)

        return value


def _to_levalue(value):
    if   isinstance(value,SymbolTable) : value = value
    elif isinstance(value,int)         : value = Int(value)
    elif isinstance(value,float)       : value = Double(value)
    elif isinstance(value,str)         : value = String(value)
    elif isinstance(value,list)        : value = Array([_to_levalue(v) for v in value])
    elif isinstance(value,dict)        : value = Object({str(k):_to_levalue(v) for k,v in value.items()})
    elif callable(value)               : value = Function(value)
    else                               : raise SyntaxError("Unsupported data type `{type(value)}`")

    return value


##############################################################################
# EVALUATOR

class Evaluator(LiteExprVisitor):
    def __init__(self, symbolTable=None):
        super().__init__()
        self.result = dict()
        self.symbolTable = SymbolTable() if symbolTable is None else symbolTable

    def visitFile(self, ctx):
        self.visitChildren(ctx)

        if ctx.expr():
            self.result[ctx] = self.result[ctx.expr()].value
        else:
            self.result[ctx] = Int(0)

        return self.result[ctx]

    def visitString(self, ctx):
        if ctx not in self.result:
            try:
                self.result[ctx] = String(_decodeString(ctx.STRING().getText()[1:-1]))
            except SyntaxError as e:
                raise SyntaxError(str(e), ctx.start.line, ctx.start.column) from None

        return self.result[ctx]

    def visitDouble(self, ctx):
        if ctx not in self.result:
            self.result[ctx] = Double(ctx.DOUBLE().getText())

        return self.result[ctx]

    def visitHex(self, ctx):
        if ctx not in self.result:
            self.result[ctx] = Int(ctx.HEX().getText()[2:], 16)

        return self.result[ctx]

    def visitInt(self, ctx):
        if ctx not in self.result:
            self.result[ctx] = Int(ctx.INT().getText())

        return self.result[ctx]

    def visitCall(self, ctx):
        self.visit(ctx.varname())
        fn = self.result[ctx.varname()].value

        if hasattr(fn, "opts") and fn.opts["delayvisit"]:
            args = ctx.list_().expr()
        else:
            self.visit(ctx.list_())
            args = self.result[ctx.list_()].value

        try:
            self.result[ctx] = fn(*args, visitor=self, sym=self.symbolTable)
        except SyntaxError as e:
            raise SyntaxError(f"Syntax error while executing `{ctx.getText()}`:\n\t{str(e)}", ctx.start.line, ctx.start.column) from None
        except RuntimeError as e:
            raise RuntimeError(f"Runtime error while executing `{ctx.getText()}`:\n\t{str(e)}", ctx.start.line, ctx.start.column) from None

        return self.result[ctx]

    def visitVariable(self, ctx):
        self.visitChildren(ctx)

        self.result[ctx] = self.result[ctx.varname()]

        return self.result[ctx]

    def visitObject(self, ctx):
        self.visitChildren(ctx)

        self.result[ctx] = self.result[ctx.pairlist()]

        return self.result[ctx]

    def visitArray(self, ctx):
        self.visitChildren(ctx)

        self.result[ctx] = self.result[ctx.list_()]

        return self.result[ctx]

    def visitParen(self, ctx):
        self.visitChildren(ctx)

        self.result[ctx] = self.result[ctx.expr()]

        return self.result[ctx]

    def visitPostfixOp(self, ctx):
        self.visitChildren(ctx)

        var = self.result[ctx.varname()]
        op = ctx.op.text

        if   op == "++" : self.result[ctx] = var.value; var.value = _op_inc(var.value)
        elif op == "--" : self.result[ctx] = var.value; var.value = _op_dec(var.value)
        else            : raise SyntaxError("Unknown postfix operator `{op}`", ctx.start.line, ctx.start.column)

        return self.result[ctx]

    def visitPrefixOp(self, ctx):
        self.visitChildren(ctx)

        var = self.result[ctx.varname()]
        op = ctx.op.text

        if   op == "++" : var = _op_inc(var.value); self.result[ctx] = var.value
        elif op == "--" : var = _op_dec(var.value); self.result[ctx] = var.value
        else            : raise SyntaxError("Unknown prefix operator `{op}`", ctx.start.line, ctx.start.column)

        return self.result[ctx]

    def visitUnaryOp(self, ctx):
        self.visitChildren(ctx)

        value = self.result[ctx.expr()].value
        op = ctx.op.text
        T = type(value)

        if   op == "!"  : self.result[ctx] = _op_not(value)
        elif op == "~"  : self.result[ctx] = _op_inv(value)
        elif op == "+"  : self.result[ctx] = _op_pos(value)
        elif op == "-"  : self.result[ctx] = _op_neg(value)
        else            : raise SyntaxError("Unknown unary operator `{op}`", ctx.start.line, ctx.start.column)

        return self.result[ctx]

    def visitBinaryOp(self, ctx):
        op = ctx.op.text
        left = self.visit(ctx.expr(0)).value
        rexpr = ctx.expr(1)

        try:
            if   op == "**"  : self.result[ctx] = _op_pow(left, self.visit(rexpr).value)
            elif op == "*"   : self.result[ctx] = _op_mul(left, self.visit(rexpr).value)
            elif op == "/"   : self.result[ctx] = _op_div(left, self.visit(rexpr).value)
            elif op == "+"   : self.result[ctx] = _op_add(left, self.visit(rexpr).value)
            elif op == "-"   : self.result[ctx] = _op_sub(left, self.visit(rexpr).value)
            elif op == "%"   : self.result[ctx] = _op_mod(left, self.visit(rexpr).value)
            elif op == "<<"  : self.result[ctx] = _op_shl(left, self.visit(rexpr).value)
            elif op == ">>"  : self.result[ctx] = _op_shr(left, self.visit(rexpr).value)
            elif op == ">>>" : self.result[ctx] = _op_asr(left, self.visit(rexpr).value)
            elif op == "<"   : self.result[ctx] = _op_lt (left, self.visit(rexpr).value)
            elif op == "<="  : self.result[ctx] = _op_lte(left, self.visit(rexpr).value)
            elif op == ">"   : self.result[ctx] = _op_gt (left, self.visit(rexpr).value)
            elif op == ">="  : self.result[ctx] = _op_gte(left, self.visit(rexpr).value)
            elif op == "=="  : self.result[ctx] = _op_eq (left, self.visit(rexpr).value)
            elif op == "!="  : self.result[ctx] = _op_ne (left, self.visit(rexpr).value)
            elif op == "&"   : self.result[ctx] = _op_and(left, self.visit(rexpr).value)
            elif op == "^"   : self.result[ctx] = _op_xor(left, self.visit(rexpr).value)
            elif op == "|"   : self.result[ctx] = _op_or (left, self.visit(rexpr).value)
            elif op == "||"  : self.result[ctx] = _op_or_logical(left, rexpr, visitor=self)
            elif op == "&&"  : self.result[ctx] = _op_and_logical(left, rexpr, visitor=self)
            elif op == ";"   : self.result[ctx] = self.visit(rexpr).value
            else             : raise SyntaxError("Unknown binary operator `{op}`", ctx.op.line, ctx.op.column)
        except RuntimeError as e:
            raise RuntimeError(str(e), ctx.op.line, ctx.op.column) from None

        return self.result[ctx]

    def visitAssignOp(self, ctx):
        op = ctx.op.text
        var = self.visit(ctx.varname())
        rexpr = ctx.expr()

        try:
            if   op == "="    : self.result[ctx] = self.visit(rexpr).value
            elif op == "**="  : self.result[ctx] = _op_pow(var.value, self.visit(rexpr).value)
            elif op == "*="   : self.result[ctx] = _op_mul(var.value, self.visit(rexpr).value)
            elif op == "/="   : self.result[ctx] = _op_div(var.value, self.visit(rexpr).value)
            elif op == "+="   : self.result[ctx] = _op_add(var.value, self.visit(rexpr).value)
            elif op == "-="   : self.result[ctx] = _op_sub(var.value, self.visit(rexpr).value)
            elif op == "%="   : self.result[ctx] = _op_mod(var.value, self.visit(rexpr).value)
            elif op == "<<="  : self.result[ctx] = _op_shl(var.value, self.visit(rexpr).value)
            elif op == ">>="  : self.result[ctx] = _op_shr(var.value, self.visit(rexpr).value)
            elif op == ">>>=" : self.result[ctx] = _op_asr(var.value, self.visit(rexpr).value)
            elif op == "&="   : self.result[ctx] = _op_and(var.value, self.visit(rexpr).value)
            elif op == "^="   : self.result[ctx] = _op_xor(var.value, self.visit(rexpr).value)
            elif op == "|="   : self.result[ctx] = _op_or (var.value, self.visit(rexpr).value)
            elif op == "||="  : self.result[ctx] = _op_or_logical(var.value, rexpr, visitor=self)
            elif op == "&&="  : self.result[ctx] = _op_and_logical(var.value, rexpr, visitor=self)
            else              : raise SyntaxError("Unknown assign operator `{op}`", ctx.op.line, ctx.op.column)
        except RuntimeError as e:
            raise RuntimeError(str(e), ctx.op.line, ctx.op.column) from None

        var.value = self.result[ctx]

        return self.result[ctx]

    def visitTernaryOp(self, ctx):
        op = (
            ctx.op1.text,
            ctx.op2.text,
        )

        if op[0] == "?" and op[1] == ":" : self.result[ctx] = self.visit(ctx.expr(1)).value if self.visit(ctx.expr(0)).value else self.visit(ctx.expr(2)).value
        else                             : raise SyntaxError("Unknown tertiary operator `{op[0]} {op[1]}`", ctx.start.line, ctx.start.column)

        return self.result[ctx]

    def visitIndexedVar(self, ctx):
        self.visitChildren(ctx)

        array = self.result[ctx.varname()].value
        index = self.result[ctx.expr()].value

        self.result[ctx] = Variable(index, array)

        return self.result[ctx]

    def visitMemberVar(self, ctx):
        self.visitChildren(ctx)

        base = self.result[ctx.varname(0)].value
        member = self.result[ctx.varname(1)].name

        self.result[ctx] = Variable(member, base)

        return self.result[ctx]

    def visitTerm(self, ctx):
        self.visitChildren(ctx)

        self.result[ctx] = self.result[ctx.expr()]

        return self.result[ctx]

    def visitNoop(self, ctx):
        self.result[ctx] = Int(0)

        return self.result[ctx]

    def visitSimpleVar(self, ctx):
        self.visitChildren(ctx)

        varname = ctx.ID().getText()

        self.result[ctx] = Variable(varname, self.symbolTable)

        return self.result[ctx]

    def visitPairlist(self, ctx):
        self.visitChildren(ctx)

        self.result[ctx] = Object()

        for pair in ctx.pair():
            for k,v in self.result[pair].items():
                self.result[ctx][k] = v

        return self.result[ctx]

    def visitPair(self, ctx):
        self.visitChildren(ctx)

        name = ctx.ID().getText()
        value = self.result[ctx.expr()]

        self.result[ctx] = { String(name) : value }

        return self.result[ctx]

    def visitList(self, ctx):
        self.visitChildren(ctx)

        self.result[ctx] = Array()

        for item in ctx.expr():
            self.result[ctx] += [self.result[item].value]

        return self.result[ctx]


def _op_inc(value):
    if   isinstance(value,int) : return Int(value+1)

    raise RuntimeError(f"Unsupported operand type for `++`: ({type(value).__name__})")


def _op_dec(value):
    if   isinstance(value,int) : return Int(value-1)

    raise RuntimeError(f"Unsupported operand type for `--`: ({type(value).__name__})")


def _op_not(value):
    if   isinstance(value,int)   : return Int(value == 0)
    elif isinstance(value,float) : return Int(value == 0.0)
    elif isinstance(value,str)   : return Int(value == "")
    elif isinstance(value,list)  : return Int(len(value) == 0)
    elif isinstance(value,dict)  : return Int(len(value) == 0)

    raise RuntimeError(f"Unsupported operand type for `!`: ({type(value).__name__})")


def _op_inv(value):
    if   isinstance(value,int)   : return Int(~value)

    raise RuntimeError(f"Unsupported operand type for `~`: ({type(value).__name__})")


def _op_pos(value):
    if   isinstance(value,int)   : return Int(value)
    elif isinstance(value,float) : return Double(value)

    raise RuntimeError(f"Unsupported operand type for `+`: ({type(value).__name__})")


def _op_neg(value):
    if   isinstance(value,int)   : return Int(-value)
    elif isinstance(value,float) : return Double(-value)

    raise RuntimeError(f"Unsupported operand type for `-`: ({type(value).__name__})")


def _op_pow(left, right):
    try:
        if   isinstance(left,int)   and isinstance(right,int)   : return (Double if right < 0 else Int)(left ** right)
        elif isinstance(left,int)   and isinstance(right,float) : return Double(left ** right)
        elif isinstance(left,float) and isinstance(right,int)   : return Double(left ** right)
        elif isinstance(left,float) and isinstance(right,float) : return Double(left ** right)
    except ZeroDivisionError as e:
        raise RuntimeError(f"Negative power of zero: ({left} ** {right})") from None

    raise RuntimeError(f"Unsupported operand type(s) for `**`: ({type(left).__name__},{type(right).__name__})")


def _op_mul(left, right):
    if   isinstance(left,int)   and isinstance(right,int)   : return Int(left * right)
    elif isinstance(left,int)   and isinstance(right,float) : return Double(left * right)
    elif isinstance(left,float) and isinstance(right,int)   : return Double(left * right)
    elif isinstance(left,float) and isinstance(right,float) : return Double(left * right)

    raise RuntimeError(f"Unsupported operand type(s) for `*`: ({type(left).__name__},{type(right).__name__})")


def _op_div(left, right):
    try:
        if   isinstance(left,int)   and isinstance(right,int)   : return Int(left // right)
        elif isinstance(left,int)   and isinstance(right,float) : return Double(Double(left) / Double(right))
        elif isinstance(left,float) and isinstance(right,int)   : return Double(Double(left) / Double(right))
        elif isinstance(left,float) and isinstance(right,float) : return Double(Double(left) / Double(right))
    except ZeroDivisionError as e:
        raise RuntimeError(f"Division by zero: ({left} / {right})") from None

    raise RuntimeError(f"Unsupported operand type(s) for `/`: ({type(left).__name__},{type(right).__name__})")


def _op_add(left, right):
    if   isinstance(left,int)   and isinstance(right,int)   : return Int(left + right)
    elif isinstance(left,int)   and isinstance(right,float) : return Double(left + right)
    elif isinstance(left,float) and isinstance(right,int)   : return Double(left + right)
    elif isinstance(left,float) and isinstance(right,float) : return Double(left + right)
    elif isinstance(left,list)  and isinstance(right,list)  : return Array(left + right)
    elif isinstance(left,str)   or  isinstance(right,str)   : return String(str(left) + str(right))

    raise RuntimeError(f"Unsupported operand type(s) for `+`: ({type(left).__name__},{type(right).__name__})")


def _op_sub(left, right):
    if   isinstance(left,int)   and isinstance(right,int)   : return Int(left - right)
    elif isinstance(left,int)   and isinstance(right,float) : return Double(left - right)
    elif isinstance(left,float) and isinstance(right,int)   : return Double(left - right)
    elif isinstance(left,float) and isinstance(right,float) : return Double(left - right)

    raise RuntimeError(f"Unsupported operand type(s) for `-`: ({type(left).__name__},{type(right).__name__})")


def _op_mod(left, right):
    try:
        if   isinstance(left,int)   and isinstance(right,int)   : return Int(left % right)
    except ZeroDivisionError as e:
        raise RuntimeError(f"Modulus by zero: ({left} % {right})") from None

    raise RuntimeError(f"Unsupported operand type(s) for `%`: ({type(left).__name__},{type(right).__name__})")


def _op_shl(left, right):
    if isinstance(left,int) and isinstance(right,int):
        if right >= 0 : return Int(left << right)
        else          : raise RuntimeError(f"Invalid attempt to shift `<<` by a negative amount: {right}")

    raise RuntimeError(f"Unsupported operand type(s) for `<<`: ({type(left).__name__},{type(right).__name__})")


def _op_shr(left, right):
    if isinstance(left,int) and isinstance(right,int):
        if right >= 0 : return Int(left >> right)
        else          : raise RuntimeError(f"Invalid attempt to shift `>>` by a negative amount: {right}")

    raise RuntimeError(f"Unsupported operand type(s) for `>>`: ({type(left).__name__},{type(right).__name__})")


def _op_asr(left, right):
    if isinstance(left,int) and isinstance(right,int):
        if right >= 0 : return Int((left & INTMASK) >> right)
        else          : raise RuntimeError(f"Invalid attempt to shift `>>>` by a negative amount: {right}")

    raise RuntimeError(f"Unsupported operand type(s) for `>>>`: ({type(left).__name__},{type(right).__name__})")


def _op_lt(left, right):
    if   isinstance(left,int)   and isinstance(right,int)   : return Int(left < right)
    elif isinstance(left,int)   and isinstance(right,float) : return Int(left < right)
    elif isinstance(left,float) and isinstance(right,int)   : return Int(left < right)
    elif isinstance(left,float) and isinstance(right,float) : return Int(left < right)
    elif isinstance(left,str)   and isinstance(right,str)   : return Int(left < right)

    raise RuntimeError(f"Unsupported operand type(s) for `<`: ({type(left).__name__},{type(right).__name__})")


def _op_lte(left, right):
    if   isinstance(left,int)   and isinstance(right,int)   : return Int(left <= right)
    elif isinstance(left,int)   and isinstance(right,float) : return Int(left <= right)
    elif isinstance(left,float) and isinstance(right,int)   : return Int(left <= right)
    elif isinstance(left,float) and isinstance(right,float) : return Int(left <= right)
    elif isinstance(left,str)   and isinstance(right,str)   : return Int(left <= right)

    raise RuntimeError(f"Unsupported operand type(s) for `<=`: ({type(left).__name__},{type(right).__name__})")


def _op_gt(left, right):
    if   isinstance(left,int)   and isinstance(right,int)   : return Int(left > right)
    elif isinstance(left,int)   and isinstance(right,float) : return Int(left > right)
    elif isinstance(left,float) and isinstance(right,int)   : return Int(left > right)
    elif isinstance(left,float) and isinstance(right,float) : return Int(left > right)
    elif isinstance(left,str)   and isinstance(right,str)   : return Int(left > right)

    raise RuntimeError(f"Unsupported operand type(s) for `>`: ({type(left).__name__},{type(right).__name__})")


def _op_gte(left, right):
    if   isinstance(left,int)   and isinstance(right,int)   : return Int(left >= right)
    elif isinstance(left,int)   and isinstance(right,float) : return Int(left >= right)
    elif isinstance(left,float) and isinstance(right,int)   : return Int(left >= right)
    elif isinstance(left,float) and isinstance(right,float) : return Int(left >= right)
    elif isinstance(left,str)   and isinstance(right,str)   : return Int(left >= right)

    raise RuntimeError(f"Unsupported operand type(s) for `>=`: ({type(left).__name__},{type(right).__name__})")


def _op_eq(left, right):
    if   isinstance(left,int)   and isinstance(right,int)   : return Int(left == right)
    elif isinstance(left,int)   and isinstance(right,float) : return Int(left == right)
    elif isinstance(left,float) and isinstance(right,int)   : return Int(left == right)
    elif isinstance(left,float) and isinstance(right,float) : return Int(left == right)
    elif isinstance(left,str)   and isinstance(right,str)   : return Int(left == right)
    elif isinstance(left,list)  and isinstance(right,list)  : return Int(left == right)
    elif isinstance(left,dict)  and isinstance(right,dict)  : return Int(left == right)
    else                                                    : return Int(False)


def _op_ne(left, right):
    if   isinstance(left,int)   and isinstance(right,int)   : return Int(left != right)
    elif isinstance(left,int)   and isinstance(right,float) : return Int(left != right)
    elif isinstance(left,float) and isinstance(right,int)   : return Int(left != right)
    elif isinstance(left,float) and isinstance(right,float) : return Int(left != right)
    elif isinstance(left,str)   and isinstance(right,str)   : return Int(left != right)
    elif isinstance(left,list)  and isinstance(right,list)  : return Int(left != right)
    elif isinstance(left,dict)  and isinstance(right,dict)  : return Int(left != right)
    else                                                    : return Int(True)


def _op_and(left, right):
    if   isinstance(left,int)   and isinstance(right,int)   : return Int(left & right)

    raise RuntimeError(f"Unsupported operand type(s) for `^`: ({type(left).__name__},{type(right).__name__})")


def _op_xor(left, right):
    if   isinstance(left,int)   and isinstance(right,int)   : return Int(left ^ right)

    raise RuntimeError(f"Unsupported operand type(s) for `^`: ({type(left).__name__},{type(right).__name__})")


def _op_or(left, right):
    if   isinstance(left,int)   and isinstance(right,int)   : return Int(left | right)

    raise RuntimeError(f"Unsupported operand type(s) for `|`: ({type(left).__name__},{type(right).__name__})")


def _op_or_logical(left, rexpr, **kwargs):
    result = None

    if   isinstance(left,int)   : result = Int(left != 0)
    elif isinstance(left,float) : result = Int(left != 0.0)
    elif isinstance(left,str)   : result = Int(left != "")
    elif isinstance(left,list)  : result = Int(len(left) != 0)
    elif isinstance(left,dict)  : result = Int(len(left) != 0)
    else : raise RuntimeError(f"Unsupported operand type(s) for `||`: ({type(left).__name__},{type(right).__name__})")

    if not result:
        right = kwargs["visitor"].visit(rexpr).value

        if   isinstance(right,int)   : result = Int(right != 0)
        elif isinstance(right,float) : result = Int(right != 0.0)
        elif isinstance(right,str)   : result = Int(right != "")
        elif isinstance(right,list)  : result = Int(len(right) != 0)
        elif isinstance(right,dict)  : result = Int(len(right) != 0)
        else : raise RuntimeError(f"Unsupported operand type(s) for `||`: ({type(left).__name__},{type(right).__name__})")

    return result


def _op_and_logical(left, rexpr, **kwargs):
    result = None

    if   isinstance(left,int)   : result = Int(left != 0)
    elif isinstance(left,float) : result = Int(left != 0.0)
    elif isinstance(left,str)   : result = Int(left != "")
    elif isinstance(left,list)  : result = Int(len(left) != 0)
    elif isinstance(left,dict)  : result = Int(len(left) != 0)
    else : raise RuntimeError(f"Unsupported operand type(s) for `&&`: ({type(left).__name__},{type(right).__name__})")

    if result:
        right = kwargs["visitor"].visit(rexpr).value

        if   isinstance(right,int)   : result = Int(right != 0)
        elif isinstance(right,float) : result = Int(right != 0.0)
        elif isinstance(right,str)   : result = Int(right != "")
        elif isinstance(right,list)  : result = Int(len(right) != 0)
        elif isinstance(right,dict)  : result = Int(len(right) != 0)
        else : raise RuntimeError(f"Unsupported operand type(s) for `&&`: ({type(left).__name__},{type(right).__name__})")

    return result


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
        elif s[i:i+1] == "\\"     : raise SyntaxError(f"Invalid backslash sequence in string at position {i}")
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

class Function:
    def __init__(self, fn, **kwargs):
        self.fn = fn
        self.opts = {
            "minargs"    : kwargs.get("minargs", kwargs.get("nargs", 0)),
            "maxargs"    : kwargs.get("maxargs", kwargs.get("nargs", float("Inf"))),
            "delayvisit" : kwargs.get("delayvisit", False),
        }

        # Validation
        if   self.opts["minargs"] > self.opts["maxargs"]:
            raise SyntaxError(f"minargs ({self.opts['minargs']}) is greater than than maxargs ({self.opts['maxargs']})")

    def __call__(self, *args, **kwargs):
        minargs = self.opts["minargs"]
        maxargs = self.opts["maxargs"]

        if   len(args) < minargs or maxargs < len(args):
            raise SyntaxError(f"Invalid argument count; expected [{minargs}, {maxargs}], got {len(args)}")

        return self.fn(*args, **kwargs)

    def __str__(self):
        return "<Function>"

    @property
    def value(self):
        return self


def __builtin_ceil(value, **kwargs):
    return Int(math.ceil(value))


def __builtin_eval(value, **kwargs):
    return eval(value, kwargs["sym"])


def __builtin_floor(value, **kwargs):
    return Int(math.floor(value))


def __builtin_for(init, cond, incr, block, **kwargs):
    visitor = kwargs["visitor"]
    result = Int(0)

    visitor.visit(init)

    while(visitor.visit(cond).value):
        visitor.visit(block)
        result = visitor.visit(incr)

    return result


def __builtin_function(sig, body, **kwargs):
    visitor = kwargs["visitor"]
    sigstr = visitor.visit(sig).value
    cbody = Compiled(body)
    minargs = 0
    maxargs = 0

    for c in sigstr:
        if   c == "*" : maxargs = float("Inf")
        elif c == "?" : minargs += 1; maxargs += 1
        else          : raise RuntimeError(f"'{c}' is an invalid function signature")

    def function(*args, **kwargs2):
        csym = SymbolTable({
            "ARG"    : Array(args),
            "GLOBAL" : kwargs["sym"].root,
        }, kwargs["sym"])

        if kwargs["sym"].parent:
            csym["UPSCOPE"] = kwargs["sym"]

        return cbody.eval(csym)

    return Function(function, minargs=minargs, maxargs=maxargs, delayvisit=False)


def __builtin_if(*args, **kwargs):
    visitor = kwargs["visitor"]
    result = Int(0)
    i = 0

    while i+1 < len(args):
        if visitor.visit(args[i]).value:
            result = visitor.visit(args[i+1])
            break

        i += 2

    if i+1 == len(args):
        result = visitor.visit(args[-1])

    return result


def __builtin_len(value, **kwargs):
    return Int(len(value))


def __builtin_print(*args, **kwargs):
    print(*[x.value for x in args])

    return Int(len(args))


def __builtin_round(value, **kwargs):
    return Int(round(value))


def __builtin_sqrt(value, **kwargs):
    return Double(math.sqrt(value))


def __builtin_while(cond, expr, **kwargs):
    visitor = kwargs["visitor"]
    result = Int(0)

    while(visitor.visit(cond).value):
        result = visitor.visit(expr)

    return result


builtins = {
    "CEIL"     : Function(__builtin_ceil     , nargs=1                   ),
    "EVAL"     : Function(__builtin_eval     , nargs=1                   ),
    "FLOOR"    : Function(__builtin_floor    , nargs=1                   ),
    "FOR"      : Function(__builtin_for      , nargs=4  , delayvisit=True),
    "FUNCTION" : Function(__builtin_function , nargs=2  , delayvisit=True),
    "IF"       : Function(__builtin_if       , minargs=2, delayvisit=True),
    "LEN"      : Function(__builtin_len      , nargs=1                   ),
    "PRINT"    : Function(__builtin_print                                ),
    "ROUND"    : Function(__builtin_round    , nargs=1                   ),
    "SQRT"     : Function(__builtin_sqrt     , nargs=1                   ),
    "WHILE"    : Function(__builtin_while    , nargs=2  , delayvisit=True),
}

