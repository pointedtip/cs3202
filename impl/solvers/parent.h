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
#include "simple/solver.h"
#include "impl/condition.h"

namespace simple {
namespace impl {

using namespace simple;

class ParentSolver {
  public:
    ParentSolver(SimpleRoot ast) : _ast(ast) { }

    /*
     * SOLVE RIGHT PART
     */
    template <typename Condition>
    ConditionSet solve_right(Condition *condition) {
        return ConditionSet();
    }

    template <typename Condition>
    ConditionSet solve_left(Condition *condition) {
        return ConditionSet();
    }

    template <typename Condition1, typename Condition2>
    bool validate(Condition1 *condition1, Condition2 *condition2) {
        return false;
    }

  private:
    SimpleRoot _ast;
};


template <>
ConditionSet ParentSolver::solve_right<StatementAst>(StatementAst *statement);

template <>
ConditionSet ParentSolver::solve_right<WhileAst>(WhileAst *loop);

template <>
ConditionSet ParentSolver::solve_right<IfAst>(IfAst *condition);

template <>
ConditionSet ParentSolver::solve_left<StatementAst>(StatementAst *ast);

template <>
bool ParentSolver::validate<StatementAst, StatementAst>(
        StatementAst *left, StatementAst *right);

template <>
bool ParentSolver::validate<IfAst, StatementAst>(
        IfAst *condition, StatementAst *statement);

template <>
bool ParentSolver::validate<WhileAst, StatementAst>(
        WhileAst *loop, StatementAst *statement);



} // namespace impl
} // namespace simple
