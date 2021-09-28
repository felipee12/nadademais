#pragma once

class CGet
{
public:
	BOOL GetEmptyMobGrid(int mob, int* tx, int* ty);
	int GetCreateMob(int id, MSG_CreateMob* sm);
	int GetCreateMobTrade(int id, MSG_CreateMobTrade* sm);
	int GetCurKill(int conn);
	int GetTotKill(int conn);
	int	GetPKPoint(int conn);
	int GetGuilty(int conn);
	void GetAffect(uint16* out, STRUCT_AFFECT* affect);
	void GetCreateItem(int idx, MSG_CreateItem* sm);
	void GetAction(int id, int targetx, int targety, MSG_Action* sm);
	unsigned char GetAttribute(int x, int y);
	int16 GetEmptyAffect(int id, uint8 type);
	bool GetEmptyItemGrid(int* gridx, int* gridy);
	int GetTeleportPosition(CUser* user, int* x, int* y);
	int GetInView(int a, CMob* target);
	int16 GetItemHab(short item, short Type);
	int16 GetSlot(CMob* mob, short itemID);
	bool GetFreeSlot(CMob* mob, uint8 quant);

};

extern CGet cGet;