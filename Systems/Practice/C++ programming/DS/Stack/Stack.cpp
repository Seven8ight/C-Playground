#include <iostream>
#include <vector>
#include <map>

template <typename T>
class AbstractStack
{
private:
    std::vector<T> stack;

public:
    AbstractStack() {}

    bool isEmpty()
    {
        return this->stack.size() <= 0 ? true : false;
    }

    size_t size()
    {
        return this->stack.size();
    }

    void addElement(T element)
    {
        this->stack.push_back(element);
    }

    T removeElement()
    {
        if (stack.size() == 0)
            throw std::out_of_range("Stack underflow");

        T element = this->stack.at(this->stack.size() - 1);
        this->stack.pop_back();

        return element;
    }

    void traverseStack()
    {
        for (const T &element : this->stack)
        {
            std::cout << element << std::endl;
        }
    }

    T peek()
    {
        if (stack.size() <= 0)
            throw std::out_of_range("Stack is empty");

        return this->stack.back();
    }
};

template <typename F>
class ManualStack
{
private:
    F *array = nullptr;
    int capacity, stackSize;

public:
    ManualStack(int capacity = 10)
    {
        this->capacity = capacity;
        this->array = new F[this->capacity]();
        this->stackSize = 0;
    }

    ~ManualStack()
    {
        delete[] this->array;
    }

    bool isEmpty()
    {
        return this->stackSize == 0 ? true : false;
    }

    int size()
    {
        return this->stackSize;
    }

    void push(F element)
    {
        if (this->stackSize >= this->capacity)
        {
            this->capacity *= 2;
            F *newArray = new F[this->capacity];

            for (int i = 0; i < this->stackSize; i++)
            {
                newArray[i] = this->array[i];
            }

            delete[] this->array;

            this->array = newArray;

            std::cout << "Array resized" << std::endl;
        }

        this->array[this->stackSize++] = element;
    }

    F pop()
    {
        if (this->isEmpty())
            throw std::out_of_range("Stack is empty");

        F element = this->array[this->stackSize - 1];
        this->stackSize--;

        return element;
    }

    F peek()
    {
        if (this->isEmpty())
            throw std::out_of_range("Stack is empty");

        return this->array[this->stackSize - 1];
    }

    void traverse()
    {
        if (this->isEmpty())
            throw std::out_of_range("Stack is empty");

        for (int i = 0; i < this->stackSize; i++)
            std::cout << this->array[i] << std::endl;
    }
};

bool validParantheses(std::string brackets)
{
    std::map<char, char> parantheses = {
        {'}', '{'},
        {')', '('},
        {']', '['}};

    ManualStack<char> stack;

    for (char letter : brackets)
    {
        if (letter == '{' || letter == '(' || letter == '[')
            stack.push(letter);
        else if (parantheses.find(letter) != parantheses.end())
        {
            if (stack.isEmpty() || stack.pop() != parantheses.at(letter))
                return false;
        }
    }

    return stack.isEmpty();
}

int main()
{
    bool valid = validParantheses("((([]{})))");

    if (valid)
        std::cout << "Valid" << std::endl;
    else
        std::cout << "Invalid" << std::endl;

    return 0;
}