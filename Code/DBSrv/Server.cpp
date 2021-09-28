#include "pch.h"

CLog cLog = CLog();
CServer cSrv = CServer();

uint32 CurrentTime = 0;

HFONT hFont = NULL;
HFONT h;
HDC hDC = NULL;
int x = 0;
int y = 0;

CLog::CLog()
{
	this->SysFile = NULL;
	this->WarnFile = NULL;
	this->ErrFile = NULL;
	this->PacketFile = NULL;

	this->SysLogDay = -1;
	this->WarnLogDay = -1;
	this->ErrLogDay = -1;
	this->PacketLogDay = -1;
}

CLog::~CLog()
{
}

void CLog::Start()
{
	if (this->SysFile != NULL)
		fclose(this->SysFile);

	if (this->WarnFile != NULL)
		fclose(this->WarnFile);

	if (this->ErrFile != NULL)
		fclose(this->ErrFile);

	if (this->PacketFile != NULL)
		fclose(this->PacketFile);

	auto when = getNow();

	snprintf(cSrv.temp, sizeof(cSrv.temp), ".\\Log\\System\\System_%02d_%02d_%02d_%02d_%02d.txt", when.tm_mday, when.tm_mon + 1, when.tm_hour, when.tm_min, when.tm_sec);
	this->SysFile = fopen(cSrv.temp, "wt");
	this->SysLogDay = when.tm_mday;

	snprintf(cSrv.temp, sizeof(cSrv.temp), ".\\Log\\Warning\\System_%02d_%02d_%02d_%02d_%02d.txt", when.tm_mday, when.tm_mon + 1, when.tm_hour, when.tm_min, when.tm_sec);
	this->WarnFile = fopen(cSrv.temp, "wt");
	this->WarnLogDay = when.tm_mday;

	snprintf(cSrv.temp, sizeof(cSrv.temp), ".\\Log\\Error\\System_%02d_%02d_%02d_%02d_%02d.txt", when.tm_mday, when.tm_mon + 1, when.tm_hour, when.tm_min, when.tm_sec);
	this->ErrFile = fopen(cSrv.temp, "wt");
	this->ErrLogDay = when.tm_mday;

	snprintf(cSrv.temp, sizeof(cSrv.temp), ".\\Log\\Packet\\System_%02d_%02d_%02d_%02d_%02d.txt", when.tm_mday, when.tm_mon + 1, when.tm_hour, when.tm_min, when.tm_sec);
	this->PacketFile = fopen(cSrv.temp, "wt");
	this->PacketLogDay = when.tm_mday;
}

void CLog::Write(uint32 ip, TL Type, const std::string& str)
{
	auto when = getNow();

	if (ip != 0)
	{
		uint8* cIP = (uint8*)&ip;
		snprintf(cSrv.temp, sizeof(cSrv.temp), "[%02d/%02d/%04d][%02d:%02d:%02d] IP: %d.%d.%d.%d", when.tm_mday, when.tm_mon + 1, when.tm_year + 1900, when.tm_hour, when.tm_min, when.tm_sec, cIP[0], cIP[1], cIP[2], cIP[3]);
	}
	else
		snprintf(cSrv.temp, sizeof(cSrv.temp), "[%02d/%02d/%04d][%02d:%02d:%02d] ", when.tm_mday, when.tm_mon + 1, when.tm_year + 1900, when.tm_hour, when.tm_min, when.tm_sec);

	snprintf(cSrv.temp, sizeof(cSrv.temp), "%s %s\n", cSrv.temp, str.c_str());

	switch (Type)
	{
	case TL::Sys:
		if (this->SysFile)
			fprintf_s(this->SysFile, cSrv.temp);
		break;
	case TL::Warn:
		if (this->WarnFile)
			fprintf_s(this->WarnFile, cSrv.temp);
		break;
	case TL::Err:
		if (this->ErrFile)
			fprintf_s(this->ErrFile, cSrv.temp);
		break;
	case TL::Pakt:
		if (this->PacketFile)
			fprintf_s(this->PacketFile, cSrv.temp);
		break;
	}

	SetWindowText(hWndMain, cSrv.temp);
}


