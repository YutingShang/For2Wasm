lexer grammar Fortran90Lexer-subset;

options {
    superClass = Fortran90LexerBase;
}

PROGRAM: 'program' | 'PROGRAM' | 'Program';

END: 'END' | 'end' | 'End';    

USE: 'USE' | 'use' | 'Use';     

OP: '==' | '!=' | '<=' | '>=' | '<' | '>' | '/=';    

DOUBLECOLON: '::';    

REAL: 'REAL' | 'real' | 'Real';      

IMPLICIT: 'implicit' | 'IMPLICIT' | 'Implicit';   

NONE: 'none' | 'NONE' | 'None';       

IF: 'IF' | 'if' | 'If';      

THEN: 'THEN' | 'then';

ELSE: 'ELSE' | 'else';

ENDIF: 'ENDIF' | 'endif';       

DO: 'DO' | 'do' | 'Do';      

READ: 'READ' | 'read';    

PRINT: 'PRINT' | 'print';    

NAME_: 'NAME' | 'name';           

fragment NEWLINE:         
    '\r\n'
    | '\r'
    | '\n'
    | '\u0085' // <Next Line CHARACTER (U+0085)>'
    | '\u2028' //'<Line Separator CHARACTER (U+2028)>'
    | '\u2029' //'<Paragraph Separator CHARACTER (U+2029)>'
;

WS: ([ \t] | NEWLINE)+ -> channel(HIDDEN);    

COMMENT:                              
    (
        ('\t'* '\u0020'* '!' (~ [\r\n])* [\r\n]*)
        | ( {this.IsColumnZero()}? ('c' | 'C') (~ [\r\n])* [\r\n]*)
    )       -> channel(HIDDEN)
;

COMMA: ',';        

LPAREN: '(';      

RPAREN: ')';      

COLON: ':';      

SEMICOLON: ';';      

ASSIGN: '=';       

MINUS: '-';     

PLUS: '+';      

DIV: '/';        

fragment STARCHAR: '*';      

INTEGER: 'INTEGER' | 'integer' | 'Integer';     

fragment SCORE: '_';       

SCON:              
    '\'' (
        '\'' '\''
        | ~ ('\'' | '\n' | '\r')
        | (('\n' | '\r' ('\n')?) '     ' CONTINUATION) ('\n' | '\r' ('\n')?) '     ' CONTINUATION
    )* '\''
    | '\'' (~('\'') | ('\'' '\''))* ('\'')
    | ('"') (~('"') | '""')* ('"')
;

ICON: NUM+;

EXIT: 'EXIT' | 'exit';        

NAME: LETTER ( ALPHANUMERIC_CHARACTER)*;    

ALPHANUMERIC_CHARACTER: LETTER | NUM | SCORE;    

fragment LETTER: ('a' ..'z' | 'A' ..'Z');    

STAR: STARCHAR;      

fragment CONTINUATION: ~ ('0' | ' ');    

fragment NUM: ('0' .. '9');      

















