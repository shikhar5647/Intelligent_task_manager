#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <limits> // Include for INT_MAX
#include <memory>

using namespace std;

class Task
{
public:
    string name;
    string description;
    int deadline;
    string status;
    vector<string> tags;
    vector<string> dependencies;
    vector<string> reminders;
    double priority;

    Task(const string &name, const string &description, int deadline, const string &status,
         const vector<string> &tags, const vector<string> &dependencies, const vector<string> &reminders)
        : name(name), description(description), deadline(deadline), status(status),
          tags(tags), dependencies(dependencies), reminders(reminders)
    {
        this->priority = calculatePriority();
    }

    double calculatePriority()
    {
        // Define weights for parameters
        double dependencyWeight = 0.3;
        double deadlineWeight = 0.3;
        double statusWeight = 0.2;
        double reminderWeight = 0.2;

        // Calculate scores for each parameter
        double dependencyScore = dependencies.size() / 10.0;      // Assuming a maximum of 10 dependencies
        double deadlineScore = 1 - (deadline / 100.0);            // Assuming deadline is a percentage of completion
        double statusScore = (status == "Completed") ? 1.0 : 0.5; // Assuming completed tasks have higher priority
        double reminderScore = reminders.size() / 5.0;            // Assuming a maximum of 5 reminders

        // Calculate priority using weighted sum
        double priority = (dependencyScore * dependencyWeight) +
                          (deadlineScore * deadlineWeight) +
                          (statusScore * statusWeight) +
                          (reminderScore * reminderWeight);

        return priority;
    }
};

// Comparison function for priority queue
class ComparePriority
{
public:
    bool operator()(const Task &t1, const Task &t2)
    {
        return t1.priority < t2.priority;
    }
};

// Function to adjust priorities of tasks in the vector
void adjustPriorities(vector<Task> &tasks)
{
    for (Task &task : tasks)
    {
        task.priority = task.calculatePriority();
    }
}

// Function to take user input and create a task
Task createTask()
{
    string name, description, status;
    int deadline;
    vector<string> tags, dependencies, reminders;

    cout << "Enter Task Name: ";
    getline(cin, name);

    cout << "Enter Description: ";
    getline(cin, description);

    cout << "Enter Deadline (days from now): ";
    cin >> deadline;
    cin.ignore(); // Consume newline left by cin

    cout << "Enter Status: ";
    getline(cin, status);

    string tag;
    cout << "Enter Tags (press Enter after each tag, type 'done' when finished):\n";
    while (true)
    {
        getline(cin, tag);
        if (tag == "done")
        {
            break;
        }
        tags.push_back(tag);
    }

    string dependency;
    cout << "Enter Dependencies (press Enter after each dependency, type 'done' when finished):\n";
    while (true)
    {
        getline(cin, dependency);
        if (dependency == "done")
        {
            break;
        }
        dependencies.push_back(dependency);
    }

    string reminder;
    cout << "Enter Reminders (press Enter after each reminder, type 'done' when finished):\n";
    while (true)
    {
        getline(cin, reminder);
        if (reminder == "done")
        {
            break;
        }
        reminders.push_back(reminder);
    }

    return Task(name, description, deadline, status, tags, dependencies, reminders);
}

// Function to perform topological sort using DFS
void topologicalSortUtil(const unordered_map<string, vector<string>> &graph, unordered_set<string> &visited,
                         const string &task, queue<string> &result)
{
    visited.insert(task);
    for (const string &dependency : graph.at(task))
    {
        if (visited.find(dependency) == visited.end())
        {
            topologicalSortUtil(graph, visited, dependency, result);
        }
    }
    result.push(task);
}

// Function to perform topological sort
queue<string> topologicalSort(const unordered_map<string, vector<string>> &graph)
{
    queue<string> result;
    unordered_set<string> visited;

    for (const auto &pair : graph)
    {
        if (visited.find(pair.first) == visited.end())
        {
            topologicalSortUtil(graph, visited, pair.first, result);
        }
    }

    return result;
}

// Red-Black Tree Implementation
template <typename K, typename V>
class RedBlackTree
{
private:
    enum class Color
    {
        RED,
        BLACK
    };

    struct RBTreeNode
    {
        K key;
        V value;
        Color color;
        shared_ptr<RBTreeNode> left;
        shared_ptr<RBTreeNode> right;
        shared_ptr<RBTreeNode> parent;

        RBTreeNode(const K &k, const V &v, Color c = Color::RED)
            : key(k), value(v), color(c), left(nullptr), right(nullptr), parent(nullptr) {}
    };

    shared_ptr<RBTreeNode> root;

