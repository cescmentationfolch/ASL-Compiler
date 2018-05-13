//////////////////////////////////////////////////////////////////////
//
//    CodeGenListener - Walk the parser tree to do
//                             the generation of code
//
//    Copyright (C) 2018  Universitat Politecnica de Catalunya
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
//    contact: José Miguel Rivero (rivero@cs.upc.edu)
//             Computer Science Department
//             Universitat Politecnica de Catalunya
//             despatx Omega.110 - Campus Nord UPC
//             08034 Barcelona.  SPAIN
//
//////////////////////////////////////////////////////////////////////

#include "CodeGenListener.h"

#include "antlr4-runtime.h"

#include "../common/TypesMgr.h"
#include "../common/SymTable.h"
#include "../common/TreeDecoration.h"
#include "../common/code.h"

#include <cstddef>    // std::size_t

// uncomment the following line to enable debugging messages with DEBUG*
// #define DEBUG_BUILD
#include "../common/debug.h"

// using namespace std;


// Constructor
CodeGenListener::CodeGenListener(TypesMgr       & Types,
				 SymTable       & Symbols,
				 TreeDecoration & Decorations,
				 code           & Code) :
  Types{Types},
  Symbols{Symbols},
  Decorations{Decorations},
  Code{Code} {
}

void CodeGenListener::enterProgram(AslParser::ProgramContext *ctx) {
  DEBUG_ENTER();
  SymTable::ScopeId sc = getScopeDecor(ctx);
  Symbols.pushThisScope(sc);
}
void CodeGenListener::exitProgram(AslParser::ProgramContext *ctx) {
  Symbols.popScope();
  DEBUG_EXIT();
}

void CodeGenListener::enterFunction(AslParser::FunctionContext *ctx) {
  DEBUG_ENTER();
  subroutine subr(ctx->ID()->getText());
  Code.add_subroutine(subr);
  SymTable::ScopeId sc = getScopeDecor(ctx);
  Symbols.pushThisScope(sc);
  codeCounters.reset();
}
void CodeGenListener::exitFunction(AslParser::FunctionContext *ctx) {
  subroutine & subrRef = Code.get_last_subroutine();
  instructionList  code;
  if(ctx->vartype())
    subrRef.add_param("_result");
  for(unsigned int i = 0; i < ctx->parameters().size(); ++i)
    subrRef.add_param(ctx->parameters(i)->ID()->getText());
  for(unsigned int i = 0; i < ctx->statements().size(); ++i)
    code = code || getCodeDecor(ctx->statements(i));
  
  if(ctx->returnStmt())
    code = code || getCodeDecor(ctx->returnStmt());
  else
    code = code || instruction::RETURN();
  subrRef.set_instructions(code);
  Symbols.popScope();
  DEBUG_EXIT();
}

void CodeGenListener::enterParameters(AslParser::ParametersContext *ctx){
  DEBUG_ENTER();
  
}
void CodeGenListener::exitParameters(AslParser::ParametersContext *ctx){
  DEBUG_EXIT();
}

void CodeGenListener::enterReturnStmt(AslParser::ReturnStmtContext *ctx){

  DEBUG_ENTER();
}
void CodeGenListener::exitReturnStmt(AslParser::ReturnStmtContext *ctx){
  //subroutine & subrRef = Code.get_last_subroutine();
  instructionList code;
  if(ctx->expr()){
    code = code || getCodeDecor(ctx->expr());
    std::string     addr = getAddrDecor(ctx->expr());
    TypesMgr::TypeId t1 = getTypeDecor(ctx->expr());
    if(Types.isIntegerTy(t1))
      code = code || instruction::ILOAD("_result", addr);
    else if(Types.isCharacterTy(t1))
      code = code || instruction::CHLOAD("_result", addr);
    else if(Types.isFloatTy(t1))
      code = code || instruction::FLOAD("_result", addr);
    else if(Types.isBooleanTy(t1))
      code = code || instruction::ILOAD("_result", addr);
  }
  code = code || instruction::RETURN();
  putCodeDecor(ctx, code);
  DEBUG_EXIT();
}

