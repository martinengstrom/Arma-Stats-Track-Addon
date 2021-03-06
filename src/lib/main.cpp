//#if defined (BOOST_OS_WINDOWS)
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    //#define _WIN32_WINNT 0x0601
//#endif


#define THREAD_SLEEP_DELAY 500
#include "common.h"
#include "httpClient.cpp"
#include "json.cpp"
#include "organizer.cpp"

/*
    Windows
    COMPILE WITH:
    -D_WIN32_WINNT=0x0501
    -shared -lboost_system -lboost_thread -lboost_date_time -lwsock32 -lws2_32

    Linux
    -shared -lboost_system -lboost_thread -lboost_date_time -lpthread -m32
*/

using namespace std;

Organizer *organizer = NULL;

//#if defined(BOOST_OS_WINDOWS)
    extern "C" int APIENTRY DllMain( HINSTANCE hinstDLL, DWORD ul_reason_for_call, LPVOID lpvReserved )
    {

        switch (ul_reason_for_call)
        {
        case DLL_PROCESS_ATTACH:
            AllocConsole();
            freopen("CONOUT$", "w", stdout);
            break;
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
        }
        return TRUE;
    }
//#endif

extern "C" void __stdcall _RVExtension(char *output, int outputSize, const char *function)
{
    if(!organizer)
        organizer = new Organizer();

    /* Format of function should be function name;data*/
    string s_function(function);
    string function_ = s_function.substr(0, s_function.find(';'));
    string data = s_function.substr(s_function.find(';')+1, s_function.length());

    if (function_ == "setup") {
            organizer->setHostname(data);
    }
    else if (function_ == "status") {
        /* TODO: Get IP of current machine */
        Organizer::status_t status = organizer->getStatus(data, "NOT_IMPLEMENTED");
        if (status.status == Organizer::OK) {
            organizer->setId(status.id);
            cout << "Oranizer reports connection OK! ID: " << status.id << endl;
            snprintf(output, outputSize, "%s", status.id.c_str());
        } else {
            if (status.status == Organizer::CONNECTION_FAILED) {
                cout << "Organizer reports connection failed. Check config" << endl;
            } else {
                cout << "Organizer reports unknonwn error." << endl;
            }
            strncpy(output, "-1", outputSize);
        }
    } else if (function_ == "event") {
        organizer->addEvent(data);
    }
}

