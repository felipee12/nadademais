
#include "CPSock.h"
#include "BaseDef.h"

int ConnectPort = 0;

unsigned char pKeyWord[512] = { // 7.xx keys
	0x84, 0x87, 0x37, 0xd7, 0xea, 0x79, 0x91, 0x7d, 0x4b, 0x4b, 0x85, 0x7d, 0x87, 0x81, 0x91, 0x7c, 0x0f, 0x73, 0x91, 0x91, 0x87, 0x7d, 0x0d, 0x7d, 0x86, 0x8f, 0x73, 0x0f, 0xe1, 0xdd, 0x85, 0x7d,
	0x05, 0x7d, 0x85, 0x83, 0x87, 0x9c, 0x85, 0x33, 0x0d, 0xe2, 0x87, 0x19, 0x0f, 0x79, 0x85, 0x86, 0x37, 0x7d, 0xd7, 0xdd, 0xe9, 0x7d, 0xd7, 0x7d, 0x85, 0x79, 0x05, 0x7d, 0x0f, 0xe1, 0x87, 0x7e,
	0x23, 0x87, 0xf5, 0x79, 0x5f, 0xe3, 0x4b, 0x83, 0xa3, 0xa2, 0xae, 0x0e, 0x14, 0x7d, 0xde, 0x7e, 0x85, 0x7a, 0x85, 0xaf, 0xcd, 0x7d, 0x87, 0xa5, 0x87, 0x7d, 0xe1, 0x7d, 0x88, 0x7d, 0x15, 0x91,
	0x23, 0x7d, 0x87, 0x7c, 0x0d, 0x7a, 0x85, 0x87, 0x17, 0x7c, 0x85, 0x7d, 0xac, 0x80, 0xbb, 0x79, 0x84, 0x9b, 0x5b, 0xa5, 0xd7, 0x8f, 0x05, 0x0f, 0x85, 0x7e, 0x85, 0x80, 0x85, 0x98, 0xf5, 0x9d,
	0xa3, 0x1a, 0x0d, 0x19, 0x87, 0x7c, 0x85, 0x7d, 0x84, 0x7d, 0x85, 0x7e, 0xe7, 0x97, 0x0d, 0x0f, 0x85, 0x7b, 0xea, 0x7d, 0xad, 0x80, 0xad, 0x7d, 0xb7, 0xaf, 0x0d, 0x7d, 0xe9, 0x3d, 0x85, 0x7d,
	0x87, 0xb7, 0x23, 0x7d, 0xe7, 0xb7, 0xa3, 0x0c, 0x87, 0x7e, 0x85, 0xa5, 0x7d, 0x76, 0x35, 0xb9, 0x0d, 0x6f, 0x23, 0x7d, 0x87, 0x9b, 0x85, 0x0c, 0xe1, 0xa1, 0x0d, 0x7f, 0x87, 0x7d, 0x84, 0x7a,
	0x84, 0x7b, 0xe1, 0x86, 0xe8, 0x6f, 0xd1, 0x79, 0x85, 0x19, 0x53, 0x95, 0xc3, 0x47, 0x19, 0x7d, 0xe7, 0x0c, 0x37, 0x7c, 0x23, 0x7d, 0x85, 0x7d, 0x4b, 0x79, 0x21, 0xa5, 0x87, 0x7d, 0x19, 0x7d,
	0x0d, 0x7d, 0x15, 0x91, 0x23, 0x7d, 0x87, 0x7c, 0x85, 0x7a, 0x85, 0xaf, 0xcd, 0x7d, 0x87, 0x7d, 0xe9, 0x3d, 0x85, 0x7d, 0x15, 0x79, 0x85, 0x7d, 0xc1, 0x7b, 0xea, 0x7d, 0xb7, 0x7d, 0x85, 0x7d,
	0x85, 0x7d, 0x0d, 0x7d, 0xe9, 0x73, 0x85, 0x79, 0x05, 0x7d, 0xd7, 0x7d, 0x85, 0xe1, 0xb9, 0xe1, 0x0f, 0x65, 0x85, 0x86, 0x2d, 0x7d, 0xd7, 0xdd, 0xa3, 0x8e, 0xe6, 0x7d, 0xde, 0x7e, 0xae, 0x0e,
	0x0f, 0xe1, 0x89, 0x7e, 0x23, 0x7d, 0xf5, 0x79, 0x23, 0xe1, 0x4b, 0x83, 0x0c, 0x0f, 0x85, 0x7b, 0x85, 0x7e, 0x8f, 0x80, 0x85, 0x98, 0xf5, 0x7a, 0x85, 0x1a, 0x0d, 0xe1, 0x0f, 0x7c, 0x89, 0x0c,
	0x85, 0x0b, 0x23, 0x69, 0x87, 0x7b, 0x23, 0x0c, 0x1f, 0xb7, 0x21, 0x7a, 0x88, 0x7e, 0x8f, 0xa5, 0x7d, 0x80, 0xb7, 0xb9, 0x18, 0xbf, 0x4b, 0x19, 0x85, 0xa5, 0x91, 0x80, 0x87, 0x81, 0x87, 0x7c,
	0x0f, 0x73, 0x91, 0x91, 0x84, 0x87, 0x37, 0xd7, 0x86, 0x79, 0xe1, 0xdd, 0x85, 0x7a, 0x73, 0x9b, 0x05, 0x7d, 0x0d, 0x83, 0x87, 0x9c, 0x85, 0x33, 0x87, 0x7d, 0x85, 0x0f, 0x87, 0x7d, 0x0d, 0x7d,
	0xf6, 0x7e, 0x87, 0x7d, 0x88, 0x19, 0x89, 0xf5, 0xd1, 0xdd, 0x85, 0x7d, 0x8b, 0xc3, 0xea, 0x7a, 0xd7, 0xb0, 0x0d, 0x7d, 0x87, 0xa5, 0x87, 0x7c, 0x73, 0x7e, 0x7d, 0x86, 0x87, 0x23, 0x85, 0x10,
	0xd7, 0xdf, 0xed, 0xa5, 0xe1, 0x7a, 0x85, 0x23, 0xea, 0x7e, 0x85, 0x98, 0xad, 0x79, 0x86, 0x7d, 0x85, 0x7d, 0xd7, 0x7d, 0xe1, 0x7a, 0xf5, 0x7d, 0x85, 0xb0, 0x2b, 0x37, 0xe1, 0x7a, 0x87, 0x79,
	0x84, 0x7d, 0x73, 0x73, 0x87, 0x7d, 0x23, 0x7d, 0xe9, 0x7d, 0x85, 0x7e, 0x02, 0x7d, 0xdd, 0x2d, 0x87, 0x79, 0xe7, 0x79, 0xad, 0x7c, 0x23, 0xda, 0x87, 0x0d, 0x0d, 0x7b, 0xe7, 0x79, 0x9b, 0x7d,
	0xd7, 0x8f, 0x05, 0x7d, 0x0d, 0x34, 0x8f, 0x7d, 0xad, 0x87, 0xe9, 0x7c, 0x85, 0x80, 0x85, 0x79, 0x8a, 0xc3, 0xe7, 0xa5, 0xe8, 0x6b, 0x0d, 0x74, 0x10, 0x73, 0x33, 0x17, 0x0d, 0x37, 0x21, 0x19
};
//Server.cpp
extern HWND	hWndMain;
extern uint32 CurrentTime;
//extern unsigned int LastSendTime;


