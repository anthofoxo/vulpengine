#pragma once

/*!
State and StateManagers are used to segment large sections of code in a game
Eg: Main Menu, Credits, Splash Screeens

Needs Improvment:
1. Its common to want to pass data through the paramters of update
This can be achieved by making these template classes instead or
by using a more c style approach with a void userdata pointer.

2. Should have on_attach and on_detach functions
Generally the constructor and destructor may be forced to be given
responsibilities it shouldn't handle depending on the state.
Idealy subclasses of State should use the implicit constructor and destructors.
*/

#include <utility>
#include <memory>
#include <cassert>

namespace vulpengine::experimental {
	class State {
	public:
		constexpr State() = default;
		State(State const&) = delete;
		State& operator=(State const&) noexcept = delete;
		State(State&&) = delete;
		State& operator=(State&&) noexcept = delete;
		virtual ~State() noexcept = default;

		virtual void update() = 0;
	};

	class StateManager final {
	public:
		template<class T, class... Args>
		void set(Args&&... args) {
			mState = std::make_unique<T>(std::forward<Args>(args)...);
		}

		inline void update() {
			assert(mState);
			mState->update();
		}

		inline void reset() {
			mState = nullptr;
		}
	private:
		std::unique_ptr<State> mState;
	};
}