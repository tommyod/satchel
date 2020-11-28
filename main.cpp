#include <stdio.h>
#include <ostream>
#include <iostream>
#include <cmath>
#include <array>
#include <vector>
#include <numeric>
#include <algorithm>
#include <memory>

/*Print a vector*/
template <typename T> // This line lets the function below take arbitrary vectors (int or float)
void print_vector(std::vector<T> &arr)
{
    for (auto element : arr)
    {
        std::cout << element << " ";
    }
    std::cout << std::endl;
}

/*Check if all values are strictly positive.*/
bool is_all_positive(std::vector<int> &arr)
{
    for (auto element : arr)
    {
        if (element <= 0)
        {
            return false;
        }
    }
    return true;
}

// Shared state used across all solution nodes
class Problem
{
public:
    std::vector<int> values;  // Values of items
    std::vector<int> weights; // Weights of items
    int capacity;             // Capacity of knapsack

    Problem(std::vector<int> values, std::vector<int> weights, int capacity)
    {

        // Sanitize the user inputs
        if (!is_all_positive(values))
        {
            std::cout << "ERROR: Values must be positive" << std::endl;
        }
        if (!is_all_positive(weights))
        {
            std::cout << "ERROR: Values must be positive" << std::endl;
        }
        if (values.size() != weights.size())
        {
            std::cout << "ERROR: Values and weights must be of the same size." << std::endl;
        }

        this->values = values;
        this->weights = weights;
        this->capacity = capacity;
    }
};

/**
 * Argsort(currently support ascending sort)
 * @tparam T array element type
 * @param array input array
 * @return indices w.r.t sorted array
 */
template <typename T>
std::vector<size_t> argsort(const std::vector<T> &array)
{
    // From: https://gist.github.com/HViktorTsoi/58eabb4f7c5a303ced400bcfa816f6f5

    // Create a vector of indices
    std::vector<size_t> indices(array.size());

    // Fill the indicies vector from begin to end, with 0, 1, 2, ...
    std::iota(indices.begin(), indices.end(), 0);

    // Sort the indices using the data from the array
    std::sort(indices.begin(), indices.end(),
              [&array](int left, int right) -> bool {
                  // sort indices according to corresponding array element
                  return array[left] < array[right];
              });

    return indices;
}

class Solution
{
public:
    std::shared_ptr<Problem> problem;
    std::vector<bool> solution;        // If index i in in solution
    std::vector<bool> allowed_indices; // If index i is allowed in the solution

    Solution(std::shared_ptr<Problem> problem)
    {
        // Store values on the instance
        this->problem = problem;
        build_initial_solution();
    }

    void build_initial_solution()
    {
        this->solution = std::vector<bool>(this->problem->values.size(), false);
        this->allowed_indices = std::vector<bool>(this->problem->values.size(), true);
    }

    Solution(const Solution &rhs)
    {
        problem = rhs.problem;
        solution = rhs.solution;
        allowed_indices = rhs.allowed_indices;
    }

    void set_solution(int index, bool value)
    {
        this->solution[index] = value;
    }

    void set_allowed_index(int index, bool value)
    {
        this->allowed_indices[index] = value;
    }

    int utility()
    {
        int answer = 0;
        for (size_t i = 0; i < this->problem->values.size(); i++)
        {
            if (this->solution[i])
            {
                answer += this->problem->values[i];
            }
        }

        return answer;
    }

    int weight()
    {
        int answer = 0;
        for (size_t i = 0; i < this->problem->values.size(); i++)
        {
            if (this->solution[i])
            {
                answer += this->problem->weights[i];
            }
        }

        return answer;
    }

    // Check that the solution is valid
    bool is_valid(){
        print_vector(this->problem->weights);
        print_vector(solution);
        std::cout << "weight: " << weight() << std::endl;
        std::cout << "cap: " << this->problem->capacity << std::endl;
        return weight() <= this->problem->capacity;
    }

    void print()
    {
        // Print out problem data
        std::cout << "==========================" << std::endl;
        std::cout << "Capacity: " << this->problem->capacity << std::endl;
        std::cout << "Values: ";
        print_vector(this->problem->values);
        std::cout << "Weights: ";
        print_vector(this->problem->weights);
        std::cout << "Solution: ";
        print_vector(solution);
        std::cout << "Allowed indices: ";
        print_vector(allowed_indices);
        std::cout << "Utility: " << this->utility() << std::endl;
        std::cout << "==========================" << std::endl;
    }

