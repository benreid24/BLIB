#ifndef BLIB_GUI_HPP
#define BLIB_GUI_HPP

/**
 * @addtogroup GUI
 * @ingroup Interface
 * @brief Full featured, extensible GUI framework
 *
 */

#include <BLIB/Interfaces/GUI/Event.hpp>
#include <BLIB/Interfaces/GUI/GUI.hpp>
#include <BLIB/Interfaces/GUI/Signal.hpp>

#include <BLIB/Interfaces/GUI/Renderers/DebugRenderer.hpp>
#include <BLIB/Interfaces/GUI/Renderers/DefaultRenderer.hpp>
#include <BLIB/Interfaces/GUI/Renderers/RenderSettings.hpp>
#include <BLIB/Interfaces/GUI/Renderers/Renderer.hpp>
#include <BLIB/Interfaces/GUI/Renderers/RendererUtil.hpp>

#include <BLIB/Interfaces/GUI/Packers/GridPacker.hpp>
#include <BLIB/Interfaces/GUI/Packers/LinePacker.hpp>
#include <BLIB/Interfaces/GUI/Packers/Packer.hpp>

#include <BLIB/Interfaces/GUI/Elements/Container.hpp>
#include <BLIB/Interfaces/GUI/Elements/Element.hpp>

#include <BLIB/Interfaces/GUI/Elements/Animation.hpp>
#include <BLIB/Interfaces/GUI/Elements/Button.hpp>
#include <BLIB/Interfaces/GUI/Elements/Canvas.hpp>
#include <BLIB/Interfaces/GUI/Elements/CheckButton.hpp>
#include <BLIB/Interfaces/GUI/Elements/ComboBox.hpp>
#include <BLIB/Interfaces/GUI/Elements/Image.hpp>
#include <BLIB/Interfaces/GUI/Elements/Label.hpp>
#include <BLIB/Interfaces/GUI/Elements/Notebook.hpp>
#include <BLIB/Interfaces/GUI/Elements/ProgressBar.hpp>
#include <BLIB/Interfaces/GUI/Elements/RadioButton.hpp>
#include <BLIB/Interfaces/GUI/Elements/ScrollArea.hpp>
#include <BLIB/Interfaces/GUI/Elements/SelectBox.hpp>
#include <BLIB/Interfaces/GUI/Elements/Separator.hpp>
#include <BLIB/Interfaces/GUI/Elements/Slider.hpp>
#include <BLIB/Interfaces/GUI/Elements/TextEntry.hpp>
#include <BLIB/Interfaces/GUI/Elements/ToggleButton.hpp>
#include <BLIB/Interfaces/GUI/Elements/Window.hpp>

#include <BLIB/Interfaces/GUI/Dialogs/FilePicker.hpp>
#include <BLIB/Interfaces/GUI/Dialogs/tinyfiledialogs.hpp>

#endif
