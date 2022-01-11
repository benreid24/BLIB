#ifndef BLIB_SCRIPTS_REFERENCEVALUE_HPP
#define BLIB_SCRIPTS_REFERENCEVALUE_HPP

#include <memory>
#include <variant>

namespace bl
{
namespace script
{
class Value;

/**
 * @brief Reference type for values. Wrapper around std::shared_ptr
 *
 * @ingroup Scripts
 *
 */
class ReferenceValue {
public:
    ReferenceValue() = delete;

    /**
     * @brief Constructs a value from a copiable value
     *
     */
    ReferenceValue(const Value& copy);

    /**
     * @brief Constructs a value from a movable value
     *
     */
    ReferenceValue(Value&& move);

    /**
     * @brief Construct the reference from an existing shared_ptr
     *
     */
    ReferenceValue(std::shared_ptr<Value>&& ptr);

    /**
     * @brief Creates a reference value and does not take ownership of the value
     *
     */
    ReferenceValue(Value* value);

    /**
     * @brief Destroy the Value Reference object
     *
     */
    ~ReferenceValue() = default;

    /**
     * @brief Access the value being referred to
     *
     */
    const Value& deref() const;

    /**
     * @brief Access the value being referred to
     *
     */
    Value& deref();

private:
    std::variant<std::shared_ptr<Value>, Value*> value;

    Value* ptr();
};

} // namespace script
} // namespace bl

#endif
