// C++ Program to implement 0/1
// knapsack using LC Branch and Bound

#include <bits/stdc++.h>
#include <vector>
#include <queue>
#include <algorithm>
#include <iostream>
// using namespace std;

// Stores the number of items
int size;

// Stores the knapsack capacity
float capacity;

typedef struct Item
{

    // Stores the weight of items
    float weight;

    // Stores the value of items
    int value;

    // Stores the index of items
    int idx;
} Item;

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
float upper_bound(float total_value, float total_weight,
                  int idx, std::vector<Item> &arr)
{
    float value = total_value;
    float weight = total_weight;
    for (int i = idx; i < size; i++)
    {
        if (weight + arr[i].weight <= capacity)
        {
            weight += arr[i].weight;
            value -= arr[i].value;
        }
        else
        {
            value -= (float)(capacity - weight) / arr[i].weight * arr[i].value;
            break;
        }
    }
    return value;
}

// Function to calculate lower bound (no fractional part of the items)
float lower_bound(float total_value, float total_weight, int idx, std::vector<Item> &arr)
{
    float value = total_value;
    float weight = total_weight;
    for (int i = idx; i < size; i++)
    {
        if (weight + arr[i].weight <= capacity)
        {
            weight += arr[i].weight;
            value -= arr[i].value;
        }
        else
        {
            break;
        }
    }
    return value;
}

class comp
{
public:
    bool operator()(Node a, Node b)
    {
        return a.lower_bound > b.lower_bound;
    }
};

void knapsack(std::vector<Item> &arr)
{

    // Sort the items based on the value/weight ratio
    sort(arr.begin(), arr.end(),
         [&](Item &a, Item &b) {
             return (a.value / a.weight) > (b.value / b.weight);
         });

    // min_lb -> Minimum lower bound of all the nodes explored

    // final_lb -> Minimum lower bound of all the paths that reached the final level
    float min_lb = 0, final_lb = INT_MAX;

    // curr_path -> Boolean array to store at every index if the element is included or not
    // final_path -> Boolean array to store the result of selection array when it reached the last level
    std::vector<bool> curr_path = std::vector<bool>(size, false);
    std::vector<bool> final_path = std::vector<bool>(size, false);

    // Priority queue to store the nodes based on lower bounds
    // https://en.cppreference.com/w/cpp/container/priority_queue
    std::priority_queue<Node, std::vector<Node>, comp> priority_queue;

    Node current, left, right;
    current.lower_bound = current.upper_bound = current.total_weight = current.total_value = current.level = current.flag = 0;

    // Insert a dummy node
    priority_queue.push(current);

    while (!priority_queue.empty())
    {
        current = priority_queue.top();
        priority_queue.pop();
        std::cout << "Popped." << std::endl;

        if (current.upper_bound > min_lb || current.upper_bound >= final_lb)
        {

            // If the current node's best case
            // value is not optimal than min_lb,
            // then there is no reason to explore
            // that path including final_lb
            // eliminates all those paths whose
            // best values is equal to final_lb
            continue;
        }

        // update the path
        if (current.level != 0)
            curr_path[current.level - 1] = current.flag;

        if (current.level == size)
        {
            // Reached last level
            if (current.lower_bound < final_lb)
                for (int i = 0; i < size; i++)
                    final_path[arr[i].idx] = curr_path[i];
            final_lb = std::min(current.lower_bound, final_lb);
            continue;
        }

        int level = current.level;

        // right node -> Exludes current item
        // Hence, cp, cw will obtain the value
        // of that of parent
        right.upper_bound = upper_bound(current.total_value,
                           current.total_weight, level + 1,
                           arr);
        right.lower_bound = lower_bound(current.total_value, current.total_weight,
                           level + 1, arr);
        right.level = level + 1;
        right.flag = false;
        right.total_value = current.total_value;
        right.total_weight = current.total_weight;

        // Check whether adding the current
        // item will not exceed the knapsack weight
        if (current.total_weight + arr[current.level].weight <= capacity)
        {

            // left node -> includes current item
            // c and lb should be calculated
            // including the current item.
            left.upper_bound = upper_bound(
                current.total_value - arr[level].value,
                current.total_weight + arr[level].weight,
                level + 1, arr);

            left.lower_bound = lower_bound(
                current.total_value - arr[level].value,
                current.total_weight + arr[level].weight,
                level + 1, arr);

            right.level = level + 1;
            right.flag = true;
            right.total_value = current.total_value - arr[level].value;
            right.total_weight = current.total_weight + arr[level].weight;
        }

        // If Left node cannot be inserted
        else
        {

            // Stop the left node from
            // getting added to the
            // priority queue
            left.upper_bound = left.lower_bound = 1;
        }

        // Update the lower bound
        min_lb = std::min(min_lb, left.lower_bound);
        min_lb = std::min(min_lb, right.lower_bound);

        // Exploring nodes whose
        // upper bound is greater than
        // min_lb will never give
        // the optimal result

        if (min_lb >= left.upper_bound)
        {
            priority_queue.push(left);
        }
        if (min_lb >= right.upper_bound)
        {
            priority_queue.push(right);
        }
    }

    if (final_lb == INT_MAX)
    {
        final_lb = 0;
    }

    std::cout << "Items in the knapsack : \n";

    for (int i = 0; i < size; i++)
    {
        std::cout << final_path[i] << " ";
    }

    std::cout << "\n";
    std::cout << "Maximum profit is : " << (-final_lb) << "\n";
}

