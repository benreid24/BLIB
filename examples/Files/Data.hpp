#ifndef EXAMPLE_DATA_HPP
#define EXAMPLE_DATA_HPP

#include <string>
#include <vector>

class Data {
public:
    Data(const std::string& name, int pointCount);

    void print() const;

    void saveToJson(const std::string& file) const;
    bool loadFromJson(const std::string& file);

    void saveToBin(const std::string& file) const;
    bool loadFromBin(const std::string& file);

private:
    std::string name;
    std::vector<uint32_t> points;
};

#endif
