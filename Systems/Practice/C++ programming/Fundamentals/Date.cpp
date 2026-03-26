#include <iostream>
#include <ctime>

typedef struct tm Date;

time_t createTimestamp(time_t timestamp = NULL)
{
    time_t currentTimestamp = time(timestamp != NULL ? &timestamp : &currentTimestamp);

    return currentTimestamp;
}

Date createDate()
{
    Date currentDate;

    currentDate.tm_year = 2026;
    currentDate.tm_mon = 3;
    currentDate.tm_mday = 25;
    currentDate.tm_hour = 11;
    currentDate.tm_min = 22;
    currentDate.tm_sec = 34;
    currentDate.tm_isdst = 0;

    mktime(&currentDate);

    return currentDate;
}

int main()
{
    clock_t before = clock();
    time_t timestamp = createTimestamp();
    Date date = createDate();

    std::cout << ctime(&timestamp);
    std::cout << asctime(&date);
    clock_t duration = clock() - before;

    std::cout << "The program takes: " << (float)duration / CLOCKS_PER_SEC << "seconds \n";

    try
    {
        int age = 14;

        if (age < 18)
            throw(age);

        std::cout << "Welcome to the club, oh im feeling good";
    }
    catch (...)
    {
        std::cout << "You must be of age 18 years and older.";
    }

    return 0;
}