//////////////////////////////////////////////////////////////////////////
//
//             CPSock,  ~CPSock
//
// Receive buffer and transmit buffer is allocated.
// Send and receive buffer pointer is reset Bubba
//////////////////////////////////////////////////////////////////////////

CPSock::CPSock()
{
	Sock = 0;
	Init = 0;
	pSendBuffer = (char*)malloc(SEND_BUFFER_SIZE);
	pRecvBuffer = (char*)malloc(RECV_BUFFER_SIZE);

	nSendPosition = 0;
	nSentPosition = 0;
	nRecvPosition = 0;
	nProcPosition = 0;
}

CPSock::~CPSock()
{
	if (pSendBuffer != NULL)
		free(pSendBuffer);

	if (pRecvBuffer != NULL)
		free(pRecvBuffer);
}

BOOL CPSock::CloseSocket()
{
	nSendPosition = 0;
	nSentPosition = 0;
	nRecvPosition = 0;
	nProcPosition = 0;
	Init = 0;

	if (Sock != 0)
		closesocket(Sock);

	Sock = 0;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////
//
//                       WSAInitialize   
//
// WSA initialization
// Server, all clients are executed once the first
/////////////////////////////////////////////////////////////////////////

BOOL CPSock::WSAInitialize()
{
	WSADATA wsa; //mudado
	return WSAStartup(MAKEWORD(2, 2), &wsa) == 0 ? TRUE : FALSE;
}

/////////////////////////////////////////////////////////////////////////
//
//							StartListen
//
// Listening socket (LSock) Listen to begin with.
// Connection should be connected to the WSA_ACCEPT.
// If successful LSock, a failure FALSE (0) is returned.
/////////////////////////////////////////////////////////////////////////

SOCKET CPSock::StartListen(HWND hWnd, int ip, int port, int WSA)
{

	SOCKADDR_IN	local_sin;
	char		Temp[256];
	SOCKET		tSock = socket(AF_INET, SOCK_STREAM, 0);

	if (tSock == INVALID_SOCKET)
	{
		MessageBox(hWnd, "Initialize socket fail", "ERROR", MB_OK);

		return FALSE;
	}

	gethostname(Temp, 256);
	local_sin.sin_family = AF_INET;
	local_sin.sin_addr.s_addr = ip;		//INADDR_ANY;
	local_sin.sin_port = htons((unsigned short int)port);

	if (bind(tSock, (struct sockaddr FAR*) & local_sin, sizeof(local_sin)) == SOCKET_ERROR)
	{
		MessageBox(hWnd, "Binding fail", "ERROR", MB_OK);
		closesocket(tSock);

		return FALSE;
	}

	if (listen(tSock, MAX_PENDING_CONNECTS) < 0)
	{
		MessageBox(hWnd, "Listen fail", "ERROR", MB_OK);
		closesocket(tSock);

		return FALSE;
	}

	if (WSAAsyncSelect(tSock, hWnd, WSA, FD_ACCEPT) > 0)
	{
		MessageBox(hWnd, "WSAAsyncSelect fail", "ERROR", MB_OK);
		closesocket(tSock);

		return FALSE;
	}

	Sock = tSock;

	return tSock;
}

SOCKET CPSock::ConnectServer(char* HostAddr, int Port, int ip, int WSA)
{
	SOCKADDR_IN InAddr;	//Connect
	SOCKADDR_IN local_sin; // Bind local IP

	memset(&InAddr, 0, sizeof(InAddr));
	memset(&local_sin, 0, sizeof(local_sin));

	nSendPosition = 0;
	nSentPosition = 0;
	nRecvPosition = 0;
	nProcPosition = 0;

	if (Sock != 0)
		CloseSocket();

	InAddr.sin_addr.s_addr = inet_addr(HostAddr);
	InAddr.sin_family = AF_INET;
	InAddr.sin_port = htons((unsigned short)Port);

	SOCKET tSock = socket(AF_INET, SOCK_STREAM, 0);

	if (tSock == INVALID_SOCKET)
	{
		MessageBox(NULL, "Initialize socket fail", "ERROR", MB_OK);
		return 0;
	}

	local_sin.sin_family = AF_INET;
	local_sin.sin_addr.s_addr = ip;
	local_sin.sin_port = 0;

	if (bind(tSock, (struct sockaddr FAR*) & local_sin, sizeof(local_sin)) == SOCKET_ERROR)
	{
		ConnectPort += 10;
		local_sin.sin_port = htons((unsigned short int)(ConnectPort + 5000));

		if (bind(tSock, (struct sockaddr FAR*) & local_sin, sizeof(local_sin)) == SOCKET_ERROR)
		{
			ConnectPort += 10;
			local_sin.sin_port = htons((unsigned short int)(ConnectPort + 5000));

			if (bind(tSock, (struct sockaddr FAR*) & local_sin, sizeof(local_sin)) == SOCKET_ERROR)
			{
				MessageBox(NULL, "Binding fail", "ERROR", MB_OK);
				closesocket(tSock);

				return 0;
			}
		}
	}

	if (tSock == INVALID_SOCKET)
		return 0;

	if (connect(tSock, (PSOCKADDR)&InAddr, sizeof(InAddr)) < 0)
	{
		closesocket(tSock);
		Sock = 0;

		return 0;
	}

	if (WSAAsyncSelect(tSock, hWndMain, WSA, FD_READ | FD_CLOSE) > 0)
	{
		closesocket(tSock);
		Sock = 0;

		return 0;
	}

	Sock = tSock;

	unsigned int InitCode = INITCODE;
	send(tSock, (char*)&InitCode, 4, 0);

	Init = 1;

	return tSock;
}

SOCKET CPSock::ConnectBillServer(char* HostAddr, int Port, int ip, int WSA)
{
	char msg[256];

	SOCKADDR_IN remote_sin;
	SOCKADDR_IN local_sin;
	SOCKET      tSock;

	nRecvPosition = 0;
	nSendPosition = 0;
	nProcPosition = 0;

	if (Sock != 0)
		CloseSocket();

	tSock = socket(AF_INET, SOCK_STREAM, 0);

	if (tSock == INVALID_SOCKET)
		return 0;

	local_sin.sin_family = AF_INET;
	local_sin.sin_addr.s_addr = ip;
	local_sin.sin_port = 0;

	if (bind(tSock, (struct sockaddr FAR*) & local_sin, sizeof(local_sin)) == SOCKET_ERROR)
	{
		local_sin.sin_port = htons((unsigned short int)(ConnectPort + 6000));

		if (bind(tSock, (struct sockaddr FAR*) & local_sin, sizeof(local_sin)) == SOCKET_ERROR)
		{
			ConnectPort += 10;
			local_sin.sin_port = htons((unsigned short int)(ConnectPort + 6000));

			if (bind(tSock, (struct sockaddr FAR*) & local_sin, sizeof(local_sin)) == SOCKET_ERROR)
			{
				MessageBox(NULL, "Binding fail", "ERROR", MB_OK);
				closesocket(tSock);

				return 0;
			}
		}
	}

	snprintf(msg, sizeof(msg), "sock:%d ip:%d.%d.%d.%d port:%d-%d", tSock, local_sin.sin_addr.S_un.S_un_b.s_b1, local_sin.sin_addr.S_un.S_un_b.s_b2,
		local_sin.sin_addr.S_un.S_un_b.s_b3, local_sin.sin_addr.S_un.S_un_b.s_b4, local_sin.sin_port, ConnectPort);

	ConnectPort++;

	remote_sin.sin_addr.s_addr = inet_addr(HostAddr);
	remote_sin.sin_family = AF_INET;
	remote_sin.sin_port = htons((unsigned short)Port);

	if (connect(tSock, (PSOCKADDR)&remote_sin, sizeof(remote_sin)) < 0)
	{
		snprintf(msg, sizeof(msg), "sock:%d ip:%d.%d.%d.%d port:%d-%d", tSock, remote_sin.sin_addr.S_un.S_un_b.s_b1, remote_sin.sin_addr.S_un.S_un_b.s_b2,
			remote_sin.sin_addr.S_un.S_un_b.s_b3, remote_sin.sin_addr.S_un.S_un_b.s_b4, remote_sin.sin_port, Port);

		closesocket(tSock);
		Sock = 0;

		return 0;
	}

	snprintf(msg, sizeof(msg), "sock:%d ip:%d.%d.%d.%d port:%d-%d", tSock, remote_sin.sin_addr.S_un.S_un_b.s_b1, remote_sin.sin_addr.S_un.S_un_b.s_b2,
		remote_sin.sin_addr.S_un.S_un_b.s_b3, remote_sin.sin_addr.S_un.S_un_b.s_b4, remote_sin.sin_port, Port);

	if (WSAAsyncSelect(tSock, hWndMain, WSA, FD_READ | FD_CLOSE) > 0)
	{
		closesocket(tSock);
		Sock = 0;

		return 0;
	}

	Sock = tSock;

	return tSock;
}

BOOL CPSock::Receive() // mudado
{
	int Rest = RECV_BUFFER_SIZE - this->nRecvPosition; // ok
	int tReceiveSize = recv(this->Sock, (char*)(this->pRecvBuffer + this->nRecvPosition), Rest, 0); // ok

	if (tReceiveSize == SOCKET_ERROR || tReceiveSize == Rest) // mudado
		return FALSE;

	this->nRecvPosition = this->nRecvPosition + tReceiveSize;

	if (tReceiveSize == Rest) // check adicionado do nix
	{
		this->RefreshRecvBuffer();
		Rest = RECV_BUFFER_SIZE - this->nRecvPosition;
		tReceiveSize = recv(Sock, (char*)(this->pRecvBuffer + this->nRecvPosition), Rest, 0);
		this->nRecvPosition += tReceiveSize;
	}

	return TRUE;
}

/*
nRecvPosition = Final do ultimo pacote
nProcPosition = Começo do ultimo pacote
*/
char* CPSock::ReadMessage(int* ErrorCode, int* ErrorType)
{
	*ErrorCode = 0;

	if (this->nProcPosition >= this->nRecvPosition)
	{
		this->nRecvPosition = 0;
		this->nProcPosition = 0;
		return 0;
	}

	//  Init packet authentication
	if (!this->Init) // era == 0
	{
		if (this->nRecvPosition - this->nProcPosition < 4)
			return 0;

		int InitCode = *((unsigned int*)(this->pRecvBuffer + this->nProcPosition)); // ok

		if (InitCode != INITCODE)
		{
			*ErrorCode = 2;
			*ErrorType = InitCode;
			this->CloseSocket(); // adicionado
			return 0;
		}

		this->Init = 1;
		this->nProcPosition += 4;
	}

	//	Check received message is larger than HEADER
	if (this->nRecvPosition - this->nProcPosition < sizeof(MSG_HEADER))
		return 0;

	//	Check full message arrived
	unsigned short	Size = *((unsigned short*)(this->pRecvBuffer + this->nProcPosition));
	unsigned char	iKeyWord = *((unsigned char*)(this->pRecvBuffer + this->nProcPosition + 2));
	unsigned char	KeyWord = pKeyWord[iKeyWord * 2];
	unsigned char	CheckSum = *((unsigned char*)(this->pRecvBuffer + this->nProcPosition + 3));

	if (Size > MAX_MESSAGE_SIZE || Size < sizeof(MSG_HEADER))
	{
		this->nRecvPosition = 0;
		this->nProcPosition = 0;
		*ErrorCode = 2;
		*ErrorType = Size;
		return 0;
	}

	unsigned int Rest = this->nRecvPosition - this->nProcPosition; // no nix é unsigned short aqui

	if (Size > Rest)
		return 0;

	//	Get message
	char* pMsg = &this->pRecvBuffer[this->nProcPosition];

	this->nProcPosition = this->nProcPosition + Size;
	if (this->nRecvPosition <= this->nProcPosition)
	{
		this->nRecvPosition = 0;
		this->nProcPosition = 0;
	}

	// Compare check_sum in packet
	unsigned char Sum1 = 0;
	unsigned char Sum2 = 0;

	int	pos = KeyWord;

	for (int i = 4; i < Size; i++, pos++)
	{
		int rst = pos % 256;

		Sum2 += pMsg[i];

		unsigned char Trans = pKeyWord[rst * 2 + 1];

		int mod = i & 0x3;
		//montar switch aqui depois
		if (mod == 0)
			pMsg[i] = pMsg[i] - (Trans << 1);

		if (mod == 1)
			pMsg[i] = pMsg[i] + (Trans >> 3);

		if (mod == 2)
			pMsg[i] = pMsg[i] - (Trans << 2);

		if (mod == 3)
			pMsg[i] = pMsg[i] + (Trans >> 5);

		Sum1 += pMsg[i];
	}

	unsigned char Sum = Sum2 - Sum1;

	// return packet, even check_sum not match
	if (Sum != CheckSum)
	{
		*ErrorCode = 1;
		*ErrorType = Size;

		return pMsg;
	}

	return pMsg;
} // trocada

char* CPSock::ReadBillMessage(int* ErrorCode, int* ErrorType)
{
	*ErrorCode = 0;
	*ErrorType = 0;

	if (nProcPosition >= nRecvPosition)
	{
		nRecvPosition = 0;
		nProcPosition = 0;

		return 0;
	}

	if (nRecvPosition - nProcPosition < g_cGame) // 196
		return 0;

	char* pMsg = &(pRecvBuffer[nProcPosition]);

	nProcPosition = nProcPosition + g_cGame;

	if (nRecvPosition <= nProcPosition)
	{
		nRecvPosition = 0;
		nProcPosition = 0;
	}

	return pMsg;
}

BOOL CPSock::SendBillMessage(char* Msg)
{
	if (nSendPosition + g_cGame >= SEND_BUFFER_SIZE)
		return FALSE;

	for (int i = 0; i < g_cGame; i++)
		pSendBuffer[nSendPosition + i] = Msg[i];

	nSendPosition = nSendPosition + g_cGame;

	BOOL Err = SendMessage();

	return Err;
}

BOOL CPSock::AddMessage(BYTE* pMsg, int Size)
{
	char temp[256];
	if (Size < 0 || Size > MAX_MESSAGE_SIZE) // check add nao tinha
	{
		//Log(temp, "[AddMessage] err, primeira checagem de size adicionada", 0);
		return FALSE;
	}
	
	MSG_HEADER* pSMsg = (MSG_HEADER*)pMsg;

	if ((this->nSendPosition + Size) >= SEND_BUFFER_SIZE)
	{
		this->RefreshSendBuffer(); // add

		if ((this->nSendPosition + Size) >= SEND_BUFFER_SIZE)
		{
			snprintf(temp, sizeof(temp), "err,add buffer full %d %d %d %d", this->nSendPosition, Size, pSMsg->Type, this->Sock);
			//Log(temp, "-system", 0);
			return FALSE;
		}
	}

	// check socket valid
	if (this->Sock <= 0)
	{
		snprintf(temp, sizeof(temp), "err,add buffer invalid %d %d %d %d", this->nSendPosition, Size, pSMsg->Type, this->Sock);
		//Log(temp, "-system", 0);
		return FALSE;
	}

	unsigned char iKeyWord = rand() % 256;
	unsigned char KeyWord = pKeyWord[iKeyWord * 2];
	unsigned char CheckSum = 0;
	pSMsg->Size = Size;
	pSMsg->KeyWord = iKeyWord;
	pSMsg->CheckSum = CheckSum;
	pSMsg->ClientTick = CurrentTime;
	//LastSendTime = CurrentTime;

#ifdef _PACKET_DEBUG
	MSG_STANDARD* debug = (MSG_STANDARD*)pMsg;

	if (BASE_CheckPacket(debug))
	{
		snprintf(temp, sizeof(temp), "**PACKET_DEBUG** Type:%d Size:%d", debug->Type, debug->Size);
		Log(temp, "-system", 0);
	}
#endif 

	unsigned char Sum1 = 0;
	unsigned char Sum2 = 0;
	int			  pos = KeyWord;

	for (int i = 4; i < Size; i++, pos++)
	{
		Sum1 += pMsg[i];

		int rst = pos % 256;

		unsigned char Trans = pKeyWord[rst * 2 + 1];

		int mod = i & 0x3;

		if (mod == 0)
			this->pSendBuffer[this->nSendPosition + i] = pMsg[i] + (Trans << 1);

		if (mod == 1)
			this->pSendBuffer[this->nSendPosition + i] = pMsg[i] - (Trans >> 3);

		if (mod == 2)
			this->pSendBuffer[this->nSendPosition + i] = pMsg[i] + (Trans << 2);

		if (mod == 3)
			this->pSendBuffer[this->nSendPosition + i] = pMsg[i] - (Trans >> 5);

		Sum2 += this->pSendBuffer[this->nSendPosition + i];
	}

	CheckSum = Sum2 - Sum1;
	pSMsg->CheckSum = CheckSum;

	memcpy(this->pSendBuffer + this->nSendPosition, pMsg, 4);

	this->nSendPosition = this->nSendPosition + Size;

	return TRUE;
}

void CPSock::RefreshRecvBuffer()
{
	int left = nRecvPosition - nProcPosition;
	if (left <= 0)
		return;

	memcpy(pRecvBuffer, pRecvBuffer + nProcPosition, left);

	nProcPosition = 0;
	nRecvPosition -= left;
}

void CPSock::RefreshSendBuffer()
{
	int left = nSendPosition - nSentPosition;
	if (left <= 0)
		return;

	memcpy(pSendBuffer, pSendBuffer + nSentPosition, left);

	nSentPosition = 0;
	nSendPosition -= left;
}

BOOL CPSock::SendMessageA()
{
#ifdef _PACKET_DEBUG
	char temp[256] = { 0, };
#endif 

	if (Sock <= 0)
	{
		this->nSendPosition = 0;
		this->nSentPosition = 0;
		return FALSE;
	}

	if (this->nSentPosition > 0)
		this->RefreshSendBuffer();

	if (this->nSendPosition > SEND_BUFFER_SIZE || this->nSendPosition < 0 || this->Sock < 0)
	{
#ifdef  _PACKET_DEBUG
		snprintf(temp, sizeof(temp), "err send1 %d %d %d", this->nSendPosition, this->nSentPosition, this->Sock);
		Log(temp, "-system", 0);
#endif
		this->nSendPosition = 0;
		this->nSentPosition = 0;
		return FALSE;
	}

	if (this->nSentPosition > this->nSendPosition || this->nSentPosition >= SEND_BUFFER_SIZE || this->nSentPosition < 0)
	{
#ifdef  _PACKET_DEBUG
		snprintf(temp, sizeof(temp), "err send2 %d %d %d", this->nSendPosition, this->nSentPosition, this->Sock);
		Log(temp, "-system", 0);
#endif
		this->nSendPosition = 0;
		this->nSentPosition = 0;
	}

	int err = 0;
	int	Left = this->nSendPosition - this->nSentPosition;
	int tSend = send(this->Sock, this->pSendBuffer + this->nSentPosition, Left, 0);
	//int nSend = send(Sock, (char*)(pSendBuffer + nSentPosition), Left, 0); // esse aqui tava no nix

	if (tSend != SOCKET_ERROR)
	{
		this->nSentPosition = this->nSentPosition + tSend;	// Bytes transmitted

		if (this->nSentPosition >= this->nSendPosition) // adicionado
		{
			this->nSendPosition = 0;
			this->nSentPosition = 0;
		}

		return TRUE;
	}
	else
	{
		err = WSAGetLastError();
		return FALSE;
	}

	if (this->nSendPosition > SEND_BUFFER_SIZE)
		return FALSE;

	return TRUE;
}

BOOL CPSock::SendOneMessage(BYTE* Msg, int Size)
{
	if (this->Sock == INVALID_SOCKET || this->Sock <= 0) // chack add
	{
		char temp[128];

		if (this->Sock == INVALID_SOCKET)
			snprintf(temp, sizeof(temp), "err [SendOneMessage - (INVALID_SOCKET)] Sock: [%d]", this->Sock);
		else if (this->Sock <= 0)
			snprintf(temp, sizeof(temp), "err [SendOneMessage - (Sock <= 0)] Sock: [%d]", this->Sock);

		//Log(temp, "-system", 0);
		return FALSE;
	}

	this->AddMessage(Msg, Size);
	return this->SendMessageA();
}