#pragma once
#include "Mob.h"


class CPacMan : public CMob
{
public:
	CPacMan();
	~CPacMan();
	void SetWaitingDir(Dir dir) { m_WaitingDir = dir; }
	Dir GetWaitingDir() { return m_WaitingDir; }

	void SwitchMouthState() { m_bIsCloseMouth = !m_bIsCloseMouth; }
	bool IsCloseMouth() { return m_bIsCloseMouth; }

private:
	Dir m_WaitingDir;
	bool m_bIsCloseMouth;
};

