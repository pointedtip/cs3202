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
#include "simple/condition.h"
#include "simple/condition_set.h"
#include "simple/predicate.h"
#include "impl/condition.h"

namespace simple {
namespace impl {

template <typename Predicate>
class PredicateGenerator : 
    public SimplePredicate, public StatementVisitor,
    public ExprVisitor
{
  public:
    PredicateGenerator(SimpleRoot ast, Predicate *pred = new Predicate());

    const ConditionSet& global_set();

    void filter_set(ConditionSet& conditions);

    void visit_assignment(AssignmentAst *assign);
    void visit_conditional(ConditionalAst *condition);
    void visit_while(WhileAst *loop);
    void visit_call(CallAst *call);

    void visit_variable(VariableAst *var);
    void visit_const(ConstAst *val);
    void visit_binary_op(BinaryOpAst *bin);
  private:
    void create_global_set();
    void filter_statement_list(StatementAst *statement);

    SimpleRoot _ast;
    ConditionSet _global_set;
    std::unique_ptr<Predicate> _pred;
};

/*
 * A predicate that accepts everything.
 */
class WildCardPredicate {
  public:
    WildCardPredicate();

    template <typename Condition>
    bool evaluate(Condition *condition);
};

class ProcPredicate {
  public:
    ProcPredicate();

    template <typename Condition>
    bool evaluate(Condition *condition);
};

template <>
bool ProcPredicate::evaluate<ProcAst>(ProcAst *proc);

class ProcPredicateWithName {
  public:
    ProcPredicateWithName(const std::string& name);

    template <typename Condition>
    bool evaluate(Condition *condition);

  private:
    std::string _name;
};

template <>
bool ProcPredicateWithName::evaluate<ProcAst>(ProcAst *proc);

class StatementPredicate {
  public:
    StatementPredicate();

    template <typename Condition>
    bool evaluate(Condition *condition);
};

template <>
bool StatementPredicate::evaluate<AssignmentAst>(AssignmentAst *assign);

template <>
bool StatementPredicate::evaluate<WhileAst>(WhileAst *loop);

template <>
bool StatementPredicate::evaluate<ConditionalAst>(ConditionalAst *condition);

template <>
bool StatementPredicate::evaluate<CallAst>(CallAst *call);

class AssignPredicate {
  public:
    AssignPredicate();

    template <typename Condition>
    bool evaluate(Condition *condition);
};

template <>
bool AssignPredicate::evaluate<AssignmentAst>(AssignmentAst *assign);

class ConditionalPredicate {
  public:
    ConditionalPredicate();

    template <typename Condition>
    bool evaluate(Condition *condition);
};

template <>
bool ConditionalPredicate::evaluate<ConditionalAst>(ConditionalAst *condition);

class WhilePredicate {
  public:
    WhilePredicate();

    template <typename Condition>
    bool evaluate(Condition *condition);
};

template <>
bool WhilePredicate::evaluate<WhileAst>(WhileAst *loop);

class VariablePredicate {
  public:
    VariablePredicate();

    template <typename Condition>
    bool evaluate(Condition *condition);
};

template <>
bool VariablePredicate::evaluate<SimpleVariable>(SimpleVariable *variable);

typedef PredicateGenerator<WildCardPredicate>   SimpleWildCardPredicate;
typedef PredicateGenerator<ProcPredicate>       SimpleProcPredicate;
typedef PredicateGenerator<StatementPredicate>  SimpleStatementPredicate;
typedef PredicateGenerator<AssignPredicate>     SimpleAssignmentPredicate;
typedef PredicateGenerator<WhilePredicate>      SimpleWhilePredicate;
typedef PredicateGenerator<ConditionalPredicate> SimpleConditionalPredicate;
typedef PredicateGenerator<VariablePredicate>   SimpleVariablePredicate;



}
}