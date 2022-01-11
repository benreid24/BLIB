#include <BLIB/Scripts/ReferenceValue.hpp>
#include <BLIB/Scripts/Value.hpp>

namespace bl
{
namespace script
{
namespace
{
Value* get(const std::variant<std::shared_ptr<Value>, Value*>& val) {
    return val.index() == 0 ? std::get_if<std::shared_ptr<Value>>(&val)->get() :
                              *std::get_if<Value*>(&val);
}
} // namespace

ReferenceValue::ReferenceValue(std::shared_ptr<Value>&& ptr)
: value(ptr) {}

ReferenceValue::ReferenceValue(Value&& mv) { value.emplace<std::shared_ptr<Value>>(new Value(mv)); }

ReferenceValue::ReferenceValue(const Value& cv) {
    value.emplace<std::shared_ptr<Value>>(new Value(cv));
}

ReferenceValue::ReferenceValue(Value* val) { value.emplace<Value*>(val); }

const Value& ReferenceValue::deref() const { return *const_cast<ReferenceValue*>(this)->ptr(); }

Value& ReferenceValue::deref() { return *ptr(); }

Value* ReferenceValue::ptr() {
    Value* v = get(value);
    while (v->value().getType() == PrimitiveValue::TRef) { v = get(v->value().getAsRef().value); }
    return v;
}

} // namespace script
} // namespace bl
