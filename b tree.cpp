#include <iostream>
#include <vector>

enum class CompletionStatus
{
    NOT_STARTED,
    JUST_STARTED,
    HALF_COMPLETED,
    ALMOST_COMPLETED,
    FINISHED
};

struct Task
{
    std::string name;
    std::string description;
    int deadline;
    CompletionStatus status;
};

struct BTreeNode
{
    std::vector<Task> tasks;
    std::vector<BTreeNode *> children;
    bool leaf;
    int t; // Minimum degree
};

BTreeNode *createNode(int t, bool leaf)
{
    BTreeNode *newNode = new BTreeNode();
    newNode->leaf = leaf;
    newNode->t = t;
    return newNode;
}

void splitChild(BTreeNode *parent, int index, BTreeNode *child)
{
    BTreeNode *newNode = createNode(child->t, child->leaf);
    newNode->tasks.resize(child->t - 1);

    for (int i = 0; i < child->t - 1; i++)
    {
        newNode->tasks[i] = child->tasks[i + child->t];
    }

    if (!child->leaf)
    {
        newNode->children.resize(child->t);
        for (int i = 0; i < child->t; i++)
            newNode->children[i] = child->children[i + child->t];
    }

    child->tasks.resize(child->t - 1);

    parent->tasks.insert(parent->tasks.begin() + index, child->tasks[child->t - 1]);
    parent->children.insert(parent->children.begin() + index + 1, newNode);
}

void insertNonFull(BTreeNode *node, const Task &task)
{
    int i = node->tasks.size() - 1;

    if (node->leaf)
    {
        node->tasks.push_back(task);
        while (i >= 0 && node->tasks[i].deadline > task.deadline)
        {
            node->tasks[i + 1] = node->tasks[i];
            i--;
        }
        node->tasks[i + 1] = task;
    }
    else
    {
        while (i >= 0 && node->tasks[i].deadline > task.deadline)
            i--;

        i++;
        if (node->children[i]->tasks.size() == 2 * node->t - 1)
        {
            splitChild(node, i, node->children[i]);
            if (node->tasks[i].deadline < task.deadline)
                i++;
        }
        insertNonFull(node->children[i], task);
    }
}

void insert(BTreeNode *&root, const Task &task)
{
    if (!root)
    {
        root = createNode(3, true);
        root->tasks.push_back(task);
    }
    else
    {
        if (root->tasks.size() == 2 * root->t - 1)
        {
            BTreeNode *newNode = createNode(root->t, false);
            newNode->children.push_back(root);
            splitChild(newNode, 0, root);
            int i = 0;
            if (newNode->tasks[0].deadline < task.deadline)
                i++;
            insertNonFull(newNode->children[i], task);
            root = newNode;
        }
        else
            insertNonFull(root, task);
    }
}

void traverse(BTreeNode *root)
{
    int i;
    for (i = 0; i < root->tasks.size(); i++)
    {
        if (!root->leaf)
            traverse(root->children[i]);
        std::cout << "Task: " << root->tasks[i].name << ", Deadline: " << root->tasks[i].deadline << ", Status: ";
        switch (root->tasks[i].status)
        {
        case CompletionStatus::NOT_STARTED:
            std::cout << "Not Started";
            break;
        case CompletionStatus::JUST_STARTED:
            std::cout << "Just Started";
            break;
        case CompletionStatus::HALF_COMPLETED:
            std::cout << "Half Completed";
            break;
        case CompletionStatus::ALMOST_COMPLETED:
            std::cout << "Almost Completed";
            break;
        case CompletionStatus::FINISHED:
            std::cout << "Finished";
            break;
        }
        std::cout << std::endl;
    }
    if (!root->leaf)
        traverse(root->children[i]);
}

Task *search(BTreeNode *root, int deadline)
{
    int i = 0;
    while (i < root->tasks.size() && deadline > root->tasks[i].deadline)
        i++;
    if (root->tasks[i].deadline == deadline)
        return &(root->tasks[i]);
    if (root->leaf)
        return nullptr;
    return search(root->children[i], deadline);
}

int main()
{
    BTreeNode *root = nullptr;
    Task tasks[] = {
        {"Task1", "Description1", 10, CompletionStatus::NOT_STARTED},
        {"Task2", "Description2", 20, CompletionStatus::JUST_STARTED},
        {"Task3", "Description3", 5, CompletionStatus::HALF_COMPLETED},
        {"Task4", "Description4", 6, CompletionStatus::ALMOST_COMPLETED},
        {"Task5", "Description5", 12, CompletionStatus::FINISHED},
        {"Task6", "Description6", 30, CompletionStatus::NOT_STARTED},
        {"Task7", "Description7", 7, CompletionStatus::HALF_COMPLETED},
        {"Task8", "Description8", 17, CompletionStatus::ALMOST_COMPLETED}};

    for (const Task &task : tasks)
        insert(root, task);

    std::cout << "Traversal of the constructed B-tree is:\n";
    traverse(root);

    int searchDeadline = 6;
    Task *foundTask = search(root, searchDeadline);
    if (foundTask)
        std::cout << "Task with deadline " << searchDeadline << " found: " << foundTask->name << std::endl;
    else
        std::cout << "Task with deadline " << searchDeadline << " not found!" << std::endl;

    return 0;
}
