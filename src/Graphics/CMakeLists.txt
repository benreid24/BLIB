target_sources(BLIB PRIVATE
	Animation2D.cpp
	BatchCircle.cpp
	BatchedShapes2D.cpp
	BatchedSprites.cpp
	BatchIcon.cpp
	BatchRectangle.cpp
	BatchSprite.cpp
	BatchTriangle.cpp
	Circle.cpp
	Dummy2D.cpp
	Icon.cpp
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
