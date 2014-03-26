#include <windows.h>
#include <cstring>
#include <detours.h>
#include <vector>

#pragma comment( lib, "Ws2_32.lib" )
#pragma comment( lib, "detours.lib" )

int (WINAPI *Real_Connect)(SOCKET s, const struct sockaddr *name, int namelen) = connect;

int WINAPI Mine_Connect(SOCKET s, const struct sockaddr *name, int namelen)
{
    char* ip;
    struct sockaddr_in sa;
    std::vector<char*> listIP;
    bool isAnkamaLogin = false;

    ip = inet_ntoa(((struct sockaddr_in*)name)->sin_addr);

    listIP.push_back("213.248.126.37");
    listIP.push_back("213.248.126.38");
    listIP.push_back("213.248.126.39");
    listIP.push_back("213.248.126.40");

    for(int i = 0; i < listIP.size(); i++)
    {  
        if((strcmp(ip, listIP[i]) == 0))
            isAnkamaLogin = true;
    }
	
    if(isAnkamaLogin)
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