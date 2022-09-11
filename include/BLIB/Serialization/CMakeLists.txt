target_sources(BLIB PUBLIC
    Binary.hpp
    Buffers.hpp
    JSON.hpp
    SerializableField.hpp
    SerializableObject.hpp
)

add_subdirectory(Binary)
add_subdirectory(Buffers)
add_subdirectory(JSON)
