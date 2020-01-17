///////////////////////////////////////////////////////////////////////////////
// maxprotein_timing.cc
//
// Example code showing how to run each algorithm while measuring
// elapsed times precisely. You should modify this program to gather
// all of your experimental data.
//
///////////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <cassert>
#include <climits>
#include <iostream>
#include <vector>

#include "knapsack.hpp"
#include "timer.hpp"

using namespace std;

void print_bar() {
  cout << string(79, '-') << endl;
}

int main() {

  const int n_request = 10000,
    total_kcal_rw1 = 1500,
    total_kcal_rw2 = 2000,
    total_kcal_rw3 = 2500,
    total_kcal_rw4 = 4209;

  auto all_foods = load_usda_abbrev("ABBREV.txt");
  assert( all_foods );

  int n = std::min(n_request, int(all_foods->size()));
  assert(n <= n_request);

  auto n_foods = filter_food_vector(*all_foods, 0, INT_MAX, n);
  assert( n_foods );
  assert( int(n_foods->size()) <= n );

double elapsed;
Timer timer;
    
 /************************************************************
  *** Varying n value tests, W = 1000
  ************************************************************/
    int n_values_w1000;
    const int smaller_total_kcal = 3000;
    
    ofstream n_W1000_results;
    n_W1000_results.open("n_W1000_results.txt");
    
    for(int counter = 100; counter < 8000; counter+=200) {
        n_values_w1000 = std::min(n_request, counter);
        assert(n_values_w1000 <= n_request);
        
        auto smaller_n_foods = filter_food_vector(*all_foods, 0, INT_MAX, n_values_w1000);
        assert( smaller_n_foods );
        assert( int(smaller_n_foods->size()) <= n_values_w1000 );
        
        print_bar();
        cout << "n_request=" << n_request
        << ", n=" << n_values_w1000
        << ", total_kcal=" << smaller_total_kcal << endl;
        print_bar();
        
        
        print_bar();
        timer.reset();
        auto knapsack_solution_smaller = knapsack_max_protein(*smaller_n_foods, smaller_total_kcal);
        elapsed = timer.elapsed();
        assert(knapsack_solution_smaller);
        cout << "knapsack solution:" << endl;
        print_food_vector(*knapsack_solution_smaller);
        
        print_bar();
        cout << "elapsed time=" << elapsed << " seconds" << endl << endl;
        n_W1000_results << n_values_w1000 << "\t" << elapsed << "\n";
        print_bar();
    }
    n_W1000_results.close();
    
    

/************************************************************
 *** n = 8490, W = 1500
 ***********************************************************/
    
print_bar();
cout << "n_request=" << n_request
<< ", n=" << n
<< ", total_kcal=" << total_kcal_rw1 << endl;
    
print_bar();
timer.reset();
auto knapsack_solution_rw1 = knapsack_max_protein(*n_foods, total_kcal_rw1);
elapsed = timer.elapsed();
assert(knapsack_solution_rw1);
cout << "knapsack solution:" << endl;
print_food_vector(*knapsack_solution_rw1);
    
print_bar();
cout << "elapsed time=" << elapsed << " seconds" << endl << endl;
    
print_bar();

    
/************************************************************
 *** n = 8490, W = 2000
 ***********************************************************/
    
print_bar();
cout << "n_request=" << n_request
<< ", n=" << n
<< ", total_kcal=" << total_kcal_rw2 << endl;
    
print_bar();
timer.reset();
auto knapsack_solution_rw2 = knapsack_max_protein(*n_foods, total_kcal_rw2);
elapsed = timer.elapsed();
assert(knapsack_solution_rw2);
cout << "knapsack solution:" << endl;
print_food_vector(*knapsack_solution_rw2);
    
print_bar();
cout << "elapsed time=" << elapsed << " seconds" << endl << endl;
    
print_bar();

    
    
/************************************************************
 *** n = 8490, W = 2500
 ***********************************************************/
    
print_bar();
cout << "n_request=" << n_request
<< ", n=" << n
<< ", total_kcal=" << total_kcal_rw3 << endl;
    
print_bar();
timer.reset();
auto knapsack_solution_rw3 = knapsack_max_protein(*n_foods, total_kcal_rw3);
elapsed = timer.elapsed();
assert(knapsack_solution_rw3);
cout << "knapsack solution:" << endl;
print_food_vector(*knapsack_solution_rw3);
    
print_bar();
cout << "elapsed time=" << elapsed << " seconds" << endl << endl;
    
print_bar();

    
/************************************************************
 *** n = 8490, W = 4209
 ***********************************************************/
  
  print_bar();
  cout << "n_request=" << n_request
       << ", n=" << n
       << ", total_kcal=" << total_kcal_rw4 << endl;

  print_bar();
  timer.reset();
  auto knapsack_solution_rw4 = knapsack_max_protein(*n_foods, total_kcal_rw4);
  elapsed = timer.elapsed();
  assert(knapsack_solution_rw4);
  cout << "knapsack solution:" << endl;
  print_food_vector(*knapsack_solution_rw4);

  print_bar();
  cout << "elapsed time=" << elapsed << " seconds" << endl << endl;

  print_bar();

  return 0;
}

