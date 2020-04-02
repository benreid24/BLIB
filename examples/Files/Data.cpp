#include "Data.hpp"

#include <BLIB/Files.hpp>
#include <cstdlib>
#include <iostream>

Data::Data(const std::string& name, int pc)
: name(name) {
    points.reserve(pc);
    for (int i = 0; i < pc; ++i) { points.push_back(rand() % 10000 + 1000); }
}

void Data::print() const {
    std::cout << name << " -> [";
    if (!points.empty()) std::cout << points[0];
    for (unsigned int i = 1; i < points.size(); ++i) { std::cout << ", " << points[i]; }
    std::cout << "]\n";
}

void Data::saveToBin(const std::string& file) const {
    bl::BinaryFile output(file, bl::BinaryFile::Write);

    output.write(name);
    output.write<uint32_t>(points.size());
    for (unsigned int i = 0; i < points.size(); ++i) { output.write(points[i]); }
}

bool Data::loadFromBin(const std::string& file) {
    bl::BinaryFile input(file, bl::BinaryFile::Read);
    if (!input.good()) return false;

    if (!input.read(name)) return false;

    uint32_t len;
    if (!input.read(len)) return false;

    points.resize(len);
    for (unsigned int i = 0; i < len; ++i) {
        if (!input.read(points[i])) return false;
    }

    return true;
}