void CodeGenListener::enterDeclarations(AslParser::DeclarationsContext *ctx) {
  DEBUG_ENTER();
}
void CodeGenListener::exitDeclarations(AslParser::DeclarationsContext *ctx) {
  DEBUG_EXIT();
}

void CodeGenListener::enterVariable_decl(AslParser::Variable_declContext *ctx) {
  DEBUG_ENTER();
}
void CodeGenListener::exitVariable_decl(AslParser::Variable_declContext *ctx) {
  subroutine       & subrRef = Code.get_last_subroutine();
  TypesMgr::TypeId        t1 = getTypeDecor(ctx->vartype());
  std::size_t           size = Types.getSizeOfType(t1);
  for(unsigned int i = 0; i < ctx->ID().size(); ++i){
    subrRef.add_var(ctx->ID(i)->getText(), size);
  }
  DEBUG_EXIT();
}

void CodeGenListener::enterType(AslParser::TypeContext *ctx) {
  DEBUG_ENTER();
}
void CodeGenListener::exitType(AslParser::TypeContext *ctx) {
  DEBUG_EXIT();
}

void CodeGenListener::enterArray(AslParser::ArrayContext *ctx) {
  DEBUG_ENTER();
}
void CodeGenListener::exitArray(AslParser::ArrayContext *ctx) {
  DEBUG_EXIT();
}
void CodeGenListener::enterStatements(AslParser::StatementsContext *ctx) {
  DEBUG_ENTER();
}
void CodeGenListener::exitStatements(AslParser::StatementsContext *ctx) {
  instructionList code;
  if(ctx->statement()) {
    code = code || getCodeDecor(ctx->statement());
  }
  else{
    code = code || getCodeDecor(ctx->returnStmt());
  }
  putCodeDecor(ctx, code);
  DEBUG_EXIT();
}

void CodeGenListener::enterAssignStmt(AslParser::AssignStmtContext *ctx) {
  DEBUG_ENTER();
}
void CodeGenListener::exitAssignStmt(AslParser::AssignStmtContext *ctx) {
  instructionList  code;
  std::string     addr1 = getAddrDecor(ctx->left_expr());
  // std::string     offs1 = getOffsetDecor(ctx->left_expr());
  instructionList code1 = getCodeDecor(ctx->left_expr());
  // TypesMgr::TypeId tid1 = getTypeDecor(ctx->left_expr());
  std::string     addr2 = getAddrDecor(ctx->expr());
  // std::string     offs2 = getOffsetDecor(ctx->expr());
  instructionList code2 = getCodeDecor(ctx->expr());
  // TypesMgr::TypeId tid2 = getTypeDecor(ctx->expr());
  code = code1 || code2 || instruction::LOAD(addr1, addr2);
  putCodeDecor(ctx, code);
  DEBUG_EXIT();
}
void CodeGenListener::enterWhileStmt(AslParser::WhileStmtContext *ctx){
  DEBUG_ENTER();
}
void CodeGenListener::exitWhileStmt(AslParser::WhileStmtContext *ctx){
  instructionList   code;
  std::string      addr1 = getAddrDecor(ctx->expr());
  instructionList  code1 = getCodeDecor(ctx->expr());
  instructionList  code2;
  for(unsigned int i = 0; i < ctx->statements().size(); ++i)
    code2 = code2 || getCodeDecor(ctx->statements(i));
  std::string      label = codeCounters.newLabelWHILE();
  std::string labelStartWhile = "startwhile"+label;
  std::string labelEndWhile = "endwhile"+label;
  code = instruction::LABEL(labelStartWhile) || code1 || instruction::FJUMP(addr1, labelEndWhile) || code2 || instruction::UJUMP(labelStartWhile)|| instruction::LABEL(labelEndWhile);
  putCodeDecor(ctx, code);
  DEBUG_EXIT();
}

