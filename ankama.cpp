#include <windows.h>
#include <cstring>
#include <detours.h>

#pragma comment( lib, "Ws2_32.lib" )
#pragma comment( lib, "detours.lib" )

int (WINAPI *Real_Connect)(SOCKET s, const struct sockaddr *name, int namelen) = connect;

int WINAPI Mine_Connect(SOCKET s, const struct sockaddr *name, int namelen)
{
    char* ip;
    struct sockaddr_in sa;

    ip = inet_ntoa(((struct sockaddr_in*)name)->sin_addr);
	
    if((strcmp(ip, "213.248.126.39") == 0) || (strcmp(ip, "213.248.126.40") == 0))
    {
        sa.sin_addr.s_addr = inet_addr("127.0.0.1");
        sa.sin_family = AF_INET;
        sa.sin_port = htons(5555);

        return Real_Connect(s, (sockaddr*)&sa, sizeof(sa));
    }

    return Real_Connect(s, name, namelen);
}

BOOL WINAPI DllMain( HINSTANCE, DWORD dwReason, LPVOID )
{
    switch ( dwReason )
    {
        case DLL_PROCESS_ATTACH:
            DetourTransactionBegin();
            DetourUpdateThread( GetCurrentThread() );
            DetourAttach( &(PVOID &)Real_Connect, Mine_Connect );
            DetourTransactionCommit();
            break;

        case DLL_PROCESS_DETACH:
            DetourTransactionBegin();
            DetourUpdateThread( GetCurrentThread() );
            DetourDetach( &(PVOID &)Real_Connect, Mine_Connect );
            DetourTransactionCommit(); 
            break;
    }

    return TRUE;
}