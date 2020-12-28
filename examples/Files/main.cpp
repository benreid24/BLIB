#include "Data.hpp"

#include <BENG/Files.hpp>
#include <iostream>

int main() {
    Data data("testData", 10);
    std::cout << "Initial data: " << data << std::endl;

    std::cout << "Saving data to 'data.bin'\n";
    data.saveToBin("data.bin");
    std::cout << "Loading data... ";
    if (!data.loadFromBin("data.bin")) {
        std::cout << "FAILED\n";
        return 1;
    }
    std::cout << "success\n";
    std::cout << "Loaded data: " << data << std::endl;

    std::cout << "Saving data to 'data.json'\n";
    data.saveToJson("data.json");
    std::cout << "Loading data... ";
    if (!data.loadFromJson("data.json")) {
        std::cout << "FAILED\n";
        return 1;
    }
    std::cout << "success\n";
    std::cout << "Loaded data: " << data << std::endl;

    return 0;
}