#include <BLIB/Containers/PriorityQueue.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace ctr
{
namespace unittest
{

// Priority function: lower int value = higher priority (returns value directly)
struct AscendingPriority {
    std::uint64_t operator()(int value) const { return static_cast<std::uint64_t>(value); }
};

// Priority function: higher int value = higher priority
struct DescendingPriority {
    std::uint64_t operator()(int value) const {
        return static_cast<std::uint64_t>(1000 - value);
    }
};

// Priority function for a struct, based on a field
struct Item {
    int id;
    int urgency;
};

struct ItemPriority {
    std::uint64_t operator()(const Item& item) const {
        return static_cast<std::uint64_t>(item.urgency);
    }
};

using IntQueue   = PriorityQueue<int, AscendingPriority>;
using IntQueueD  = PriorityQueue<int, DescendingPriority>;
using ItemQueue  = PriorityQueue<Item, ItemPriority>;

TEST(PriorityQueue, DefaultEmpty) {
    IntQueue q;
    EXPECT_TRUE(q.empty());
    EXPECT_EQ(q.size(), 0u);
}

TEST(PriorityQueue, PushSingleElement) {
    IntQueue q;
    q.push(5);
    EXPECT_FALSE(q.empty());
    EXPECT_EQ(q.size(), 1u);
    EXPECT_EQ(q.front(), 5);
}

TEST(PriorityQueue, PopSingleElement) {
    IntQueue q;
    q.push(5);
    q.pop();
    EXPECT_TRUE(q.empty());
    EXPECT_EQ(q.size(), 0u);
}

TEST(PriorityQueue, FrontReturnsHighestPriorityAscending) {
    // AscendingPriority: lower value = lower priority bucket number,
    // front() reads from buckets.back() so higher bucket number = higher priority = larger value
    IntQueue q;
    q.push(3);
    q.push(1);
    q.push(2);
    // front() should be the element with the highest priority value (3)
    EXPECT_EQ(q.front(), 3);
}

TEST(PriorityQueue, FrontReturnsHighestPriorityDescending) {
    // DescendingPriority: lower value -> priority = 1000 - value (higher number = higher priority)
    // so value 1 has priority 999 and should be at front
    IntQueueD q;
    q.push(3);
    q.push(1);
    q.push(2);
    EXPECT_EQ(q.front(), 1);
}

TEST(PriorityQueue, PopDrainsInPriorityOrder) {
    IntQueueD q;
    q.push(10);
    q.push(30);
    q.push(20);
    EXPECT_EQ(q.front(), 10); q.pop();
    EXPECT_EQ(q.front(), 20); q.pop();
    EXPECT_EQ(q.front(), 30); q.pop();
    EXPECT_TRUE(q.empty());
}

TEST(PriorityQueue, PushMultipleSamePriority) {
    IntQueue q;
    q.push(5);
    q.push(5);
    q.push(5);
    EXPECT_EQ(q.size(), 3u);
    EXPECT_EQ(q.front(), 5);
    q.pop();
    EXPECT_EQ(q.size(), 2u);
    q.pop();
    EXPECT_EQ(q.size(), 1u);
    q.pop();
    EXPECT_TRUE(q.empty());
}

TEST(PriorityQueue, EmplaceSingleElement) {
    ItemQueue q;
    q.emplace(Item{1, 10});
    EXPECT_EQ(q.size(), 1u);
    EXPECT_FALSE(q.empty());
    EXPECT_EQ(q.front().id, 1);
    EXPECT_EQ(q.front().urgency, 10);
}

TEST(PriorityQueue, EmplaceMultipleOrdering) {
    ItemQueue q;
    q.emplace(Item{1, 5});
    q.emplace(Item{2, 15});
    q.emplace(Item{3, 10});
    // higher urgency = higher priority = at front
    EXPECT_EQ(q.front().id, 2);
    q.pop();
    EXPECT_EQ(q.front().id, 3);
    q.pop();
    EXPECT_EQ(q.front().id, 1);
    q.pop();
    EXPECT_TRUE(q.empty());
}

TEST(PriorityQueue, ReferenceDerefStar) {
    IntQueue q;
    auto ref = q.push(42);
    EXPECT_EQ(*ref, 42);
}

TEST(PriorityQueue, ReferenceDerefArrow) {
    ItemQueue q;
    auto ref = q.emplace(Item{7, 5});
    EXPECT_EQ(ref->id, 7);
    EXPECT_EQ(ref->urgency, 5);
}

TEST(PriorityQueue, ReferenceEqualitySameRef) {
    IntQueue q;
    auto ref = q.push(1);
    auto copy = ref;
    EXPECT_EQ(ref, copy);
}

TEST(PriorityQueue, ReferenceInequalityDifferentElements) {
    IntQueue q;
    auto ref1 = q.push(1);
    auto ref2 = q.push(2);
    EXPECT_NE(ref1, ref2);
}

TEST(PriorityQueue, DefaultConstructedReferenceIsNulllike) {
    IntQueue::TReference ref;
    // A default-constructed reference should not crash on construction
    // and should compare not-equal to a valid reference
    IntQueue q;
    auto valid = q.push(1);
    EXPECT_NE(ref, valid);
}

TEST(PriorityQueue, RepositionSamePriority) {
    // When priority hasn't changed, reposition should be a no-op
    IntQueueD q;
    q.push(10);
    auto ref = q.push(20);
    q.push(30);
    // value is 20, urgency unchanged
    ref.reposition();
    EXPECT_EQ(q.size(), 3u);
    EXPECT_EQ(q.front(), 10);
}

TEST(PriorityQueue, RepositionToHigherPriority) {
    // Use a mutable priority function via a struct with external state
    // We simulate by using a custom priority function that reads from the value itself
    // We'll use ItemQueue and mutate the urgency field via the reference, then reposition
    ItemQueue q;
    auto ref1 = q.emplace(Item{1, 5});
    auto ref2 = q.emplace(Item{2, 10});
    // Item{2} is at front (urgency 10 > 5)
    EXPECT_EQ(q.front().id, 2);
    // Mutate item 1's urgency to make it highest priority, then reposition
    ref1->urgency = 20;
    ref1.reposition();
    EXPECT_EQ(q.front().id, 1);
    EXPECT_EQ(q.size(), 2u);
}

TEST(PriorityQueue, RepositionToLowerPriority) {
    ItemQueue q;
    auto ref1 = q.emplace(Item{1, 20});
    q.emplace(Item{2, 10});
    EXPECT_EQ(q.front().id, 1);
    // Lower item 1's urgency so item 2 becomes highest
    ref1->urgency = 5;
    ref1.reposition();
    EXPECT_EQ(q.front().id, 2);
    EXPECT_EQ(q.size(), 2u);
}

TEST(PriorityQueue, SizeTracksCorrectly) {
    IntQueue q;
    EXPECT_EQ(q.size(), 0u);
    q.push(1);
    EXPECT_EQ(q.size(), 1u);
    q.push(2);
    EXPECT_EQ(q.size(), 2u);
    q.push(2); // same priority bucket
    EXPECT_EQ(q.size(), 3u);
    q.pop();
    EXPECT_EQ(q.size(), 2u);
    q.pop();
    EXPECT_EQ(q.size(), 1u);
    q.pop();
    EXPECT_EQ(q.size(), 0u);
    EXPECT_TRUE(q.empty());
}

TEST(PriorityQueue, MultipleBucketsAllDrained) {
    IntQueueD q;
    for (int i = 1; i <= 5; ++i) { q.push(i); }
    EXPECT_EQ(q.size(), 5u);
    for (int expected = 1; expected <= 5; ++expected) {
        EXPECT_EQ(q.front(), expected);
        q.pop();
    }
    EXPECT_TRUE(q.empty());
}

TEST(PriorityQueue, PushReturnsValidReferenceAfterMultiplePushes) {
    IntQueue q;
    auto ref1 = q.push(10);
    auto ref2 = q.push(20);
    auto ref3 = q.push(30);
    EXPECT_EQ(*ref1, 10);
    EXPECT_EQ(*ref2, 20);
    EXPECT_EQ(*ref3, 30);
}

TEST(PriorityQueue, FrontConstAccess) {
    IntQueue q;
    q.push(7);
    const IntQueue& cq = q;
    EXPECT_EQ(cq.front(), 7);
}

TEST(PriorityQueue, MixedPushAndEmplace) {
    ItemQueue q;
    q.push(Item{1, 10});
    q.emplace(Item{2, 20});
    q.push(Item{3, 15});
    EXPECT_EQ(q.front().id, 2);
    q.pop();
    EXPECT_EQ(q.front().id, 3);
    q.pop();
    EXPECT_EQ(q.front().id, 1);
    q.pop();
    EXPECT_TRUE(q.empty());
}

} // namespace unittest
} // namespace ctr
} // namespace bl
