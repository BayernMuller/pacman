#pragma once

#include "Mob.h" 
#include <thread>
#include <vector>
class CPacMap;

using namespace std;

class CGhost : public CMob
{
public:
	CGhost(CPacMap* pMap, int nNum, int nLevel);
	~CGhost();
	bool m_bFlag;
	bool m_bRunAway;
	bool m_bIsDead;
	void InitGhost();

	void SuspendMove();
	void ResumeMove();

private:
	int m_nNum;
	int m_nLevel;
	thread* m_pThread;
	CPacMap* m_pMap;

};

