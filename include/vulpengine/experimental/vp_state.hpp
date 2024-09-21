#pragma once

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