// Driver Code
int main()
{

    std::vector<int> values = {3, 8, 3, 5, 2, 2, 9, 6, 1, 9, 7, 1, 4, 3, 1, 3, 6, 3, 2, 1, 1, 3, 6, 7, 3, 1, 8, 5, 5, 3, 9, 5, 9, 6, 1, 5, 4, 4, 1, 5, 8, 5, 4, 5, 4, 1, 5, 7, 8, 3, 6, 3, 4, 1, 6, 3, 4, 7, 1, 7, 1, 1, 7, 7, 3, 7, 6, 1, 9, 9, 3, 2, 1, 8, 4, 4, 2, 1, 1, 7, 9, 3, 4, 3, 9, 1, 6, 2, 8, 4, 2, 3, 7, 9, 5, 5, 1, 1, 5, 9, 9, 8, 8, 8, 6, 4, 5, 2, 9, 1, 1, 2, 5, 8, 4, 7, 4, 8, 6, 8, 7, 2, 9, 9, 3, 5, 3, 2, 7, 4, 5, 7, 1, 4, 2, 1, 9, 1, 5, 8, 3, 5, 3, 1, 1, 6, 5, 4, 1, 9, 9, 9, 6, 6, 4, 3, 8, 5, 9, 5, 1, 4, 6, 8, 3, 8, 3, 1, 1, 5, 8, 1, 6, 5, 7, 5, 6, 2, 1, 9, 7, 5, 9, 9, 2, 8, 9, 5, 7, 8, 3, 2, 2, 7, 8, 9, 6, 9, 5, 4, 3, 8, 7, 3, 8, 2, 4, 7, 5, 3, 4, 1, 6, 7, 7, 4, 8, 7, 6, 8, 9, 5, 9, 7, 4, 9, 8, 4, 3, 1, 9, 9, 7, 8, 2, 5, 2, 1, 7, 3, 4, 2, 1, 7, 4, 2, 9, 7, 8, 1, 2, 3, 8, 7, 2, 7, 1, 9, 9, 6, 8, 5, 4, 3, 5, 5, 5, 8, 1, 8, 8, 1, 9, 8, 9, 7, 1, 2, 8, 7, 1, 5, 6, 4, 9, 1, 1, 8, 4, 3, 9, 3, 2, 9, 7, 4, 4, 7, 6, 3, 6, 7, 5, 3, 7, 9, 7, 7, 8, 1, 6, 5, 5, 4, 1, 2, 8, 5, 5, 9, 3, 4, 7, 4, 9, 3, 3, 5, 5, 5, 1, 4, 3, 8, 2, 7, 8, 9, 4, 9, 1, 7, 4, 3, 2, 9, 5, 3, 7, 5, 1, 8, 4, 5, 8, 7, 3, 6, 5, 3, 6, 4, 4, 4, 7, 6, 4, 2, 1, 9, 7, 9, 3, 5, 2, 4, 1, 6, 7, 5, 9, 2, 2, 8, 9, 2, 1, 6, 6, 8, 1, 2, 9, 5, 4, 4, 3, 2, 6, 2, 2, 5, 2, 7, 5, 7, 7, 1, 7, 8, 7, 8, 8, 8, 2, 5, 4, 6, 7, 8, 8, 3, 6, 6, 8, 8, 6, 1, 3, 9, 4, 2, 5, 7, 9, 1, 2, 3, 4, 3, 7, 2, 1, 3, 6, 8, 1, 1, 3, 2, 5, 8, 1, 2, 2, 6, 7, 5, 4, 4, 8, 3, 1, 1, 6, 7, 5, 4, 6, 8, 1, 2, 5, 9, 5, 4, 9, 4, 2, 6, 8, 6, 2, 3, 2, 3, 9, 6, 9, 9, 9, 3, 9, 3, 6, 8, 3, 7, 2, 1, 9, 1, 3, 9, 9, 8, 1, 4, 6, 9, 5, 4, 8, 8, 5, 5, 4, 3, 4, 4, 8, 9, 8, 5, 1, 8, 6, 3, 8, 1, 2, 6, 1, 2, 8, 9, 4, 4, 7, 9, 6, 3, 4, 3, 3, 4, 4, 9, 5, 2, 8, 4, 6, 5, 8, 1, 1, 6, 7, 8, 8, 9, 2, 7, 2, 3, 2, 7, 2, 5, 5, 1, 1, 9, 7, 7, 3, 1, 1, 9, 1, 8, 3, 5, 7, 3, 5, 3, 5, 2, 3, 6, 8, 4, 3, 6, 7, 3, 8, 8, 8, 2, 9, 5, 9, 1, 8, 3, 5, 4, 8, 9, 2, 9, 5, 4, 6, 6, 4, 2, 8, 6, 9, 8, 9, 5, 5, 1, 9, 7, 6, 8, 2, 8, 4, 9, 6, 4, 7, 4, 1, 1, 5, 9, 8, 4, 1, 5, 5, 8, 2, 3, 2, 2, 2, 5, 1, 7, 5, 9, 2, 2, 9, 7, 6, 8, 6, 4, 3, 2, 2, 9, 6, 9, 6, 3, 4, 6, 5, 7, 8, 3, 5, 9, 8, 2, 4, 9, 1, 3, 1, 8, 9, 5, 1, 8, 1, 4, 4, 5, 2, 1, 4, 6, 7, 2, 4, 8, 2, 6, 6, 4, 8, 4, 9, 2, 3, 4, 5, 3, 4, 2, 9, 6, 6, 5, 7, 3, 7, 1, 7, 1, 8, 6, 5, 1, 2, 8, 4, 5, 7, 8, 3, 3, 7, 1, 2, 5, 6, 6, 4, 4, 7, 4, 2, 9, 4, 1, 5, 8, 3, 7, 5, 7, 7, 2, 1, 3, 2, 8, 3, 2, 9, 3, 7, 3, 4, 7, 8, 7, 2, 7, 8, 9, 2, 6, 5, 1, 7, 3, 4, 3, 5, 8, 8, 9, 3, 2, 9, 5, 8, 9, 1, 9, 8, 5, 6, 6, 6, 6, 1, 5, 8, 1, 2, 3, 5, 1, 8, 2, 3, 7, 5, 2, 2, 2, 6, 1, 8, 8, 4, 4, 6, 7, 3, 8, 1, 5, 5, 5, 8, 8, 6, 1, 2, 2, 2, 2, 1, 6, 4, 7, 5, 8, 6, 8, 1, 4, 8, 1, 5, 4, 5, 5, 1, 3, 8, 2, 9, 8, 8, 1, 3, 5, 3, 2, 2, 2, 7, 8, 7, 2, 7, 4, 3, 3, 6, 5, 8, 2, 2, 2, 3, 6, 6, 7, 4, 3, 2, 8, 3, 5, 6, 3, 9, 2, 6, 5, 2, 7, 7, 6, 9, 9, 9, 6, 8, 4, 6, 2, 8, 4, 9, 6, 9, 4, 2, 7, 4, 8, 2, 3, 2, 4, 1, 8, 2, 6, 5, 8, 3, 7, 7, 3, 1, 6, 9, 8, 8, 9, 9, 3, 3, 2, 4, 9, 7, 3, 5, 1, 7, 1, 5, 1, 9, 8, 3, 2, 8, 6, 8, 5, 7, 5, 2, 2, 8, 3, 4, 8, 9, 6, 1, 8, 2, 9, 7, 2, 5, 5, 2, 4, 2, 7, 1, 5, 5, 4, 4, 6};
    std::vector<int> weights = {9, 3, 6, 8, 2, 8, 4, 8, 3, 4, 1, 6, 3, 1, 8, 3, 5, 7, 2, 8, 7, 3, 1, 6, 1, 9, 4, 5, 3, 1, 9, 1, 6, 3, 9, 9, 2, 4, 5, 6, 6, 5, 5, 6, 2, 6, 8, 9, 3, 5, 6, 4, 5, 3, 4, 4, 6, 9, 2, 1, 5, 5, 6, 1, 1, 1, 2, 9, 9, 5, 1, 2, 4, 7, 5, 1, 4, 1, 3, 4, 1, 7, 9, 5, 2, 4, 8, 8, 2, 4, 9, 2, 6, 7, 9, 1, 6, 6, 2, 5, 4, 6, 7, 8, 3, 6, 2, 3, 8, 7, 4, 8, 6, 7, 1, 8, 8, 5, 3, 7, 2, 3, 3, 8, 5, 1, 3, 4, 6, 7, 9, 5, 2, 3, 3, 3, 5, 7, 6, 9, 2, 4, 6, 3, 8, 8, 7, 5, 2, 9, 5, 4, 8, 8, 9, 8, 6, 3, 7, 8, 3, 4, 3, 6, 4, 9, 4, 9, 9, 2, 7, 3, 8, 2, 7, 5, 2, 3, 4, 1, 2, 7, 6, 4, 9, 8, 7, 7, 4, 7, 2, 2, 9, 6, 2, 5, 8, 9, 9, 8, 6, 3, 9, 3, 4, 4, 7, 2, 1, 9, 2, 7, 2, 3, 7, 6, 1, 8, 4, 2, 2, 8, 4, 4, 4, 4, 1, 8, 4, 4, 3, 3, 5, 6, 3, 8, 5, 8, 6, 3, 3, 1, 8, 2, 9, 9, 9, 2, 2, 3, 3, 2, 6, 3, 5, 8, 1, 7, 2, 2, 8, 6, 3, 9, 6, 8, 4, 1, 5, 4, 5, 5, 5, 8, 2, 8, 1, 3, 5, 2, 7, 9, 9, 5, 2, 1, 5, 7, 1, 2, 3, 9, 2, 5, 1, 7, 7, 1, 7, 7, 1, 6, 5, 1, 5, 9, 7, 2, 4, 4, 3, 2, 1, 3, 5, 2, 2, 4, 7, 1, 4, 5, 1, 5, 4, 1, 2, 4, 6, 7, 2, 9, 5, 7, 2, 6, 6, 2, 1, 5, 2, 4, 8, 2, 4, 8, 1, 4, 6, 8, 8, 2, 7, 9, 4, 4, 4, 8, 5, 5, 7, 2, 4, 6, 9, 2, 6, 9, 6, 7, 6, 9, 2, 8, 2, 6, 8, 8, 6, 4, 9, 9, 5, 9, 9, 5, 6, 6, 1, 7, 1, 4, 7, 3, 2, 8, 5, 6, 5, 3, 5, 4, 9, 7, 5, 5, 3, 1, 8, 4, 7, 2, 7, 9, 5, 6, 3, 6, 4, 9, 2, 8, 5, 5, 5, 1, 9, 3, 7, 2, 2, 1, 2, 7, 7, 9, 3, 3, 9, 7, 7, 1, 8, 6, 9, 7, 2, 5, 1, 8, 3, 2, 6, 4, 5, 3, 5, 4, 1, 8, 4, 4, 5, 4, 7, 4, 1, 1, 1, 5, 5, 6, 2, 5, 8, 7, 7, 9, 2, 8, 3, 7, 5, 9, 7, 4, 1, 5, 4, 6, 9, 4, 6, 1, 6, 3, 2, 3, 1, 9, 2, 5, 2, 6, 4, 2, 8, 4, 4, 8, 1, 4, 8, 7, 4, 1, 1, 5, 3, 8, 6, 9, 2, 9, 5, 6, 6, 6, 3, 2, 4, 5, 7, 4, 8, 5, 7, 3, 9, 5, 6, 3, 7, 2, 5, 8, 8, 2, 8, 4, 3, 5, 1, 9, 9, 5, 9, 2, 8, 1, 2, 4, 3, 5, 5, 3, 4, 2, 9, 1, 7, 8, 2, 7, 1, 9, 8, 2, 7, 3, 4, 3, 3, 1, 1, 1, 6, 1, 3, 6, 1, 6, 4, 4, 2, 9, 6, 2, 7, 7, 8, 9, 5, 5, 8, 6, 9, 9, 6, 7, 3, 9, 3, 3, 9, 5, 2, 1, 3, 5, 2, 8, 8, 4, 4, 6, 1, 4, 3, 4, 2, 6, 3, 3, 3, 3, 1, 4, 8, 1, 4, 5, 1, 6, 4, 3, 2, 1, 6, 8, 9, 6, 7, 5, 9, 9, 5, 5, 2, 2, 1, 9, 3, 9, 6, 9, 4, 5, 7, 7, 1, 3, 8, 3, 7, 6, 5, 8, 8, 1, 9, 6, 2, 9, 4, 6, 6, 2, 6, 1, 7, 5, 5, 9, 5, 4, 9, 6, 7, 4, 8, 5, 2, 2, 7, 9, 5, 1, 3, 5, 8, 2, 7, 8, 4, 1, 3, 1, 4, 5, 4, 4, 4, 9, 9, 3, 2, 6, 1, 8, 3, 7, 8, 1, 9, 3, 8, 9, 7, 8, 4, 8, 5, 8, 7, 3, 9, 7, 3, 8, 4, 9, 9, 5, 2, 3, 7, 1, 7, 9, 2, 1, 7, 7, 3, 6, 5, 3, 5, 6, 5, 8, 1, 8, 7, 4, 8, 3, 4, 1, 6, 9, 5, 6, 2, 2, 2, 2, 5, 5, 3, 8, 1, 3, 8, 9, 6, 2, 6, 6, 8, 9, 8, 5, 2, 7, 9, 6, 4, 2, 3, 2, 4, 5, 3, 3, 5, 4, 9, 4, 1, 3, 4, 3, 7, 4, 5, 2, 2, 2, 5, 6, 1, 7, 3, 9, 1, 7, 3, 9, 7, 7, 7, 3, 7, 9, 4, 4, 6, 2, 9, 7, 4, 9, 1, 8, 5, 3, 8, 6, 4, 9, 7, 7, 4, 1, 2, 8, 6, 6, 5, 5, 2, 9, 2, 6, 9, 7, 6, 7, 2, 7, 9, 7, 1, 6, 9, 4, 3, 6, 4, 7, 5, 9, 5, 5, 9, 5, 6, 8, 1, 4, 8, 9, 4, 8, 2, 1, 3, 5, 2, 4, 4, 1, 4, 5, 7, 6, 7, 4, 7, 7, 9, 4, 3, 3, 5, 7, 1, 6, 3, 3, 2, 5, 2, 9, 6, 1, 4, 5, 7, 2, 1, 2, 5, 4, 7, 9, 5, 7, 7, 1, 4, 3, 3, 8, 5, 5, 2, 6, 5, 6, 4, 9, 4, 9, 7, 2, 1, 1, 1, 2, 5, 6, 9, 1, 2, 8, 6, 1, 4, 8, 4, 7, 6, 5, 3, 3, 1, 3, 2, 3, 1, 9, 6, 6, 7, 7, 9, 2};

    size = values.size();
    capacity = 2000;

    if (true){
    values = {4, 5, 4};
    weights = {3, 4, 3};
    size = values.size();
    capacity = 4;

    }

    std::vector<Item> arr;
    for (int i = 0; i < size; i++)
    {
        arr.push_back({(float)weights[i], values[i], i});
    }

    knapsack(arr);

    return 0;
}

// https://www.geeksforgeeks.org/0-1-knapsack-using-least-count-branch-and-bound/?ref=rp