void CodeGenListener::enterIfStmt(AslParser::IfStmtContext *ctx) {
  DEBUG_ENTER();
}
void CodeGenListener::exitIfStmt(AslParser::IfStmtContext *ctx) {
  instructionList   code;
  std::string      addr1 = getAddrDecor(ctx->expr());
  instructionList  code1 = getCodeDecor(ctx->expr());
  instructionList  code2;
  for(unsigned int i = 0; i < ctx->statements().size(); ++i)
    code2 = code2 || getCodeDecor(ctx->statements(i));
  std::string      label = codeCounters.newLabelIF();
  std::string labelEndIf = "endif"+label;
  code = code1 || instruction::FJUMP(addr1, labelEndIf) ||
         code2 || instruction::LABEL(labelEndIf);
  putCodeDecor(ctx, code);
  DEBUG_EXIT();
}

void CodeGenListener::enterProcCall(AslParser::ProcCallContext *ctx) {
  DEBUG_ENTER();
}
void CodeGenListener::exitProcCall(AslParser::ProcCallContext *ctx) {
  instructionList code;
  // std::string name = ctx->ident()->ID()->getSymbol()->getText();
  std::string name = ctx->ID()->getText();
  TypesMgr::TypeId t = Symbols.getType(name);
  for(unsigned int i = 0; i < ctx->expr().size(); ++i){
    code = code || getCodeDecor(ctx->expr(i));
  }
  if(not Types.isVoidFunction(t))
    code = code || instruction::PUSH();
  for(unsigned int i = 0; i < ctx->expr().size(); ++i){
    std::string addr = getAddrDecor(ctx->expr(i));
    code = code || instruction::PUSH(addr);
  }
  code = code || instruction::CALL(name);
  for(unsigned int i = 0; i < ctx->expr().size(); ++i){
    std::string addr = getAddrDecor(ctx->expr(i));
    code = code || instruction::POP();
  }
  if(not Types.isVoidFunction(t)){
    std::string temp = "%"+codeCounters.newTEMP();
    code = code || instruction::POP(temp);
    putAddrDecor(ctx, temp);
  }
  putCodeDecor(ctx, code);
  DEBUG_EXIT();
}

void CodeGenListener::enterReadStmt(AslParser::ReadStmtContext *ctx) {
  DEBUG_ENTER();
}
void CodeGenListener::exitReadStmt(AslParser::ReadStmtContext *ctx) {
  instructionList  code;
  std::string     addr1 = getAddrDecor(ctx->left_expr());
  // std::string     offs1 = getOffsetDecor(ctx->left_expr());
  instructionList code1 = getCodeDecor(ctx->left_expr());
  // TypesMgr::TypeId tid1 = getTypeDecor(ctx->left_expr());
  code = code1 || instruction::READI(addr1);
  putCodeDecor(ctx, code);
  DEBUG_EXIT();
}

void CodeGenListener::enterWriteExpr(AslParser::WriteExprContext *ctx) {
  DEBUG_ENTER();
}
void CodeGenListener::exitWriteExpr(AslParser::WriteExprContext *ctx) {
  instructionList code;
  std::string     addr1 = getAddrDecor(ctx->expr());
  // std::string     offs1 = getOffsetDecor(ctx->expr());
  instructionList code1 = getCodeDecor(ctx->expr());
  TypesMgr::TypeId tid1 = getTypeDecor(ctx->expr());
  if(Types.isFloatTy(tid1))
    code = code1 || instruction::WRITEF(addr1);
  else if(Types.isCharacterTy(tid1))
    code = code1 || instruction::WRITEC(addr1);
  else
    code = code1 || instruction::WRITEI(addr1);
  putCodeDecor(ctx, code);
  DEBUG_EXIT();
}

