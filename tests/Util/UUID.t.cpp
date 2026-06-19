#include <BLIB/Util/UUID.hpp>
#include <gtest/gtest.h>
#include <unordered_map>
#include <unordered_set>

namespace bl
{
namespace util
{
namespace unittest
{

TEST(UUID, ManualConstructToString) {
    // Test with known values
    // UUID: 1cd26c7a-9430-4735-aea7-b02fe89463b8
    // Breaking this down:
    // part1: 0x1cd26c7a94304735
    // part2: 0xaea7b02fe89463b8
    
    const std::uint64_t part1 = 0x1cd26c7a94304735ULL;
    const std::uint64_t part2 = 0xaea7b02fe89463b8ULL;
    const std::string expected = "1cd26c7a-9430-4735-aea7-b02fe89463b8";
    
    UUID uuid(part1, part2);
    const std::string result = uuid.toString();
    
    EXPECT_EQ(result, expected);
    EXPECT_EQ(uuid.getPart1(), part1);
    EXPECT_EQ(uuid.getPart2(), part2);
}

TEST(UUID, ManualConstructToStringAllZeros) {
    const std::uint64_t part1 = 0x0000000000000000ULL;
    const std::uint64_t part2 = 0x0000000000000000ULL;
    const std::string expected = "00000000-0000-0000-0000-000000000000";
    
    UUID uuid(part1, part2);
    const std::string result = uuid.toString();
    
    EXPECT_EQ(result, expected);
}

TEST(UUID, ManualConstructToStringAllOnes) {
    const std::uint64_t part1 = 0xffffffffffffffffULL;
    const std::uint64_t part2 = 0xffffffffffffffffULL;
    const std::string expected = "ffffffff-ffff-ffff-ffff-ffffffffffff";
    
    UUID uuid(part1, part2);
    const std::string result = uuid.toString();
    
    EXPECT_EQ(result, expected);
}

TEST(UUID, StringConstructToNumbers) {
    const std::string input = "1cd26c7a-9430-4735-aea7-b02fe89463b8";
    const std::uint64_t expectedPart1 = 0x1cd26c7a94304735ULL;
    const std::uint64_t expectedPart2 = 0xaea7b02fe89463b8ULL;
    
    UUID uuid(input);
    
    EXPECT_EQ(uuid.getPart1(), expectedPart1);
    EXPECT_EQ(uuid.getPart2(), expectedPart2);
}

TEST(UUID, StringConstructUppercase) {
    const std::string input = "1CD26C7A-9430-4735-AEA7-B02FE89463B8";
    const std::uint64_t expectedPart1 = 0x1cd26c7a94304735ULL;
    const std::uint64_t expectedPart2 = 0xaea7b02fe89463b8ULL;
    
    UUID uuid(input);
    
    EXPECT_EQ(uuid.getPart1(), expectedPart1);
    EXPECT_EQ(uuid.getPart2(), expectedPart2);
}

TEST(UUID, StringConstructMixedCase) {
    const std::string input = "1Cd26C7a-9430-4735-AeA7-B02fE89463b8";
    const std::uint64_t expectedPart1 = 0x1cd26c7a94304735ULL;
    const std::uint64_t expectedPart2 = 0xaea7b02fe89463b8ULL;
    
    UUID uuid(input);
    
    EXPECT_EQ(uuid.getPart1(), expectedPart1);
    EXPECT_EQ(uuid.getPart2(), expectedPart2);
}

TEST(UUID, RoundTripConversion) {
    // Generate random UUID
    UUID original = UUID::generate();
    
    // Convert to string
    const std::string uuidString = original.toString();
    
    // Construct new UUID from string
    UUID reconstructed(uuidString);
    
    // Validate they are equal
    EXPECT_EQ(original.getPart1(), reconstructed.getPart1());
    EXPECT_EQ(original.getPart2(), reconstructed.getPart2());
    EXPECT_EQ(original, reconstructed);
}

TEST(UUID, RoundTripMultiple) {
    // Test multiple random UUIDs
    for (int i = 0; i < 100; ++i) {
        UUID original = UUID::generate();
        UUID reconstructed(original.toString());
        
        EXPECT_EQ(original, reconstructed) << "Failed on iteration " << i;
    }
}

TEST(UUID, DefaultConstructor) {
    UUID uuid;
    
    EXPECT_EQ(uuid.getPart1(), 0ULL);
    EXPECT_EQ(uuid.getPart2(), 0ULL);
    EXPECT_EQ(uuid.toString(), "00000000-0000-0000-0000-000000000000");
}

TEST(UUID, CopyConstructor) {
    const std::uint64_t part1 = 0x1cd26c7a94304735ULL;
    const std::uint64_t part2 = 0xaea7b02fe89463b8ULL;
    
    UUID original(part1, part2);
    UUID copy(original);
    
    EXPECT_EQ(copy.getPart1(), part1);
    EXPECT_EQ(copy.getPart2(), part2);
    EXPECT_EQ(original, copy);
}

TEST(UUID, AssignmentOperator) {
    const std::uint64_t part1 = 0x1cd26c7a94304735ULL;
    const std::uint64_t part2 = 0xaea7b02fe89463b8ULL;
    
    UUID original(part1, part2);
    UUID assigned;
    assigned = original;
    
    EXPECT_EQ(assigned.getPart1(), part1);
    EXPECT_EQ(assigned.getPart2(), part2);
    EXPECT_EQ(original, assigned);
}

TEST(UUID, EqualityOperator) {
    UUID uuid1(0x1234567890abcdefULL, 0xfedcba0987654321ULL);
    UUID uuid2(0x1234567890abcdefULL, 0xfedcba0987654321ULL);
    UUID uuid3(0x1234567890abcdefULL, 0x0000000000000000ULL);
    
    EXPECT_TRUE(uuid1 == uuid2);
    EXPECT_FALSE(uuid1 == uuid3);
}

TEST(UUID, InvalidStringTooShort) {
    UUID uuid("1cd26c7a-9430-4735-aea7-b02fe8946");
    
    EXPECT_EQ(uuid.getPart1(), 0ULL);
    EXPECT_EQ(uuid.getPart2(), 0ULL);
}

TEST(UUID, InvalidStringTooLong) {
    UUID uuid("1cd26c7a-9430-4735-aea7-b02fe89463b8-extra");
    
    EXPECT_EQ(uuid.getPart1(), 0ULL);
    EXPECT_EQ(uuid.getPart2(), 0ULL);
}

TEST(UUID, InvalidStringMissingHyphen) {
    UUID uuid("1cd26c7a94304735-aea7-b02fe89463b8");
    
    EXPECT_EQ(uuid.getPart1(), 0ULL);
    EXPECT_EQ(uuid.getPart2(), 0ULL);
}

TEST(UUID, InvalidStringWrongHyphenPosition) {
    UUID uuid("1cd26c7a9-430-4735-aea7-b02fe89463b8");
    
    EXPECT_EQ(uuid.getPart1(), 0ULL);
    EXPECT_EQ(uuid.getPart2(), 0ULL);
}

TEST(UUID, InvalidStringInvalidCharacter) {
    UUID uuid("1cd26c7g-9430-4735-aea7-b02fe89463b8");
    
    EXPECT_EQ(uuid.getPart1(), 0ULL);
    EXPECT_EQ(uuid.getPart2(), 0ULL);
}

TEST(UUID, InvalidStringSpecialCharacter) {
    UUID uuid("1cd26c7@-9430-4735-aea7-b02fe89463b8");
    
    EXPECT_EQ(uuid.getPart1(), 0ULL);
    EXPECT_EQ(uuid.getPart2(), 0ULL);
}

TEST(UUID, HashFunction) {
    UUID uuid1(0x1234567890abcdefULL, 0xfedcba0987654321ULL);
    UUID uuid2(0x1234567890abcdefULL, 0xfedcba0987654321ULL);
    UUID uuid3(0x0000000000000000ULL, 0x0000000000000001ULL);
    
    std::hash<UUID> hasher;
    
    // Same UUIDs should have same hash
    EXPECT_EQ(hasher(uuid1), hasher(uuid2));
    
    // Different UUIDs should (very likely) have different hashes
    EXPECT_NE(hasher(uuid1), hasher(uuid3));
}

TEST(UUID, UnorderedMapUsage) {
    std::unordered_map<UUID, int> uuidMap;
    
    UUID uuid1 = UUID::generate();
    UUID uuid2 = UUID::generate();
    UUID uuid3(uuid1);
    
    uuidMap[uuid1] = 42;
    uuidMap[uuid2] = 100;
    
    EXPECT_EQ(uuidMap[uuid1], 42);
    EXPECT_EQ(uuidMap[uuid2], 100);
    EXPECT_EQ(uuidMap[uuid3], 42); // uuid3 is copy of uuid1
}

TEST(UUID, GenerateUniqueness) {
    std::unordered_set<UUID> generatedUuids;
    const int numUuids = 10000;
    
    for (int i = 0; i < numUuids; ++i) {
        UUID uuid = UUID::generate();
        EXPECT_TRUE(generatedUuids.insert(uuid).second) 
            << "Duplicate UUID generated at iteration " << i;
    }
    
    EXPECT_EQ(generatedUuids.size(), static_cast<size_t>(numUuids));
}

TEST(UUID, StringFormatValidation) {
    UUID uuid(0x1234567890abcdefULL, 0xfedcba0987654321ULL);
    const std::string str = uuid.toString();
    
    // Check length
    EXPECT_EQ(str.length(), 36);
    
    // Check hyphen positions
    EXPECT_EQ(str[8], '-');
    EXPECT_EQ(str[13], '-');
    EXPECT_EQ(str[18], '-');
    EXPECT_EQ(str[23], '-');
    
    // Check all other characters are hex digits
    auto isHexDigit = [](char c) {
        return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
    };
    
    for (size_t i = 0; i < str.length(); ++i) {
        if (i == 8 || i == 13 || i == 18 || i == 23) {
            continue; // Skip hyphens
        }
        EXPECT_TRUE(isHexDigit(str[i])) << "Invalid character at position " << i;
    }
}

} // namespace unittest
} // namespace util
} // namespace bl

