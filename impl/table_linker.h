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

#include <vector>
#include "simple/tuple.h"
#include "simple/predicate.h"
#include "simple/condition_set.h"

namespace simple {

/**
 * Since PQL is almost the same as logic programming in Prolog, there is 
 * one problem that we have when solving PQL queries especially with multiple 
 * clauses. The problem is that when a condition in one of the query variable of
 * a two-variable-clause is dropped after the clause is solved, it invalidates 
 * the linked conditions in it's counterpart query variable. 
 *
 * For example, if the results of a query 
 * 
 * ClauseA(A, B) 
 *
 * is 
 *
 * <A, B> = [<1, 2>, <3,4>]
 * A = 1, 3
 * B = 2, 4
 *
 * and the result of a query
 *
 * ClauseB(B, "x")
 *
 * is
 *
 * <B> = [<4>, <5>]
 * 
 * then the combined query
 *
 * ClauseA(A, B) AND ClauseB(B, "x")
 *
 * should have the result
 *
 * <A, B> = [<3, 4>]
 * A = 3
 * B = 4
 *
 * From the above example, we can see that the condition (4) in query variable B 
 * is invalidated when solving ClauseB, and that in turns invalidates condition (3)
 * in query variable A. Because of this the result returned when selecting only 
 * the result of A must not contain condition (3) because it's sole linkage is 
 * to query variable B is broken.
 *
 * The problem cannot be solved by simply executing single-variable clauses first,
 * as two two-variable-clauses such as
 *
 * ClauseA(A, B) AND ClauseB(B, C)
 *
 * can cause the same problem as well. As a result, we need a linker class to 
 * clean the broken links after executing all queries.
 *
 * The linker class should also be especially careful to not cause infinite loops
 * when resolving cyclic links, such as
 *
 * ClauseA(A, B) AND ClauseB(B, C) AND ClauseC(C, A)
 *
 * and the linking algorithm must be able to solve that problem.
 */
class TableQueryLinker {
  public:
    /**
     * Add or update binary links between two query variables into the 
     * database. The binary links make sure that the removal of a 
     * condition in one variable will affect linked conditions on 
     * the other side.
     *
     * The query variable must have already been defined when the link
     * is added, and the linking will perform an intersection on both 
     * query variables to remove conditions that do not have a link, 
     * or links with one side of conditions not in the query variable.
     */
    virtual void update_links(
            const std::string& qvar1, 
            const std::string& qvar2, 
            const std::vector<ConditionPair>& links) = 0;

    /**
     * Update the result for a single query variable by intersecting 
     * the old results with the new results supplied in the parameter.
     *
     * If the query variable is not initialized it will be initialized
     * first. The first call of update_result() on a variable should be
     * the global set defined by the variable's predicate.
     */
    virtual void update_results(
            const std::string& qvar,
            const ConditionSet& conditions) = 0;

    /*
     * SHAWNNNN
     * This method retrieves the possible values of the given query variables
     * qvar1 and qvar2 from the results table. i.e. tuple results. 
     * Just find the index of the qvars(if they exist),
     * then row by row, head to the correct columns in the row and get the values.
     * You can change the method inputs or return type to what you want.
     * BTW, the table stores the values as ConditionPtr (\simple\condition_set.h)
     */
    virtual ConditionSet get_links(const std::string& qvar1,
            const std::string& qvar2) = 0;
  
    /*
     * SHAWNNNN
     * This method retrieves the possible values of the given query variable
     * qvar from the results table. Just find the index of the qvar(if it exists),
     * then row by row, head to the correct column in the row and get the value.
     * You can change the method inputs or return type to what you want.
     */
    virtual ConditionSet get_conditions(const std::string& qvar) = 0;
  
  private:
  typedef std::vector<ConditionPtr> tableRow; //rows represented as vector of values
  typedef std::vector<tableRow> table; //table represented as vector of rows
  typedef std::map<std::string, int> indexMap; //map with the name of the query variable as the key and its index in the tableRow as the content
  
  table _results; //results table
  indexMap _indexes; //map of indexes
  bool _initialised; //false if the results table is empty, true otherwise
};


}
