#include "vulpengine/experimental/vp_command_palette.hpp"

#ifdef VP_LIB_IMGUI

#include <regex>
#include <ranges>
#include <misc/cpp/imgui_stdlib.h>
#include <imgui_internal.h> // GetKeyChordName

namespace vulpengine::experimental {
	namespace {
		int count_matches_in_regex(std::string const& str, std::string const& aPattern) {
			try {
				std::regex const pattern(aPattern);
				auto const matchBegin = std::sregex_iterator(str.begin(), str.end(), pattern);
				auto const matchEnd = std::sregex_iterator();
				return std::distance(matchBegin, matchEnd);
			}
			// On regex failure, assume 0 matches
			catch (std::regex_error const&) {
				return 0;
			}
		}

		std::string to_lower(std::string str) {
			for (auto& c : str) c = std::tolower(c);
			return str;
		}

		// This fuzzy scoring works using the following method
		// Split the filter by space ' ' and for each token the scoring is as follows:
		// * If the string contains the token (case insensitive) ; score += 100
		// * If the token can be used as a regex, do so ; score += num_regex_matches
		int fuzzy_score(std::string const& aFilter, std::string text) {
			std::vector<std::string> patterns = aFilter | std::ranges::views::split(' ') | std::ranges::to<std::vector<std::string>>();
			int score = 0;

			for (auto const& pattern : patterns) {
				if (pattern.empty()) continue;
				if (to_lower(text).contains(to_lower(pattern))) score += 100;
				score += count_matches_in_regex(text, pattern);
			}

			return score;
		}

		// Ranks all the commands via `fuzzy_score`. The vector is then sorted using this value
		void rank_commands(std::string const& aFilter, CommandPalette::CommandSort& aCommandSorted, CommandPalette::CommandStore const& aCommandStore) {
			for (auto& [cmdIdx, score] : aCommandSorted) {
				score = fuzzy_score(aFilter, aCommandStore.at(cmdIdx).detail);
			}

			std::sort(aCommandSorted.begin(), aCommandSorted.end(), [](auto const& a, auto const& b) {
				return a.second > b.second;
			});
		}

		// Draw the menu item gui
		void command_gui(CommandPalette::Command& cmd, CommandPalette& aPalette, char const* name = nullptr) {
			char const* shortcutString = cmd.shortcut ? ImGui::GetKeyChordName(cmd.shortcut) : nullptr;
			bool selectedCopy = cmd.selected; // Intentially copy

			if (ImGui::MenuItem(name ? name : cmd.detail.c_str(), shortcutString, &selectedCopy, cmd.enabled)) {
				aPalette.mVisible = false;
				if (cmd.stateSwap) cmd.selected ^= true;
				cmd.onAction();
			}
		}

		void push_menu(CommandPalette& aPalette, std::string str, CommandPalette::Command& cmd) {
			size_t pos = str.find_first_of('/');

			if (pos == std::string::npos) {
				command_gui(cmd, aPalette, str.substr(pos + 1).c_str());
			}
			else {
				if (ImGui::BeginMenu(str.substr(0, pos).c_str())) {
					push_menu(aPalette, str.substr(pos + 1), cmd);
					ImGui::EndMenu();
				}
			}
		}
	}

	CommandPalette::Command& CommandPalette::command_get(std::string const& cmdId) {
		return mCommandStore[mCommandMap.at(cmdId)];
	}

	// If enabled, when the menu items are pushed
	// They will be pushed in the order commands are registered
	void CommandPalette::command_register(std::string const& cmdId, Command&& cmd) {
		size_t const cmdIdx = mCommandStore.size();
		mCommandMap[cmdId] = cmdIdx;
		mCommandStorted.emplace_back(cmdIdx, 0);
		mCommandStore.push_back(std::move(cmd));
	}

	void CommandPalette::render() {
		if (mChord && ImGui::IsKeyChordPressed(mChord))
			mVisible ^= true;

		if (ImGui::IsKeyChordPressed(ImGuiKey_Escape))
			mVisible = false;

		if (mMenuMenuBarCarat) {
			if (ImGui::BeginMainMenuBar()) {
				if (ImGui::MenuItem(">")) mVisible = true;
				ImGui::EndMainMenuBar();
			}
		}

		// Check global shortcuts
		for (auto& command : mCommandStore) {
			command.onUpdate(command);

			if (mRespondShortcuts) {
				if (command.enabled && command.shortcut) {
					if (ImGui::GetShortcutRoutingData(command.shortcut)->RoutingCurr == ImGuiKeyOwner_NoOwner && ImGui::IsKeyChordPressed(command.shortcut)) {
						mVisible = false;
						command.onAction();
						break;
					}
				}
			}
			
			if (mInsertMenuBarItems) {
				if (!command.path.empty()) {
					if (ImGui::BeginMainMenuBar()) {
						push_menu(*this, command.path, command);
						ImGui::EndMainMenuBar();
					}
				}
			}
		}

		if (!mVisible) return;

		// Auto position the command palette
		auto viewport = ImGui::GetMainViewport()->Size;
		ImGui::SetNextWindowPos(ImVec2(viewport.x / 2.0f, 0.0f), ImGuiCond_None, ImVec2(0.5f, 0.0f));
		ImGui::SetNextWindowSize(ImVec2(viewport.x * mSize.x, 0.0f));

		// No padding on the main window
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		if (!ImGui::Begin("Command Palette", &mVisible, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoDocking)) {
			ImGui::End();
			ImGui::PopStyleVar();
			return;
		}
		ImGui::PopStyleVar();

		if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows)) mVisible = false;

		// Since there's no padding, we need to manually place dummy objects to pad out the text box
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 0.0f });
		ImGui::Dummy({ 0.0f, ImGui::GetStyle().WindowPadding.y });
		ImGui::Dummy({ ImGui::GetStyle().WindowPadding.x, 0.0f });
		ImGui::SameLine();

		// Input Box
		ImGui::SetNextItemWidth(ImGui::GetWindowSize().x - ImGui::GetStyle().WindowPadding.x * 2.0f);
		if (ImGui::IsWindowAppearing()) {
			ImGui::SetKeyboardFocusHere();
			mFilter = "";
		}
		if (ImGui::InputText("##Input", &mFilter)) rank_commands(mFilter, mCommandStorted, mCommandStore);

		// Finish manually padding
		ImGui::Dummy({ 0.0f, ImGui::GetStyle().WindowPadding.y });
		ImGui::Separator();
		ImGui::PopStyleVar();

		// Use child window for its own scrolling, use window padding inside this element
		// This workaround is needed since MenuItem doesn't behave correctly in a child window
		ImGui::BeginChild("SearchResults", ImVec2(0.0f, mSize.y * viewport.y), ImGuiChildFlags_NavFlattened | ImGuiChildFlags_AlwaysUseWindowPadding);

		int i = 0;
		for (auto& [cmdIdx, fuzzyScore] : mCommandStorted) {
			Command& cmd = mCommandStore.at(cmdIdx);
			if (mUseAltBindings && i < 10) ImGui::SetNextItemShortcut(ImGuiMod_Alt | (ImGuiKey_1 + i), ImGuiInputFlags_RouteAlways);
			command_gui(cmd, *this, nullptr);
			++i;
		}

		ImGui::EndChild();
		ImGui::End();
	}
}
#endif // VP_LIB_IMGUI