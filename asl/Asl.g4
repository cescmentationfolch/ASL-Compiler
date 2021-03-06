//////////////////////////////////////////////////////////////////////
//
//    Asl - Another simple language (grammar)
//
//    Copyright (C) 2017  Universitat Politecnica de Catalunya
//
//    This library is free software; you can redistribute it and/or
//    modify it under the terms of the GNU General Public License
//    as published by the Free Software Foundation; either version 3
//    of the License, or (at your option) any later version.
//
//    This library is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    Affero General Public License for more details.
//
//    You should have received a copy of the GNU Affero General Public
//    License along with this library; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
//    contact: José Miguel Rivero (rivero@cs.upc.es)
//             Computer Science Department
//             Universitat Politecnica de Catalunya
//             despatx Omega.110 - Campus Nord UPC
//             08034 Barcelona.  SPAIN
//
//////////////////////////////////////////////////////////////////////

grammar Asl;

//////////////////////////////////////////////////
/// Parser Rules
//////////////////////////////////////////////////

// A program is a list of functions
program : function+ EOF
        ;

// A function has a name, a list of parameters and a list of statements
function
        : FUNC ID '(' ((parameters (',' parameters)*) | ) ')' ((':' vartype declarations statements* returnStmt)| (declarations statements*)) ENDFUNC
        ;
        
parameters
        : ID ':' vartype
        ;
        
declarations
        : (variable_decl)*
        ;
        
vartype : (type|array) ;



variable_decl
        : VAR ID (',' ID)* ':' vartype
        ;

array   : ARRAY '[' INTVAL ']' 'of' type
        ;
        
type    : INT
        | FLOAT
        | BOOL
        | CHAR
        ;

statements
        : (statement | returnStmt)
        ;
        
returnStmt
        : RETURN expr? ';'
        ;
// The different types of instructions
statement
          // Assignment
        : left_expr ASSIGN expr ';'           # assignStmt
          // if-then-else statement (else is optional)
        | IF expr THEN statements* els? ENDIF       # ifStmt
          // A function/procedure call has a list of arguments in parenthesis (possibly empty)
        | WHILE expr DO statements* ENDWHILE      # whileStmt
        
        | ID '(' (expr (',' expr)*)? ')' ';'  # procCall
          // Read a variable
        | READ left_expr ';'                  # readStmt
          // Write an expression
        | WRITE expr ';'                      # writeExpr
          // Write a string
        | WRITE STRING ';'                    # writeString
        ;
        
els: ELSE statements* ;

// Grammar for left expressions (l-values in C++)
left_expr
        : ident
        ;

// Grammar for expressions with boolean, relational and aritmetic operators
expr    : op=(NOT|PLUS|MINUS) expr           #unary
        | expr op=(MUL|DIV|MODUL) expr                # arithmetic
        | expr op=(PLUS|MINUS) expr                   # arithmetic
        | expr op=(EQUAL|NEQ|GR|GEQ|LEQ|LESS) expr    # relational
        | expr op=AND expr                    # logicalOp
        | expr op=OR expr                     # logicalOp
        | (INTVAL|CHARVAL|TRUE|FALSE|FLOATVAL) # value
        | ident                               # exprIdent
        | '(' expr ')'                        #parenthesis
        ;

ident   : ID '(' (expr (',' expr)*)? ')'    #funcID
        | ID    #simpleID
        | ID '[' expr ']'  #arrayID
        
        ;

//////////////////////////////////////////////////
/// Lexer Rules
//////////////////////////////////////////////////

ASSIGN    : '=' ;
EQUAL     : '==' ;
NEQ       : '!=' ;
GR        : '>' ;
GEQ       : '>=' ;
LEQ       : '<=' ;
LESS      : '<' ;
PLUS      : '+' ;
MINUS     : '-' ;
MUL       : '*' ;
DIV       : '/' ;
MODUL     : '%' ;
AND       : 'and' ;
OR        : 'or' ;
NOT       : 'not' ;
TRUE      : 'true' ;
FALSE     : 'false' ;
VAR       : 'var';
INT       : 'int';
FLOAT     : 'float' ;
BOOL      : 'bool' ;
CHAR      : 'char' ;
CHARVAL   : '\'' . '\'' ; 
ARRAY     : 'array' ;
IF        : 'if' ;
THEN      : 'then' ;
ELSE      : 'else' ;
ENDIF     : 'endif' ;
WHILE     : 'while' ;
DO        : 'do' ;
ENDWHILE  : 'endwhile' ;
FUNC      : 'func' ;
RETURN    : 'return' ;
ENDFUNC   : 'endfunc' ;
READ      : 'read' ;
WRITE     : 'write' ;
ID        : ('a'..'z'|'A'..'Z'|'_') ('a'..'z'|'A'..'Z'|'_'|'0'..'9')* ;
INTVAL    : ('0'..'9')+ ;
FLOATVAL  : ('0'..'9')+ '.' ('0'..'9')+ ;


// Strings (in quotes) with escape sequences
STRING    : '"' ( ESC_SEQ | ~('\\'|'"') )* '"' ;

fragment
ESC_SEQ   : '\\' ('b'|'t'|'n'|'f'|'r'|'"'|'\''|'\\') ;

// Comments (inline C++-style)
COMMENT   : '//' ~('\n'|'\r')* '\r'? '\n' -> skip ;

// White spaces
WS        : (' '|'\t'|'\r'|'\n')+ -> skip ;
// Alternative description
// WS        : [ \t\r\n]+ -> skip ;
