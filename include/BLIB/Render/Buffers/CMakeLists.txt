target_sources(BLIB PUBLIC
    AlignedBuffer.hpp
    Alignment.hpp
	BatchIndexBuffer.hpp
	BufferDoubleHostVisibleSourced.hpp
	BufferDoubleHostVisible.hpp
	BufferDoubleStaged.hpp
	BufferSingleDeviceLocalSourced.hpp
	IndexBuffer.hpp
	VertexBuffer.hpp
)

add_subdirectory(Interfaces)