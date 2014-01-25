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

#include "impl/selector.h"
#include "simple/util/condition_utils.h"

namespace simple {
namespace impl {

using namespace simple;

template <typename Selector>
std::vector<std::string> format_selector(
    Selector *selector, PqlQuerySet *query, QueryLinker *linker);

template <>
std::vector<std::string> format_selector<PqlSingleVarSelector>(
    PqlSingleVarSelector *var_selector, PqlQuerySet *query, QueryLinker *linker)
{
    std::string qvar = var_selector->get_qvar_name();
    SimplePredicate *pred;

    if(query->predicates.count(qvar) > 0) {
        pred = query->predicates[qvar].get();
    } else {
        pred = query->predicates["*"].get();
    }

    ConditionSet conditions = linker->get_conditions(qvar, pred);
    
    std::vector<std::string> result;

    for(ConditionSet::iterator it = conditions.begin(); 
            it != conditions.end(); ++it)
    {
        result.push_back(condition_to_string(*it));
    }

    return result;
}

template <>
std::vector<std::string> format_selector<PqlBooleanSelector>(
    PqlBooleanSelector *selector, PqlQuerySet *query, QueryLinker *linker)
{
    std::vector<std::string> result;

    if(linker->is_valid_state()) {
        result.push_back("true");
    } else {
        result.push_back("false");
    }

    return result;
}

template <>
std::vector<std::string> format_selector<PqlTupleSelector>(
    PqlTupleSelector *selector, PqlQuerySet *query, QueryLinker *linker)
{
    return std::vector<std::string>();
}

class ResultFormatterVisitor : public PqlSelectorVisitor {
  public:
    ResultFormatterVisitor(PqlQuerySet *query, QueryLinker *linker) : 
      _query(query), _linker(linker)
    { }

    void visit_single_var(PqlSingleVarSelector *selector) {
        _result = format_selector<PqlSingleVarSelector>(
            selector, _query, _linker);
    }

    void visit_boolean(PqlBooleanSelector *selector) {
        _result = format_selector<PqlBooleanSelector>(
            selector, _query, _linker);
    }

    void visit_tuple(PqlTupleSelector *selector) {
        _result = format_selector<PqlTupleSelector>(
            selector, _query, _linker);
    }

    std::vector<std::string> return_result() {
        return std::move(_result);
    }


  private:
    PqlQuerySet *_query;
    QueryLinker *_linker;
    std::vector<std::string> _result;
};

std::vector<std::string> format_result(
    PqlQuerySet *query, QueryLinker *linker)
{
    PqlSelector *selector = query->selector.get();

    ResultFormatterVisitor visitor(query, linker);
    selector->accept_pql_selector_visitor(&visitor);
    
    return visitor.return_result();
}

}
}