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
// using namespace std;

/*Print a vector*/
template <typename T> // This line lets the function below take arbitrary vectors (int or float)
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
    float weight;
    float value;
    int idx;

    Item(float weight, float value, int idx)
    {
        // Store values on the instance
        this->weight = weight;
        this->value = value;
        this->idx = idx;
    }

    void print()
    {
        std::cout << "Item(weight=" << weight << ", value=" << value << ", idx=" << idx << ")" << std::endl;
    }
};

typedef struct Node
{
    float upper_bound;  // Upper Bound: Best case (Fractional Knapsck)
    float lower_bound;  // Lower Bound: Worst case (0/1)
    int level;          // Level of the node in the decision tree
    bool flag;          // If the curren item in selected or not
    float total_value;  // Sum of the values of the items included
    float total_weight; // Sum of the weights of the items included
} Node;


// Function to calculate upper bound (includes fractional part of the items)
// Function to calculate lower bound (no fractional part of the items)
std::pair<float, float> bound(float capacity, float total_value, float total_weight,
                  int start_index, std::vector<Item> &items)
{
    float upper_bound = total_value;
    float lower_bound  = total_value;

    float weight = total_weight;
    for (int i = start_index; i < items.size(); i++)
    {
        if (weight + items[i].weight <= capacity)
        {
            weight += items[i].weight;
            upper_bound -= items[i].value;
            lower_bound -= items[i].value;
        }
        else
        {
            upper_bound -= (capacity - weight) / items[i].weight * items[i].value;
            break;
        }
    }
    return std::make_pair(lower_bound, upper_bound);
}

class Compare
{
public:
    bool operator()(Node &a, Node &b)
    {
        return a.lower_bound > b.lower_bound;
    }
};

std::vector<bool> knapsack(std::vector<Item> &items, float capacity)
{

    // Sort the items based on efficiency, e.g., value/weight ratio
    sort(items.begin(), items.end(),
         [&](Item &a, Item &b) {
             return (a.value / a.weight) > (b.value / b.weight);
         });

    // min_lb -> Minimum lower bound of all the nodes explored
    // final_lb -> Minimum lower bound of all the paths that reached the final level
    float lower_bound_explored = 0;
    float lower_bound_bottom_level = FLT_MAX;

    // curr_path -> Boolean array to store at every index if the element is included or not
    // final_path -> Boolean array to store the result of selection array when it reached the last level
    std::vector<bool> current_solution = std::vector<bool>(items.size(), false);
    std::vector<bool> final_solution = std::vector<bool>(items.size(), false);

    // Priority queue to store the nodes based on lower bounds
    // https://en.cppreference.com/w/cpp/container/priority_queue
    std::priority_queue<Node, std::vector<Node>, Compare> priority_queue;

    Node current, left, right;
    current.lower_bound = current.upper_bound = current.total_weight = 0;
    current.total_value = current.level = current.flag = 0;

    // Insert a dummy node
    priority_queue.push(current);

    while (!priority_queue.empty())
    {

        std::cout << "===================================" << std::endl;

        current = priority_queue.top();
        priority_queue.pop();
        std::cout << "Current level: " << current.level << std::endl;
        std::cout << "Current lower bound: " << current.lower_bound << std::endl;
        std::cout << "Current upper bound: " << current.upper_bound << std::endl;
        std::cout << "Current lower bound explored: " << lower_bound_explored << std::endl;
        print_vector(final_solution);

        if (current.upper_bound > lower_bound_explored || current.upper_bound >= lower_bound_bottom_level)
        {

            // If the current node's best case
            // value is not optimal than min_lb,
            // then there is no reason to explore
            // that path including final_lb
            // eliminates all those paths whose
            // best values is equal to final_lb
            std::cout << "Pruning solution." << std::endl;
            continue;
        }

        // std::cout << current.level << std::endl;

        // update the path
        if (current.level != 0)
        {
            current_solution[current.level - 1] = current.flag;
        }

        // Reached the bottom of the tree
        if (current.level == items.size())
        {
            // We found a new best solution
            if (current.lower_bound < lower_bound_bottom_level)
                for (int i = 0; i < items.size(); i++)
                    final_solution[items[i].idx] = current_solution[i];

            // Update the lower bound
            lower_bound_bottom_level = std::min(current.lower_bound, lower_bound_bottom_level);

            std::cout << "Reached the bottom." << std::endl;
            print_vector(final_solution);

            continue;
        }

        int level = current.level;

        //std:std::cout << level << std::endl;

        // right node -> Exludes current item

        std::pair<float, float> lower_upper_bounds = bound(capacity, 
                                        current.total_value,
                                        current.total_weight, 
                                        level + 1,
                                        items);

        right.upper_bound = lower_upper_bounds.second;
        right.lower_bound = lower_upper_bounds.first;

        right.level = level + 1;
        right.flag = false;
        right.total_value = current.total_value;
        right.total_weight = current.total_weight;

        // Check whether adding the current
        // item will not exceed the knapsack weight
        if (current.total_weight + items[current.level].weight <= capacity)
        {

            // left node -> includes current item

            std::pair<float, float> lower_upper_bounds = bound(capacity,
                                           current.total_value - items[level].value,
                                           current.total_weight + items[level].weight,
                                           level + 1, items);

            left.upper_bound = lower_upper_bounds.second;
            left.lower_bound = lower_upper_bounds.first;

            left.level = level + 1;
            left.flag = true;
            left.total_value = current.total_value - items[level].value;
            left.total_weight = current.total_weight + items[level].weight;
        }

        // If Left node cannot be inserted
        else
        {
            // Stop the left node from getting added to the priority queue
            left.upper_bound = left.lower_bound = 1;
        }

        // Update the lower bound
        lower_bound_explored = std::min(lower_bound_explored, left.lower_bound);
        lower_bound_explored = std::min(lower_bound_explored, right.lower_bound);

        // Exploring nodes with upper bound is greater than
        // min_lb will never give the optimal result

        if (lower_bound_explored >= left.upper_bound)
        {
            priority_queue.push(left);
        }
        if (lower_bound_explored >= right.upper_bound)
        {
            priority_queue.push(right);
        }
    }

    // Prepare the solution and return it
    std::cout << "Items in the knapsack : \n";
    print_vector(final_solution);

    std::cout << "\n";
    std::cout << "Maximum profit is : " << (-lower_bound_bottom_level) << "\n";
    return final_solution;
}

// Driver Code
int main()
{

    std::vector<float> values;
    std::vector<float> weights;

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
    for (size_t i = 0; i < 5; i++)
    {
        float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        values.push_back(r);
        r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        weights.push_back(r);
    }
    float capacity = weights.size() * 0.2;

    // Create a small example
    if (true)
    {
        values = {4, 5, 4};
        weights = {3, 4, 3};
        capacity = 4.0;
    }

    // Prepare the example for the algorithm
    std::vector<Item> items;
    for (int i = 0; i < values.size(); i++)
    {
        items.push_back(Item((float)weights[i], (float)values[i], i));
    }

    knapsack(items, capacity);

    return 0;
}

// https://www.geeksforgeeks.org/0-1-knapsack-using-least-count-branch-and-bound/?ref=rp