void CodeGenListener::enterWriteString(AslParser::WriteStringContext *ctx) {
  DEBUG_ENTER();
}
void CodeGenListener::exitWriteString(AslParser::WriteStringContext *ctx) {
  instructionList code;
  std::string s = ctx->STRING()->getText();
  std::string temp = "%"+codeCounters.newTEMP();
  int i = 1;
  while (i < int(s.size())-1) {
    if (s[i] != '\\') {
      code = code ||
	     instruction::CHLOAD(temp, s.substr(i,1)) ||
	     instruction::WRITEC(temp);
      i += 1;
    }
    else {
      assert(i < int(s.size())-2);
      if (s[i+1] == 'n') {
        code = code || instruction::WRITELN();
        i += 2;
      }
      else if (s[i+1] == 't' or s[i+1] == '"' or s[i+1] == '\\') {
        code = code ||
               instruction::CHLOAD(temp, s.substr(i,2)) ||
	       instruction::WRITEC(temp);
        i += 2;
      }
      else {
        code = code ||
               instruction::CHLOAD(temp, s.substr(i,1)) ||
	       instruction::WRITEC(temp);
        i += 1;
      }
    }
  }
  putCodeDecor(ctx, code);
  DEBUG_EXIT();
}

void CodeGenListener::enterLeft_expr(AslParser::Left_exprContext *ctx) {
  DEBUG_ENTER();
}
void CodeGenListener::exitLeft_expr(AslParser::Left_exprContext *ctx) {
  putAddrDecor(ctx, getAddrDecor(ctx->ident()));
  putOffsetDecor(ctx, getOffsetDecor(ctx->ident()));
  putCodeDecor(ctx, getCodeDecor(ctx->ident()));
  DEBUG_ENTER();
}

void CodeGenListener::enterArithmetic(AslParser::ArithmeticContext *ctx) {
  DEBUG_ENTER();
}
void CodeGenListener::exitArithmetic(AslParser::ArithmeticContext *ctx) {
  std::string     temp1 = getAddrDecor(ctx->expr(0));
  instructionList code1 = getCodeDecor(ctx->expr(0));
  std::string     temp2 = getAddrDecor(ctx->expr(1));
  instructionList code2 = getCodeDecor(ctx->expr(1));
  instructionList code  = code1 || code2;
  TypesMgr::TypeId t1 = getTypeDecor(ctx->expr(0));
  TypesMgr::TypeId t2 = getTypeDecor(ctx->expr(1));
  bool integer;
  std::string addr1 = temp1;
  std::string addr2 = temp2;
  if(Types.isIntegerTy(t1) and Types.isIntegerTy(t2))
    integer = true;
  else{
    integer = false;
    if(Types.isIntegerTy(t1)){
      addr1 = "%"+codeCounters.newTEMP();
      code = code || instruction::FLOAT(addr1, temp1);
    }
    if(Types.isIntegerTy(t2)){
      addr2 = "%"+codeCounters.newTEMP();
      code = code || instruction::FLOAT(addr2, temp2);
    }
  }
  std::string temp = "%"+codeCounters.newTEMP();
  if (ctx->MUL()){
    if(integer)
      code = code || instruction::MUL(temp, addr1, addr2);
    else
      code = code || instruction::FMUL(temp, addr1, addr2);
  }
  else if (ctx->PLUS()){
    if(integer)
      code = code || instruction::ADD(temp, addr1, addr2);
    else
      code = code || instruction::FADD(temp, addr1, addr2);
  }
  else if (ctx->MINUS()){
    if(integer)
      code = code || instruction::SUB(temp, addr1, addr2);
    else
      code = code || instruction::FSUB(temp, addr1, addr2);
  }
  else if (ctx->DIV()){
    if(integer)
      code = code || instruction::DIV(temp, addr1, addr2);
    else
      code = code || instruction::FDIV(temp, addr1, addr2);
  }
  else if (ctx->MODUL()){
    if(integer){
      code = code || instruction::DIV(temp, addr1, addr2);
      code = code || instruction::MUL(temp, temp, addr2);
      code = code || instruction::SUB(temp, addr1, temp);
    }
    else{
      code = code || instruction::FDIV(temp, addr1, addr2);
      code = code || instruction::FMUL(temp, temp, addr2);
      code = code || instruction::FSUB(temp, addr1, temp);
    }
  }
  putAddrDecor(ctx, temp);
  putOffsetDecor(ctx, "");
  putCodeDecor(ctx, code);
  DEBUG_EXIT();
}