CServer::CServer()
{
	this->SecCounter = 0;
	this->MinCounter = 0;
	this->HourCounter = 0;
	this->ServerIndex = -1;
	this->LastCapsule = 0;
	this->Sapphire = 8;

	memset(this->temp, 0, sizeof(this->temp));
	memset(this->LocalIP, 0, sizeof(this->LocalIP));
}

CServer::~CServer()
{
}

CUser* CServer::getUserDB(int id)
{
	if (id >= 0 && id < MAX_SERVER)
		return &pUser[id];

	return nullptr;
}

int CServer::getUserSocket(int Sock)
{
	for (int i = 0; i < MAX_SERVER; i++)
	{
		if (pUser[i].cSock.Sock == (unsigned)Sock)
			return i;
	}

	return -1;
}

void CServer::ProcessClientMessage(int conn, char* msg)
{
	MSG_HEADER* m = (MSG_HEADER*)msg;

	if (!(m->Type & FLAG_GAME2DB) || (m->ID < 0) || (m->ID >= MAX_USER))
	{
		snprintf(this->temp, sizeof(this->temp), "err,packet Type:%d ID:%d Size:%d KeyWord:%d", m->Type, m->ID, m->Size, m->KeyWord);
		cLog.Write(0, TL::Err, this->temp);
		return;
	}

	cFileDB.ProcessMessage(conn, msg);
}

int CServer::WriteConfig()
{
	FILE* fp = fopen("Config.txt", "wt");

	if (fp == NULL)
		return FALSE;

	fprintf(fp, "Sapphire %d\n", Sapphire);
	fprintf(fp, "LastCapsule %d\n", LastCapsule);

	fclose(fp);

	return TRUE;
}

void CServer::TextOutWind(const char* str, int color)
{
	SetTextColor(hDC, color);
	sprintf(cSrv.temp, str);
	auto len = strlen(cSrv.temp);
	TextOutA(hDC, x, y, cSrv.temp, len);
	y += 16;
}

void CServer::DrawConfig()
{
	x = 0;
	y = 0;
	h = 0;

	hDC = GetDC(hWndMain);

	if (hDC == NULL)
		return;

	if (hFont == 0)
		return;

	if (SelectObject(hDC, hFont) != 0)
		h = (HFONT)SelectObject(hDC, hFont);

	this->TextOutWind("Server Zone Status:", 255); // int VERMELHO = 255;

	if (auto userDB = cSrv.getUserDB(0))
	{
		uint8* cIP = (uint8*)&(userDB->IP);

		snprintf(this->temp, sizeof(this->temp), " IP: %3d. %3d. %3d. %3d  Socket: %3d  Guild: %4d %4d %4d %4d %4d  User: %4d ", cIP[0], cIP[1], cIP[2], cIP[3],
			userDB->cSock.Sock, ChargedGuildList[0][0], ChargedGuildList[0][1], ChargedGuildList[0][2], ChargedGuildList[0][3], ChargedGuildList[0][4], userDB->Count);

		this->TextOutWind(this->temp, 0); //int PRETO = 0;
	}

	if (hFont && h)
		h = (HFONT)SelectObject(hDC, h);

	ReleaseDC(hWndMain, hDC);
}

void CServer::ProcessSecTimer()
{
	this->SecCounter++;

	//if (SecCounter % 2 == 0)
	//{
	//	CReadFiles::ImportItem(); // Once every two minutes
	//	CReadFiles::ImportDonate();
	//}

	if (SecCounter % 10 == 0)
		this->DrawConfig();

	if (SecCounter % 30 == 0)
	{
		this->MinCounter++;

		if (this->MinCounter % 30 == 0)
			this->HourCounter++;
	}

	auto when = getNow();

	if (when.tm_mday != cLog.SysLogDay)
		cLog.Start();

	/*if (when.tm_hour == 0 && when.tm_wday == 0 && when.tm_min == 0 && when.tm_sec == 0)
	{
		for (int i = 0; i < 65536; i++)
		{
			if (GuildInfo[i].Fame)
			{
				GuildInfo[i].Fame = 0;

				for (int j = 0; j < MAX_SERVER; j++)
				{
					if (pUser[j].cSock.Sock == 0)
						continue;

					if (pUser[j].Mode == 0)
						continue;

					cFileDB.SendGuildInfo(j, i);
				}
			}
		}

		readFile.WriteGuildInfo();
	}*/
}

