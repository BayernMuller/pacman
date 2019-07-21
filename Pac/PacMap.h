#pragma once

#include "PacMan.h"
#include "Ghost.h"
#include <thread>
using namespace std;

class CPacMap
{
public:
	CPacMap(HWND hParent);
	~CPacMap();
	bool m_bFlag;
	void OnKeyDown(UINT nChar) const;
	void Draw(CDC* pDC);

	void EatFood();
	void OnRestartGame();
	CGhost* IsTouch();
	bool CanTurn(Dir dir, CMob* pMob) const;
	bool CanMove(Dir dir, CMob* pMob) const;
	void OnTouch(CGhost* pGhost);

	int GetForkedCount(CMob* pMob);
	Dir GetDirOnUnableMove(CMob* pMob) const;
	Dir GetRandDir(CMob* pMob);	

	static void GetRect(int x, int y, CRect& rect);
	static void GetPos(int &x, int &y, CRect& rect);
	static bool IsValidArrIndex(int x, int y)
	{ return (0 <= x && x < ARR_SIZE) && (0 <= y && y < ARR_SIZE); }
	void SwitchBigFoodState()
	{m_bBigFoodState = !m_bBigFoodState;}

	auto GetArray() { return m_Array; }

	static int GetRandom(int min, int max);
private:
	void DrawPacMan(CDC* pDC);
	void DrawGhost(CDC* pDC);
	void DrawMap(CDC* pDC) const;
	void DrawItem(CDC* pDC)	const;	
	void OnEatBigFood();

	inline double GetRadian(const int degree) 
	{ return degree * 3.141592 / 180; }

private:
	bool m_bIsDead;
	bool m_bBigFoodState;
	const int m_nNumOfFood;
	int m_nFoodCount;
	int m_nLevel;
	int m_nLife;

	CGhost *m_pGhost[4];
	byte m_Array[ARR_SIZE][ARR_SIZE];
	thread* m_pThread;
	CPacMan *m_pPacMan;
	HWND m_hParent;
};

