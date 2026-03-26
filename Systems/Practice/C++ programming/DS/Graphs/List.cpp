#include <iostream>
#include <vector>
#include <unordered_set>
#include <stack>

template <typename F>
class Node
{
public:
    F val;
    Node *nextNode;
    std::vector<Node<F> *> children;

    Node(F value)
    {
        this->val = value;
        this->nextNode = nullptr;
    }
};

template <typename F>
Node<F> *createNode(F value)
{
    Node<F> *newNode = new Node<F>(value);

    return newNode;
}

template <typename F>
class Graph
{
public:
    Node<F> *head;

    Graph()
    {
        this->head = nullptr;
    }

    ~Graph()
    {
        Node<F> *currentNode = this->head;

        while (currentNode != nullptr)
        {
            Node<F> *next = currentNode->nextNode;
            delete currentNode;
            currentNode = next;
        }
    }

    bool isEmpty()
    {
        return this->head == nullptr ? true : false;
    }

    Node<F> *findNode(F value)
    {
        Node<F> *currentNode = this->head;

        while (currentNode != nullptr)
        {
            if (currentNode->val == value)
                return currentNode;

            currentNode = currentNode->nextNode;
        }

        return nullptr;
    }

    void addNode(F value)
    {
        Node<F> *newNode = createNode<F>(value);

        if (this->isEmpty())
        {
            this->head = newNode;
            return;
        }

        Node<F> *currentNode = this->head;

        while (currentNode->nextNode != nullptr)
        {
            currentNode = currentNode->nextNode;
        }

        currentNode->nextNode = newNode;
    }

    void addEdge(F node, F child)
    {
        Node<F> *parent = this->findNode(node),
                *childNode = this->findNode(child);

        if (parent == nullptr || childNode == nullptr)
            throw std::out_of_range("Node could not be found");

        for (auto c : parent->children)
            if (c == childNode)
                return;

        parent->children.push_back(childNode);
        childNode->children.push_back(parent);
    }

    void display()
    {
        if (this->isEmpty())
            throw std::out_of_range("List is empty");

        Node<F> *currentNode = this->head;

        while (currentNode != nullptr)
        {
            std::cout << "Node " << currentNode->val << std::endl;

            if (!currentNode->children.empty())
            {
                std::cout << " Children: ";
                for (const auto element : currentNode->children)
                {
                    std::cout << element->val << " ";
                }
                std::cout << std::endl;
            }

            currentNode = currentNode->nextNode;
        }
    }

    void DFS(F start);
    void BFS(F start);
};

template <typename F>
void Graph<F>::DFS(F startVal)
{
    Node<F> *start = this->findNode(startVal);

    if (!start)
        throw std::out_of_range("Start node not found");

    std::unordered_set<Node<F> *> visited;
    std::stack<Node<F> *> stk;

    stk.push(start);

    while (!stk.empty())
    {
        Node<F> *current = stk.top();
        stk.pop();

        if (visited.count(current))
            continue;

        visited.insert(current);
        std::cout << current->val << " ";

        // push children (reverse order optional)
        for (auto child : current->children)
        {
            if (!visited.count(child))
                stk.push(child);
        }
    }

    std::cout << std::endl;
}

template <typename F>
void Graph<F>::BFS(F startVal)
{
    Node<F> *start = this->findNode(startVal);

    if (!start)
        throw std::out_of_range("Start node not found");

    std::unordered_set<Node<F> *> visited;
    std::queue<Node<F> *> q;

    q.push(start);
    visited.insert(start);

    while (!q.empty())
    {
        Node<F> *current = q.front();
        q.pop();

        std::cout << current->val << " ";

        for (auto child : current->children)
        {
            if (!visited.count(child))
            {
                visited.insert(child);
                q.push(child);
            }
        }
    }

    std::cout << std::endl;
}

int main(int argc, char *args[])
{
    Graph<int> list = Graph<int>();

    for (int i = 0; i < 5; i++)
        list.addNode(i + 1);

    list.addEdge(2, 1);
    list.addEdge(3, 4);
    list.addEdge(3, 1);

    list.display();
    std::cout << std::endl;
    list.BFS(1);
    list.DFS(3);
    return 0;
}