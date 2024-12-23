#include "Application.h"
#include "imgui.h"

using namespace ImGui;

namespace RemoteLink {
	void RenderUI() {
		DockSpaceOverViewport(0);

        Begin("Settings");
		Button("Connect");
		End();

		Begin("Screen");
		End();
	}
}