void CServer::ReadConfig()
{
	FILE* fp = fopen("Config.txt", "rt");

	if (fp == NULL)
	{
		MessageBox(hWndMain, "cant find Config.txt", "BOOTING ERROR", NULL);
		return;
	}

	fscanf(fp, "Sapphire %d\n", &Sapphire);
	fscanf(fp, "LastCapsule %hu\n", &LastCapsule);

	fclose(fp);
}

void CServer::loadCharacter()
{
	int handle = -1;

	handle = _open("./BaseMob/TK", _O_RDONLY | _O_BINARY);

	if (handle == -1)
	{
		MessageBoxA(hWndMain, "no TransKnight file", "BOOTING ERROR", MB_OK);
		return;
	}

	_read(handle, &g_pBaseSet[0], sizeof(STRUCT_MOB));
	_close(handle);

	handle = -1;

	handle = _open("./BaseMob/FM", _O_RDONLY | _O_BINARY);

	if (handle == -1)
	{
		MessageBoxA(hWndMain, "no Foema file", "BOOTING ERROR", MB_OK);
		return;
	}

	_read(handle, &g_pBaseSet[1], sizeof(STRUCT_MOB));
	_close(handle);

	handle = -1;

	handle = _open("./BaseMob/BM", _O_RDONLY | _O_BINARY);

	if (handle == -1)
	{
		MessageBoxA(hWndMain, "no BeastMaster file", "BOOTING ERROR", MB_OK);
		return;
	}

	_read(handle, &g_pBaseSet[2], sizeof(STRUCT_MOB));
	_close(handle);

	handle = -1;

	handle = _open("./BaseMob/HT", _O_RDONLY | _O_BINARY);

	if (handle == -1)
	{
		MessageBoxA(hWndMain, "no Huntress file", "BOOTING ERROR", MB_OK);
		return;
	}

	_read(handle, &g_pBaseSet[3], sizeof(STRUCT_MOB));
	_close(handle);

	g_pBaseSet[0].BaseScore = g_pBaseSet[0].CurrentScore;
	g_pBaseSet[1].BaseScore = g_pBaseSet[1].CurrentScore;
	g_pBaseSet[2].BaseScore = g_pBaseSet[2].CurrentScore;
	g_pBaseSet[3].BaseScore = g_pBaseSet[3].CurrentScore;
}

bool CServer::geralStart()
{
	int ret = ListenSocket.WSAInitialize();

	if (!ret)
	{
		cLog.Write(0, TL::Err, "-system err,wsainitialize fail");
		return false;
	}

	BASE_InitializeServerList();

	char name[256];
	int i = 0;

	struct addrinfo hints, * res, * p;

	memset(&hints, 0, sizeof(addrinfo));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	gethostname(name, sizeof(name));

	getaddrinfo(name, NULL, &hints, &res);

	for (p = res; p != NULL; p = p->ai_next)
	{
		if (p->ai_family != AF_INET)
			continue;

		struct sockaddr_in* ipv4 = (struct sockaddr_in*)p->ai_addr;

		inet_ntop(p->ai_family, &(ipv4->sin_addr), name, sizeof(name));

		for (i = 0; i < MAX_SERVERGROUP; i++)
		{
			if (!strcmp(g_pServerList[i][0], name))
			{
				ServerIndex = i;

				sscanf(name, "%hhu.%hhu.%hhu.%hhu", &LocalIP[0], &LocalIP[1], &LocalIP[2], &LocalIP[3]);

				break;
			}
		}

		if (ServerIndex != -1)
			break;
	}

	if (ServerIndex == -1)
	{
		MessageBox(hWndMain, "Can't get Server Group Index LocalIP:", "", MB_OK | MB_SYSTEMMODAL);
		MessageBox(hWndMain, "Can't get Server Group Index TestServerIP:", g_pServerList[i][0], MB_OK | MB_SYSTEMMODAL);
		return false;
	}

	ListenSocket.StartListen(hWndMain, 0, DB_PORT, WSA_ACCEPT);

	return true;
}