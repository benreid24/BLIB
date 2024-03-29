#ifndef BLIB_INTERFACES_MENU_HPP
#define BLIB_INTERFACES_MENU_HPP

/**
 * @addtogroup Menu
 * @ingroup Interface
 * @brief A mouseless menu that accepts basic inputs and defers rendering to the client
 *
 */

#include <BLIB/Interfaces/Menu/Drivers/KeyboardDriver.hpp>
#include <BLIB/Interfaces/Menu/Drivers/MouseDriver.hpp>
#include <BLIB/Interfaces/Menu/Drivers/MovementDriver.hpp>
#include <BLIB/Interfaces/Menu/Drivers/TriggerDriver.hpp>
#include <BLIB/Interfaces/Menu/Event.hpp>
#include <BLIB/Interfaces/Menu/Item.hpp>
#include <BLIB/Interfaces/Menu/Items/ImageItem.hpp>
#include <BLIB/Interfaces/Menu/Items/SubmenuItem.hpp>
#include <BLIB/Interfaces/Menu/Items/TextItem.hpp>
#include <BLIB/Interfaces/Menu/Items/ToggleTextItem.hpp>
#include <BLIB/Interfaces/Menu/Menu.hpp>
#include <BLIB/Interfaces/Menu/Selector.hpp>
#include <BLIB/Interfaces/Menu/Selectors/ArrowSelector.hpp>
#include <BLIB/Interfaces/Menu/Selectors/NoSelector.hpp>

#endif