void CodeGenListener::enterUnary(AslParser::UnaryContext *ctx){
  DEBUG_ENTER();
}
void CodeGenListener::exitUnary(AslParser::UnaryContext *ctx){
  std::string     addr1 = getAddrDecor(ctx->expr());
  instructionList code = getCodeDecor(ctx->expr());
  TypesMgr::TypeId t1 = getTypeDecor(ctx->expr());
  bool integer = Types.isIntegerTy(t1);
  std::string temp = "%"+codeCounters.newTEMP();
  if(ctx->NOT())
    code = code || instruction::NOT(temp, addr1);
  if(ctx->MINUS()){
    if(integer)
      code = code || instruction::NEG(temp, addr1);
    else
      code = code || instruction::FNEG(temp, addr1);
  }
  putAddrDecor(ctx, temp);
  putOffsetDecor(ctx, "");
  putCodeDecor(ctx, code);
  DEBUG_EXIT();
}

void CodeGenListener::enterRelational(AslParser::RelationalContext *ctx) {
  DEBUG_ENTER();
}
void CodeGenListener::exitRelational(AslParser::RelationalContext *ctx) {
  std::string     temp1 = getAddrDecor(ctx->expr(0));
  instructionList code1 = getCodeDecor(ctx->expr(0));
  std::string     temp2 = getAddrDecor(ctx->expr(1));
  instructionList code2 = getCodeDecor(ctx->expr(1));
  instructionList code  = code1 || code2;
  TypesMgr::TypeId t1 = getTypeDecor(ctx->expr(0));
  TypesMgr::TypeId t2 = getTypeDecor(ctx->expr(1));
  bool integer;
  std::string addr1 = temp1;
  std::string addr2 = temp2;
  if(Types.isIntegerTy(t1) and Types.isIntegerTy(t2))
    integer = true;
  else{
    integer = false;
    if(Types.isIntegerTy(t1)){
      addr1 = "%"+codeCounters.newTEMP();
      code = code || instruction::FLOAT(addr1, temp1);
    }
    if(Types.isIntegerTy(t2)){
      addr2 = "%"+codeCounters.newTEMP();
      code = code || instruction::FLOAT(addr2, temp2);
    }
  }
    
  std::string temp = "%"+codeCounters.newTEMP();
  if (ctx->EQUAL()){
    if(integer)
      code = code || instruction::EQ(temp, addr1, addr2);
    else
      code = code || instruction::FEQ(temp, addr1, addr2);
  }
  else if (ctx->NEQ()){
    if(integer){
      code = code || instruction::EQ(temp, addr1, addr2);
      code = code || instruction::NOT(temp, temp);
    }
    else{
      code = code || instruction::FEQ(temp, addr1, addr2);
      code = code || instruction::NOT(temp, temp);
    }
  }
  else if (ctx->GR()){
    if(integer){
      code = code || instruction::LE(temp, addr1, addr2);
      code = code || instruction::NOT(temp, temp);
    }
    else{
      code = code || instruction::FLE(temp, addr1, addr2);
      code = code || instruction::NOT(temp, temp);
    }
  }
  else if (ctx->GEQ()){
    if(integer){
      code = code || instruction::LT(temp, addr1, addr2);
      code = code || instruction::NOT(temp, temp);
    }
    else{
      code = code || instruction::FLT(temp, addr1, addr2);
      code = code || instruction::NOT(temp, temp);
    }
  }
  else if (ctx->LEQ()){
    if(integer)
      code = code || instruction::LE(temp, addr1, addr2);
    else
      code = code || instruction::FLE(temp, addr1, addr2);
  }
  else if (ctx->LESS()){
    if(integer)
      code = code || instruction::LT(temp, addr1, addr2);
    else
      code = code || instruction::FLT(temp, addr1, addr2);
  }

  putAddrDecor(ctx, temp);
  putOffsetDecor(ctx, "");
  putCodeDecor(ctx, code);
  DEBUG_EXIT();
}

