#include "stdafx.h"
#include "Ghost.h"
#include <chrono>
#include "PacMap.h"
#include "Astar.h"

UINT GhostThread(void* wP, void* lP);

CGhost::CGhost(CPacMap* pMap, int nNum, int nLevel) :
	CMob(CRect(0, 0, WINSIZE / ARR_SIZE, WINSIZE / ARR_SIZE)),
	m_bFlag(true), m_nNum(nNum), m_bRunAway(false), m_pMap(pMap),
	m_pThread(nullptr), m_nLevel(nLevel), m_bIsDead(false)
{
	InitGhost();
	m_pThread = new thread(GhostThread, m_pMap, this);
}

CGhost::~CGhost()
{
	m_bIsDead = m_bRunAway = m_bFlag = false;
	if (m_pThread)
	{
		m_pThread->join();
		delete m_pThread;
		m_pThread = nullptr;
	}
}

void CGhost::InitGhost()
{
	m_nNum == 0 ? MoveTo(10, 7) : MoveTo(12 - m_nNum, 9);
}

void CGhost::SuspendMove()
{
	if (m_pThread)
		SuspendThread(m_pThread->native_handle());
}

void CGhost::ResumeMove()
{
	if (m_pThread)
		ResumeThread(m_pThread->native_handle());
}


UINT GhostThread(void* wP, void* lP)
{	
	CPacMap *pMap = (CPacMap *)wP;
	CGhost* pGhost = (CGhost*)lP;

	while (pGhost->m_bFlag)
	{
		if (pGhost->m_bIsDead && pGhost->IsCenter())
		{
			int x, y;
			CPacMap::GetPos(x, y, pGhost->GetRect());
			vector<Dir> way;
			CAstar astar(pMap->GetArray());
			astar.GetWay({ x,y }, { 10, 9 }, way);
			for (int i = 0; i < (int)way.size() && pGhost->m_bIsDead; i++)
			{
				for (int j = 0; j < 6 && pGhost->m_bIsDead; j++)
				{
					pGhost->Move(way[i], 4);
					this_thread::sleep_for(chrono::milliseconds(20));
				}
			}
			pGhost->m_bIsDead = false;
			continue;
		}

		if (pGhost->m_bRunAway && pGhost->IsCenter())			// 집으로 도망치기
		{			
			int x, y;
			CPacMap::GetPos(x, y, pGhost->GetRect());
			vector<Dir> way;
			CAstar astar(pMap->GetArray());
			astar.GetWay({ x,y }, { 10, 9 }, way);
			for (int i = 0; i < (int)way.size() && pGhost->m_bRunAway; i++)
			{
				for (int j = 0; j < 12 && pGhost->m_bRunAway; j++)
				{
					pGhost->Move(way[i]);
					this_thread::sleep_for(chrono::milliseconds(33));
				}
			}
			pGhost->m_bRunAway = false;
		}
		else  // 일반 이동
		{
			if (pGhost->IsCenter())
			{
				int nForkedCount = pMap->GetForkedCount(pGhost);
				if (nForkedCount > 1)
					pGhost->SetDirection(pMap->GetRandDir(pGhost));
				else if (nForkedCount == 1)
					pGhost->SetDirection(pMap->GetDirOnUnableMove(pGhost));
				else if (nForkedCount == 0)
					pGhost->SetDirection(Dir((pGhost->GetDirection() + 2) % 4));

			}
			if (pMap->CanMove(pGhost->GetDirection(), pGhost))
				pGhost->Move(pGhost->GetDirection());

			this_thread::sleep_for(chrono::milliseconds(22));
		}
	}
	return 0;
}