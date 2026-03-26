#include <iostream>

template <typename F>
class Queue
{
private:
    F *array = nullptr;
    int capacity;
    int size;

public:
    Queue(int capacity = 10)
    {
        this->array = new F[capacity];
        this->capacity = capacity;
        this->size = 0;
    }

    ~Queue()
    {
        this->capacity = 0;
        this->size = 0;
        delete this->array;
    }

    bool isEmpty()
    {
        return this->size == 0 ? true : false;
    }

    void enqueue(F element)
    {
        if (this->size >= this->capacity)
        {
            this->capacity *= 2;
            F *newArray = new F[this->capacity];

            for (int i = 0; i < this->size; i++)
            {
                newArray[i] = this->array[i];
            }

            delete this->array;
            this->array = newArray;
        }

        this->array[this->size++] = element;
    }

    void dequeue()
    {
        if (this->isEmpty())
            throw std::out_of_range("Queue underflow");

        for (int i = 0; i < this->size; i++)
            this->array[i] = this->array[i + 1];

        this->size--;
    }

    void traverse()
    {
        if (this->isEmpty())
            throw std::out_of_range("Empty queue");

        for (int i = 0; i < this->size; i++)
        {
            std::cout << this->array[i] << std::endl;
        }
    }

    F peek()
    {
        if (this->isEmpty())
            throw std::out_of_range("Empty queue");

        return this->array[this->size - 1];
    }

    F frontPeek()
    {
        if (this->isEmpty())
            throw std::out_of_range("Empty queue");

        return this->array[0];
    }
};

int main()
{
    Queue<int> queue = Queue<int>();
    int values[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

    for (int &value : values)
    {
        queue.enqueue(value);
    }

    queue.traverse();
    queue.dequeue();

    std::cout << "After dequeued" << std::endl;
    queue.traverse();
    std::cout << "First element: " << queue.frontPeek() << std::endl;
    std::cout << "Last element: " << queue.peek() << std::endl;
}