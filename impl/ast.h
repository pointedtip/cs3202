/*
 * CS3201 Simple Static Analyzer
 * Copyright (C) 2011 Soares Chen Ruo Fei
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "simple/ast.h"

namespace simple {
namespace impl {

using namespace simple;

class SimpleProcAst : public ProcAst {
  public:
    SimpleProcAst(std::string name) : _name(name) { }

    void set_first_statement(StatementAst* statement) {
        if(_statement) { throw InconsistentAstError(); }
        _statement.reset(statement);
    }

    virtual std::string get_name() {
        return _name;
    }

    virtual StatementAst* get_statement() {
        return _statement.get();
    }

  private:
    std::string _name;

    // ProcAst owns the first statement ast.
    std::unique_ptr<StatementAst> _statement;
};

class SimpleStatementAst {
  public:
    virtual void set_source_line(unsigned int line) = 0;
    virtual void set_statement_line(unsigned int line) = 0;
    virtual void set_proc(ProcAst *proc) = 0;
    virtual void set_container(ContainerAst *container) = 0;
    virtual void set_next(StatementAst *next) = 0;
    virtual void set_prev(StatementAst *prev) = 0;

    virtual StatementAst* as_ast() = 0;

    virtual ~SimpleStatementAst() { }
};

/*
 * Abstract Class
 */
template <typename ParentType>
class SimpleStatementBase : public ParentType, public SimpleStatementAst {
  public:
    SimpleStatementBase() :
        _source_line(0), _statement_line(0), _proc(0), _container(0),
        _prev(0), _next()
    { }

    SimpleStatementBase(int line) :
        _source_line(line), _statement_line(line), _proc(0), _container(0),
        _prev(0), _next()
    { }

    void set_source_line(unsigned int line) {
        _source_line = line;
    }

    void set_statement_line(unsigned int line) {
        _statement_line = line;
    }

    void set_line(unsigned int line) {
        set_source_line(line);
        set_statement_line(line);
    }

    void set_proc(ProcAst *proc) {
        _proc = proc;
    }

    void set_container(ContainerAst *container) {
        _container = container;
    }

    void set_next(StatementAst *statement) {
        _next.reset(statement);
    }

    void set_prev(StatementAst *statement) {
        _prev = statement;
    }

    int get_source_line() {
        return _source_line;
    }

    int get_statement_line() {
        return _statement_line;
    }

    StatementAst* next() {
        return _next.get();
    }

    StatementAst* prev() {
        return _prev;
    }

    ContainerAst* get_parent() {
        return _container;
    }

    ProcAst* get_proc() {
        return _proc;
    }

    StatementAst* as_ast() {
        return this;
    }

    virtual ~SimpleStatementBase() { }

  protected:
    int _source_line;
    int _statement_line;
    ProcAst *_proc;
    ContainerAst *_container;
    StatementAst *_prev;

    // StatementAst owns the next statement after itself.
    std::unique_ptr<StatementAst> _next;
};

class SimpleIfAst : public SimpleStatementBase<IfAst> {
  public:
    SimpleIfAst() :
        SimpleStatementBase<IfAst>(),
        _then_branch(), _else_branch(), _var()
    { }

    void set_variable(const SimpleVariable& var) {
        _var = var;
    }

    void set_then_branch(StatementAst *then_branch) {
        _then_branch.reset(then_branch);
    }

    void set_else_branch(StatementAst *else_branch) {
        _else_branch.reset(else_branch);
    }

    SimpleVariable* get_variable() {
        return &_var;
    }

    StatementAst* get_then_branch() {
        return _then_branch.get();
    }

    StatementAst* get_else_branch() {
        return _else_branch.get();
    }

    void accept_statement_visitor(StatementVisitor *visitor) {
        visitor->visit_if(this);
    }

    void accept_container_visitor(ContainerVisitor *visitor) {
        visitor->visit_if(this);
    }

    ~SimpleIfAst() { }

  private:
    std::unique_ptr<StatementAst>   _then_branch;
    std::unique_ptr<StatementAst>   _else_branch;
    SimpleVariable                  _var;
};

class SimpleWhileAst : public SimpleStatementBase<WhileAst> {
  public:
    SimpleWhileAst() : 
        SimpleStatementBase<WhileAst>(),
        _body(), _var() 
    { }

    void set_variable(const SimpleVariable& var) {
        _var = var;
    }

    void set_body(StatementAst *body) {
        _body.reset(body);
    }

    StatementAst* get_body() {
        return _body.get();
    }

