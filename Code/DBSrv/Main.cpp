#include "pch.h"

HWND hWndMain;
STRUCT_MOB g_pBaseSet[MAX_CLASS];

LRESULT wsa_Read(WPARAM wParam, LPARAM lParam)//WSA_READ
{
    auto User = cSrv.getUserSocket(wParam);

    if (User == -1)
    {
        closesocket(wParam);
        cLog.Write(0, TL::Err, "-system err wsa_read unregistered game server socket");
        return TRUE;
    }

    auto userDB = cSrv.getUserDB(User);
    if (!userDB)
    {
        cLog.Write(0, TL::Err, "-system err, nullptr user[wsa_Read] ");
        return TRUE;
    }

    if (WSAGETSELECTEVENT(lParam) != FD_READ)
    {
        cLog.Write(0, TL::Sys, std::format("-system clo server fd {}", User));
        userDB->cSock.CloseSocket();
        userDB->Mode = USER_EMPTY;
        return TRUE;
    }

    CurrentTime = timeGetTime();

    if (userDB->cSock.Receive() != 1)
    {
        cLog.Write(0, TL::Sys, std::format("-system clo, server receive {}", User));
        userDB->cSock.nRecvPosition = 0;
        userDB->cSock.nProcPosition = 0;
        return TRUE;
    }

    int Error = 0;
    int ErrorCode = 0;

    do
    {
        char* Msg = userDB->cSock.ReadMessage(&Error, &ErrorCode);

        if (Msg == NULL)
            break;

#ifdef  _PACKET_DEBUG
        MSG_STANDARD* debug = (MSG_STANDARD*)Msg;
        if (BASE_CheckPacket(debug))
        {
            sprintf(temp, "**PACKET_DEBUG** Type:%d Size:%d", debug->Type, debug->Size);
            Log(temp, "-system", 0);
        }
#endif

        if (Error == 1 || Error == 2)
        {
            cLog.Write(0, TL::Err, std::format("-system err wsa_read ({}/{})", Error, ErrorCode));
            break;
        }

        cSrv.ProcessClientMessage(User, Msg);

    } while (1);

    return FALSE;
}

