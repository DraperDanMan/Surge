
#include "Application.h"

namespace Surge
{

int Main(int argc, char** argv)
{
    const auto app = new Application();
    app->Run();
    delete app;

    return 0;
}

}

#ifdef NDEBUG

#include <Windows.h>

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    return Surge::Main(__argc, __argv);
}

#else

int main(int argc, char** argv)
{
    return Surge::Main(argc, argv);
}

#endif // NDEBUG