void CodeGenListener::enterLogicalOp(AslParser::LogicalOpContext *ctx){
  DEBUG_ENTER();
}
void CodeGenListener::exitLogicalOp(AslParser::LogicalOpContext *ctx){
  std::string     addr1 = getAddrDecor(ctx->expr(0));
  instructionList code1 = getCodeDecor(ctx->expr(0));
  std::string     addr2 = getAddrDecor(ctx->expr(1));
  instructionList code2 = getCodeDecor(ctx->expr(1));
  instructionList code  = code1 || code2;
    
  std::string temp = "%"+codeCounters.newTEMP();
  if(ctx->OR())
    code = code || instruction::OR(temp, addr1, addr2);
  else
    code = code || instruction::AND(temp, addr1, addr2);
  
  putAddrDecor(ctx, temp);
  putOffsetDecor(ctx, "");
  putCodeDecor(ctx, code);
  DEBUG_EXIT();
}

void CodeGenListener::enterParenthesis(AslParser::ParenthesisContext *ctx){
  DEBUG_ENTER();
}
void CodeGenListener::exitParenthesis(AslParser::ParenthesisContext *ctx){
  std::string     addr1 = getAddrDecor(ctx->expr());
  instructionList code = getCodeDecor(ctx->expr());
  putAddrDecor(ctx, addr1);
  putOffsetDecor(ctx, "");
  putCodeDecor(ctx, code);
  DEBUG_EXIT();
}

void CodeGenListener::enterValue(AslParser::ValueContext *ctx) {
  DEBUG_ENTER();
}
 //(INTVAL|CHARVAL|TRUE|FALSE|FLOATVAL) # value
void CodeGenListener::exitValue(AslParser::ValueContext *ctx) {
  instructionList code;
  std::string temp = "%"+codeCounters.newTEMP();
  if(ctx->INTVAL())
    code = instruction::ILOAD(temp, ctx->getText());
  if(ctx->CHARVAL())
    code = instruction::CHLOAD(temp, ctx->getText());
  if(ctx->FLOATVAL())
    code = instruction::FLOAD(temp, ctx->getText());
  if(ctx->TRUE())
    code = instruction::ILOAD(temp, "1");
  if(ctx->FALSE())
    code = instruction::ILOAD(temp, "0");
  putAddrDecor(ctx, temp);
  putOffsetDecor(ctx, "");
  putCodeDecor(ctx, code);
  DEBUG_EXIT();
}

void CodeGenListener::enterExprIdent(AslParser::ExprIdentContext *ctx) {
  DEBUG_ENTER();
}
void CodeGenListener::exitExprIdent(AslParser::ExprIdentContext *ctx) {
  putAddrDecor(ctx, getAddrDecor(ctx->ident()));
  putOffsetDecor(ctx, getOffsetDecor(ctx->ident()));
  putCodeDecor(ctx, getCodeDecor(ctx->ident()));
  DEBUG_EXIT();
}

void CodeGenListener::enterIdent(AslParser::IdentContext *ctx) {
  DEBUG_ENTER();
}
void CodeGenListener::exitIdent(AslParser::IdentContext *ctx) {

  DEBUG_EXIT();
}

void CodeGenListener::enterSimpleID(AslParser::SimpleIDContext *ctx){
  DEBUG_ENTER();
}
void CodeGenListener::exitSimpleID(AslParser::SimpleIDContext *ctx){
  putAddrDecor(ctx, ctx->ID()->getText());
  putOffsetDecor(ctx, "");
  putCodeDecor(ctx, instructionList());
  DEBUG_EXIT();
}
  
