#pragma once

enum USER
{
	USER_EMPTY,
	USER_ACCEPT,
	USER_LOGIN,

	USER_SELCHAR = 11,
	USER_CHARWAIT,
	USER_WAITDB,

	USER_PLAY = 22,
	USER_SAVING4QUIT = 24
};

class CUser
{
public:

	stUserExtra uExtra; // eee

	char AccountName[16];
	uint8 Slot;
	uint32 IP;

	int Mode;
	int TradeMode;
	CPSock cSock;

	STRUCT_ITEM Cargo[128];
	int Coin;

	uint16 cProgress;

	STRUCT_SELCHAR SelChar;
	MSG_Trade Trade;
	MSG_SendAutoTrade AutoTrade;

	int LastAttack;
	int LastAttackTick;
	int LastMove;
	int LastAction;
	int LastActionTick;
	int LastIllusionTick;

	char LastChat[16];

	unsigned char CharShortSkill[16];

	bool Whisper;
	bool Guildchat;
	bool PartyChat;
	bool MuteChat;
	bool KingChat;
	bool Chatting;
	bool PKMode;
	bool Primary;

	int HP;
	int MP;

	int LastReceiveTime;

	int Range;
	int CastleStatus;

	uint16 conn;

public:
	CUser();
	~CUser();
	void AcceptUser();
	bool Disconnect();
	//void CloseUser();

	bool PutItem(CMob* mob, STRUCT_ITEM* item);

	//SEND
	void ClientMsg(const std::string& Message);
	void SendClientSignal(int16 id, uint16 signal);
	void SendRemoveParty(int16 connExit);
	void SendEtc();
	void SendCargoCoin();
	void SendShopList(int MobIndex, int ShopType);
	void SendAffect();
	void SendItem(short type, short Slot, STRUCT_ITEM* item);
	void SendEquip(int skip);
	//RECV

	bool RequestAccountLogin(int id, MSG_HEADER* pMsg);
	bool RequestSecondPass(MSG_HEADER* pMsg);
	bool RequestCreateChar(MSG_HEADER* pMsg);
	bool RequestLogout(MSG_HEADER* pMsg);
	bool RequestCharacterLogin(MSG_HEADER* pMsg);
	bool RequestDeleteCharacter(MSG_HEADER* pMsg);
	bool RequestMovement(MSG_HEADER* pMsg);
	bool RequestChangeCity(MSG_HEADER* pMsg);
	bool RequestTeleport(MSG_HEADER* pMsg);
	bool RequestShopList(MSG_HEADER* pMsg);
	bool RequestDeposit(MSG_HEADER* pMsg);
	bool RequestWithdraw(MSG_HEADER* pMsg);
	bool RequestMoveItem(MSG_HEADER* pMsg);
	bool RequestDeleteItem(MSG_HEADER* pMsg);
	bool RequestSplitItem(MSG_HEADER* pMsg);
	void RequestWhisper(MSG_HEADER* pMsg);
	void RequestChat(MSG_HEADER* pMsg);
	void RequestAttack(MSG_HEADER* pMsg);
};