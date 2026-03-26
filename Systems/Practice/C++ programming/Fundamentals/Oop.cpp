#include <iostream>

class Vehicle
{
private:
    std::string engineManufacturer;

public:
    std::string name;
    int maxSpeed, torque;

    Vehicle(std::string name, int speed, int torque)
    {
        this->name = name;
        this->maxSpeed = speed;
        this->torque = torque;
    }

    virtual void displayInformation();

    void vehicleManufacturer(std::string engineManufacturer)
    {
        if (engineManufacturer.length() == 0)
        {
            std::cout << "Invalid engine manufacturer\n";
            return;
        }

        this->engineManufacturer = engineManufacturer;
    }
};

void Vehicle::displayInformation()
{
    std::cout << "------------------VEHICLE------------------\n";
    std::cout << "Vehicle name: " << this->name << "\n";
    std::cout << "Vehicle speed: " << this->maxSpeed << "\n";
    std::cout << "Vehicle torque: " << this->torque << "\n";
    std::cout << "-------------------------------------------\n";
}

class Car : public Vehicle
{
public:
    std::string brand;
    std::string type;
    std::string model;

    Car(std::string name, int speed, int torque, std::string brand, std::string type, std::string model) : Vehicle(name, speed, torque)
    {

        this->brand = brand;
        this->type = type;
        this->model = model;
    }

    void displayInformation() override
    {
        std::cout << "------------------SALOON CAR------------------\n";
        std::cout << "Vehicle name: " << this->name << "\n";
        std::cout << "Vehicle brand: " << this->brand << "\n";
        std::cout << "Vehicle model: " << this->model << "\n";
        std::cout << "Vehicle speed: " << this->maxSpeed << "\n";
        std::cout << "Vehicle torque: " << this->torque << "\n";
        std::cout << "----------------------------------------------\n";
    }
};

int main()
{
    Vehicle *vehicle1;
    Car car = Car("Subaru", 250, 2500, "Subaru Japan", "Saloon", "Outback");
    vehicle1 = &car;

    vehicle1->displayInformation();
    return 0;
}