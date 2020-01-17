///////////////////////////////////////////////////////////////////////////////
// maxprotein.hh
//
// Compute the set of foods that maximizes protein, within a calorie budget,
// with the greedy method or exhaustive search.
//
///////////////////////////////////////////////////////////////////////////////


#pragma once

#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

// One food item in the USDA database.
class Food {
private:
    // Human-readable description of the food, e.g. "all-purpose wheat
    // flour". Must be non-empty.
    std::string _description;
    
    // Human-readable description of the amount of the food in one
    // sample, e.g. "1 cup". Must be non-empty.
    std::string _amount;
    
    // Number of grams in one sample; must be non-negative.
    int _amount_g;
    
    // Energy, in units of kilocalories (commonly called "calories"), in
    // one sample; must be non-negative.
    int _kcal;
    
    // Number of grams of protein in one sample; most be non-negative.
    int _protein_g;
    
public:
    Food(const std::string& description,
         const std::string& amount,
         int amount_g,
         int kcal,
         int protein_g)
    : _description(description),
    _amount(amount),
    _amount_g(amount_g),
    _kcal(kcal),
    _protein_g(protein_g) {
        
        assert(!description.empty());
        assert(!amount.empty());
        assert(amount_g >= 0);
        assert(kcal >= 0);
        assert(protein_g >= 0);
    }
    
    const std::string& description() const { return _description; }
    const std::string& amount() const { return _amount; }
    int amount_g() const { return _amount_g; }
    int kcal() const { return _kcal; }
    int protein_g() const { return _protein_g; }
    
};

// Alias for a vector of shared pointers to Food objects.
using FoodVector = std::vector<std::shared_ptr<Food>>;

// Load all the valid foods from a USDA database in their ABBREV
// format. Foods that are missing fields such as the amount string are
// skipped. Returns nullptr on I/O error.
std::unique_ptr<FoodVector> load_usda_abbrev(const std::string& path) {
    
    std::unique_ptr<FoodVector> failure(nullptr);
    
    std::ifstream f(path);
    if (!f) {
        return failure;
    }
    
    std::unique_ptr<FoodVector> result(new FoodVector);
    
    for (std::string line; std::getline(f, line); ) {
        
        std::vector<std::string> fields;
        std::stringstream ss(line);
        for (std::string field; std::getline(ss, field, '^'); ) {
            fields.push_back(field);
        }
        
        if (fields.size() != 53) {
            return failure;
        }
        
        std::string descr_field = fields[1],
        kcal_field = fields[3],
        protein_g_field = fields[4],
        amount_g_field = fields[48],
        amount_field = fields[49];
        
        auto remove_tildes = [](std::string& output,
                                const std::string& field) {
            if ((field.size() < 3) ||
                (field.front() != '~') ||
                (field.back() != '~')) {
                return false;
            } else {
                output.assign(field.begin() + 1, field.end() - 1);
                return true;
            }
        };
        
        auto parse_mil = [](int& output, const std::string& field) {
            std::stringstream ss(field);
            double floating;
            ss >> floating;
            if ( ! ss ) {
                return false;
            } else {
                output = lround(floating);
                return true;
            }
        };
        
        std::string description, amount;
        int amount_g, kcal, protein_g;
        if ( remove_tildes(description, descr_field) &&
            remove_tildes(amount, amount_field) &&
            parse_mil(amount_g, amount_g_field) &&
            parse_mil(kcal, kcal_field) &&
            parse_mil(protein_g, protein_g_field) ) {
            result->push_back(std::shared_ptr<Food>(new Food(description,
                                                             amount,
                                                             amount_g,
                                                             kcal,
                                                             protein_g)));
        }
    }
    
    f.close();
    
    return result;
}

// Convenience function to compute the total kilocalories and protein
// in a FoodVector. Those values are returned through the
// first two pass-by-reference arguments.
void sum_food_vector(int& total_kcal,
                     int& total_protein_g,
                     const FoodVector& foods) {
    total_kcal = total_protein_g = 0;
    for (auto& food : foods) {
        total_kcal += food->kcal();
        total_protein_g += food->protein_g();
    }
}


// Convenience function to print out each food in a FoodVector,
// followed by the total kilocalories and protein in it.
void print_food_vector(const FoodVector& foods) {
    for (auto& food : foods) {
        std::cout << food->description()
        << " (100 g where each " << food->amount()
        << " is " << food->amount_g() << " g)"
        << " kcal=" << food->kcal()
        << " protein=" << food->protein_g() << " g"
        << std::endl;
    }
    
    int total_kcal, total_protein_g;
    sum_food_vector(total_kcal, total_protein_g, foods);
    std::cout << "total kcal=" << total_kcal
    << " total_protein=" << total_protein_g << " g"
    << std::endl;
}