    SimpleVariable* get_variable() {
        return &_var;
    }

    void accept_statement_visitor(StatementVisitor *visitor) {
        visitor->visit_while(this);
    }

    void accept_container_visitor(ContainerVisitor *visitor) {
        visitor->visit_while(this);
    }

    ~SimpleWhileAst() { }

  private:
    std::unique_ptr<StatementAst>   _body;
    SimpleVariable                  _var;
};

class SimpleCallAst : public SimpleStatementBase<CallAst> {
  public:
    SimpleCallAst() : 
        SimpleStatementBase<CallAst>(), 
        _proc_called(0) 
    { }

    SimpleCallAst(ProcAst *proc) : 
        SimpleStatementBase<CallAst>(), 
        _proc_called(proc) 
    { }

    void set_proc_called(ProcAst *proc) {
        _proc_called = proc;
    }

    ProcAst* get_proc_called() {
        return _proc_called;
    }

    void accept_statement_visitor(StatementVisitor *visitor) {
        visitor->visit_call(this);
    }

    ~SimpleCallAst() { }

  private:
    ProcAst *_proc_called;
};

class SimpleAssignmentAst : public SimpleStatementBase<AssignmentAst> {
  public:
    SimpleAssignmentAst() : 
        SimpleStatementBase<AssignmentAst>(),
        _var(), _expr() 
    { }

    SimpleAssignmentAst(int line) :
        SimpleStatementBase<AssignmentAst>(line),
        _var(), _expr()
    { }


    void set_variable(const SimpleVariable& var) {
        _var = var;
    }

    void set_expr(ExprAst *expr) {
        _expr.reset(expr);
    }

    SimpleVariable* get_variable() {
        return &_var;
    }

    ExprAst* get_expr() {
        return _expr.get();
    }

    void accept_statement_visitor(StatementVisitor *visitor) {
        visitor->visit_assignment(this);
    }

    ~SimpleAssignmentAst() { }

  private:
    SimpleVariable            _var;
    std::unique_ptr<ExprAst>  _expr;
};

class SimpleVariableAst : public VariableAst {
  public:
    SimpleVariableAst() : _var() { }
    SimpleVariableAst(const SimpleVariable& var) : _var(var) { }
    SimpleVariableAst(const char *var) : _var(var) { }

    void set_variable(const SimpleVariable& var) {
        _var = var;
    }

    SimpleVariable* get_variable() {
        return &_var;
    }

    void accept_expr_visitor(ExprVisitor *visitor) {
        visitor->visit_variable(this);
    }

    ExprAst* clone() {
        return new SimpleVariableAst(_var);
    }

    ~SimpleVariableAst() { }

  private:
    SimpleVariable  _var;
};

class SimpleConstAst : public ConstAst {
  public:
    SimpleConstAst() : _value(0) { }
    SimpleConstAst(int value) : _value(value) { }

    void set_value(int value) {
        _value = value;
    }

    SimpleConstant* get_constant() {
        return &_value;
    }

    int get_value() {
        return _value;
    }
    
    void accept_expr_visitor(ExprVisitor *visitor) {
        visitor->visit_const(this);
    }

    ExprAst* clone() {
        return new SimpleConstAst(_value.get_int());
    }

    ~SimpleConstAst() { }

  private:
    SimpleConstant  _value;
};

class SimpleBinaryOpAst : public BinaryOpAst {
  public:
    SimpleBinaryOpAst() : 
        _lhs(), _rhs(), _op(' ')
    { }

    SimpleBinaryOpAst(char op, ExprAst *lhs, ExprAst *rhs) :
        _lhs(lhs), _rhs(rhs), _op(op)
    { }

    void set_lhs(ExprAst *lhs) {
        _lhs.reset(lhs);
    }

    void set_rhs(ExprAst *rhs) {
        _rhs.reset(rhs);
    }

    void set_op(char op) {
        _op = op;
    }

    ExprAst* get_lhs() {
        return _lhs.get();
    }

    ExprAst* get_rhs() {
        return _rhs.get();
    }

    char get_op() {
        return _op;
    }

    ExprAst* clone() {
        return new SimpleBinaryOpAst(_op, _lhs->clone(), _rhs->clone());
    }

    void accept_expr_visitor(ExprVisitor *visitor) {
        visitor->visit_binary_op(this);
    }

    ~SimpleBinaryOpAst() { }
  private:
    std::unique_ptr<ExprAst>    _lhs;
    std::unique_ptr<ExprAst>    _rhs;
    char                        _op;
};

} // namespace impl
} // namespace simple
