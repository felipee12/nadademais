#include "pch.h"

void CServer::TimerSec()
{
	cSrv.SecCounter++;

	CurrentTime = timeGetTime();

	for (size_t i = 0; i < MAX_USER; i++)
	{
		if (this->pUser[i].Mode)
		{
			if (this->pUser[i].cSock.nSendPosition)
			{
				if (!this->pUser[i].cSock.SendMessageA())
				{
					cLog.Write(0, TL::Err, "packet process error!");
				}
			}

		}
	}
}

void CServer::TimerMin()
{
	CurrentTime = timeGetTime();

#pragma region GenerateMobs
	for (uint16 i = 0; i < mNPCGen.NumList; i++)
	{
		if (i < MAX_NPCGENERATOR) // check pra nao estourar a array da pList
		{
			int MinuteGenerate = mNPCGen.pList[i].MinuteGenerate;

			if (i != 0 && i != 1 && i != 2 && i != 5 && i != 6 && i != 7 && i != -1)
			{
				if (MinuteGenerate <= 0)
					continue;

				int mod = i % MinuteGenerate;

				if (MinCounter % MinuteGenerate == mod)
				{
					GenerateMob(i, 0, 0);

					if (MinuteGenerate >= 500 && MinuteGenerate < 1000)
					{
						mNPCGen.pList[i].MinuteGenerate = rand() % 500 + 500;
						continue;
					}

					if (MinuteGenerate >= 1000 && MinuteGenerate < 2000)
					{
						mNPCGen.pList[i].MinuteGenerate = rand() % 1000 + 1000;
						continue;
					}

					if (MinuteGenerate >= 2000 && MinuteGenerate < 3800)
					{
						mNPCGen.pList[i].MinuteGenerate = rand() % 1800 + 2000;
						continue;
					}

					if (MinuteGenerate >= 3800)
					{
						mNPCGen.pList[i].MinuteGenerate = rand() % 1000 + 3800;

						/*if (DUNGEONEVENT)
						{
							int RndPos = rand() % 30;
							int dpX = DungeonPos[RndPos][0];
							int dpY = DungeonPos[RndPos][1];

							int RndL = rand() % 5 + 5;

							for (int j = 0; j < RndL; j++)
							{
								STRUCT_ITEM PrizeItem;

								memset(&PrizeItem, 0, sizeof(STRUCT_ITEM));

								PrizeItem.sIndex = DungeonItem[rand() % 10];

								SetItemBonus(&PrizeItem, 0, 0, 0);

								int rnd = rand() % 4;

								CreateItem(dpX, dpY, &PrizeItem, rnd, 1);
							}
						}*/
					}
				}
			}
		}
	}
#pragma endregion
	MinCounter++;
}