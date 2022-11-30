#pragma once

#ifdef SNAP_PLATFORM_WINDOWS


extern SnapEngine::Application* SnapEngine::CreatApplication();

int main(int argc, const char* argv[])
{
	SnapEngine::Log::init();
	SnapEngine::Application* app = SnapEngine::CreatApplication();
	app->Run();

	SnapEngine::Log::free_memory();
	delete app;
	return 0;
}
#endif