#include "Data.hpp"

#include <BENG/Files.hpp>
#include <cstdlib>
#include <iostream>

namespace
{
bg::json::Schema buildSchema() {
    using namespace bg::json::schema;

    Value point     = Numeric::Any;
    Value pointList = List(point); // any length
    Value name      = String::Any;

    bg::json::Schema schema;
    schema.addRequiredField("name", name);
    schema.addRequiredField("points", pointList);

    return schema;
}
} // namespace

Data::Data(const std::string& name, int pc)
: name(name) {
    points.reserve(pc);
    for (int i = 0; i < pc; ++i) { points.push_back(rand() % 9000 + 1000); }
}

void Data::print(std::ostream& os) const {
    os << name << " -> [";
    if (!points.empty()) os << points[0];
    for (unsigned int i = 1; i < points.size(); ++i) { os << ", " << points[i]; }
    os << "]\n";
}

std::ostream& operator<<(std::ostream& os, const Data& data) {
    data.print(os);
    return os;
}

void Data::saveToBin(const std::string& file) const {
    bg::BinaryFile output(file, bg::BinaryFile::Write);

    output.write(name);
    output.write<uint32_t>(points.size());
    for (unsigned int i = 0; i < points.size(); ++i) { output.write(points[i]); }
}

bool Data::loadFromBin(const std::string& file) {
    bg::BinaryFile input(file, bg::BinaryFile::Read);
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

void Data::saveToJson(const std::string& file) const {
    bg::json::Group data;
    bg::json::List jlist;

    jlist.reserve(points.size());
    for (const auto& p : points) jlist.push_back(p);

    data.addField("name", name);
    data.addField("points", jlist);

    bg::JSON::saveToFile(file, data);
}

bool Data::loadFromJson(const std::string& file) {
    static const bg::json::Schema schema = buildSchema();
    const bg::json::Group data           = bg::JSON::loadFromFile(file);

    if (!schema.validate(data, true)) {
        std::cout << "Schema validation failed\n";
        return false;
    }

    points.clear();

    // fields return std::optional, but schema ensures they are present
    name                       = data.getString("name").value();
    const bg::json::List plist = data.getList("points").value();
    points.reserve(plist.size());
    for (const bg::json::Value& p : plist) { points.push_back(p.getAsNumeric().value()); }
    return true;
}