///////////////////////////////////////////////////////////////////////////////
// knapsack_test.cc
//
// Unit tests for knapsack.hh
//
///////////////////////////////////////////////////////////////////////////////


#include <cassert>
#include <sstream>

#include "rubrictest.hpp"

#include "knapsack.hpp"

int main() {
    
    Rubric rubric;
    
    FoodVector empty_foods;
    
    FoodVector trivial_foods;
    trivial_foods.push_back(std::shared_ptr<Food>(new Food("banana", "1 each", 20, 100, 1)));
    trivial_foods.push_back(std::shared_ptr<Food>(new Food("hotdog", "1 piece", 25, 150, 5)));
    
    auto all_foods = load_usda_abbrev("ABBREV.txt");
    assert( all_foods );
    
    auto filtered_foods = filter_food_vector(*all_foods, 1, 2500, all_foods->size());
    
    rubric.criterion("load_usda_abbrev still works", 2,
                     [&]() {
                         TEST_TRUE("non-null", all_foods);
                         TEST_EQUAL("size", 8490, all_foods->size());
                     });
    
    rubric.criterion("filter_food_vector still works", 2,
                     [&]() {
                         auto three = filter_food_vector(*all_foods, 1, 2000, 3),
                         ten = filter_food_vector(*all_foods, 1, 2000, 10);
                         TEST_TRUE("non-null", three);
                         TEST_TRUE("non-null", ten);
                         TEST_EQUAL("total_size", 3, three->size());
                         TEST_EQUAL("total_size", 10, ten->size());
                         TEST_EQUAL("contents", "BUTTER,WITH SALT", (*ten)[0]->description());
                         TEST_EQUAL("contents", "CHEESE,CHESHIRE", (*ten)[9]->description());
                         for (int i = 0; i < 3; i++) {
                             TEST_EQUAL("contents", (*three)[i]->description(), (*ten)[i]->description());
                         }
                     });
    
    
    rubric.criterion("knapsack_max_protein trivial inputs", 4,
                     [&]() {
                         auto soln = knapsack_max_protein(empty_foods, 99);
                         
                         cout << "SOLUTION CREATED SUCCESSFULLY\n";
                         TEST_TRUE("empty foods", soln->empty());
                         
                         cout << "TEST EMPTY 99 PASSED\n\n";
                         
                         soln = knapsack_max_protein(trivial_foods, 0);
                         TEST_TRUE("non-null", soln);
                         TEST_TRUE("empty solution", soln->empty());
                         
                         cout << "NON_NULL 0 PASSED\n\n";
                         
                         soln = knapsack_max_protein(trivial_foods, 99);
                         TEST_TRUE("non-null", soln);
                         TEST_TRUE("empty solution", soln->empty());
                         
                         cout << "NON_NULL 99 PASSED\n\n";
                         
                         soln = knapsack_max_protein(trivial_foods, 100);
                         TEST_TRUE("non-null", soln);
                         TEST_EQUAL("banana only", 1, soln->size());
                         TEST_EQUAL("banana only", "banana", (*soln)[0]->description());
                         
                         cout << "TRIVIAL 100 PASSED\n\n";
                         
                         soln = knapsack_max_protein(trivial_foods, 150);
                         TEST_TRUE("non-null", soln);
                         TEST_EQUAL("hotdog only", 1, soln->size());
                         TEST_EQUAL("hotdog only", "hotdog", (*soln)[0]->description());
                         
                         cout << "TRIVIAL 150 PASSED\n\n";
                         
                         soln = knapsack_max_protein(trivial_foods, 250);
                         TEST_TRUE("non-null", soln);
                         TEST_EQUAL("hotdog and banana", 2, soln->size());
                         
                         cout << "TRIVIAL 250 PASSED\n\n";
                     });
    
    rubric.criterion("knapsack_max_protein non-trivial inputs", 6,
                     
                     [&]() {
                         TEST_TRUE("filtered foods exist", filtered_foods);
                         
                         auto soln200 = knapsack_max_protein(*filtered_foods, 200);
                         auto soln500 = knapsack_max_protein(*filtered_foods, 500);
                         
                         TEST_TRUE("non-null", soln200);
                         TEST_TRUE("non-null", soln500);
                         
                         TEST_FALSE("non-empty", soln200->empty());
                         TEST_FALSE("non-empty", soln500->empty());
                         
                         int kcal200, protein200, kcal500, protein500;
                         sum_food_vector(kcal200, protein200, *soln200);
                         sum_food_vector(kcal500, protein500, *soln500);
                         
                         TEST_EQUAL("200 kcal solution", 49, protein200);
                         TEST_EQUAL("500 kcal solution", 132, protein500);
                         
                     });
    
    return rubric.run();
}

