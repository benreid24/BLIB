target_sources(BLIB PUBLIC
	Animation2D.hpp
	BatchCircle.hpp
	BatchedShapes2D.hpp
	BatchedSlideshows.hpp
	BatchedSprites.hpp
	BatchIcon.hpp
	BatchRectangle.hpp
	BatchSlideshow.hpp
	BatchSprite.hpp
	BatchTriangle.hpp
	Circle.hpp
	Drawable.hpp
	Dummy2D.hpp
	Icon.hpp
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
