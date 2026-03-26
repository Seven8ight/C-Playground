#include <iostream>

template <typename F>
class Node
{
public:
    F value;
    Node *prevNode,
        *nextNode;

    Node() {}
};

template <typename F>
Node<F> *createNode(F element)
{
    Node<F> *newNode = new Node<F>;

    newNode->prevNode = nullptr;
    newNode->nextNode = nullptr;
    newNode->value = element;

    return newNode;
}

template <typename F>
class CList
{
public:
    Node<F> *head = nullptr;

    CList() {}

    bool isEmpty()
    {
        return this->head == nullptr ? true : false;
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

        while (currentNode->nextNode != this->head && currentNode->nextNode != nullptr)
            currentNode = currentNode->nextNode;

        currentNode->nextNode = newNode;
        newNode->prevNode = currentNode;
        newNode->nextNode = this->head;
    }

    void removeNode(F value)
    {
        if (this->isEmpty())
            throw std::out_of_range("List is empty");

        Node<F> *currentNode = this->head;

        if (currentNode->value == value)
        {
            this->head = this->head->nextNode;
            this->head->nextNode->prevNode = nullptr;

            delete currentNode;

            return;
        }

        while (currentNode != nullptr)
        {
            if (currentNode->value == value)
            {
                currentNode->prevNode->nextNode = currentNode->nextNode;
                currentNode->nextNode->prevNode = currentNode->prevNode;

                delete currentNode;

                std::cout << "Node deleted" << std::endl;
                return;
            }
            if (currentNode->nextNode == this->head)
                break;
            currentNode = currentNode->nextNode;
        }
    }

    void traverse()
    {
        if (this->isEmpty())
            throw std::out_of_range("List is empty");

        Node<F> *currentNode = this->head;

        while (currentNode != nullptr)
        {
            std::cout << currentNode->value << std::endl;
            if (currentNode->nextNode == this->head)
                break;
            currentNode = currentNode->nextNode;
        }
    }

    Node<F> *findNode(F value)
    {
        if (this->isEmpty())
            throw std::out_of_range("List is empty");

        Node<F> *currentNode = this->head;

        while (currentNode != nullptr)
        {
            if (currentNode->value == value)
                return currentNode;

            if (currentNode->nextNode == this->head)
                break;

            currentNode = currentNode->nextNode;
        }

        return nullptr;
    }
};

int main()
{
    CList<int> list = CList<int>();
    std::vector<int> values = {1, 2, 3, 4, 5, 6, 7, 8, 9};

    for (int value : values)
    {
        list.addNode(value);
    }

    list.traverse();
    list.removeNode(5);
    std::cout << std::endl;
    list.traverse();
    Node<int> *node = list.findNode(11);

    if (node != nullptr)
        std::cout << "Node found: " << node->value << " : " << node << std::endl;
    else
        std::cout << "Node not found\n";

    return 0;
}