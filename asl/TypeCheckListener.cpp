    //////////////////////////////////////////////////////////////////////
//
//    TypeCheckListener - Walk the parser tree to do the semantic
//                        typecheck for the Asl programming language
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

#include "TypeCheckListener.h"

#include "antlr4-runtime.h"

#include "../common/TypesMgr.h"
#include "../common/SymTable.h"
#include "../common/TreeDecoration.h"
#include "../common/SemErrors.h"

#include <iostream>
#include <string>

// uncomment the following line to enable debugging messages with DEBUG*
// #define DEBUG_BUILD
#include "../common/debug.h"

// using namespace std;


// Constructor
TypeCheckListener::TypeCheckListener(TypesMgr       & Types,
				     SymTable       & Symbols,
				     TreeDecoration & Decorations,
				     SemErrors      & Errors) :
  Types{Types},
  Symbols {Symbols},
  Decorations{Decorations},
  Errors{Errors} {
}

void TypeCheckListener::enterProgram(AslParser::ProgramContext *ctx) {
  DEBUG_ENTER();
  SymTable::ScopeId sc = getScopeDecor(ctx);
  Symbols.pushThisScope(sc);
}
void TypeCheckListener::exitProgram(AslParser::ProgramContext *ctx) {
  if (Symbols.noMainProperlyDeclared())
    Errors.noMainProperlyDeclared(ctx);
  Symbols.popScope();
  Errors.print();
  DEBUG_EXIT();
}

void TypeCheckListener::enterFunction(AslParser::FunctionContext *ctx) {
  DEBUG_ENTER();
  SymTable::ScopeId sc = getScopeDecor(ctx);
  Symbols.pushThisScope(sc);
  // Symbols.print();
}
void TypeCheckListener::exitFunction(AslParser::FunctionContext *ctx) {
  Symbols.popScope();
  std::string ident = ctx->ID()->getText();
  TypesMgr::TypeId t2 = Symbols.getType(ident);
  if(Types.isFunctionTy(t2))
    t2 = Types.getFuncReturnType(t2);
  //
  DEBUG_EXIT();
}

