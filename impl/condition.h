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

#include "simple/condition.h"

namespace simple {
namespace impl {

using namespace simple;

class SimpleStatementCondition : public StatementCondition {
  public:
    SimpleStatementCondition(StatementAst *ast) : _ast(ast) { }

    StatementAst* get_statement_ast() {
        return _ast;
    }

    void accept_condition_visitor(ConditionVisitor *visitor) {
        visitor->visit_statement_condition(this);
    }

    ~SimpleStatementCondition() { }

  private:
    StatementAst *_ast;
};

class SimpleProcCondition : public ProcCondition {
  public:
    SimpleProcCondition(ProcAst *proc) : _proc(proc) { }

    ProcAst* get_proc_ast() {
        return _proc;
    }

    void accept_condition_visitor(ConditionVisitor *visitor) {
        visitor->visit_proc_condition(this);
    }

    ~SimpleProcCondition() { }

  private:
    ProcAst *_proc;
};

class SimpleVariableCondition : public VariableCondition {
  public:
    SimpleVariableCondition(const SimpleVariable& var) : 
        _var(var) 
    { }

    virtual SimpleVariable* get_variable() {
        return &_var;
    }

    virtual void accept_condition_visitor(ConditionVisitor *visitor) {
        visitor->visit_variable_condition(this);
    }

    virtual ~SimpleVariableCondition() { }
  private:
    SimpleVariable _var;
};

class SimpleConstantCondition : public ConstantCondition {
  public:
    SimpleConstantCondition(SimpleConstant value) : _value(value) { }

    virtual SimpleConstant* get_constant() {
        return &_value;
    }

    virtual void accept_condition_visitor(ConditionVisitor *visitor) {
        visitor->visit_constant_condition(this);
    }

    virtual ~SimpleConstantCondition() { }
  private:
    SimpleConstant _value;
};

class SimplePatternCondition : public PatternCondition {
  public:
    SimplePatternCondition(ExprAst *pattern) : _pattern(pattern) { }

    ExprAst* get_expr_ast() {
        return _pattern.get();
    }

    void accept_condition_visitor(ConditionVisitor *visitor) {
        visitor->visit_pattern_condition(this);
    }

    ~SimplePatternCondition() { }
  private:
    std::unique_ptr<ExprAst> _pattern;
};

class SimpleOperatorCondition : public OperatorCondition {
  public:
    SimpleOperatorCondition(char op) : _op(op) { }

    char get_operator() {
        return _op;
    }

    void accept_condition_visitor(ConditionVisitor *visitor) {
        visitor->visit_operator_condition(this);
    }

    ~SimpleOperatorCondition() { }
    
  private:
    char _op;
};

} // namespace impl
} // namespace simple
