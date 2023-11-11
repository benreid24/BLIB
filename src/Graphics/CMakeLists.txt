target_sources(BLIB PRIVATE
	Animation2D.cpp
	BatchedShapes2D.cpp
	Circle.cpp
	Rectangle.cpp
	Slideshow.cpp
	Sprite.cpp
	Text.cpp
	Triangle.cpp
)

add_subdirectory(Animation2D)
add_subdirectory(Components)
add_subdirectory(Shapes2D)
add_subdirectory(Text)
