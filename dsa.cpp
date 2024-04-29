#include <iostream>
#include <string>
#include <unordered_map>
#include <queue>
#include <list>
#include <vector>
#include <algorithm>
#include <limits>

using namespace std;

// Forward declaration of Task class
class Task;

// Dependency graph node
struct GraphNode
{
    Task *task;
    std::vector<GraphNode *> dependencies;
};

// Enumeration for task completion status
enum class CompletionStatus
{
    NOT_STARTED,
    JUST_STARTED,
    HALF_COMPLETED,
    ALMOST_COMPLETED,
    FINISHED
};

// Task class
class Task
{
private:
    std::string name;
    std::string description;
    int deadline;
    double priority;
    double completion;
    CompletionStatus status;
    double completionFactor;
    double deadlineFactor;

public:
    Task(std::string name, std::string description, int deadline, double completion) : name(name), description(description), deadline(deadline),completion(completion),status(CompletionStatus::NOT_STARTED)
    {
        deadlineFactor = calculateDeadlineFactor();
        completionFactor = completion;
        priority = calculatePriority();
        
    }

    float calculatePriority()
    {
        // Calculate priority based on the provided formula
        std::cout << "deadline : " << deadlineFactor  << " -------------------- " << completionFactor << std::endl;
        float priority = 0.7f * deadlineFactor + 0.3f * completionFactor;
        return priority;
    }

    double getPriority() const
    {
        return priority;
    }

    float calculateDeadlineFactor()
    {
        // Calculate deadline factor based on the provided formula
        return 1.0f - (static_cast<float>(deadline) / (deadline + 30));
    }

    std::string getName() const
    {
        return name;
    }

    std::string getDescription() const
    {
        return description;
    }

    int getDeadline() const
    {
        return deadline;
    }

    float getStatus() const
    {
        // Calculate completion status in float
        // switch (status)
        // {
        // case completion:
        //     return 0.0;
        // case 0.25:
        //     return 0.25;
        // case CompletionStatus::HALF_COMPLETED:
        //     return 0.5;
        // case CompletionStatus::ALMOST_COMPLETED:
        //     return 0.75;
        // case CompletionStatus::FINISHED:
        //     return 1.0;
        // default:
        //     return 0.0; // Default to NOT_STARTED if status is invalid
        // }

        return this -> completionFactor;
    }

    void setStatus(CompletionStatus newStatus)
    {
        status = newStatus;
        completionFactor = 1.0 - getStatus();
    }

    double getCompletionFactor() const
    {
        return completionFactor;
    }
};

// Comparison function for priority queue
struct CompareTask
{
    bool operator()(const Task *a, const Task *b) const
    {
        return a->getPriority() > b->getPriority();
    }
};

// Task Manager class
class TaskManager
{
public:
    std::unordered_map<std::string, Task *> taskMap;

private:
    std::priority_queue<Task *, std::vector<Task *>, CompareTask> taskQueue; // Priority queue for tasks
    std::unordered_map<std::string, GraphNode *> dependencyGraph;            // Graph for dependency management
    std::list<Task *> taskList;                                              // Linked list for dynamic task insertion/deletion

    // Helper function for topological sorting
    void topologicalSortUtil(GraphNode *node, std::unordered_map<std::string, bool> &visited, std::list<Task *> &sortedTasks)
    {
        visited[node->task->getName()] = true;
        for (auto &dep : node->dependencies)
        {
            if (!visited[dep->task->getName()])
            {
                topologicalSortUtil(dep, visited, sortedTasks);
            }
        }
        sortedTasks.push_front(node->task);
    }

    // Perform topological sorting
    std::list<Task *> topologicalSort()
    {
        std::unordered_map<std::string, bool> visited;
        std::list<Task *> sortedTasks;

        for (auto &pair : dependencyGraph)
        {
            visited[pair.first] = false;
        }

        for (auto &pair : dependencyGraph)
        {
            if (!visited[pair.first])
            {
                topologicalSortUtil(pair.second, visited, sortedTasks);
            }
        }

        return sortedTasks;
    }

public:
    void addTask(Task *task)
    {
        taskMap[task->getName()] = task;
        taskList.push_back(task);

        // Push the new task onto the priority queue
        taskQueue.push(task);

        GraphNode *newNode = new GraphNode();
        newNode->task = task;
        dependencyGraph[task->getName()] = newNode;
    }

