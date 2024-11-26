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
				std::regex const pattern(aPattern, std::regex_constants::icase);
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
		void rank_commands(std::string const& aFilter, decltype(CommandPalette::mCommands)& aCommands) {
			for (auto& [pCommand, score] : aCommands) {
				score = fuzzy_score(aFilter, pCommand->detail);
			}

			std::sort(aCommands.begin(), aCommands.end(), [](auto const& a, auto const& b) {
				if (a.second == b.second) return a.first->detail.size() < b.first->detail.size();
				else return a.second > b.second;
			});
		}
	}

	void CommandPalette::Command::draw_gui(CommandPalette& aPalette, char const* aName, int aScore) {
		char const* shortcutString = shortcut ? ImGui::GetKeyChordName(shortcut) : nullptr;
		bool selectedCopy = showCheckbox();

		if (ImGui::MenuItem(aName ? aName : detail.c_str(), shortcutString, &selectedCopy, enabled)) {
			trigger(aPalette);
		}

		if (aScore != -1) {
			ImGui::SameLine();
			ImGui::Text("%d", aScore);
		}
	}

	bool CommandPalette::Command::trigger(CommandPalette& aPalette) {
		if (!enabled) return false;
		aPalette.mVisible = false;
		onAction();
		return true;
	}

	// If enabled, when the menu items are pushed
	void CommandPalette::command_register(Command* aCommand) {
		mCommands.emplace_back(aCommand, 0);
	}

	void CommandPalette::render() {
		if (mChord && ImGui::IsKeyChordPressed(mChord))
			mVisible ^= true;

		if (ImGui::IsKeyChordPressed(ImGuiKey_Escape))
			mVisible = false;

		// Check global shortcuts
		for (auto& [pCommand, score] : mCommands) {
			pCommand->onUpdate(*pCommand);

			if (mRespondShortcuts) {
				if (pCommand->enabled && pCommand->shortcut) {
					if (ImGui::GetShortcutRoutingData(pCommand->shortcut)->RoutingCurr == ImGuiKeyOwner_NoOwner && ImGui::IsKeyChordPressed(pCommand->shortcut)) {
						mVisible = false;
						pCommand->onAction();
						continue;
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

		bool shouldRankCommands = false;
		bool shouldTriggerFirst = false;

		auto onEdit = [](ImGuiInputTextCallbackData* data) -> int {
			*reinterpret_cast<bool*>(data->UserData) = true;
			return 0;
		};

		// Since there's no padding, we need to manually place dummy objects to pad out the text box
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 0.0f });
		ImGui::Dummy({ 0.0f, ImGui::GetStyle().WindowPadding.y });
		ImGui::Dummy({ ImGui::GetStyle().WindowPadding.x, 0.0f });
		ImGui::SameLine();

		// Input Box
		ImGui::SetNextItemWidth(ImGui::GetWindowSize().x - ImGui::GetStyle().WindowPadding.x * 2.0f);
		if (ImGui::IsWindowAppearing() || mAssertFilterFocus) {
			mAssertFilterFocus = false;
			ImGui::SetKeyboardFocusHere();
		}

		if (ImGui::InputText("##Input", &mFilter, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackEdit, onEdit, &shouldRankCommands)) {
			shouldTriggerFirst = true;
		}

		if (shouldRankCommands) {
			rank_commands(mFilter, mCommands);
		}

		// Finish manually padding
		ImGui::Dummy({ 0.0f, ImGui::GetStyle().WindowPadding.y });
		ImGui::Separator();
		ImGui::PopStyleVar();

		// Use child window for its own scrolling, use window padding inside this element
		// This workaround is needed since MenuItem doesn't behave correctly in a child window
		ImGui::BeginChild("SearchResults", ImVec2(0.0f, mSize.y * viewport.y), ImGuiChildFlags_NavFlattened | ImGuiChildFlags_AlwaysUseWindowPadding);

		int i = 0;
		for (auto& [pCommand, fuzzyScore] : mCommands) {
			if (i == 0 && shouldTriggerFirst) { 
				if (pCommand->trigger(*this)) break;
				else mAssertFilterFocus = true;
			}

			if (mUseAltBindings && i < 10) ImGui::SetNextItemShortcut(ImGuiMod_Alt | (ImGuiKey_1 + i), ImGuiInputFlags_RouteAlways);
			pCommand->draw_gui(*this, nullptr, mShowScores ? fuzzyScore : -1);
			++i;
		}

		ImGui::EndChild();
		ImGui::End();
	}
}
#endif // VP_LIB_IMGUI