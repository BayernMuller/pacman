#include "stdafx.h"
#include "PacMan.h"
#include <chrono>

CPacMan::CPacMan() : CMob(CRect(0, 0, WINSIZE / ARR_SIZE, WINSIZE / ARR_SIZE)), m_WaitingDir(LEFT), m_bIsCloseMouth(true)
{
	MoveTo(10, 15);	
}

CPacMan::~CPacMan()
{
	
}
