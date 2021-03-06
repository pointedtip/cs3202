cmake_minimum_required (VERSION 2.6)

project (Simple3201 CXX)

set(CMAKE_BUILD_TYPE Debug)

if(UNIX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=gnu++0x -pthread")
endif()

include_directories("${PROJECT_SOURCE_DIR}" , "${PROJECT_SOURCE_DIR}/test")

set(SOURCE
  simple/ast.cpp 
  simple/condition_set.cpp 
  simple/spa.cpp 
  simple/tuple.cpp 
  simple/next_solver.cpp 
  simple/query.cpp 
  simple/util/ast_utils.cpp 
  simple/util/condition_utils.cpp 
  simple/util/expr_util.cpp
  simple/util/query_utils.cpp 
  simple/util/term_utils.cpp 
  impl/linker.cpp 
  impl/predicate.cpp 
  impl/processor.cpp 
  impl/selector.cpp 
  impl/solver_table.cpp 
  impl/predicate_table.cpp 
  impl/solvers/affects.cpp 
  impl/solvers/iaffects.cpp 
  impl/solvers/assign.cpp 
  impl/solvers/direct_uses.cpp 
  impl/solvers/equal.cpp 
  impl/solvers/expr.cpp 
  impl/solvers/iexpr.cpp 
  impl/solvers/follows.cpp 
  impl/solvers/ifollows.cpp 
  impl/solvers/parent.cpp 
  impl/solvers/iparent.cpp 
  impl/solvers/modifies.cpp 
  impl/solvers/next.cpp 
  impl/solvers/next_bip.cpp 
  impl/solvers/next_cached.cpp 
  impl/solvers/inext.cpp 
  impl/solvers/contains.cpp
  impl/solvers/sibling.cpp
  impl/solvers/call.cpp 
  impl/solvers/icall.cpp 
  impl/solvers/uses.cpp 
  impl/parser/token.cpp 
  impl/parser/parser.cpp 
  impl/parser/expr_parser.cpp 
  impl/parser/pql_parser.cpp 
  spa/affects_star.cpp
  spa/affects.cpp
  spa/ast.cpp
  spa/calls.cpp
  spa/calls_star.cpp
  spa/follows_star.cpp
  spa/follows.cpp
  spa/modifies.cpp
  spa/next_star.cpp
  spa/next.cpp
  spa/parent_star.cpp
  spa/parent.cpp
  spa/pkb.cpp
  spa/statement.cpp
  spa/uses.cpp
  spa/var_table.cpp
  )

add_library(spa STATIC ${SOURCE})

add_executable(main 
  impl/command_line.cpp 
  )

target_link_libraries(main spa)

add_executable(batch 
  impl/batch.cpp 
  )
target_link_libraries(batch spa)

add_executable(unit_tests 
  test/test_main.cpp
  test/gtest/gtest-all.cpp
  test/test_ast.cpp 
  test/test_solver.cpp 
  test/test_call.cpp 
  test/test_icall.cpp 
  test/test_expr.cpp 
  test/test_follows.cpp 
  test/test_ifollows.cpp 
  test/test_parent.cpp 
  test/test_iparent.cpp 
  test/test_modifies.cpp 
  test/test_uses.cpp 
  test/test_condition.cpp 
  test/test_next.cpp 
  test/test_inext.cpp 
  test/test_frontend.cpp 
  test/test_linker.cpp 
  test/test_parser.cpp 
  test/test_pql_parser.cpp 
  test/test_predicate.cpp 
  test/test_processor.cpp 
  test/test_query.cpp 
  test/test_tokenizer.cpp 
  test/test_types.cpp 
  test/spa/test_uses.cpp
  )

target_link_libraries(unit_tests spa)

add_executable(system_test
  test/test_main.cpp
  test/gtest/gtest-all.cpp
  test/system-test/system_modifies.cpp
  test/system-test/system_basic.cpp
)

target_link_libraries(system_test spa)