    void deleteTask(const std::string &taskName)
    {
        // Find the task in the taskMap
        auto it = taskMap.find(taskName);
        if (it != taskMap.end())
        {
            Task *taskToDelete = it->second;

            // Remove the task from the taskMap
            taskMap.erase(it);

            // Remove the task from the taskList
            taskList.remove(taskToDelete);

            // Remove the task from the priority queue taskQueue
            std::priority_queue<Task *, std::vector<Task *>, CompareTask> newQueue;
            while (!taskQueue.empty())
            {
                Task *currentTask = taskQueue.top();
                taskQueue.pop();
                if (currentTask->getName() != taskName)
                {
                    newQueue.push(currentTask);
                }
            }
            taskQueue = newQueue;

            // Remove the task from the dependency graph
            auto depIt = dependencyGraph.find(taskName);
            if (depIt != dependencyGraph.end())
            {
                GraphNode *nodeToDelete = depIt->second;
                dependencyGraph.erase(depIt);

                // Remove all dependencies on this task from other nodes
                for (auto &pair : dependencyGraph)
                {
                    auto &dependencies = pair.second->dependencies;
                    dependencies.erase(std::remove_if(dependencies.begin(), dependencies.end(), [taskToDelete](GraphNode *dep)
                                                      { return dep->task == taskToDelete; }),
                                       dependencies.end());
                }

                delete nodeToDelete;
            }

            // Delete the task object
            delete taskToDelete;

            std::cout << "Task '" << taskName << "' deleted successfully." << std::endl;
        }
        else
        {
            std::cout << "Task '" << taskName << "' not found." << std::endl;
        }
    }

    // Method to prompt user for task name and delete the task if found
    void deleteTaskPrompt()
    {
        std::string taskNameToDelete;
        std::cout << "Enter the name of the task to delete: ";
        std::cin.ignore(); // Ignore any previous newline character
        std::getline(std::cin, taskNameToDelete);

        auto it = taskMap.find(taskNameToDelete);
        if (it != taskMap.end())
        {
            Task *taskToDelete = it->second;

            // Remove task from taskMap
            taskMap.erase(it);

            // Remove task from taskQueue
            std::priority_queue<Task *, std::vector<Task *>, CompareTask> newTaskQueue;
            while (!taskQueue.empty())
            {
                Task *task = taskQueue.top();
                taskQueue.pop();
                if (task != taskToDelete)
                {
                    newTaskQueue.push(task);
                }
                else
                {
                    // Delete task object to free memory
                    delete task;
                }
            }
            taskQueue = newTaskQueue;

            // Remove task from taskList
            taskList.remove(taskToDelete);

            // Remove task from dependencyGraph
            auto dependencyIt = dependencyGraph.find(taskNameToDelete);
            if (dependencyIt != dependencyGraph.end())
            {
                delete dependencyIt->second;
                dependencyGraph.erase(dependencyIt);
            }

            std::cout << "Task '" << taskNameToDelete << "' has been deleted successfully." << std::endl;
        }
        else
        {
            std::cout << "Task '" << taskNameToDelete << "' not found. No task was deleted." << std::endl;
        }
    }

    // Method to add dependencies between tasks
    void addDependency(const std::string &taskName, const std::string &dependencyName)
    {
        if (dependencyGraph.find(taskName) != dependencyGraph.end() && dependencyGraph.find(dependencyName) != dependencyGraph.end())
        {
            dependencyGraph[taskName]->dependencies.push_back(dependencyGraph[dependencyName]);
            std::cout << "Dependency added successfully!" << std::endl;
        }
        else
        {
            std::cout << "Task or dependency not found. Please make sure both tasks exist." << std::endl;
        }
    }

    // Method to mark task as completed
    void markTaskCompleted(Task *task, CompletionStatus status)
    {
        task->setStatus(status);
    }

    double getUserCompletionStatus()
    {
        std::cout << "Select completion status:" << std::endl;
        std::cout << "1. Not Started" << std::endl;
        std::cout << "2. Just Started" << std::endl;
        std::cout << "3. Half Completed" << std::endl;
        std::cout << "4. Almost Completed" << std::endl;
        std::cout << "5. Finished" << std::endl;

        int choice;
        std::cout << "Enter your choice (1-5): ";
        std::cin >> choice;

        switch (choice)
        {
        case 1:
            return 0.0;
        case 2:
            return 0.25;
        case 3:
            return 0.5;
        case 4:
            return 0.75;
        case 5:
            return 1.0;
        default:
            std::cout << "Invalid choice. Defaulting to Not Started." << std::endl;
            return -1.0;
        }
    }

    // Method to add a new task interactively during program execution
    // void addNewTask()
    // {
    //     std::string name, description;
    //     int deadline;

    //     std::cin.ignore(); // Ignore the newline character from previous input

    //     std::cout << "Enter task name: ";
    //     std::getline(std::cin, name);

    //     if (name.empty())
    //     {
    //         std::cout << "Error: Please enter a name for the task." << std::endl;
    //         return; // Or handle the empty name case differently
    //     }

