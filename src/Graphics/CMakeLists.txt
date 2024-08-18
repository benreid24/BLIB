target_sources(BLIB PRIVATE
	Animation2D.cpp
	BatchCircle.cpp
	BatchedShapes2D.cpp
	BatchedSlideshows.cpp
	BatchedSprites.cpp
	BatchIcon.cpp
	BatchRectangle.cpp
	BatchSlideshow.cpp
	BatchSlideshowSimple.cpp
	BatchSprite.cpp
	BatchSpriteSimple.cpp
	BatchTriangle.cpp
	Circle.cpp
	Dummy2D.cpp
	Icon.cpp
	Rectangle.cpp
	Slideshow.cpp
	Sprite.cpp
	Text.cpp
	Triangle.cpp
	VertexBuffer2D.cpp
)

add_subdirectory(Animation2D)
add_subdirectory(Components)
add_subdirectory(Shapes2D)
add_subdirectory(Text)
