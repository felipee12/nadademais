#pragma once

class CSend
{
public:
	//void ClientSignal(CUser* user, int id, uint16 signal);
	//void ClientMsg(CUser* user, const std::string& Message);
	void GridMulticast(int tx, int ty, MSG_HEADER* msg, int skip);
	void GridMulticast(int conn, int tx, int ty, MSG_HEADER* msg);
	void SendPKInfo(CUser* user, int target);
	void SendGridMob(CUser* user);
	void SendCreateMob(CUser* user, int otherconn, int bSend);
	void SendCreateItem(CUser* user, int item, int bSend);
	void SendRemoveMob(CUser* user, int sour, int Type, int bSend);
	void SendRemoveItem(CUser* user, int itemid, int bSend);
	void SendHpMp(CUser* user);
	void SendSetHpMp(CUser* user);
	void SendHpMode(CUser* user);
	void SendScore(int conn);
};

extern CSend cSend;