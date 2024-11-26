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
			// Description visible in command palette
			std::string detail;
			// Enables or disables the interaction
			bool enabled = true;
			// Implement this function if a checkbox wants to be shown
			std::function<bool()> showCheckbox = []() { return false; };
			// The main action
			std::function<void()> onAction = []() {};
			// Performed every frame
			std::function<void(Command&)> onUpdate = [](Command&) {};

			void draw_gui(CommandPalette& aPalette, char const* aName = nullptr, int aScore = -1);
			bool trigger(CommandPalette& aPalette);
		};
	public:
		void command_register(Command* aCommand);
		inline void open() { mVisible = true; }
		inline void close() { mVisible = false; }
		void render();
	public:
		// The key chord in which the palette will be opened
		// Set to 0 to disable this option
		ImGuiKeyChord mChord = ImGuiMod_Ctrl | ImGuiMod_Shift | ImGuiKey_P;

		// The size of the command palette
		// Multiplier for the viewport size
		ImVec2 mSize = ImVec2(0.3f, 0.5f);

		// If true, will check for the associated keybinds
		bool mRespondShortcuts = true;

		// When the command palette is open, the first 9 items
		// will have Alt+1 through Alt+9 bound to them
		bool mUseAltBindings = true;

		// If the command palette is visble
		bool mVisible = false;

		// If true, the results of the fuzzy scoring step are shown next to the gui
		bool mShowScores = false;

		// Filter used for fuzzy search
		std::string mFilter;

		// Command storage
		std::vector<std::pair<Command*, int>> mCommands;

		// Internal usage, if the filter enter key matches a disabled item, setting this to true will refocus the filter
		bool mAssertFilterFocus = false;
	};
}
#endif // VP_LIB_IMGUI