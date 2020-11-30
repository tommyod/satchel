// C++ Program to implement 0/1
// knapsack using LC Branch and Bound

//#include <bits/stdc++.h>
#include <vector>
#include <queue>
#include <algorithm>
#include <iostream>
#include <climits>
#include <cfloat>
#include <utility>
#include <cmath>
// using namespace std;


int VERBOSITY = 99;

/*Print a vector*/
template <typename T>
void print_vector(std::vector<T> &vector)
{
    for (auto element : vector)
    {
        std::cout << element << " ";
    }
    std::cout << std::endl;
}

class Item
{
public:
    double weight;
    double value;
    int idx;

    Item(double weight, double value, int idx)
    {
        // Store values on the instance
        this->weight = weight;
        this->value = value;
        this->idx = idx;
    }

    void print()
    {
        std::cout << "Item(weight=" << weight << ", value="
                  << value << ", idx=" << idx << ")" << std::endl;
    }
};

class Node
{
public:
    double upper_bound;  // Upper Bound: Best case
    double lower_bound;  // Lower Bound: Worst case
    int depth;           // current.depth of the node in the decision tree
    bool flag;           // If the curren item in selected or not
    double total_value;  // Sum of the values of the items included
    double total_weight; // Sum of the weights of the items included

    void print()
    {
        std::cout << "Node(upper_bound=" << upper_bound
                  << ", lower_bound=" << lower_bound
                  << ", depth=" << depth
                  << ", flag=" << flag
                  << ", total_value=" << total_value
                  << ", total_weight=" << total_weight
                  << ")" << std::endl;
    }
};

// Function to calculate upper bound (includes fractional part of the items)
// Function to calculate lower bound (no fractional part of the items)
std::pair<double, double> bound(double capacity,
                                double total_value,
                                double total_weight,
                                int start_index,
                                std::vector<Item> &items)
{
    double upper_bound = total_value;
    double lower_bound = total_value;

    // std::cout << "==== bounds ====" << std::endl;
    // std::cout << "upper_bound: " << upper_bound << std::endl;
    // std::cout << "lower_bound: " << lower_bound << std::endl;

    double weight = total_weight;
    for (int i = start_index; i < items.size(); i++)
    {

        // items[i].print();
        // std::cout << weight << "  " << items[i].weight << "  " << capacity << std::endl;

        if (weight + items[i].weight <= capacity)
        {
            weight += items[i].weight;
            upper_bound += items[i].value;
            lower_bound += items[i].value;
        }
        else
        {
            // std::cout << weight << "  " << items[i].weight << "  " << capacity << std::endl;
            // std::cout << "capacity:" << capacity << std::endl;
            // std::cout << "weight[i]:" << items[i].weight << std::endl;
            // std::cout << "(capacity - weight):" << (capacity - weight) << std::endl;
            // std::cout << "(items[i].value / items[i].weight):" << (items[i].value / items[i].weight) << std::endl;
            upper_bound += (capacity - weight) * (items[i].value / items[i].weight);
            break;
        }
    }
    // std::cout << "lower_bound: " << lower_bound << "\nupper_bound: " << upper_bound << std::endl;
    return std::make_pair(lower_bound, upper_bound);
}

// Comparison used in priority queue
class Compare
{
public:
    bool operator()(Node &a, Node &b)
    {
        return a.upper_bound < b.upper_bound;
    }
};

