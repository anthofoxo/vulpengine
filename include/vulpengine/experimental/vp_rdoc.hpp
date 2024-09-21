#pragma once

namespace vulpengine::experimental::rdoc {
	void setup(bool load);
	bool is_attached();
	bool is_target_control_connected();
	bool is_frame_capturing();
	void trigger_capture();
	void launch_replay_ui();
}