    /*Greedy solution.*/
    void solve_greedily()
    {
        std::cout << "== START greedy solver ===" << std::endl;
        std::vector<double> value_per_weight(problem->values.size(), 1.0);

        for (int i = 0; i < problem->values.size(); i++)
        {
            // In order to return a double, at least one of the ints must be explicitly cast to a double.
            value_per_weight[i] = -(double)problem->values[i] / (double)problem->weights[i];
        }

        std::cout << "Values over weights: ";
        print_vector(value_per_weight);

        // Sort indices with respect to the values over weights
        std::vector<size_t> indices = argsort(value_per_weight);

        std::cout << "Indices: ";
        print_vector(indices);

        int chosen_weights = 0;
        for (auto i : indices)
        {

            if (!this->allowed_indices[i])
            {
                continue;
            }
            // Not enough space to add this one
            if (this->problem->weights[i] + chosen_weights > this->problem->capacity)
            {
                continue;
            }

            this->solution[i] = true;
            chosen_weights += this->problem->weights[i];
        }

        std::cout << "Solution: ";
        print_vector(solution);
        std::cout << "==== END greedy solver ===" << std::endl;
    }


    double upper_bound(){

        double result = (double) value();

        std::vector<double> value_per_weight(problem->values.size(), 1.0);

        for (int i = 0; i < problem->values.size(); i++)
        {
            // In order to return a double, at least one of the ints must be explicitly cast to a double.
            value_per_weight[i] = -problem->values[i] / (double)problem->weights[i];
        }

        // Sort indices with respect to the values over weights
        std::vector<size_t> indices = argsort(value_per_weight);

        
        


    }

};

Solution solve_by_branching(Solution &s)
{
    std::cout << "===========================================" << std::endl;
    s.print();

    // If all allowed indices are False, then do nothing
    if (std::all_of(s.allowed_indices.begin(),
                    s.allowed_indices.end(), [](bool v) { return !v; }))
    {
        std::cout << "Bottom of recursion" << std::endl;
        return s;
    }

    for (int i = 0; i < s.problem->values.size(); i++)
    {

        // If we're not allowed to change this index, move to the next
        if (!s.allowed_indices[i])
        {
            continue;
        }

        // Left branching
        Solution s_left = Solution(s);
        s_left.set_allowed_index(i, false); // Cannot change in the future
        s_left.set_solution(i, 0);

        // Right branching
        Solution s_right = Solution(s);
        s_right.set_allowed_index(i, false); // Cannot change in the future

        if (s_right.weight() + s_right.problem->weights[i] <= s_right.problem->capacity)
        {
            s_right.set_solution(i, 1);
        }

        Solution s_left_solved = solve_by_branching(s_left);
        Solution s_right_solved = solve_by_branching(s_right);

        if (s_left_solved.utility() > s_right_solved.utility())
        {
            std::cout << "On index " << i << " left was better." << std::endl;
            return s_left_solved;
        }
        else
        {
            std::cout << "On index " << i << " right was better." << std::endl;
            return s_right_solved;
        }
    }
    return s;
}

int main()
{
    std::cout << "Welcome to Tommy's 1/0 knapsack solver." << std::endl;

    // These variables never change
    std::vector<int> values = {1, 2, 3, 4, 5};
    std::vector<int> weights = {1, 9, 4, 2, 8};
    int capacity = 10;

    auto problem = Problem(values, weights, capacity);

    auto solution = Solution(std::make_shared<Problem>(problem));
    solution.set_solution(0, true);
    solution.print();

    // These variables never change
    values = {6, 5, 5, 10, 5, 5, 5};
    weights = {4, 3, 3, 4, 4, 5, 5};
    capacity = 4;

    problem = Problem(values, weights, capacity);

    solution = Solution(std::make_shared<Problem>(problem));
    solution.print();
    solution = solve_by_branching(solution);
    if (!solution.is_valid()){
        std::cout << "INVALID SOLUTION" << std::endl;
    }
    solution.print();
}
