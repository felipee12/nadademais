#include "pch.h"

void CUser::RequestWhisper(MSG_HEADER* pMsg)
{
	if (pMsg->Size != sizeof(MSG_MessageWhisper))
	{
		cLog.Write(0, TL::Pakt, "-packet Size Diferente [MSG_MessageWhisper]");
		return;
	}

	if (this->Mode != USER_PLAY)
		return;

	auto mob = cSrv.getMob(this->conn);
	if (!mob)
		return;

	MSG_MessageWhisper* m = reinterpret_cast<MSG_MessageWhisper*>(pMsg);

	//size do comando maximo 15 caracteres e pós comando maximo 40
	if (strlen(m->MobName) > 15 || strlen(m->String) > 40)
		return;

	if (!CheckString(m->MobName, sizeof(m->MobName)) || !CheckString(m->String, 50))
		return;

	m->MobName[NAME_LENGTH - 1] = '\0';
	m->String[41] = '\0';

	if (this->MuteChat == 1)
	{
		this->ClientMsg(g_pMessageStringTable[_NN_No_Speak]);
		return;
	}

	if (!_stricmp(m->MobName, "aa"))
	{
		STRUCT_ITEM item;
		memset(&item, 0, sizeof(item));

		item.sIndex = 413;
		item.stEffect[0].cEffect = 61;
		item.stEffect[0].cValue = 10;

		this->PutItem(mob, &item);
	}

	//switch (m->Color)
	//{
	//case cmws::cp:			wMsg._cp(m);			return;
	//case cmws::getout:		wMsg._getout(m);		return;
	//case cmws::block:		wMsg._block(m);			return;
	//case cmws::unblock:		wMsg._unblock(m);		return;
	//case cmws::nig:			wMsg._nig(m);			return;
	//case cmws::spk:			wMsg._spk(m);			return;
	//case cmws::srv:			wMsg._srv(m);			return;
	//case cmws::nt:			wMsg._nt(m);			return;
	//case cmws::tab:			wMsg._tab(m);			return;
	//case cmws::snd:			wMsg._snd(m);			return;
	//case cmws::day: SendClientMsg(conn, "!#11  2"); return;
	//case cmws::kings:		wMsg._kings(m);			return;
	//case cmws::TIME:		wMsg._time(m);			return;
	//case cmws::create:		wMsg._create(m);		return;
	//case cmws::subcreate:	wMsg._subcreate(m);		return;
	//case cmws::expulsar:	wMsg._expulsar(m);		return;
	//case cmws::summonguild: wMsg._summonguild(m);	return; // ver
	//case cmws::relo:		wMsg._relo(m);			return; // ver
	//case cmws::geral1:		wMsg._geral1(m);		return;
	//case cmws::geral2:		wMsg._geral2(m);		return;
	//case cmws::armia: DoTeleport(conn, 2100, 2100);	return;
	//case cmws::gindex:		wMsg._gindex(m);		return;

	//default:
	//{
	//	if (m->Color >= 24 && m->Color <= 66)
	//		wMsg._gm(m);
	//}break;
	//}
}