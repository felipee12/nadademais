#include "pch.h"

HWND hWndMain;

bool readPacketDB(LPARAM lParam) //  WSA_READDB
{
    if (WSAGETSELECTEVENT(lParam) != FD_READ)
    {
        cLog.Write(0, TL::Err, "err wsareaddb fail");
        cSrv.DBSocket.CloseSocket();

        int ret = 0;

        for (int i = 0; i < 2; i++)
        {
            int* pip = (int*)cSrv.LocalIP;
            ret = cSrv.DBSocket.ConnectServer(cSrv.DBServerAddress, cSrv.DBServerPort, *pip, WSA_READDB);

            if (ret != NULL)
                break;

            Sleep(200);
        }

        if (ret == 0)
        {
            cLog.Write(0, TL::Err, "reconnect DB fail.");
            PostQuitMessage(NULL);
            return true;
        }

        Sleep(200);
        PostQuitMessage(NULL);
        return true;
    }

    if (cSrv.DBSocket.Receive() == FALSE)
    {
        cLog.Write(0, TL::Err, "err wsareaddb fail");
        cSrv.DBSocket.CloseSocket();

        int ret = 0;

        for (int i = 0; i < 2; i++)
        {
            int* pip = (int*)cSrv.LocalIP;

            ret = cSrv.DBSocket.ConnectServer(cSrv.DBServerAddress, cSrv.DBServerPort, *pip, WSA_READDB);

            if (ret != NULL)
                break;

            Sleep(200);
        }

        if (ret == 0)
        {
            cLog.Write(0, TL::Err, "reconnect DB fail.");
            PostQuitMessage(NULL);
            return true;

        }
        PostQuitMessage(NULL);
        return true;
    }

    int Error = 0;
    int ErrorCode = 0;

    while (true)
    {
        char* Msg = cSrv.DBSocket.ReadMessage(&Error, &ErrorCode);

        if (Msg == NULL)
            break;

#ifdef  _PACKET_DEBUG
        MSG_STANDARD* debug = (MSG_STANDARD*)Msg;

        if (BASE_CheckPacket(debug))
        {
            snprintf(temp, sizeof(temp), "**PACKET_DEBUG** Type:%d Size:%d", debug->Type, debug->Size);
            Log(temp, "-system", 0);
            break;
        }
#endif

        if (Error == 1 || Error == 2)
        {
            cLog.Write(0, TL::Err, std::format("err readdb ({}),{}", Error, ErrorCode));
            break;
        }

        cSrv.ProcessDBMessage(Msg);
    };

    return false;
}

bool readPacketMSG(WPARAM wParam, LPARAM lParam)
{
    auto User = cSrv.GetUserFromSocket(wParam);
    if (User == 0)
    {
        closesocket(wParam);
        return true;
    }

    auto user = cSrv.getUser(User);
    //auto mob = cSrv.getPlayer(User);
  /*  if (!user || !mob)
    {
        cLog.Write(0, TL::Err, "user ou mob retorno nullo na func [readPacketMSG]");
        return true;
    }*/

    if (!user)
    {
        cLog.Write(0, TL::Err, "user ou mob retorno nullo na func [readPacketMSG]");
        return true;
    }

    if (WSAGETSELECTEVENT(lParam) != FD_READ)
    {
        cLog.Write(User, TL::Err, std::format("{} clo,fd user mode:{}", user->AccountName, user->Mode/*, mob->Mode,*/));
        cSrv.CloseUser(User);
        return true;
    }

    int ret = user->cSock.Receive();

    if (ret == FALSE)
    {
        ret = WSAGetLastError();
        cLog.Write(0, TL::Err, std::format("err,socket {} {}-{}-{} {}-", ret, user->cSock.nRecvPosition, user->cSock.nProcPosition, user->cSock.nSendPosition, user->Mode));

        ret = user->cSock.Receive();

        if (ret == FALSE)
        {
            cLog.Write(User, TL::Err, std::format("{} err,retry_recv fail", user->AccountName));
            cSrv.CloseUser(User);
            return true;
        }
    }
    else if (ret == -1)
    {
        cLog.Write(User, TL::Err, std::format("clo,receive {} {}-{}-{} {}", user->AccountName, user->cSock.nRecvPosition, user->cSock.nProcPosition, user->cSock.nSendPosition, user->Mode));
        ret = recv(user->cSock.Sock, user->cSock.pRecvBuffer, RECV_BUFFER_SIZE, NULL);
        cSrv.CloseUser(User);
        return true;
    }

    int Error = 0;
    int ErrorCode = 0;

    while (true)
    {
        char* Msg = user->cSock.ReadMessage(&Error, &ErrorCode);

        if (Msg == NULL)
            break;

#ifdef  _PACKET_DEBUG
        MSG_STANDARD* debug = (MSG_STANDARD*)Msg;

        if (Msg != NULL && BASE_CheckPacket(debug))
        {
            snprintf(temp, sizeof(temp), "**PACKET_DEBUG** Type:%d Size:%d", debug->Type, debug->Size);
            Log(temp, "-system", 0);
        }
#endif
        if (Error == 1 || Error == 2)
        {
            user->AccountName[ACCOUNTNAME_LENGTH - 1] = 0;
            user->AccountName[ACCOUNTNAME_LENGTH - 2] = 0;

            cLog.Write(0, TL::Err, std::format("err,read ({}),{}", Error, ErrorCode));
            break;
        }

        cSrv.ProcessClientMSG(User, Msg, FALSE);
    };

    return false;
}

