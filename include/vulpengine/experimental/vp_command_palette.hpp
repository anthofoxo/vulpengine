#pragma once

#include "vulpengine/vp_features.hpp"

#ifdef VP_LIB_IMGUI

#include <imgui.h>

#include <utility>
#include <cstddef>
#include <functional>
#include <string>
#include <vector>
#include <unordered_map>

namespace vulpengine::experimental {
	class CommandPalette final {
	public:
		struct Command final {
			// If non-zero will display and respond to global keyboard shortcuts
			ImGuiKeyChord shortcut = 0;
			// If set will appear in the main menu bar. Delimited with `/`
			std::string path;
			// Description visible in command palette
			std::string detail;
			// Enables or disables the interaction
			bool enabled = true;
			// Stored state. Will display a checkbox if true.
			bool selected = false;
			// Enabled changing the select state when clicking the item. Default off
			bool stateSwap = false;
			// The main action
			std::function<void()> onAction = []() {};
			// Performed every frame
			std::function<void(Command&)> onUpdate = [](Command&) {};
		};
	public:
		Command& command_get(std::string const& cmdId);
		void command_register(std::string const& cmdId, Command&& cmd);
		inline void open() { mVisible = true; }
		inline void close() { mVisible = false; }
		void render();
	public:
		// The key chord in which the palette will be opened
		// Set to 0 to disable this option
		ImGuiKeyChord mChord = ImGuiMod_Ctrl | ImGuiMod_Shift | ImGuiKey_P;

		// The size of the command palette
		// Multiplier for the viewport size
		ImVec2 mSize = ImVec2(0.2f, 0.5f);

		// If true a `>` Menu will be pushed to the main menu bar
		// When clicked the pallete will open
		bool mMenuMenuBarCarat = true;

		// If true each command with a valid `path` will be pushed to the main menu bar
		bool mInsertMenuBarItems = true;

		// If true, will check for the associated keybinds
		bool mRespondShortcuts = true;

		// When the command palette is open, the first 9 items
		// will have Alt+1 through Alt+9 bound to them
		bool mUseAltBindings = true;

		// If the command palette is visble
		bool mVisible = false;

		// Filter used for fuzzy search
		std::string mFilter;

		using CommandStore = std::vector<Command>;
		using CommandMap = std::unordered_map<std::string, size_t>;
		using CommandSort = std::vector<std::pair<size_t, int>>;

		// Command storage
		CommandStore mCommandStore;
		// Maps commands from a string to an index into the storage
		CommandMap mCommandMap;
		// Sorted references to the command store, stored with their fuzzy score
		CommandSort mCommandStorted;
	};
}
#endif // VP_LIB_IMGUI