#include <BLIB/Signals.hpp>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace bl
{
namespace sig
{
namespace unittest
{
namespace
{
class IntListener : public Listener<int> {
public:
    virtual ~IntListener() = default;

    MOCK_METHOD(void, process, (const int&), (override));
};

using IntEmitter = Emitter<int>;

class StringListener : public Listener<std::string> {
public:
    virtual ~StringListener() = default;

    MOCK_METHOD(void, process, (const std::string&), (override));
};

using StringEmitter = Emitter<std::string>;

class MultiListener : public Listener<int, std::string> {
public:
    virtual ~MultiListener() = default;

    MOCK_METHOD(void, process, (const int&), (override));
    MOCK_METHOD(void, process, (const std::string&), (override));
};

using MultiEmitter = Emitter<int, std::string>;

class IntListenerWithSubscribe : public Listener<int> {
public:
    IntListenerWithSubscribe(Channel& channel, IntListener& toSubscribe)
    : calledWith(-1)
    , toSubscribe(toSubscribe)
    , channel(channel) {}

    virtual ~IntListenerWithSubscribe() = default;

    virtual void process(const int& signal) override {
        calledWith = signal;
        toSubscribe.subscribeDeferred(channel);
    }

    int calledWith;

private:
    IntListener& toSubscribe;
    Channel& channel;
};

class IntListenerWithBadSubscribe : public Listener<int> {
public:
    IntListenerWithBadSubscribe(Channel& channel, IntListener& toSubscribe)
    : calledWith(-1)
    , toSubscribe(toSubscribe)
    , channel(channel) {}

    virtual ~IntListenerWithBadSubscribe() = default;

    virtual void process(const int& signal) override {
        calledWith = signal;
        toSubscribe.subscribe(channel);
    }

    int calledWith;

private:
    IntListener& toSubscribe;
    Channel& channel;
};

class IntListenerWithBadUnsubscribe : public Listener<int> {
public:
    IntListenerWithBadUnsubscribe()
    : calledWith(-1) {}

    virtual ~IntListenerWithBadUnsubscribe() = default;

    virtual void process(const int& signal) override {
        calledWith = signal;
        unsubscribe();
    }

    int calledWith;
};
} // namespace

TEST(Channel, DispatchSingleType) {
    IntEmitter emitter;
    IntListener listener;
    Channel channel;

    emitter.connect(channel);
    listener.subscribe(channel);
    EXPECT_CALL(listener, process(42)).Times(1);
    emitter.emit(42);
}

TEST(Channel, DispatchMultipleTypes) {
    IntEmitter intEmitter;
    StringEmitter stringEmitter;
    MultiEmitter emitter;
    IntListener intListener;
    StringListener stringListener;
    MultiListener multiListener;
    Channel channel;

    intEmitter.connect(channel);
    stringEmitter.connect(channel);
    emitter.connect(channel);
    intListener.subscribe(channel);
    stringListener.subscribe(channel);
    multiListener.subscribe(channel);

    EXPECT_CALL(intListener, process(42)).Times(1);
    EXPECT_CALL(multiListener, process(42)).Times(1);
    EXPECT_CALL(intListener, process(36)).Times(1);
    EXPECT_CALL(multiListener, process(36)).Times(1);
    emitter.emit(42);
    intEmitter.emit(36);

    EXPECT_CALL(multiListener, process(std::string("s1"))).Times(1);
    EXPECT_CALL(stringListener, process(std::string("s1"))).Times(1);
    EXPECT_CALL(multiListener, process(std::string("s2"))).Times(1);
    EXPECT_CALL(stringListener, process(std::string("s2"))).Times(1);
    emitter.emit<std::string>("s1");
    stringEmitter.emit<std::string>("s2");
}

TEST(Channel, MultipleChannels) {
    Channel channel1;
    Channel channel2;

    IntEmitter emitter1;
    IntListener listener1;

    StringEmitter emitter2;
    StringListener listener2;

    emitter1.connect(channel1);
    listener1.subscribe(channel1);
    EXPECT_CALL(listener1, process(100)).Times(1);
    emitter1.emit(100);

    emitter2.connect(channel2);
    listener2.subscribe(channel2);
    EXPECT_CALL(listener2, process("Hello")).Times(1);
    emitter2.emit<std::string>("Hello");
}

TEST(Channel, Disconnect) {
    Channel channel;
    IntEmitter emitter;
    IntListener listener;

    emitter.connect(channel);
    listener.subscribe(channel);
    EXPECT_CALL(listener, process(42)).Times(1);
    emitter.emit(42);

    emitter.disconnect();
    emitter.emit(100);

    emitter.connect(channel);
    EXPECT_CALL(listener, process(50)).Times(1);
    emitter.emit(50);
}

TEST(Channel, Unsubscribe) {
    Channel channel;
    IntEmitter emitter;
    IntListener listener;

    emitter.connect(channel);
    listener.subscribe(channel);
    EXPECT_CALL(listener, process(42)).Times(1);
    emitter.emit(42);

    listener.unsubscribe();
    emitter.emit(100);

    listener.subscribe(channel);
    EXPECT_CALL(listener, process(50)).Times(1);
    emitter.emit(50);
}

TEST(Channel, MoveChannel) {
    Channel channel;
    IntEmitter emitter;
    IntListener listener;

    emitter.connect(channel);
    listener.subscribe(channel);
    EXPECT_CALL(listener, process(42)).Times(1);
    emitter.emit(42);

    Channel movedChannel(std::move(channel));
    EXPECT_CALL(listener, process(100)).Times(1);
    emitter.emit(100);

    // verify that unsubscribe works after moving
    listener.unsubscribe();
    emitter.emit(200);
}

TEST(Channel, MoveEmitter) {
    Channel channel;
    IntEmitter emitter;
    IntListener listener;

    emitter.connect(channel);
    listener.subscribe(channel);
    EXPECT_CALL(listener, process(42)).Times(1);
    emitter.emit(42);

    IntEmitter movedEmitter(std::move(emitter));
    EXPECT_CALL(listener, process(100)).Times(1);
    emitter.emit(200); // should not go to listener
    movedEmitter.emit(100);
}

TEST(Channel, CopyEmitter) {
    Channel channel;
    IntEmitter emitter;
    IntListener listener;

    emitter.connect(channel);
    listener.subscribe(channel);
    EXPECT_CALL(listener, process(42)).Times(1);
    emitter.emit(42);

    IntEmitter copiedEmitter(emitter);
    EXPECT_CALL(listener, process(200)).Times(1);
    EXPECT_CALL(listener, process(100)).Times(1);
    emitter.emit(200);
    copiedEmitter.emit(100);
}

TEST(Channel, SyncDeferred) {
    Channel channel;
    IntEmitter emitter;
    IntListener listener;

    emitter.connect(channel);
    listener.subscribeDeferred(channel);
    emitter.emit(42);
    channel.syncDeferred();
    EXPECT_CALL(listener, process(100)).Times(1);
    emitter.emit(100);
}

TEST(Channel, SyncDeferredNested) {
    Channel channel;
    IntEmitter emitter;
    IntListener deferredListener;
    IntListenerWithSubscribe listener(channel, deferredListener);

    emitter.connect(channel);
    listener.subscribe(channel);

    EXPECT_FALSE(deferredListener.isSubscribed());
    emitter.emit(42);
    EXPECT_EQ(listener.calledWith, 42);
    EXPECT_FALSE(deferredListener.isSubscribed());

    emitter.emit(69);
    EXPECT_EQ(listener.calledWith, 69);

    channel.syncDeferred();
    EXPECT_TRUE(deferredListener.isSubscribed());
    EXPECT_CALL(deferredListener, process(100)).Times(1);
    emitter.emit(100);
    EXPECT_EQ(listener.calledWith, 100);
}

TEST(Channel, AutoDeferBadSubscribe) {
    Channel channel;
    IntEmitter emitter;
    IntListener deferredListener;
    IntListenerWithBadSubscribe listener(channel, deferredListener);

    emitter.connect(channel);
    listener.subscribe(channel);

    EXPECT_FALSE(deferredListener.isSubscribed());
    emitter.emit(42);
    EXPECT_EQ(listener.calledWith, 42);
    EXPECT_FALSE(deferredListener.isSubscribed());

    emitter.emit(69);
    EXPECT_EQ(listener.calledWith, 69);

    channel.syncDeferred();
    EXPECT_TRUE(deferredListener.isSubscribed());
    EXPECT_CALL(deferredListener, process(100)).Times(1);
    emitter.emit(100);
    EXPECT_EQ(listener.calledWith, 100);
}

TEST(Channel, AutoDeferBadUnsubscribe) {
    Channel channel;
    IntEmitter emitter;
    IntListenerWithBadUnsubscribe listener;

    emitter.connect(channel);
    listener.subscribe(channel);

    EXPECT_TRUE(listener.isSubscribed());
    emitter.emit(42);
    EXPECT_EQ(listener.calledWith, 42);
    EXPECT_FALSE(listener.isSubscribed());

    emitter.emit(69);
    EXPECT_EQ(listener.calledWith, 69);

    channel.syncDeferred();
    emitter.emit(100);
    EXPECT_EQ(listener.calledWith, 69);
}

TEST(Channel, ScopeCleanupEmitterListener) {
    Channel channel;
    IntEmitter outerEmitter;
    IntListener outerListener;
    outerEmitter.connect(channel);
    outerListener.subscribe(channel);

    {
        IntEmitter emitter;
        IntListener listener;
        emitter.connect(channel);
        listener.subscribe(channel);
        EXPECT_CALL(listener, process(42)).Times(1);
        EXPECT_CALL(listener, process(100)).Times(1);
        EXPECT_CALL(outerListener, process(42)).Times(1);
        EXPECT_CALL(outerListener, process(100)).Times(1);
        emitter.emit(42);
        outerEmitter.emit(100);
    }

    EXPECT_TRUE(outerEmitter.isConnected());
    EXPECT_TRUE(outerListener.isSubscribed());
    EXPECT_CALL(outerListener, process(200)).Times(1);
    outerEmitter.emit(200);
}

TEST(Channel, ScopeCleanupChannel) {
    IntEmitter outerEmitter;
    IntListener outerListener;

    {
        Channel channel;
        IntEmitter emitter;
        IntListener listener;
        emitter.connect(channel);
        listener.subscribe(channel);
        outerEmitter.connect(channel);
        outerListener.subscribe(channel);
        EXPECT_CALL(listener, process(42)).Times(1);
        EXPECT_CALL(listener, process(100)).Times(1);
        EXPECT_CALL(outerListener, process(42)).Times(1);
        EXPECT_CALL(outerListener, process(100)).Times(1);
        emitter.emit(42);
        outerEmitter.emit(100);
    }

    EXPECT_FALSE(outerEmitter.isConnected());
    EXPECT_FALSE(outerListener.isSubscribed());
    outerEmitter.emit(200);
}

TEST(Channel, Shutdown) {
    Channel channel;
    IntEmitter emitter;
    IntListener listener;

    emitter.connect(channel);
    listener.subscribe(channel);

    EXPECT_CALL(listener, process(42)).Times(1);
    emitter.emit(42);

    IntListener deferredListener;
    deferredListener.subscribeDeferred(channel);
    channel.shutdown();
    channel.syncDeferred();
    EXPECT_TRUE(channel.isShutdown());
    EXPECT_FALSE(emitter.isConnected());
    EXPECT_FALSE(listener.isSubscribed());
    EXPECT_FALSE(deferredListener.isSubscribed());

    // Should not crash or throw
    emitter.emit(100);
    listener.unsubscribe();

    listener.subscribe(channel);
    EXPECT_FALSE(listener.isSubscribed());

    emitter.connect(channel);
    EXPECT_FALSE(emitter.isConnected());
}

TEST(Channel, Parenting) {
    Channel shutdownParent;
    Channel parent;
    Channel failedParent;
    Channel child;

    EXPECT_FALSE(child.isShutdown());

    shutdownParent.shutdown();
    child.setParent(shutdownParent);
    EXPECT_FALSE(child.hasParent());

    child.setParent(parent);
    EXPECT_TRUE(child.hasParent());
    EXPECT_EQ(&child.getParent(), &parent);

    child.setParent(failedParent);
    EXPECT_EQ(&child.getParent(), &parent);

    parent.shutdown();
    EXPECT_TRUE(child.isShutdown());
}

} // namespace unittest
} // namespace sig
} // namespace bl