void TypeCheckListener::enterParameters(AslParser::ParametersContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitParameters(AslParser::ParametersContext *ctx) {
  TypesMgr::TypeId t = getTypeDecor(ctx->vartype());
  putTypeDecor(ctx, t);
  DEBUG_EXIT();
}

void TypeCheckListener::enterReturnStmt(AslParser::ReturnStmtContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitReturnStmt(AslParser::ReturnStmtContext *ctx) {
  TypesMgr::TypeId t;
  if(ctx->expr())
    t = getTypeDecor(ctx->expr());
  else
    t = Types.createVoidTy(); 
  putTypeDecor(ctx, t);
  putIsLValueDecor(ctx, false);
  TypesMgr::TypeId t2 = Symbols.getCurrentFunctionTy();
  t2 = Types.getFuncReturnType(t2);
  if(not Types.copyableTypes(t2, t)){
    if(ctx->expr())
      Errors.incompatibleReturn(ctx->expr());  
    else
      Errors.incompatibleReturn(ctx);    
  }
  
  //std::cout << Types.to_string(t) << std::endl;
  DEBUG_EXIT();
}


void TypeCheckListener::enterDeclarations(AslParser::DeclarationsContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitDeclarations(AslParser::DeclarationsContext *ctx) {
  DEBUG_EXIT();
}

void TypeCheckListener::enterVariable_decl(AslParser::Variable_declContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitVariable_decl(AslParser::Variable_declContext *ctx) {
  DEBUG_EXIT();
}

void TypeCheckListener::enterType(AslParser::TypeContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitType(AslParser::TypeContext *ctx) {
  DEBUG_EXIT();
}

void TypeCheckListener::enterStatements(AslParser::StatementsContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitStatements(AslParser::StatementsContext *ctx) {
  DEBUG_EXIT();
}

void TypeCheckListener::enterAssignStmt(AslParser::AssignStmtContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitAssignStmt(AslParser::AssignStmtContext *ctx) {
  TypesMgr::TypeId t1 = getTypeDecor(ctx->left_expr());
  TypesMgr::TypeId t2 = getTypeDecor(ctx->expr());
  if(Types.isVoidTy(t2))
    Errors.isNotFunction(ctx->expr());
  else if ((not Types.isErrorTy(t1)) and (not Types.isErrorTy(t2)) and
        (not Types.copyableTypes(t1, t2)))
        Errors.incompatibleAssignment(ctx->ASSIGN());
    
    
  if ((not Types.isErrorTy(t1)) and (not getIsLValueDecor(ctx->left_expr())))
        Errors.nonReferenceableLeftExpr(ctx->left_expr());
  DEBUG_EXIT();
}

void TypeCheckListener::enterIfStmt(AslParser::IfStmtContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitIfStmt(AslParser::IfStmtContext *ctx) {
  TypesMgr::TypeId t1 = getTypeDecor(ctx->expr());
  if ((not Types.isErrorTy(t1)) and (not Types.isBooleanTy(t1)))
    Errors.booleanRequired(ctx);
  DEBUG_EXIT();
}

void TypeCheckListener::enterWhileStmt(AslParser::WhileStmtContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitWhileStmt(AslParser::WhileStmtContext *ctx) {
  TypesMgr::TypeId t1 = getTypeDecor(ctx->expr());
  if ((not Types.isErrorTy(t1)) and (not Types.isBooleanTy(t1)))
    Errors.booleanRequired(ctx);
  DEBUG_EXIT();
}


void TypeCheckListener::enterProcCall(AslParser::ProcCallContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitProcCall(AslParser::ProcCallContext *ctx) {
  std::string ident = ctx->ID()->getText();
  if (Symbols.findInStack(ident) == -1) {
    Errors.undeclaredIdent(ctx->ID());
  }
  else{
    if (not Symbols.isFunctionClass(ident)) {
      Errors.isNotCallable(ctx);
    }
    else{
      TypesMgr::TypeId ft = Symbols.getType(ident);
      const std::vector<TypesMgr::TypeId> paramtypes = Types.getFuncParamsTypes(ft);
      if(ctx->expr().size() != Types.getNumOfParameters(ft))
        Errors.numberOfParameters(ctx);
      else
        for(unsigned int i = 0; i < ctx->expr().size(); ++i){
          TypesMgr::TypeId t1 = getTypeDecor(ctx->expr(i));
//           Types.dump(paramtypes[i]);
//           Types.dump(t1);
          if(not Types.copyableTypes(paramtypes[i],t1))
            Errors.incompatibleParameter(ctx->expr(i), i+1, ctx);
        }
    }
  }
  
  
  DEBUG_EXIT();
}

void TypeCheckListener::enterReadStmt(AslParser::ReadStmtContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitReadStmt(AslParser::ReadStmtContext *ctx) {
  TypesMgr::TypeId t1 = getTypeDecor(ctx->left_expr());
  if ((not Types.isErrorTy(t1)) and (not Types.isPrimitiveTy(t1)) and
      (not Types.isFunctionTy(t1)))
    Errors.readWriteRequireBasic(ctx);
  if ((not Types.isErrorTy(t1)) and (not getIsLValueDecor(ctx->left_expr())))
    Errors.nonReferenceableExpression(ctx);
  DEBUG_EXIT();
}

void TypeCheckListener::enterWriteExpr(AslParser::WriteExprContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitWriteExpr(AslParser::WriteExprContext *ctx) {
  TypesMgr::TypeId t1 = getTypeDecor(ctx->expr());
  if ((not Types.isErrorTy(t1)) and (not Types.isPrimitiveTy(t1)))
    Errors.readWriteRequireBasic(ctx);
  DEBUG_EXIT();
}

void TypeCheckListener::enterWriteString(AslParser::WriteStringContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitWriteString(AslParser::WriteStringContext *ctx) {
  DEBUG_EXIT();
}

void TypeCheckListener::enterLeft_expr(AslParser::Left_exprContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitLeft_expr(AslParser::Left_exprContext *ctx) {
  TypesMgr::TypeId t1 = getTypeDecor(ctx->ident());
  putTypeDecor(ctx, t1);
  bool b = getIsLValueDecor(ctx->ident());
  putIsLValueDecor(ctx, b);
  DEBUG_EXIT();
}

void TypeCheckListener::enterArithmetic(AslParser::ArithmeticContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitArithmetic(AslParser::ArithmeticContext *ctx) {
  TypesMgr::TypeId t1 = getTypeDecor(ctx->expr(0));
  TypesMgr::TypeId t2 = getTypeDecor(ctx->expr(1));
  if (((not Types.isErrorTy(t1)) and (not Types.isNumericTy(t1))) or
      ((not Types.isErrorTy(t2)) and (not Types.isNumericTy(t2))))
    Errors.incompatibleOperator(ctx->op);
  TypesMgr::TypeId t;
  if(Types.isFloatTy(t1) or Types.isFloatTy(t2))
    t = Types.createFloatTy();
  else
    t = Types.createIntegerTy();
  putTypeDecor(ctx, t);
  putIsLValueDecor(ctx, false);
  DEBUG_EXIT();
}

void TypeCheckListener::enterLogicalOp(AslParser::LogicalOpContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitLogicalOp(AslParser::LogicalOpContext *ctx) {
  TypesMgr::TypeId t1 = getTypeDecor(ctx->expr(0));
  TypesMgr::TypeId t2 = getTypeDecor(ctx->expr(1));
  if (((not Types.isErrorTy(t1)) and (not Types.isBooleanTy(t1))) or
      ((not Types.isErrorTy(t2)) and (not Types.isBooleanTy(t2))))
    Errors.incompatibleOperator(ctx->op);
  TypesMgr::TypeId t = Types.createBooleanTy();
  putTypeDecor(ctx, t);
  putIsLValueDecor(ctx, false);
  DEBUG_EXIT();
}

void TypeCheckListener::enterRelational(AslParser::RelationalContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitRelational(AslParser::RelationalContext *ctx) {
  TypesMgr::TypeId t1 = getTypeDecor(ctx->expr(0));
  TypesMgr::TypeId t2 = getTypeDecor(ctx->expr(1));
  std::string oper = ctx->op->getText();
  if ((not Types.isErrorTy(t1)) and (not Types.isErrorTy(t2)) and
      (not Types.comparableTypes(t1, t2, oper)))
    Errors.incompatibleOperator(ctx->op);
  TypesMgr::TypeId t = Types.createBooleanTy();
  putTypeDecor(ctx, t);
  putIsLValueDecor(ctx, false);
  DEBUG_EXIT();
}

void TypeCheckListener::enterParenthesis(AslParser::ParenthesisContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitParenthesis(AslParser::ParenthesisContext *ctx) {
  TypesMgr::TypeId t = getTypeDecor(ctx->expr());
  putTypeDecor(ctx, t);
  putIsLValueDecor(ctx, false);
  DEBUG_EXIT();
}

void TypeCheckListener::enterUnary(AslParser::UnaryContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitUnary(AslParser::UnaryContext *ctx) {
  TypesMgr::TypeId t1 = getTypeDecor(ctx->expr());
  if(ctx->NOT()){
    if((not Types.isErrorTy(t1)) and (not Types.isBooleanTy(t1)))
      Errors.booleanRequired(ctx);
    TypesMgr::TypeId t = Types.createBooleanTy();
    putTypeDecor(ctx, t);
  }
  else{
    if((not Types.isErrorTy(t1)) and (not Types.isNumericTy(t1)))
       Errors.incompatibleOperator(ctx->op);
    TypesMgr::TypeId t = t1;
    putTypeDecor(ctx, t);
  }
  putIsLValueDecor(ctx, false);
  DEBUG_EXIT();
}


void TypeCheckListener::enterValue(AslParser::ValueContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitValue(AslParser::ValueContext *ctx) {
  TypesMgr::TypeId t;
  if(ctx->INTVAL())
    t = Types.createIntegerTy();
  else if(ctx->CHARVAL())
    t = Types.createCharacterTy();
  else if(ctx->FLOATVAL())
    t = Types.createFloatTy();
  else
    t = Types.createBooleanTy();
  
  putTypeDecor(ctx, t);
  putIsLValueDecor(ctx, false);
  DEBUG_EXIT();
}

void TypeCheckListener::enterExprIdent(AslParser::ExprIdentContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitExprIdent(AslParser::ExprIdentContext *ctx) {
  TypesMgr::TypeId t1 = getTypeDecor(ctx->ident());
//   if(Types.isVoidTy(t1))
//       Errors.incompatibleAssignment(ctx->ident());
  putTypeDecor(ctx, t1);
  bool b = getIsLValueDecor(ctx->ident());
  putIsLValueDecor(ctx, b);
  DEBUG_EXIT();
}



void TypeCheckListener::enterSimpleID(AslParser::SimpleIDContext *ctx){
  DEBUG_ENTER();
}
void TypeCheckListener::exitSimpleID(AslParser::SimpleIDContext *ctx){
  std::string ident = ctx->ID()->getText();
  if (Symbols.findInStack(ident) == -1) {
    Errors.undeclaredIdent(ctx->ID());
    TypesMgr::TypeId te = Types.createErrorTy();
    putTypeDecor(ctx, te);
    putIsLValueDecor(ctx, true);
  }
  else {
    TypesMgr::TypeId t1 = Symbols.getType(ident);
    if(Types.isArrayTy(t1)){
       //ERROOOOR 
    }
      
    if(Symbols.isFunctionClass(ident)){
        putIsLValueDecor(ctx, false);
        putTypeDecor(ctx, t1);
    }
    else{
        putTypeDecor(ctx, t1); 
        putIsLValueDecor(ctx, true);
    }
  }
  DEBUG_EXIT();
}
  
void TypeCheckListener::enterArrayID(AslParser::ArrayIDContext *ctx){
  DEBUG_ENTER();
}
void TypeCheckListener::exitArrayID(AslParser::ArrayIDContext *ctx){
  std::string ident = ctx->ID()->getText();
  if (Symbols.findInStack(ident) == -1) {
    Errors.undeclaredIdent(ctx->ID());
    TypesMgr::TypeId te = Types.createErrorTy();
    putTypeDecor(ctx, te);
    putIsLValueDecor(ctx, true);
  }
  else {
    TypesMgr::TypeId t1 = Symbols.getType(ident);
    TypesMgr::TypeId t2 = getTypeDecor(ctx->expr());
    if(not Types.isArrayTy(t1)){
      Errors.nonArrayInArrayAccess(ctx);
      t1 = Types.createErrorTy();
    }
    else 
      t1 = Types.getArrayElemType(t1);
    
    if(not Types.isIntegerTy(t2))
      Errors.nonIntegerIndexInArrayAccess(ctx->expr());
   
    putTypeDecor(ctx, t1);
    putIsLValueDecor(ctx, true);
  }
  DEBUG_EXIT();
}
  
void TypeCheckListener::enterFuncID(AslParser::FuncIDContext *ctx){
  DEBUG_ENTER();
}
void TypeCheckListener::exitFuncID(AslParser::FuncIDContext *ctx){
  std::string ident = ctx->ID()->getText();
  if (Symbols.findInStack(ident) == -1) {
    Errors.undeclaredIdent(ctx->ID());
    TypesMgr::TypeId te = Types.createErrorTy();
    putTypeDecor(ctx, te);
    putIsLValueDecor(ctx, false);
  }
  else {
    TypesMgr::TypeId ft = Symbols.getType(ident);
    if(Symbols.isFunctionClass(ident)){
      const std::vector<TypesMgr::TypeId> paramtypes = Types.getFuncParamsTypes(ft);
      if(ctx->expr().size() != Types.getNumOfParameters(ft))
        Errors.numberOfParameters(ctx);
      else
        for(unsigned int i = 0; i < ctx->expr().size(); ++i){
          TypesMgr::TypeId t1 = getTypeDecor(ctx->expr(i));
          if(not Types.copyableTypes(paramtypes[i],t1))
            Errors.incompatibleParameter(ctx->expr(i), i+1, ctx);
        }
      ft = Types.getFuncReturnType(ft);
    }
    else{
      Errors.isNotCallable(ctx);
    }
    
    putTypeDecor(ctx, ft);
    putIsLValueDecor(ctx, false); 
  }
  DEBUG_EXIT();
}



void TypeCheckListener::enterIdent(AslParser::IdentContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitIdent(AslParser::IdentContext *ctx) {
//   std::string ident = ctx->ID()->getText();
//   if (Symbols.findInStack(ident) == -1) {
//     Errors.undeclaredIdent(ctx->ID());
//     TypesMgr::TypeId te = Types.createErrorTy();
//     putTypeDecor(ctx, te);
//     putIsLValueDecor(ctx, true);
//   }
//   else {
//     TypesMgr::TypeId t1 = Symbols.getType(ident);
//     
//     if(ctx->expr(0)){
//       if((not Types.isArrayTy(t1)) and not(Symbols.isFunctionClass(ident)))
//         Errors.nonArrayInArrayAccess(ctx);
//       else if(Symbols.isFunctionClass(ident)){
//         t1 = Types.getFuncReturnType(t1);
//       }
//       else{
//         TypesMgr::TypeId t = getTypeDecor(ctx->expr(0));
//         if(not Types.isIntegerTy(t))
//           Errors.nonIntegerIndexInArrayAccess(ctx);
//         t1 = Types.getArrayElemType(t1);
//       }
//     }
//     putTypeDecor(ctx, t1);
//     if (Symbols.isFunctionClass(ident))
//       putIsLValueDecor(ctx, false);
//     else
//       putIsLValueDecor(ctx, true);
//   }
  DEBUG_EXIT();
}

// void TypeCheckListener::enterEveryRule(antlr4::ParserRuleContext *ctx) {
//   DEBUG_ENTER();
// }
// void TypeCheckListener::exitEveryRule(antlr4::ParserRuleContext *ctx) {
//   DEBUG_EXIT();
// }
// void TypeCheckListener::visitTerminal(antlr4::tree::TerminalNode *node) {
//   DEBUG("visitTerminal");
// }
// void TypeCheckListener::visitErrorNode(antlr4::tree::ErrorNode *node) {
// }


// Getters for the necessary tree node atributes:
//   Scope, Type ans IsLValue
SymTable::ScopeId TypeCheckListener::getScopeDecor(antlr4::ParserRuleContext *ctx) {
  return Decorations.getScope(ctx);
}
TypesMgr::TypeId TypeCheckListener::getTypeDecor(antlr4::ParserRuleContext *ctx) {
  return Decorations.getType(ctx);
}
bool TypeCheckListener::getIsLValueDecor(antlr4::ParserRuleContext *ctx) {
  return Decorations.getIsLValue(ctx);
}

// Setters for the necessary tree node attributes:
//   Scope, Type ans IsLValue
void TypeCheckListener::putScopeDecor(antlr4::ParserRuleContext *ctx, SymTable::ScopeId s) {
  Decorations.putScope(ctx, s);
}
void TypeCheckListener::putTypeDecor(antlr4::ParserRuleContext *ctx, TypesMgr::TypeId t) {
  Decorations.putType(ctx, t);
}
void TypeCheckListener::putIsLValueDecor(antlr4::ParserRuleContext *ctx, bool b) {
  Decorations.putIsLValue(ctx, b);
}
