#pragma once

// To enable loading the RenderDoc shared library at startup
// #define VP_FEATURE_RDOC_UNSUPPORTED
// 
// This method of attaching RenderDoc is unsupported by the RenderDoc developers and may break.

namespace vulpengine::rdoc {
	void setup(bool load);
	bool is_attached();
	bool is_target_control_connected();
	bool is_frame_capturing();
	void trigger_capture();
	void launch_replay_ui();
}