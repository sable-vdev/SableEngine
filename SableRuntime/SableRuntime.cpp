#include "Engine.hpp"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
	Engine* engine = new Engine;

	try
	{
		engine->Run();
	}
	catch (const std::exception& ex)
	{
		std::cout << ex.what() << std::endl;
		return EXIT_FAILURE;
	}
	engine->Shutdown();
	engine = nullptr;
	delete engine;

	return EXIT_SUCCESS;
}