    void leftRotate(shared_ptr<RBTreeNode> &x)
    {
        shared_ptr<RBTreeNode> y = x->right;
        x->right = y->left;
        if (y->left != nullptr)
        {
            y->left->parent = x;
        }
        y->parent = x->parent;
        if (x->parent == nullptr)
        {
            root = y;
        }
        else if (x == x->parent->left)
        {
            x->parent->left = y;
        }
        else
        {
            x->parent->right = y;
        }
        y->left = x;
        x->parent = y;
    }

    void rightRotate(shared_ptr<RBTreeNode> &y)
    {
        shared_ptr<RBTreeNode> x = y->left;
        y->left = x->right;
        if (x->right != nullptr)
        {
            x->right->parent = y;
        }
        x->parent = y->parent;
        if (y->parent == nullptr)
        {
            root = x;
        }
        else if (y == y->parent->right)
        {
            y->parent->right = x;
        }
        else
        {
            y->parent->left = x;
        }
        x->right = y;
        y->parent = x;
    }

    void insertFixup(shared_ptr<RBTreeNode> &z)
    {
        while (z != root && z->parent->color == Color::RED)
        {
            if (z->parent == z->parent->parent->left)
            {
                shared_ptr<RBTreeNode> y = z->parent->parent->right;
                if (y != nullptr && y->color == Color::RED)
                {
                    z->parent->color = Color::BLACK;
                    y->color = Color::BLACK;
                    z->parent->parent->color = Color::RED;
                    z = z->parent->parent;
                }
                else
                {
                    if (z == z->parent->right)
                    {
                        z = z->parent;
                        leftRotate(z);
                    }
                    z->parent->color = Color::BLACK;
                    z->parent->parent->color = Color::RED;
                    rightRotate(z->parent->parent);
                }
            }
            else
            {
                shared_ptr<RBTreeNode> y = z->parent->parent->left;
                if (y != nullptr && y->color == Color::RED)
                {
                    z->parent->color = Color::BLACK;
                    y->color = Color::BLACK;
                    z->parent->parent->color = Color::RED;
                    z = z->parent->parent;
                }
                else
                {
                    if (z == z->parent->left)
                    {
                        z = z->parent;
                        rightRotate(z);
                    }
                    z->parent->color = Color::BLACK;
                    z->parent->parent->color = Color::RED;
                    leftRotate(z->parent->parent);
                }
            }
        }
        root->color = Color::BLACK;
    }

    void insertNode(const K &key, const V &value)
    {
        shared_ptr<RBTreeNode> z = make_shared<RBTreeNode>(key, value);
        shared_ptr<RBTreeNode> y = nullptr;
        shared_ptr<RBTreeNode> x = root;
        while (x != nullptr)
        {
            y = x;
            if (z->key < x->key)
            {
                x = x->left;
            }
            else
            {
                x = x->right;
            }
        }
        z->parent = y;
        if (y == nullptr)
        {
            root = z;
        }
        else if (z->key < y->key)
        {
            y->left = z;
        }
        else
        {
            y->right = z;
        }
        z->left = nullptr;
        z->right = nullptr;
        z->color = Color::RED;
        insertFixup(z);
    }

    void inorderTraversal(shared_ptr<RBTreeNode> &node, vector<V> &result)
    {
        if (node != nullptr)
        {
            inorderTraversal(node->left, result);
            result.push_back(node->value);
            inorderTraversal(node->right, result);
        }
    }

public:
    RedBlackTree() : root(nullptr) {}

    void insert(const K &key, const V &value)
    {
        insertNode(key, value);
    }

    vector<V> inorderTraversal()
    {
        vector<V> result;
        inorderTraversal(root, result);
        return result;
    }
};

int main()
{
    vector<Task> tasks;
    priority_queue<Task, vector<Task>, ComparePriority> priorityQueue;

    // Take user input to create tasks
    cout << "Enter task details (press Ctrl + Z or type 'exit' to finish):\n";
    while (true)
    {
        Task task = createTask();
        if (task.name == "exit")
        {
            break;
        }
        tasks.push_back(task);
    }

    adjustPriorities(tasks);

    // Populate the priority queue with tasks
    for (const Task &task : tasks)
    {
        priorityQueue.push(task);
    }

    // Model task dependencies using graph structure
    unordered_map<string, vector<string>> graph;
    for (const Task &task : tasks)
    {
        for (const string &dependency : task.dependencies)
        {
            graph[dependency].push_back(task.name);
        }
    }

    // Perform topological sort to determine execution order
    queue<string> sortedTasks = topologicalSort(graph);

    // Process tasks in topological order
    while (!sortedTasks.empty())
    {
        string taskName = sortedTasks.front();
        sortedTasks.pop();
        cout << "Processing task: " << taskName << endl;
        // Find and process task from original list based on name
        for (const Task &task : tasks)
        {
            if (task.name == taskName)
            {
                cout << "Priority: " << task.priority << endl;
                break;
            }
        }
    }

    return 0;
}