void CodeGenListener::enterArrayID(AslParser::ArrayIDContext *ctx){
  DEBUG_ENTER();
}
void CodeGenListener::exitArrayID(AslParser::ArrayIDContext *ctx){
  std::string     addrexpr = getAddrDecor(ctx->expr());
  instructionList code = getCodeDecor(ctx->expr());
  putAddrDecor(ctx, ctx->ID()->getText()+"["+addrexpr+"]");
  putOffsetDecor(ctx, "");
  putCodeDecor(ctx, code);
  DEBUG_EXIT();
}
  
void CodeGenListener::enterFuncID(AslParser::FuncIDContext *ctx){
  DEBUG_ENTER();
}
void CodeGenListener::exitFuncID(AslParser::FuncIDContext *ctx){
  instructionList code;
  // std::string name = ctx->ident()->ID()->getSymbol()->getText();
  std::string name = ctx->ID()->getText();
  for(unsigned int i = 0; i < ctx->expr().size(); ++i){
    code = code || getCodeDecor(ctx->expr(i));
  }
  code = code || instruction::PUSH();
  for(unsigned int i = 0; i < ctx->expr().size(); ++i){
    std::string addr = getAddrDecor(ctx->expr(i));
    code = code || instruction::PUSH(addr);
  }
  
  code = code || instruction::CALL(name);
  
  for(unsigned int i = 0; i < ctx->expr().size(); ++i){
    std::string addr = getAddrDecor(ctx->expr(i));
    code = code || instruction::POP();
  }
  std::string temp = "%"+codeCounters.newTEMP();
  code = code || instruction::POP(temp);
  putAddrDecor(ctx, temp);
  putCodeDecor(ctx, code);
  DEBUG_EXIT();
}

// void CodeGenListener::enterEveryRule(antlr4::ParserRuleContext *ctx) {
//   DEBUG_ENTER();
// }
// void CodeGenListener::exitEveryRule(antlr4::ParserRuleContext *ctx) {
//   DEBUG_EXIT();
// }
// void CodeGenListener::visitTerminal(antlr4::tree::TerminalNode *node) {
//   DEBUG(">>> visit " << node->getSymbol()->getLine() << ":" << node->getSymbol()->getCharPositionInLine() << " CodeGen TerminalNode");
// }
// void CodeGenListener::visitErrorNode(antlr4::tree::ErrorNode *node) {
// }


// Getters for the necessary tree node atributes:
//   Scope, Type, Addr, Offset and Code
SymTable::ScopeId CodeGenListener::getScopeDecor(antlr4::ParserRuleContext *ctx) {
  return Decorations.getScope(ctx);
}
TypesMgr::TypeId CodeGenListener::getTypeDecor(antlr4::ParserRuleContext *ctx) {
  return Decorations.getType(ctx);
}
std::string CodeGenListener::getAddrDecor(antlr4::ParserRuleContext *ctx) {
  return Decorations.getAddr(ctx);
}
std::string  CodeGenListener::getOffsetDecor(antlr4::ParserRuleContext *ctx) {
  return Decorations.getOffset(ctx);
}
instructionList CodeGenListener::getCodeDecor(antlr4::ParserRuleContext *ctx) {
  return Decorations.getCode(ctx);
}

// Setters for the necessary tree node attributes:
//   Addr, Offset and Code
void CodeGenListener::putAddrDecor(antlr4::ParserRuleContext *ctx, const std::string & a) {
  Decorations.putAddr(ctx, a);
}
void CodeGenListener::putOffsetDecor(antlr4::ParserRuleContext *ctx, const std::string & o) {
  Decorations.putOffset(ctx, o);
}
void CodeGenListener::putCodeDecor(antlr4::ParserRuleContext *ctx, const instructionList & c) {
  Decorations.putCode(ctx, c);
}
