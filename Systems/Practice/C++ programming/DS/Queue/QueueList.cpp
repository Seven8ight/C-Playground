#include <iostream>

template <typename F>
class Node
{
public:
    F value;
    Node *nextNode;
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
class Queue
{
public:
    Node<F> *head = nullptr;

    bool isEmpty()
    {
        return this->head == nullptr ? true : false;
    }

    void enqueue(F value)
    {
        Node<F> *newNode = createNode<F>(value);

        if (this->isEmpty())
        {
            this->head = newNode;
            return;
        }

        Node<F> *currentNode = this->head;

        while (currentNode->nextNode != nullptr)
            currentNode = currentNode->nextNode;

        currentNode->nextNode = newNode;
    }

    F dequeue()
    {
        if (this->isEmpty())
            throw std::out_of_range("Stack is empty");

        Node<F> *currentNode = this->head,
                *prevNode = nullptr;

        if (this->head->nextNode)
            this->head = this->head->nextNode;
        else
            this->head = nullptr;

        F value = currentNode->value;

        delete currentNode;

        return value;
    }

    void traverse()
    {
        if (this->isEmpty())
            throw std::out_of_range("List is empty");

        Node<F> *currentNode = this->head;

        while (currentNode != nullptr)
        {
            std::cout << currentNode->value << std::endl;
            currentNode = currentNode->nextNode;
        }
    }

    F peek()
    {
        Node<F> *currentNode = this->head;

        while (currentNode->nextNode != nullptr)
            currentNode = currentNode->nextNode;

        return currentNode->value;
    }

    F frontPeek()
    {
        return this->head->value;
    }
};

int main(int args, char *argc[])
{
    Queue<int> queue = Queue<int>();
    int values[] = {1, 2, 3, 4, 5};

    for (int value : values)
    {
        queue.enqueue(value);
    }

    queue.traverse();
    std::cout << std::endl;

    return 0;
}