// Filter the vector source, i.e. create and return a new FoodVector
// containing the subset of the foods in source that match given
// criteria. This is intended to 1) filter out foods with zero
// calories that are irrelevant to our optimization, and 2) limit the
// size of inputs to the exhaustive search algorithm since it will
// probably be slow. Each food that is included must have at least
// min_kcal kilocalories and at most max_kcal kilocalories. In
// addition, the the vector includes only the first total_size foods
// that match these criteria.
std::unique_ptr<FoodVector> filter_food_vector(const FoodVector& source,
                                               int min_kcal,
                                               int max_kcal,
                                               int total_size) {
    
    //cleaned FoodVector
    FoodVector cleaned;
    
    // pre-filter Foods
    for(unsigned long i = 0; i < source.size(); i++) {
        if(((source[i]->kcal() > min_kcal && source[i]->kcal() < max_kcal) && source[i]->protein_g() >= 0) && cleaned.size() < total_size) {
            cleaned.push_back(source[i]);
        }
    }
    
    //generate a return unique_ptr<FoodVector> type of best
    std::unique_ptr<FoodVector> ptr_best(new FoodVector(cleaned));
    
    return ptr_best;
}

// Compute the optimal set of foods with a greedy
// algorithm. Specifically, among the food items that fit within a
// total_kcal calorie budget, choose the food whose protein is
// greatest. Repeat until no more foods can be chosen, either because
// we've run out of foods, or run out of calories.
std::unique_ptr<FoodVector> greedy_max_protein(const FoodVector& foods, int total_kcal) {
    
    std::unique_ptr<FoodVector> result(new FoodVector);
    
    //create copy of foods FoodVector
    std::unique_ptr<FoodVector> foods_cpy(new FoodVector(foods));
    
    //initialize variables
    int max_index = 0;
    int max_protein = 0;
    int calories = 0;
    int result_cal = 0;
    
    while(!(*foods_cpy).empty()) {
        //Set the highest protein food to first food in foods_cpy initially
        max_protein = (*foods_cpy).at(0)->protein_g();
        //Set calories to first food in foods_cpy
        calories = (*foods_cpy).at(0)->kcal();
        max_index = 0;
        
        //for each food
        for(int j = 0; j < (*foods_cpy).size(); j++) {
            //if the food's protein value is greater than the protein value in max_protein
            if((*foods_cpy).at(j)->protein_g() > max_protein) {
                //set the new max_protein value
                max_protein = (*foods_cpy).at(j)->protein_g();
                //record the index as well
                max_index = j;
                //set the calories of the food as well
                calories = (*foods_cpy).at(j)->kcal();
            }
        }
        
        if(result_cal + calories <= total_kcal) {
            
            result->push_back(std::shared_ptr<Food>(new Food((*foods_cpy).at(max_index)->description(),
                                                             (*foods_cpy).at(max_index)->amount(),
                                                             (*foods_cpy).at(max_index)->amount_g(),
                                                             (*foods_cpy).at(max_index)->kcal(),
                                                             (*foods_cpy).at(max_index)->protein_g())));
            //add the calories of the food to the accumulator
            result_cal += calories;
        }
        //Remove that food from foods_cpy FoodVector
        (*foods_cpy).erase((*foods_cpy).begin() + max_index);
    }
    return result;
}

// Compute the optimal set of foods with an exhaustive search
// algorithm. Specifically, among all subsets of foods, return the
// subset whose calories fit within the total_kcal budget, and whose
// total protein is greatest. To avoid overflow, the size of the foods
// vector must be less than 64.
std::unique_ptr<FoodVector> exhaustive_max_protein(const FoodVector& foods, int total_kcal) {
    const int n = foods.size();
    assert(n < 64);
    
    uint64_t MAX_POSSIBLE_SUBSETS = (long long)pow(2.0, n);      //define the max possible number of subsets for the outer loop.
    
    //Return best vector containing subset of foods that has the highest total of protein
    FoodVector best;
    
    //Cycle through every possible subset
    for(uint64_t subset = 0; subset < MAX_POSSIBLE_SUBSETS; subset++) {
        
        //Create empty candidate vector for every iteration of outer loop (each subset possible)
        FoodVector candidate;
        int total_calories_can = 0;
        int total_protein_can = 0;
        int total_calories_best = 0;
        int total_protein_best = 0;
        
        //For every bit that makes up the 64 bit binary representation of the outer loop int counter i value
        for(uint64_t shift_num = 0; shift_num < n; shift_num++) {
            /*Shift right for every bit (in binary representation of outer loop int counter i) and check if the food element is present in the subset (represented by the binary value of the outer loop's current iteration counter variable i).*/
            if(((subset >> shift_num) & 1) == 1) {
                candidate.push_back(foods[shift_num]);
            }
        }
        
        //Populate outer loop total_calories and total_protein with sums from candidate vector (subset)
        //the variable arguments are passed by reference so they are updated within the function call.
        sum_food_vector(total_calories_can, total_protein_can, candidate);
        
        //Find the same as above for best
        sum_food_vector(total_calories_best, total_protein_best, best);
        
        /*If total_calories of current iteration candidate (subset) < the positive calorie budget "C" which is total_kcal*/
        if(total_calories_can <= total_kcal) {
            if(best.size() == 0 || (total_protein_can > total_protein_best)) {
                //reassign new best
                best = candidate;
            }
        }
    }
    //Generate a return unique_ptr<FoodVector> type of best
    std::unique_ptr<FoodVector> ptr_best(new FoodVector(best));
    
    return ptr_best;
}

