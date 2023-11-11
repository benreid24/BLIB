target_sources(BLIB PUBLIC
	Animation2D.hpp
	BatchedShapes2D.hpp
	Circle.hpp
	Drawable.hpp
	Rectangle.hpp
	Slideshow.hpp
	Sprite.hpp
	Text.hpp
	Triangle.hpp
)

add_subdirectory(Animation2D)
add_subdirectory(Components)
add_subdirectory(Shapes2D)
add_subdirectory(Text)
