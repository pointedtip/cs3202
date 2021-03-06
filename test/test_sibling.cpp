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

#include "gtest/gtest.h"
#include "simple/util/ast_utils.h"
#include "impl/solvers/sibling.h"
#include "impl/ast.h"
#include "impl/condition.h"

namespace simple {
namespace test {

using namespace simple;
using namespace simple::impl;
using namespace simple::util;

TEST(SiblingTest, Test_Procedure) {
    /*
     * procedure first {
     *   x = y;
     * }
	 * procedure second {
	 * }
	 * procedure third{
	 * }
     */
    SimpleProcAst *proc_first = new SimpleProcAst("first");
	SimpleProcAst *proc_second = new SimpleProcAst("second");
	SimpleProcAst *proc_third = new SimpleProcAst("third");

    SimpleAssignmentAst *assign = new SimpleAssignmentAst();
    SimpleVariable var("x");
    SimpleVariable var2("y"); // different name
    SimpleVariable var3("y"); // same name different pointer

    assign->set_variable(var);
    assign->set_expr(new SimpleVariableAst(var2));
    assign->set_line(1);
    assign->set_proc(proc_first);

    proc_first->set_first_statement(assign);

	std::vector<ProcAst*> procs;
    procs.push_back(proc_first);
    procs.push_back(proc_second);
	procs.push_back(proc_third);

    SimpleRoot root(procs.begin(), procs.end());

    SiblingSolver solver(root);

    /*
     * Positive testing
     */
	EXPECT_TRUE((solver.validate(ConditionPtr(new SimpleProcCondition(proc_first)), ConditionPtr(new SimpleProcCondition(proc_second)))));	//Directly right sibling
	EXPECT_TRUE((solver.validate(ConditionPtr(new SimpleProcCondition(proc_second)), ConditionPtr(new SimpleProcCondition(proc_first)))));	//Directly left sibling
	EXPECT_TRUE((solver.validate(ConditionPtr(new SimpleProcCondition(proc_third)), ConditionPtr(new SimpleProcCondition(proc_first)))));	//Indirectly sibling

	/*
	 * Negative testing
	 */
	SimpleProcAst *proc_wrong = new SimpleProcAst("wrong");

	EXPECT_FALSE((solver.validate(ConditionPtr(new SimpleProcCondition(proc_wrong)), ConditionPtr(new SimpleProcCondition(proc_first)))));		//Procedure in first parameter not inside AST
	EXPECT_FALSE(((solver.validate(ConditionPtr(new SimpleProcCondition(proc_third)), ConditionPtr(new SimpleProcCondition(proc_wrong))))));	//Procedure in second parameter not inside AST
	EXPECT_FALSE(((solver.validate(ConditionPtr(new SimpleProcCondition(proc_second)), ConditionPtr(new SimpleProcCondition(proc_second))))));	//Cannot have same procedures in Sibling
	EXPECT_FALSE(((solver.validate(ConditionPtr(new SimpleStatementCondition(assign)), ConditionPtr(new SimpleProcCondition(proc_second))))));	//Sibling with Procedure cannot mix with Assignment type

	/*
	 * Solve left and right
	 * Since solve left and solve right is symmetric, the test case and result is the same 
	 */
	ConditionSet result;

	//Left side
	result.clear();
	result.insert(new SimpleProcCondition(proc_second));
	result.insert(new SimpleProcCondition(proc_third));

	EXPECT_EQ(result, solver.solve_left(new SimpleProcCondition(proc_first)));
	EXPECT_EQ(result, solver.solve_right(new SimpleProcCondition(proc_first)));

	////Middle
	result.clear();
	result.insert(new SimpleProcCondition(proc_first));
	result.insert(new SimpleProcCondition(proc_third));

	EXPECT_EQ(result, solver.solve_left(new SimpleProcCondition(proc_second)));
	EXPECT_EQ(result, solver.solve_right(new SimpleProcCondition(proc_second)));

	//Right side
	result.clear();
	result.insert(new SimpleProcCondition(proc_first));
	result.insert(new SimpleProcCondition(proc_second));
	
	EXPECT_EQ(result, solver.solve_left(new SimpleProcCondition(proc_third)));
	EXPECT_EQ(result, solver.solve_right(new SimpleProcCondition(proc_third)));

	//External procedure
	result.clear();
	EXPECT_EQ(result, solver.solve_left(new SimpleProcCondition(proc_wrong)));
	EXPECT_EQ(result, solver.solve_right(new SimpleProcCondition(proc_wrong)));
}

TEST(SiblingTest, Test_Assignment) {
	/*
	 * procedure first {
	 *  x = 2;
	 *  z = 3;
	 * }
	 * procedure second {
	 *  x = 0;
	 *  z = 3;
	 *  i = 5;
	 * }
	 */
    SimpleProcAst *proc_first = new SimpleProcAst("first");
	SimpleProcAst *proc_second = new SimpleProcAst("second");
	
	// x = 2
    SimpleAssignmentAst *assign1 = new SimpleAssignmentAst();
    SimpleVariable varX("x");

    assign1->set_variable(varX);
	assign1->set_expr(new SimpleConstAst(2));
    assign1->set_line(1);
    assign1->set_proc(proc_first);

    proc_first->set_first_statement(assign1);

	// z = 3
	SimpleAssignmentAst *assign2 = new SimpleAssignmentAst();
    SimpleVariable varZ("z");

	assign2->set_variable(varZ);
	assign2->set_expr(new SimpleConstAst(3));
    assign2->set_line(2);
    assign2->set_proc(proc_first);

	set_next(assign1, assign2);

	// x = 0
	SimpleAssignmentAst *assign3 = new SimpleAssignmentAst();

	assign3->set_variable(varX);
	assign3->set_expr(new SimpleConstAst(0));
    assign3->set_line(3);
    assign3->set_proc(proc_second);

	proc_second->set_first_statement(assign3);

	//z = 3
	SimpleAssignmentAst *assign4 = new SimpleAssignmentAst();

	assign4->set_variable(varZ);
	assign4->set_expr(new SimpleConstAst(4));
    assign4->set_line(4);
    assign4->set_proc(proc_second);

	set_next(assign3, assign4);

	//i = 5
	SimpleAssignmentAst *assign5 = new SimpleAssignmentAst();
	SimpleVariable varI("i");

	assign5->set_variable(varI);
	assign5->set_expr(new SimpleConstAst(5));
    assign5->set_line(5);
    assign5->set_proc(proc_second);

	set_next(assign4, assign5);

	//Procedure
	std::vector<ProcAst*> procs;
    procs.push_back(proc_first);
    procs.push_back(proc_second);

    SimpleRoot root(procs.begin(), procs.end());

    SiblingSolver solver(root);

	/*
	 * Positive testing
	 */
	EXPECT_TRUE((solver.validate(ConditionPtr(new SimpleStatementCondition(assign1)), ConditionPtr(new SimpleStatementCondition(assign2)))));	//Directly right sibling
	EXPECT_TRUE((solver.validate(ConditionPtr(new SimpleStatementCondition(assign2)), ConditionPtr(new SimpleStatementCondition(assign1)))));	//Directly left sibling

	/*
	 * Negative testing
	 */
	EXPECT_FALSE((solver.validate(ConditionPtr(new SimpleStatementCondition(assign1)), ConditionPtr(new SimpleStatementCondition(assign3)))));	//Not inside the same procedure
	EXPECT_FALSE((solver.validate(ConditionPtr(new SimpleStatementCondition(assign4)), ConditionPtr(new SimpleStatementCondition(assign2)))));	//Directly right sibling

	/*
	 * Solve left and right
	 * Since solve left and solve right is symmetric, the test case and result is the same 
	 */
	ConditionSet result;

	//Left
	result.clear();
	result.insert(new SimpleStatementCondition(assign2));
	EXPECT_EQ(result, solver.solve_left(new SimpleStatementCondition(assign1)));

	//Right
	result.clear();
	result.insert(new SimpleStatementCondition(assign1));
	EXPECT_EQ(result, solver.solve_left(new SimpleStatementCondition(assign2)));

	//Middle
	result.clear();
	result.insert(new SimpleStatementCondition(assign3));
	result.insert(new SimpleStatementCondition(assign5));
	EXPECT_EQ(result, solver.solve_left(new SimpleStatementCondition(assign4)));


	//Verify that it is not inter-procedure
	result.clear();
	result.insert(new SimpleStatementCondition(assign1));
	result.insert(new SimpleStatementCondition(assign2));
	result.insert(new SimpleStatementCondition(assign3));
	EXPECT_NE(result, solver.solve_left(new SimpleStatementCondition(assign4)));
}

TEST(SiblingTest, Test_If){
	/*
	* procedure second{
	*	x=0;
	*	i=5;
	*	if x then {
	*		x = 1;}
	*	else{
	*		z = 1;}
	*	z = i;}
	*/

	SimpleProcAst *proc_second = new SimpleProcAst("second");

	//x = 0
	SimpleAssignmentAst *assign1 = new SimpleAssignmentAst();
	SimpleVariable varX("x");
	SimpleConstant const0(0);

	assign1->set_variable(varX);
	assign1->set_expr(new SimpleConstAst(0));
	assign1->set_line(1);
	assign1->set_proc(proc_second);

	proc_second->set_first_statement(assign1);

	// i = 5
	SimpleAssignmentAst *assign2 = new SimpleAssignmentAst();
    SimpleVariable varI("i");

	assign2->set_variable(varI);
	assign2->set_expr(new SimpleConstAst(5));
    assign2->set_line(2);
    assign2->set_proc(proc_second);

	set_next(assign1, assign2);

	// if x 
	SimpleIfAst *if_ = new SimpleIfAst();
	if_->set_variable(varX);
	if_->set_line(3);
	
	set_next(assign2, if_);

	// x = 1;
	SimpleAssignmentAst *if_then_assign = new SimpleAssignmentAst();
	if_then_assign->set_variable(varX);

	if_then_assign->set_expr(new SimpleConstAst(1));

    if_then_assign->set_line(4);
    if_then_assign->set_proc(proc_second);
	if_->set_then_branch(if_then_assign);

	// z = 1;
	SimpleAssignmentAst *if_else_assign = new SimpleAssignmentAst();
	SimpleVariable varZ("z");
	if_else_assign->set_variable(varZ);
	if_else_assign->set_expr(new SimpleConstAst(1));

    if_else_assign->set_line(5);
    if_else_assign->set_proc(proc_second);
	if_->set_else_branch(if_else_assign);
	
	// z = i;
	SimpleAssignmentAst *assign3 = new SimpleAssignmentAst();

	assign3->set_variable(varZ);
	assign3->set_expr(new SimpleVariableAst(varI));
    assign3->set_line(6);
    assign3->set_proc(proc_second);

	set_next(if_, assign3);

	//Solver
	SimpleRoot root(proc_second);
	SiblingSolver solver(root);

	/*
	 * Positive testing
	 */
	EXPECT_TRUE((solver.validate(ConditionPtr(new SimpleStatementCondition(assign2)), ConditionPtr(new SimpleStatementCondition(if_)))));	//Directly right sibling
	EXPECT_TRUE((solver.validate(ConditionPtr(new SimpleStatementCondition(assign1)), ConditionPtr(new SimpleStatementCondition(if_)))));	//Indirectly right sibling
	EXPECT_TRUE((solver.validate(ConditionPtr(new SimpleVariableCondition(varX)), ConditionPtr(new SimpleStatementCondition(if_then_assign)))));	//if inside 
	EXPECT_TRUE((solver.validate(ConditionPtr(new SimpleStatementCondition(if_then_assign)), ConditionPtr(new SimpleVariableCondition(varX)))));	//if inside 

	/*
	 * Negative testing
	 */
	EXPECT_FALSE((solver.validate(ConditionPtr(new SimpleStatementCondition(if_else_assign)), ConditionPtr(new SimpleStatementCondition(assign1)))));	//Not inside the same nesting level
	EXPECT_FALSE((solver.validate(ConditionPtr(new SimpleStatementCondition(assign2)), ConditionPtr(new SimpleStatementCondition(if_then_assign)))));	//Not inside same nesting level

	/*
	 * Solve left and right
	 * Since solve left and solve right is symmetric, the test case and result is the same 
	 */
	ConditionSet result;

	//1st level Left
	result.clear();
	result.insert(new SimpleStatementCondition(assign2));
	result.insert(new SimpleStatementCondition(if_));
	result.insert(new SimpleStatementCondition(assign3));
	EXPECT_EQ(result, solver.solve_left(new SimpleStatementCondition(assign1)));
	EXPECT_EQ(result, solver.solve_right(new SimpleStatementCondition(assign1)));
	
	//1st level Middle
	result.clear();
	result.insert(new SimpleStatementCondition(assign1));
	result.insert(new SimpleStatementCondition(assign2));
	result.insert(new SimpleStatementCondition(assign3));
	EXPECT_EQ(result, solver.solve_left(new SimpleStatementCondition(if_)));
	EXPECT_EQ(result, solver.solve_right(new SimpleStatementCondition(if_)));

	//1st level right
	result.clear();
	result.insert(new SimpleStatementCondition(assign1));
	result.insert(new SimpleStatementCondition(assign2));
	result.insert(new SimpleStatementCondition(if_));
	EXPECT_EQ(result, solver.solve_left(new SimpleStatementCondition(assign3)));
	EXPECT_EQ(result, solver.solve_right(new SimpleStatementCondition(assign3)));

	//Variable inside if
	result.clear();
	result.insert(new SimpleVariableCondition(varX));
	result.insert(new SimpleStatementCondition(if_else_assign));
	EXPECT_EQ(result, solver.solve_right(new SimpleStatementCondition(if_then_assign)));
}

TEST(SiblingTest, Test_While){
	/*
	* procedure first{
	*	x = x;
	*	while x{
	*		x = 0;}
	*	i = 0;
	*/

	SimpleProcAst *proc_first = new SimpleProcAst("first");

	// x = x
	SimpleVariable varX("x");
	SimpleAssignmentAst *assign1 = new SimpleAssignmentAst(1);
	assign1->set_variable(varX);
	assign1->set_expr(new SimpleVariableAst(varX));

	proc_first->set_first_statement(assign1);

	//while x
	SimpleWhileAst *while_ = new SimpleWhileAst();
	while_->set_variable(varX);
	while_->set_statement_line(2);

	set_next(assign1, while_);

	//x = 0;
	SimpleAssignmentAst *while_assign = new SimpleAssignmentAst(3);
	while_assign->set_variable(varX);
	while_assign->set_expr(new SimpleConstAst(0));
	
	while_->set_body(while_assign);

	//i = 0
	SimpleAssignmentAst *assign2 = new SimpleAssignmentAst(4);
	SimpleVariable varI("i");
	assign2->set_variable(varI);
	assign2->set_expr(new SimpleConstAst(0));

	set_next(while_, assign2);

	//Solver
	SimpleRoot root(proc_first);
	SiblingSolver solver(root);

	
	/*
	 * Positive testing
	 */
	EXPECT_TRUE((solver.validate(ConditionPtr(new SimpleStatementCondition(assign2)), ConditionPtr(new SimpleStatementCondition(while_)))));	//Left of while
	EXPECT_TRUE((solver.validate(ConditionPtr(new SimpleStatementCondition(assign1)), ConditionPtr(new SimpleStatementCondition(while_)))));	//Right of while (reverse testing)
	EXPECT_TRUE((solver.validate(ConditionPtr(new SimpleVariableCondition(varX)), ConditionPtr(new SimpleStatementCondition(while_assign)))));
}

TEST(SiblingTest, Test_Expression) {
	/*
	 * procedure first{
	 *  x = x + 7 * g + y * a * 2;
	 * }
	 */

	SimpleProcAst *proc_first = new SimpleProcAst("first");

    SimpleAssignmentAst *assign = new SimpleAssignmentAst();
    SimpleVariable varX("x");
    SimpleVariable varY("y");
	SimpleVariable varG("g");
	SimpleVariable varA("a");

	SimpleConstant const7(7);
	SimpleConstant const2(2);

	SimpleBinaryOpAst *plus_first = new SimpleBinaryOpAst();
	SimpleBinaryOpAst *plus_second = new SimpleBinaryOpAst();
	SimpleBinaryOpAst *time_first = new SimpleBinaryOpAst();
	SimpleBinaryOpAst *time_second = new SimpleBinaryOpAst();
	SimpleBinaryOpAst *time_third = new SimpleBinaryOpAst();

	//Assignment Statement
    assign->set_variable(varX);
	assign->set_expr(plus_second);
    
    assign->set_line(1);
    assign->set_proc(proc_first);

    proc_first->set_first_statement(assign);

	//First Level
	plus_first->set_op('+');
	plus_first->set_lhs(new SimpleVariableAst(varX));
	plus_first->set_rhs(plus_second);

	//Second Level
	plus_second->set_op('+');
	plus_second->set_lhs(time_first);
	plus_second->set_rhs(time_second);

	//Third Level
	time_first->set_op('*');
	time_first->set_lhs(new SimpleConstAst(7));
	time_first->set_rhs(new SimpleVariableAst(varG));

	time_second->set_op('*');
	time_second->set_lhs(new SimpleVariableAst(varY));
	time_second->set_rhs(time_third);

	//Fourth Level
	time_third->set_op('*');
	time_third->set_lhs(new SimpleVariableAst(varA));
	time_third->set_rhs(new SimpleConstAst(2));

	SimpleRoot root(proc_first);

    SiblingSolver solver(root);

	/*
	 * Positive testing
	 */
	OperatorCondition *times = new SimpleOperatorCondition('*');

	OperatorCondition *plus = new SimpleOperatorCondition('+');

	//Variable and top expression (and swap)
	EXPECT_TRUE((solver.validate(ConditionPtr(new SimpleVariableCondition(varX)), ConditionPtr(new SimpleOperatorCondition('+')))));
	EXPECT_TRUE((solver.validate(ConditionPtr(new SimpleOperatorCondition('+')), ConditionPtr(new SimpleVariableCondition(varX)))));

	//4th Level (and swap)
	EXPECT_TRUE((solver.validate(ConditionPtr(new SimpleVariableCondition(varG)), ConditionPtr(new SimpleConstantCondition(7)))));
	EXPECT_TRUE((solver.validate(ConditionPtr(new SimpleConstantCondition(7)) ,ConditionPtr(new SimpleVariableCondition(varG)))));
	EXPECT_TRUE((solver.validate(ConditionPtr(new SimpleVariableCondition(varY)), ConditionPtr(new SimpleOperatorCondition('*')))));
	EXPECT_TRUE((solver.validate(ConditionPtr(new SimpleOperatorCondition('*')), ConditionPtr(new SimpleVariableCondition(varY)))));

	/*
	 * Negative testing
	 */
	//Operators at different nesting level
	EXPECT_FALSE((solver.validate(plus, plus)));	

	//Same constant/ variable
	EXPECT_FALSE((solver.validate(ConditionPtr(new SimpleVariableCondition(varY)), ConditionPtr(new SimpleVariableCondition(varY)))));
	
	//Constant and Operators at different nesting level (and swap)
	EXPECT_FALSE((solver.validate(ConditionPtr(new SimpleConstantCondition(2)) ,ConditionPtr(new SimpleOperatorCondition('+')))));
	EXPECT_FALSE((solver.validate(ConditionPtr(new SimpleOperatorCondition('+')), ConditionPtr(new SimpleConstantCondition(2)))));	
	EXPECT_FALSE((solver.validate(ConditionPtr(new SimpleOperatorCondition('*')), ConditionPtr(new SimpleConstantCondition(2)))));	
	
	EXPECT_FALSE((solver.validate(ConditionPtr(new SimpleConstantCondition(7)) ,ConditionPtr(new SimpleOperatorCondition('+')))));
	EXPECT_FALSE((solver.validate(ConditionPtr(new SimpleOperatorCondition('*')), ConditionPtr(new SimpleConstantCondition(7)))));	

	//Variable and operators at differents nesting level
	EXPECT_FALSE((solver.validate(ConditionPtr(new SimpleOperatorCondition('+')), ConditionPtr(new SimpleVariableCondition(varY)))));

	EXPECT_FALSE((solver.validate(ConditionPtr(new SimpleVariableCondition(varG)), ConditionPtr(new SimpleOperatorCondition('*')))));
	EXPECT_FALSE((solver.validate(ConditionPtr(new SimpleOperatorCondition('+')), ConditionPtr(new SimpleVariableCondition(varG)))));

	//Variables and constant at different branches
	EXPECT_FALSE((solver.validate(ConditionPtr(new SimpleVariableCondition(varX)), ConditionPtr(new SimpleConstantCondition(2)))));
	EXPECT_FALSE((solver.validate(ConditionPtr(new SimpleConstantCondition(2)) ,ConditionPtr(new SimpleVariableCondition(varX)))));

	EXPECT_FALSE((solver.validate(ConditionPtr(new SimpleVariableCondition(varY)), ConditionPtr(new SimpleConstantCondition(7)))));
	EXPECT_FALSE((solver.validate(ConditionPtr(new SimpleConstantCondition(7)) ,ConditionPtr(new SimpleVariableCondition(varY)))));

	EXPECT_FALSE((solver.validate(ConditionPtr(new SimpleConstantCondition(7)) ,ConditionPtr(new SimpleConstantCondition(2)))));

	/*
	 * Solve left and right
	 * Since solve left and solve right is symmetric, the test case and result is the same 
	 */
	ConditionSet result;

	//1st Level
	result.clear();
	result.insert(new SimpleVariableCondition(varX));

	EXPECT_EQ(result, solver.solve_left(plus));

	//4th Level
	result.clear();
	result.insert(new SimpleConstantCondition(7));

	EXPECT_EQ(result, solver.solve_left(new SimpleVariableCondition(varG)));
	EXPECT_EQ(result, solver.solve_right(new SimpleVariableCondition(varG)));

	result.clear();
	result.insert(new SimpleVariableCondition(varG));

	EXPECT_EQ(result, solver.solve_left(new SimpleConstantCondition(7)));

	result.clear();
	result.insert(new SimpleOperatorCondition('*'));
	result.insert(new SimpleVariableCondition(varY));

	EXPECT_EQ(result, solver.solve_left(times));

	//5th Level
	result.clear();
	result.insert(new SimpleConstantCondition(2));

	EXPECT_EQ(result, solver.solve_left(new SimpleVariableCondition(varA)));

	result.clear();
	result.insert(times);

	EXPECT_EQ(result, solver.solve_left(new SimpleVariableCondition(varY)));
}
}
}