bool wsa_Accept(LPARAM lParam)//WSA_ACCEPT
{
    if (WSAGETSELECTERROR(lParam) == 0)
    {
        SOCKADDR_IN acc_sin;
        int Size = sizeof(acc_sin);

        int tSock = accept(cSrv.ListenSocket.Sock, (struct sockaddr FAR*) & acc_sin, (int FAR*) & (Size));
        if (tSock == INVALID_SOCKET)
        {
            closesocket(cSrv.ListenSocket.Sock);
            return false;
        }

        if (cSrv.ServerDown != -1000)
        {
            closesocket(tSock);
            return true;
        }

        int User = cSrv.GetEmptyUser();
        if (User == 0)
        {
            cLog.Write(0, TL::Err, "err,accept fail - no empty");
            closesocket(tSock);
            return true;
        }

        auto user = cSrv.getUser(User);
        if (!user)
        {
            closesocket(tSock);
            return false;
        }

        if (WSAAsyncSelect(tSock, hWndMain, WSA_READ, FD_READ | FD_CLOSE) > 0)
        {
            closesocket(tSock);
            return false;
        }

        user->AcceptUser();
        user->cSock.Sock = tSock;
        char* tmp = inet_ntoa(acc_sin.sin_addr);
        user->IP = acc_sin.sin_addr.S_un.S_addr;

        //if (User >= MAX_USER - ADMIN_RESERV) // ver se precisa disso aqui
        //{
        //    //SendClientMsg(User, g_pMessageStringTable[_NN_Reconnect]);
        //    user->cSock.SendMessageA();
        //    user->CloseUser();
        //    return true;
        //}
    }
    else
        cLog.Write(0, TL::Err, "err,- accept - wsagetselecterror");

    return false;
}//checar os retornos dessa func

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    if (!InitApplication(hInstance))
        return FALSE;

    if (!InitInstance(hInstance, nCmdShow))
        return FALSE;

    if (!cSrv.StartServer())
    {
        MessageBox(hWndMain, "não foi possivel iniciar o server", "Erro ao iniciar", NULL);
        return FALSE;
    }

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
    hWndMain = CreateWindow("MainClass", "TMSrv", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, 800, 100, nullptr, nullptr, hInstance, nullptr);

    /*hWndMain = CreateWindow("MainClass", "TMSrv", WS_OVERLAPPEDWINDOW,
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
            cSrv.TimerSec();
        else if (LOWORD(wParam) == TIMER_MIN)
            cSrv.TimerMin();
    }break;
    case WSA_READDB:
        if (readPacketDB(lParam)) return TRUE;
        break;

    case WSA_READ:
        if (readPacketMSG(wParam, lParam)) return TRUE;
        break;

    case WSA_ACCEPT:
        if (wsa_Accept(lParam)) return TRUE;
        break;

    case WM_COMMAND:
    {

    }break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        BeginPaint(hWnd, &ps);

        EndPaint(hWnd, &ps);
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