#ifndef BLIB_INTERFACES_MENU_HPP
#define BLIB_INTERFACES_MENU_HPP

/**
 * @addtogroup Menu
 * @ingroup Interface
 * @brief A mouseless menu that accepts basic inputs and defers rendering to the client
 *
 */

#include <BLIB/Interfaces/Menu/Event.hpp>
#include <BLIB/Interfaces/Menu/EventGenerators/KeyboardEventGenerator.hpp>
#include <BLIB/Interfaces/Menu/EventGenerators/MouseEventGenerator.hpp>
#include <BLIB/Interfaces/Menu/Item.hpp>
#include <BLIB/Interfaces/Menu/Menu.hpp>
#include <BLIB/Interfaces/Menu/RenderItem.hpp>
#include <BLIB/Interfaces/Menu/RenderItems/SpriteRenderItem.hpp>
#include <BLIB/Interfaces/Menu/RenderItems/TextRenderItem.hpp>
#include <BLIB/Interfaces/Menu/Renderer.hpp>
#include <BLIB/Interfaces/Menu/Renderers/BasicRenderer.hpp>
#include <BLIB/Interfaces/Menu/Selector.hpp>
#include <BLIB/Interfaces/Menu/Selectors/ArrowSelector.hpp>

#endif
