#pragma once

/*!
Defines vulpengine main function, vulpengine will defined the main function
and invoke the entry_init and entry_uninit functions automatically.


If you don't want vulpengine to define the main function #define VP_NO_ENTRYPOINT
Then in your main function you should invoke entry_init and entry_uninit yourself.
You may optionally invoke the vulpengine main function, but this is not required
when defining your own main.
*/

namespace vulpengine {
	void entry_init();
	void entry_uninit();

	int main(int argc, char* argv[]); // Defined by application
}