LRESULT wsa_Accept(LPARAM lParam)//WSA_ACCEPT
{
    if (WSAGETSELECTERROR(lParam))
    {
        cLog.Write(0, TL::Err, "-system err WSAGETSELECTERROR - accepting new client.");
        return TRUE;
    }

    cSrv.TempUser.AcceptUser(cSrv.ListenSocket.Sock, WSA_READ);

    uint8* cIP = (uint8*)&(cSrv.TempUser.IP);

    snprintf(cSrv.temp, sizeof(cSrv.temp), "%d.%d.%d.%d", cIP[0], cIP[1], cIP[2], cIP[3]);

    int User = -1;

    for (int i = 0; i < MAX_SERVER; i++)
    {
        if (auto DBUser = cSrv.getUserDB(i))
        {
            if (DBUser->IP == cSrv.TempUser.IP)
            {
                User = i;
                break;
            }
        }
    }

    if (User == -1)
    {
        int i = 1;

        for (; i < MAX_SERVERNUMBER; i++)
        {
            if (!strcmp(g_pServerList[cSrv.ServerIndex][i], cSrv.temp))
                break;
        }

        if (i == MAX_SERVERNUMBER)
        {
            cSrv.TempUser.cSock.CloseSocket();
            cLog.Write(0, TL::Err, std::format("-system err wsa_accept request from {}", cSrv.temp));
            return TRUE;
        }

        User = i - 1;
    }

    auto userDB = cSrv.getUserDB(User);
    if (!userDB)
    {
        cLog.Write(0, TL::Err, "err userDB retornou nullo função [wsa_Accept]");
        return TRUE;
    }

    if (User >= 0 && User < MAX_SERVERNUMBER)
    {
        if (userDB->Mode != USER_EMPTY)
        {
            cLog.Write(0, TL::Err, std::format("-system err wsa_accept no previous slot {}", User));

            cSrv.TempUser.cSock.CloseSocket();
            cSrv.TempUser.Mode = 0;

            userDB->cSock.CloseSocket();
            userDB->Mode = 0;
            return TRUE;
        }
    }
    else
    {
        cLog.Write(0, TL::Err, "-system err wsa_accept unknown attempt");
        cSrv.TempUser.cSock.CloseSocket();
    }

    if (User == -1)
    {
        cLog.Write(0, TL::Err, "-system err wsa_accept no empty");
        return TRUE;
    }

    if (cIP[0] == cSrv.LocalIP[0] && cIP[1] == cSrv.LocalIP[1] && cIP[2] == cSrv.LocalIP[2])
    {
        userDB->IP = cSrv.TempUser.IP;
        userDB->Mode = cSrv.TempUser.Mode;
        userDB->cSock.Sock = cSrv.TempUser.cSock.Sock;
        userDB->cSock.nRecvPosition = cSrv.TempUser.cSock.nRecvPosition;
        userDB->cSock.nProcPosition = cSrv.TempUser.cSock.nProcPosition;
        userDB->cSock.nSendPosition = cSrv.TempUser.cSock.nSendPosition;

        cFileDB.SendDBSignalParm3(User, 0, _MSG_DBSetIndex, cSrv.ServerIndex, cSrv.Sapphire, User);

        /*for (int i = 0; i < 65536; i++)
        {
            if (GuildInfo[i].Citizen)
                cFileDB.SendGuildInfo(User, i);

            if (g_pGuildWar[i])
                cFileDB.SendDBSignalParm2(User, 0, _MSG_War, i, g_pGuildWar[i]);

            if (g_pGuildAlly[i])
                cFileDB.SendDBSignalParm2(User, 0, _MSG_GuildAlly, i, g_pGuildAlly[i]);
        }*/

        /*if (TransperCharacter)
            cFileDB.SendDBSignalParm2(User, 0, _MSG_TransperCharacter, 0, 0);*/

        return TRUE;
    }
    else
    {
        cSrv.TempUser.cSock.CloseSocket();
        cLog.Write(0, TL::Err, std::format("-system err,wsa_accept outer ethernet IP: {}.{}.{}.{}", cIP[0], cIP[1], cIP[2], cIP[3]));
        return TRUE;
    }

    return FALSE;
}

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    if (!InitApplication(hInstance))
        return FALSE;

    if (!InitInstance(hInstance, nCmdShow))
        return FALSE;

    BASE_InitModuleDir();
    BASE_InitializeBaseDef();

    cSrv.loadCharacter();
    cLog.Start();
    cLog.Write(0, TL::Sys, "-system start log");

    /*memset(g_pGuildWar, 0, sizeof(g_pGuildWar));
    memset(g_pGuildAlly, 0, sizeof(g_pGuildAlly));*/

    cSrv.ReadConfig();

    if (!cSrv.geralStart())
        return FALSE;

    SetTimer(hWndMain, TIMER_SEC, 1000, NULL);

    MSG msg;

    // Loop de mensagem principal:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

BOOL InitApplication(HINSTANCE hInstance)
{
    WNDCLASS  wc;

    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(hInstance, "MAINICON");
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "MainClass";

    if (!RegisterClass(&wc))
        return RegisterClass(&wc);

    return TRUE;
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hWndMain = CreateWindow("MainClass", "DBSrv", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, 800, 100, nullptr, nullptr, hInstance, nullptr);

    /*hWndMain = CreateWindow("MainClass", "DBSrv", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);*/

    if (!hWndMain)
        return FALSE;

    ShowWindow(hWndMain, nCmdShow);
    UpdateWindow(hWndMain);

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_TIMER:
    {
        if (LOWORD(wParam) == TIMER_SEC)
            cSrv.ProcessSecTimer();
    }break;

    case WSA_READ:
        return wsa_Read(wParam, lParam);

    case WSA_ACCEPT:
        return wsa_Accept(lParam);
    case WM_COMMAND:
    {

    }break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        BeginPaint(hWnd, &ps);

        hFont = CreateFont(12, 0, 0, 0, FW_LIGHT, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEVICE_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Fixedsys");
        hDC = ps.hdc;
        cSrv.DrawConfig();

        EndPaint(hWnd, &ps);
    }break;
    case WM_CLOSE:
    {
        cSrv.WriteConfig();

        if (cLog.SysFile)
            fclose(cLog.SysFile);

        if (cLog.WarnFile)
            fclose(cLog.WarnFile);

        if (cLog.ErrFile)
            fclose(cLog.ErrFile);

        if (cLog.PacketFile)
            fclose(cLog.PacketFile);

        DefWindowProc(hWnd, message, wParam, lParam);
        return TRUE;

    }break;
    case WM_DESTROY:
        WSACleanup();
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}