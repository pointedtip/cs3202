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

#include "impl/solvers/assign.h"

namespace simple {
namespace impl {

using namespace simple;
using namespace simple::util;

class UsesSolver : public AssignmentSolver {
  public:
    UsesSolver(const SimpleRoot& ast);

    VariableSet get_vars_used_by_statement(StatementAst *statement);
    VariableSet get_vars_used_by_proc(ProcAst *proc);
};

} // namespace impl
} // namespace simple
