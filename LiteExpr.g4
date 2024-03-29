grammar LiteExpr;

file    : expr EOF
        | EOF
        ;

expr    : STRING                                        # String
        | DOUBLE                                        # Double
        | HEX                                           # Hex
        | INT                                           # Int
        | '(' expr ')'                                  # Paren
        | varname '(' list ')'                          # Call
        | varname                                       # Variable
        | '{' pairlist '}'                              # Object
        | '[' list ']'                                  # Array
        | varname op=('++'|'--')                        # PostfixOp
        | op=('++'|'--') varname                        # PrefixOp
        | op=('!'|'~'|'+'|'-') expr                     # UnaryOp
        | <assoc=right> expr op='**' expr               # BinaryOp
        | expr op=('*'|'/'|'%') expr                    # BinaryOp
        | expr op=('+'|'-') expr                        # BinaryOp
        | expr op=('<<'|'>>'|'>>>') expr                # BinaryOp
        | expr op=('<'|'<='|'>'|'>=') expr              # BinaryOp
        | expr op=('=='|'!=') expr                      # BinaryOp
        | expr op='&' expr                              # BinaryOp
        | expr op='^' expr                              # BinaryOp
        | expr op='|' expr                              # BinaryOp
        | expr op='&&' expr                             # BinaryOp
        | expr op='||' expr                             # BinaryOp
        | <assoc=right> varname op=('='|'**='|'*='|'/='|'%='|'+='|'-='|'<<='|'>>='|'>>>='|'&='|'^='|'|='|'&&='|'||=') expr
                                                        # AssignOp
        | <assoc=right> expr op1='?' expr op2=':' expr  # TernaryOp
        | expr op=';' expr                              # BinaryOp
        | expr ';'                                      # Term
        ;

varname : varname '.' varname                           # MemberVar
        | varname '[' expr ']'                          # IndexedVar
        | ID                                            # SimpleVar
        ;

pairlist: pair (',' pair)* ','?
        |
        ;

pair    : ID ':' expr
        ;

list    : expr (',' expr)* ','?
        |
        ;

STRING  : '"' ('\\\\'|'\\"'|~'"')*? '"'
        ;

DOUBLE  : [1-9][0-9]* '.' [0-9]+
        | [0]? '.' [0-9]+
        ;

INT     : [1-9][0-9]*
        | [0]
        ;

HEX     : '0x' [0-9A-Fa-f]+
        ;

ID      : [_a-zA-Z][_a-zA-Z0-9]* ;

WS      : [ \t\r\n]+ -> skip ;

COMMENT : ( '#' ~[\n]* | '/*' .*? '*/' ) -> channel(HIDDEN) ;