std::vector<bool> knapsack(std::vector<Item> &items, double capacity)
{

    // Sort the items based on efficiency, e.g., value/weight ratio
    // The most efficient items are placed first
    sort(items.begin(), items.end(),
         [&](Item &a, Item &b) {
             return (a.value / a.weight) > (b.value / b.weight);
         });

    for (auto var : items)
    {
        var.print();
    }

    // min_lb -> Minimum lower bound of all the nodes explored
    // final_lb -> Minimum lower bound of all the paths that reached the final current.depth
    double max_lower_bound_partial = 0;
    double best_objective_value = 0;
    int nodes_explored = 0;
    int nodes_pruned = 0;
    int nodes_remaining = (int) std::pow(2.0, items.size());

    // curr_path -> Boolean array to store at every index if the element is included or not
    // final_path -> Boolean array to store the result of selection array when it reached the last current.depth
    std::vector<bool> current_solution = std::vector<bool>(items.size(), false);
    std::vector<bool> final_solution = std::vector<bool>(items.size(), false);

    // Priority queue to store the nodes based on lower bounds
    // https://en.cppreference.com/w/cpp/container/priority_queue
    std::priority_queue<Node, std::vector<Node>, Compare> priority_queue;

    Node current, left, right;
    current.lower_bound = current.upper_bound = current.total_weight = 0;
    current.total_value = current.depth = current.flag = 0;

    // Insert a dummy node
    priority_queue.push(current);

    while (!priority_queue.empty())
    {

        std::cout << "===================================" << std::endl;

        // Get the top item and pop it off
        current = priority_queue.top();
        priority_queue.pop();

        if (VERBOSITY >= 3){
            std::cout << "====== POPPED FROM PRIORITY QUEUE =====" << std::endl;
            current.print();
            std::cout << "Maximal lower bound (partial solution): " << max_lower_bound_partial << std::endl;
            std::cout << "Maximal lower bound (full solution): " << best_objective_value << std::endl;
        }

        // If the upper bound on the node is lower than the maximal upper bound on 
        // partial or full solutions, do not proceed.
        if (current.upper_bound < std::min(max_lower_bound_partial, best_objective_value))
        {
            int pruned_now = (int) std::pow(2.0, items.size() - current.depth);
            nodes_pruned -= pruned_now;
            if (VERBOSITY >= 2){
                std::cout << "Solutions pruned (now): " << pruned_now << std::endl;
                std::cout << "Solutions pruned (total): " << nodes_pruned << std::endl;
            }
            continue;
        }

        // update the path
        if (current.depth != 0)
        {
            nodes_explored += 1;
            current_solution[current.depth - 1] = current.flag;
        }

        // Reached the bottom of the tree
        if (current.depth == items.size())
        {
            std::cout << "Reached the bottom." << std::endl;
            // We found a new best solution
            if (current.lower_bound > best_objective_value)
            {
                for (int i = 0; i < items.size(); i++)
                {
                    final_solution[items[i].idx] = current_solution[i];
                }
            }

            // Update the lower bound
            best_objective_value = current.lower_bound;

            if (VERBOSITY >= 2){
                std::cout << "Found new optimal full solution. Objective: " << best_objective_value << std::endl;
            }
            continue;
        }


        // ==============================================================================
        // ===== RIGHT NODE - Includes the current item =================================
        // ==============================================================================
        if (current.total_weight + items[current.depth].weight <= capacity)
        {
            std::pair<double, double> lower_upper_bounds = bound(capacity,
                                                                 current.total_value + items[current.depth].value,
                                                                 current.total_weight + items[current.depth].weight,
                                                                 current.depth + 1,
                                                                 items);

            right.upper_bound = lower_upper_bounds.second;
            right.lower_bound = lower_upper_bounds.first;

            right.depth = current.depth + 1;
            right.flag = true;
            right.total_value = current.total_value + items[current.depth].value;
            right.total_weight = current.total_weight + items[current.depth].weight;

            if (right.upper_bound >= max_lower_bound_partial)
            {
                std::cout << "Added right node to priority queue." << std::endl;
                priority_queue.push(right);
                max_lower_bound_partial = std::max(max_lower_bound_partial, right.lower_bound);
            }
            
        }

        // ==============================================================================
        // ===== LEFT NODE - Excludes the current item ==================================
        // ==============================================================================
        std::pair<double, double> lower_upper_bounds = bound(capacity,
                                                             current.total_value,
                                                             current.total_weight,
                                                             current.depth + 1,
                                                             items);

        left.upper_bound = lower_upper_bounds.second;
        left.lower_bound = lower_upper_bounds.first;
        
        left.depth = current.depth + 1;
        left.flag = false;
        left.total_value = current.total_value;
        left.total_weight = current.total_weight;

        if (left.upper_bound >= max_lower_bound_partial)
        {
            std::cout << "Added left node to priority queue." << std::endl;
            priority_queue.push(left);
            max_lower_bound_partial = std::max(max_lower_bound_partial, left.lower_bound);
        }
    }

    // Prepare the solution and return it
    // std::cout << "Items in the knapsack : \n";
    // print_vector(final_solution);

    // std::cout << "\n";
    std::cout << "Maximum profit is : " << (best_objective_value) << "\n";
    print_vector(final_solution);
    return final_solution;
}

// Driver Code
int main()
{

    std::vector<double> values;
    std::vector<double> weights;

    // this code
    // 100 items: 0.654 seconds
    // 1000 items: 0.734 seconds
    // 10000 items: 11.089 seconds
    // 50000 items: 200.795 seconds

    // google OR tools
    // 1000 items: 0.006114335 seconds
    // 10000 items: 0.3559 seconds
    // 50000 items: 10.606341 seconds
    // 100000 items: 52.735 seconds

    // Create a (possibly) large, random example
    for (size_t i = 0; i < 10; i++)
    {
        double r = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
        values.push_back(r);
        r = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
        weights.push_back(r);
    }
    double capacity = weights.size() * 0.2;

    // Create a small example
    if (false)
    {
        values = {4, 5, 4, 1, 5, 7, 9};
        weights = {3, 4, 3, 3, 4, 6, 8};
        capacity = 12.0;
    }

    // Prepare the example for the algorithm
    std::vector<Item> items;
    for (int i = 0; i < values.size(); i++)
    {
        items.push_back(Item((double)weights[i], (double)values[i], i));
    }

    knapsack(items, capacity);

    return 0;
    // Maximum profit is : 3642.57 // 10000
}

// https://www.geeksforgeeks.org/0-1-knapsack-using-least-count-branch-and-bound/?ref=rp