    //     std::cout << "Enter task description: ";
    //     std::getline(std::cin, description);

    //     std::cout << "Enter task deadline: ";
    //     std::cin >> deadline;
    //     std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the input buffer

    //     Task *newTask = new Task(name, description, deadline, );
    //     if (newTask == nullptr)
    //     {
    //         std::cerr << "Error: Memory allocation failed for new task." << std::endl;
    //         return;
    //     }
    //     addTask(newTask);

    //     displayTasks(); // Display all tasks after adding the new task
    // }

    // Perform topological sorting and execute tasks accordingly
    void executeTasks()
    {
        while (true)
        {
            std::priority_queue<Task *, std::vector<Task *>, CompareTask> sortedTasksQueue;
            while (!taskQueue.empty())
            {
                sortedTasksQueue.push(taskQueue.top());
                taskQueue.pop();
            }

            while (!sortedTasksQueue.empty())
            {
                Task *task = sortedTasksQueue.top();
                sortedTasksQueue.pop();
                std::cout << "Executing task: " << task->getName() << std::endl;

                // Prompt user for completion status
                // CompletionStatus status = getUserCompletionStatus();
                // markTaskCompleted(task, status);
            }

            displayTasks();

            std::cout << "Select an option:" << std::endl;
            std::cout << "1. Add Task" << std::endl;
            std::cout << "2. Delete Task" << std::endl;
            std::cout << "3. View Tasks" << std::endl;
            std::cout << "4. Exit" << std::endl;

            int option;
            std::cout << "Enter your choice (1-4): ";
            std::cin >> option;

            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the input buffer

            switch (option)
            {
            case 1:
                // addNewTask();
                break;
            case 2:
                deleteTaskPrompt();
                break;
            case 3:
                displayTasks();
                break;
            case 4:
                std::cout << "Exiting the program." << std::endl;
                return;
            default:
                std::cout << "Invalid option. Please try again." << std::endl;
                break;
            }
        }
    }

    // Method to display all task details sorted by priority
    void displayTasks()
    {
        std::vector<Task *> allTasks;
        while (!taskList.empty())
        {
            allTasks.push_back(taskList.front());
            taskList.pop_front();
        }

        std::sort(allTasks.begin(), allTasks.end(), [](Task *a, Task *b)
                  { return a->getPriority() > b->getPriority(); });

        std::cout << "All Tasks (Sorted by Priority):\n";
        for (auto &task : allTasks)
        {
            std::cout << "Name: " << task->getName() << ", Description: " << task->getDescription() << ", Deadline: " << task->getDeadline() << ", Status: ";
            float completionStatus = task->getStatus();
            if (completionStatus == 0.0)
                std::cout << "Not started";
            else if (completionStatus == 0.25)
                std::cout << "Just started";
            else if (completionStatus == 0.5)
                std::cout << "Half completed";
            else if (completionStatus == 0.75)
                std::cout << "Almost completed";
            else if (completionStatus == 1.0)
                std::cout << "Finished";
            else
                std::cout << "Unknown";
            // std::cout << std::endl;

            std::cout << "Priority: " << task -> getPriority() << std::endl;
        }
    }

    // Method to prompt user for dependency and add it
    void addDependencyPrompt()
    {
        std::string taskName;
        std::string dependencyName;
        std::cout << "Enter the name of the task: ";
        std::cin >> taskName;
        std::cout << "Enter the name of the dependency task: ";
        std::cin >> dependencyName;
        addDependency(taskName, dependencyName);
    }
};

int main()
{
    TaskManager taskManager;

    // Prompt user to create tasks
    int numTasks;
    std::cout << "Enter the number of tasks: ";
    std::cin >> numTasks;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the input buffer

    for (int i = 0; i < numTasks; ++i)
    {
        std::string name, description;
        int deadline;

        std::cout << "Enter task name: ";
        std::getline(std::cin, name);

        std::cout << "Enter task description: ";
        std::getline(std::cin, description);

        std::cout << "Enter task deadline: ";
        std::cin >> deadline;
       

        double completion = taskManager.getUserCompletionStatus();
        cout << "Completion : " << completion << std::endl;
         std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the input buffer
        Task *newTask = new Task(name, description, deadline, completion);
        taskManager.addTask(newTask);
    }

    // Prompt user to add dependencies
    char addDependencies;
    std::cout << "Do you want to add dependencies between tasks? (Y/N): ";
    std::cin >> addDependencies;

    if (addDependencies == 'Y' || addDependencies == 'y')
    {
        taskManager.addDependencyPrompt();
    }

    // Execute tasks
    taskManager.executeTasks();

    return 0;
}