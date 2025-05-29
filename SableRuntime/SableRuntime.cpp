#include "Engine.hpp"
#include <memory>
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
	
	std::unique_ptr<Engine> engine = std::make_unique<Engine>();

	try
	{
		engine->Run();
	}
	catch (const std::exception& ex)
	{
#ifdef ERROR
#undef ERROR
#endif // ERROR
		Logger::Log(ERROR, ex.what());
		return EXIT_FAILURE;
	}
	engine->Shutdown();

	return EXIT_SUCCESS;
}