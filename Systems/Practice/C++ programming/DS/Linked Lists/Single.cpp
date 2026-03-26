#include <iostream>

template <typename F>
class Node
{
public:
    F value;
    Node *nextNode = nullptr;
};

template <typename F>
Node<F> *createNode(F value)
{
    Node<F> *newNode = new Node<F>;

    newNode->value = value;
    newNode->nextNode = nullptr;

    return newNode;
}

template <typename F>
class List
{
public:
    Node<F> *head = nullptr;

    List() {}

    bool isEmpty()
    {
        return this->head == nullptr ? true : false;
    }

    void addNode(F element)
    {
        if (this->isEmpty())
        {
            this->head = createNode<F>(element);
            return;
        }

        Node<F> *currentNode = this->head;

        while (currentNode->nextNode != nullptr)
        {
            currentNode = currentNode->nextNode;
        }

        Node<F> *newNode = createNode<F>(element);
        currentNode->nextNode = newNode;
    }

    void removeNode()
    {
        if (this->isEmpty())
            throw std::out_of_range("Empty list");

        if (this->head->nextNode == nullptr)
        {
            delete this->head;
            this->head = nullptr;
            return;
        }

        Node<F> *currentNode = this->head,
                *prevNode = nullptr;

        while (currentNode->nextNode != nullptr)
        {
            prevNode = currentNode;
            currentNode = currentNode->nextNode;
        }

        prevNode->nextNode = nullptr;
        delete currentNode;
    }

    Node<F> *findNode(F value)
    {
        if (this->isEmpty())
            throw std::out_of_range("List is empty");

        if (this->head->value == value)
            return this->head;

        Node<F> *currentNode = this->head;

        while (currentNode != nullptr)
        {
            if (currentNode->value == value)
                return currentNode;

            currentNode = currentNode->nextNode;
        }

        return nullptr;
    }

    void traverse()
    {
        if (this->isEmpty())
            throw std::out_of_range("Empty list");

        Node<F> *currentNode = this->head;

        while (currentNode != nullptr)
        {
            std::cout << currentNode->value << std::endl;
            currentNode = currentNode->nextNode;
        }
    }
};

int main()
{
    int values[] = {1, 2, 3, 4, 5, 6};
    List<int> list = List<int>();

    for (int value : values)
    {
        list.addNode(value);
    }

    list.traverse();
    list.removeNode();
    std::cout << std::endl;
    list.traverse();

    Node<int> *nodeFound = list.findNode(3);

    if (nodeFound != nullptr)
        std::cout << "Node found: " << nodeFound->value << " : " << nodeFound << std::endl;
    else
        std::cout << "Node not found\n";
}