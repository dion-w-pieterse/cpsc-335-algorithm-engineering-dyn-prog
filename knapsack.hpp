///////////////////////////////////////////////////////////////////////////////
// knapsack.hpp
//
// Solve the maximum-protein-diet problem described in maxprotein.hh
// using dynamic programming.
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <memory>
#include <vector>
#include "maxprotein.hh"

using namespace std;

std::unique_ptr<FoodVector> knapsack_max_protein(const FoodVector& all_foods,
                                                 int total_kcal) {
    
    assert(total_kcal >= 0);
    
    unsigned long n = all_foods.size();
    
    //new matrix with n+1 rows and W columns
    FoodVector** A;
    
    //allocate memory for the array of pointers for each row
    A = new FoodVector*[n+1];
    
    //allocate memory for each row of the 2d array
    for(unsigned long i = 0; i != (n+1); ++i) {
        A[i] = new FoodVector[total_kcal+1];
    }
    
    //base case
    for(int j = 0; j <= total_kcal; j++) {
        (A[0][j]) = {};
    }
    
    //inductive case
    for(unsigned long i = 1; i <= n; i++) {
        //I[i-1] is now available to be used
        shared_ptr<Food> this_item = all_foods[i-1];
        
        for(int j = 0; j <= total_kcal; j++) {
            
            //old solution that definetely does not include this item
            FoodVector dont_use = A[i-1][j];
            
            //do we even have the budget for this item?
            if((*this_item).kcal() > j) {
                //no this item is too heavy, default to the previously-known solution
                A[i][j] = A[i-1][j];
                
            }
            else {
                //build a vector including this_item, and additional items that fit within j
                unsigned long pre_exist_soln = j - all_foods[i-1]->kcal();
                FoodVector do_use = A[i-1][pre_exist_soln];
                
                do_use.push_back(all_foods[i-1]);
                //prefer whichever candidate has a greater value
                int dont_use_protein = 0;
                int do_use_protein = 0;
                int dont_use_kcal = 0;
                int do_use_kcal = 0;
                
                sum_food_vector(dont_use_kcal, dont_use_protein, dont_use);
                sum_food_vector(do_use_kcal, do_use_protein, do_use);
                
                if(dont_use_protein >= do_use_protein) {
                    //picking previous solution
                    A[i][j] = dont_use;
                }
                else {
                    //picking new solution
                    A[i][j] = do_use;
                }
            }
        }
    }
    
    FoodVector soln = A[n][total_kcal];
    
    //destroy the dynamically allocated 2d array
    for(unsigned long i = 0; i < (n+1); ++i) {
        delete [] A[i];
    }
    delete [] A;
    
    //create return value
    std::unique_ptr<FoodVector> solution(new FoodVector(soln));
    
    return solution;
}
