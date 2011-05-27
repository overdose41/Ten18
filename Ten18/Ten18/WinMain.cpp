#include "Ten18/PCH.h"
#include "Ten18/Console.h"
#include "Ten18/Expect.h"
#include "Ten18/OnExit.h"
#include "Ten18/Program.h"
#include "Ten18/Tests.h"
#include "Ten18/Timer.h"
#include "Ten18/Tracer.h"
#include "Ten18/Sandbox.h"
#include "Ten18/Interop/Host.h"

using namespace Ten18;
using namespace Ten18::Interop;

static void InitializeCrt()
{
    #ifdef DEBUG        
        Expect.NotEqualTo(-1) = _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
        Expect.NotEqualTo(-1) = _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
        Expect.NotEqualTo(-1) = _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);

        Expect.NotEqualTo(_CRTDBG_HFILE_ERROR) = _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
        Expect.NotEqualTo(_CRTDBG_HFILE_ERROR) = _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
        Expect.NotEqualTo(_CRTDBG_HFILE_ERROR) = _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);

        const auto crtDbgFlags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
        const auto addDbgFlags = _CRTDBG_CHECK_EVERY_128_DF | _CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF;
        Expect.EqualTo(crtDbgFlags) = _CrtSetDbgFlag(crtDbgFlags | addDbgFlags);
        (void)addDbgFlags;
    #endif
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int nCmdShow)
{
    Expect.True = HeapSetInformation(nullptr, HeapEnableTerminationOnCorruption, nullptr, 0);
    Expect.True = SetDllDirectory(L"");
    Expect.True = XMVerifyCPUSupport();
    Expect.HR = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE | COINIT_SPEED_OVER_MEMORY);
    
    Console::Initialize(L"Ten18 Console");
    Timer::Initialize();
    
    Sandbox();    
    
    Host host;    
    host.Rendezvous();

    InitializeCrt();

    RunTests();
    
    const auto procExitCode = Program(hInstance, nCmdShow, &host);

    // Todo, Jaap Suter, April 2011
    // Expect.Zero = 
    // _CrtDumpMemoryLeaks();

    // Todo, Jaap Suter, May 2011, we don't call CoUninitialize because the CLR host is running past main, and while
    // multiple matching CoInit and CoUninit calls should balance out, I'm just gonnna 
    // throw this cargo cult voodoo out the window and hope for the best...
    

    return procExitCode;
}
