#pragma once

/*
* To disable vulpengine from implementing main:
* #define VP_NO_ENTRYPOINT
*/

namespace vulpengine {
	void entry_init();
	void entry_uninit();

	int main(int argc, char* argv[]); // Defined by application
}