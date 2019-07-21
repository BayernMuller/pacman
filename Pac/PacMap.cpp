#include "stdafx.h"
#include "PacMap.h"
#include "resource.h"
#include <chrono>
#include <random>
#include "Astar.h"

UINT MoveThread(void* wP, void* wL);
int row[] = { -1,  0,  1,  0 };
int col[] = { 0,  -1,  0,  1 };



CPacMap::CPacMap(HWND hParent) : m_Array
{0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
 0,1,2,2,2,2,2,2,2,2,1,2,2,2,2,2,2,2,2,1,0,
 0,1,3,1,1,2,1,1,1,2,1,2,1,1,1,2,1,1,3,1,0,
 0,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,0,
 0,1,2,1,1,2,1,2,1,1,1,1,1,2,1,2,1,1,2,1,0,
 0,1,2,2,2,2,1,2,2,2,1,2,2,2,1,2,2,2,2,1,0,
 0,1,1,1,1,2,1,1,1,2,1,2,1,1,1,2,1,1,1,1,0,
 0,0,0,0,1,2,1,2,2,2,2,2,2,2,1,2,1,0,0,0,0,
 1,1,1,1,1,2,1,2,1,1,0,1,1,2,1,2,1,1,1,1,1,
 0,0,0,0,0,2,2,2,1,0,0,0,1,2,2,2,0,0,0,0,0,
 1,1,1,1,1,2,1,2,1,1,1,1,1,2,1,2,1,1,1,1,1,
 0,0,0,0,1,2,1,2,2,2,2,2,2,2,1,2,1,0,0,0,0,
 0,1,1,1,1,2,1,2,1,1,1,1,1,2,1,2,1,1,1,1,0,
 0,1,2,2,2,2,2,2,2,2,1,2,2,2,2,2,2,2,2,1,0,
 0,1,2,1,1,2,1,1,1,2,1,2,1,1,1,2,1,1,2,1,0,
 0,1,3,2,1,2,2,2,2,2,2,2,2,2,2,2,1,2,3,1,0,
 0,1,1,2,1,2,1,2,1,1,1,1,1,2,1,2,1,2,1,1,0,
 0,1,2,2,2,2,1,2,2,2,1,2,2,2,1,2,2,2,2,1,0,
 0,1,2,1,1,1,1,1,1,2,1,2,1,1,1,1,1,1,2,1,0,
 0,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,0,
 0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0 },
	m_pPacMan(nullptr), m_pThread(nullptr), m_bFlag(true)
	, m_nFoodCount(0), m_nNumOfFood(177), m_pGhost{ nullptr, }, m_hParent(hParent)
	, m_bBigFoodState(true), m_nLevel(0), m_bIsDead(false), m_nLife(3)
{
	m_pPacMan = new CPacMan;
	m_pThread = new thread(MoveThread, this, m_pPacMan);
	for (int i = 0; i < 4; i++)
	{
		m_pGhost[i] = new CGhost(this, i, 0);
	}	
}

CPacMap::~CPacMap()
{
	m_bFlag = false;
	if (m_pThread)
	{
		m_pThread->join();
		delete m_pThread;
		m_pThread = nullptr;
	}	
	for (int i = 0; i < 4; i++)
		if (m_pGhost[i])
			delete m_pGhost[i];
	delete m_pPacMan;
}

void CPacMap::OnKeyDown(UINT nChar) const
{
	if (VK_LEFT <= nChar && nChar <= VK_DOWN)
	{
		m_pPacMan->SetWaitingDir(Dir(nChar - VK_LEFT));
	}
}

void CPacMap::Draw(CDC * pDC)
{
	DrawMap(pDC);
	DrawItem(pDC);
	DrawPacMan(pDC);
	DrawGhost(pDC);
	if (m_bIsDead)
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(24, 24, 255));
		pDC->SetTextAlign(TA_CENTER);
		pDC->TextOut(WINSIZE / 2, WINSIZE / 2 - 9, _T("READY!"));
	}	
}

void CPacMap::DrawMap(CDC * pDC) const
{
	pDC->PatBlt(0, 0, WINSIZE, WINSIZE, BLACKNESS);
	CPen pen(PS_INSIDEFRAME, 2, RGB(24, 24, 255)), *pOldPen;
	pOldPen = pDC->SelectObject(&pen);
	int size = WINSIZE / ARR_SIZE;
	for (int i = 0; i < ARR_SIZE; i++)
		for (int j = 0; j < ARR_SIZE; j++)
			if (m_Array[j][i] == WALL)
			{
				CRect rect(i * size, j * size, i * size + size, j * size + size);
				int pt[] = { rect.left, rect.top,rect.right,rect.bottom };
				for (int k = 0; k < 4; k++)
				{
					if (!IsValidArrIndex(i + row[k], j + col[k]))
						continue;
					if (m_Array[j + col[k]][i + row[k]] != WALL)
					{
						pDC->MoveTo(pt[k < 2 ? 0 : 2], pt[k < 2 ? 1 : 3]);
						pDC->LineTo(pt[(k % 3) ? 2 : 0], pt[(k % 3) ? 1 : 3]);
					}
				}
			}
	CRect rect;
	GetRect(10, 8, rect);
	rect.DeflateRect(-1, 5);
	pDC->SelectStockObject(NULL_BRUSH);
	pDC->Rectangle(&rect);
	pDC->SelectStockObject(WHITE_BRUSH);
	pDC->SelectObject(pOldPen);
	pen.DeleteObject();
	
	CString str;
	str.Format(_T("Life : %d | Level : %d"), m_nLife, m_nLevel);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(24, 24, 255));
	pDC->SetTextAlign(TA_RIGHT);
	pDC->TextOut(140, WINSIZE - 20, str);
}

void CPacMap::DrawPacMan(CDC * pDC)
{
	CBrush brush(RGB(242, 219, 11)), *pOld;
	pOld = pDC->SelectObject(&brush);
	pDC->SelectStockObject(NULL_PEN);
	Dir dir = m_pPacMan->GetDirection();
	CRect rect = m_pPacMan->GetRect();
	int Degree = 210 - dir * 90;
	int radius = rect.Width();
	CPoint pt = rect.CenterPoint();
	if (m_pPacMan->IsCloseMouth())
		pDC->Pie(rect.left, rect.top, rect.right, rect.bottom,
		pt.x + int(radius * cos(GetRadian(Degree))),
		pt.y - int(radius * sin(GetRadian(Degree))),
		pt.x + int(radius * cos(GetRadian(Degree - 60))),
		pt.y - int(radius * sin(GetRadian(Degree - 60))));
	else
		pDC->Ellipse(rect);

	pDC->SelectObject(pOld);
	brush.DeleteObject();
	pDC->SelectStockObject(BLACK_PEN);
}

void CPacMap::DrawGhost(CDC * pDC)
{
	if (*m_pGhost == nullptr)
		return;

	CDC MemDC;
	MemDC.CreateCompatibleDC(pDC);
	MemDC.PatBlt(0, 0, WINSIZE, WINSIZE, BLACKNESS);
	int nCnt = 0;
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
		{
			CRect rect = m_pGhost[nCnt]->GetRect();
			if (m_pGhost[nCnt]->m_bRunAway)
			{
				CBitmap bitmap;
				bitmap.LoadBitmap(IDB_BITMAP2);
				BITMAP bmpinfo;
				bitmap.GetBitmap(&bmpinfo);
				CBitmap *pOldBitmap = (CBitmap*)MemDC.SelectObject(&bitmap);
				pDC->TransparentBlt(rect.left, rect.top, rect.Width(), rect.Height(), &MemDC, 0, 0, bmpinfo.bmWidth, bmpinfo.bmHeight, RGB(0, 0, 0));
				MemDC.SelectObject(pOldBitmap);
				bitmap.DeleteObject();
			}
			else if (m_pGhost[nCnt]->m_bIsDead)
			{
				rect.DeflateRect(rect.Width() / 4, rect.Height() / 4);
				pDC->Rectangle(rect);
			}
			else
			{
				CBitmap bitmap;
				bitmap.LoadBitmap(IDB_BITMAP1);
				BITMAP bmpinfo;
				bitmap.GetBitmap(&bmpinfo);
				CBitmap *pOldBitmap = (CBitmap*)MemDC.SelectObject(&bitmap);
				pDC->TransparentBlt(rect.left, rect.top, rect.Width(), rect.Height(), &MemDC, i * bmpinfo.bmWidth / 2, j * bmpinfo.bmHeight / 2, bmpinfo.bmWidth / 2, bmpinfo.bmHeight / 2, RGB(0, 0, 0));
				MemDC.SelectObject(pOldBitmap);
				bitmap.DeleteObject();
			}
			nCnt++;
		}
}


void CPacMap::DrawItem(CDC * pDC) const
{
	int size = WINSIZE / ARR_SIZE;
	pDC->SelectStockObject(NULL_PEN);
	CBrush brush(RGB(248, 176, 144)), *pOld;
	pOld = pDC->SelectObject(&brush);
	for (int i = 0; i < ARR_SIZE; i++)	
		for (int j = 0; j < ARR_SIZE; j++)		
			if (m_Array[j][i] != WALL && m_Array[j][i] != EMPTY)
			{
				int Deflate = (m_Array[j][i] == SMALL_FOOD) ? 3 : 5;
				if (m_bBigFoodState && Deflate == 5)
					continue;
				CRect rect(i * size, j * size, i * size + size, j * size + size);
				rect.DeflateRect(rect.Width() / Deflate + 1, rect.Height() / Deflate + 1);
				(m_Array[j][i] == SMALL_FOOD) ? pDC->Rectangle(rect): pDC->Ellipse(rect);
			}	
	pDC->SelectObject(pOld);
	pDC->SelectStockObject(BLACK_PEN);
	brush.DeleteObject();
}

void CPacMap::EatFood()
{
	if (m_pPacMan->IsCenter())
	{
		int x, y;
		GetPos(x, y, m_pPacMan->GetRect());
		if (m_Array[y][x] != WALL && m_Array[y][x] != EMPTY)
		{
			if (m_Array[y][x] == BIG_FOOD)
				OnEatBigFood();
			m_Array[y][x] = EMPTY;
			if (++m_nFoodCount == m_nNumOfFood)
				AfxMessageBox(L"끝");// 수정해
		}
	}
}

bool CPacMap::CanTurn(Dir dir, CMob* pMob) const
{
	int size = WINSIZE / ARR_SIZE, x, y;
	GetPos(x, y, pMob->GetRect());
	if (pMob == m_pPacMan && dir == DOWN && x == 10 && y == 7)
		return false;
	bool bIsVertical = (pMob->GetDirection() - dir) % 2 != 0;
	if (!(0 <= x + row[dir] && x + row[dir] < ARR_SIZE))
		return false;
	bool bIsWall = m_Array[y + col[dir]][x + row[dir]] == WALL;
	return (pMob->IsCenter() || !bIsVertical) && !bIsWall;
}

bool CPacMap::CanMove(Dir dir, CMob* pMob) const
{
	int size = WINSIZE / ARR_SIZE, x, y;
	if (pMob->IsCenter())
	{
		GetPos(x, y, pMob->GetRect());
		if (x == ARR_SIZE - 1 || x == 0)
			return true;
		if (x < 0 && dir == LEFT)
			pMob->MoveTo(21, 9);
		if (x > ARR_SIZE - 1 && dir == RIGHT)
			pMob->MoveTo(-1, 9);
		return m_Array[y + col[dir]][x + row[dir]] != WALL;
	}
	return true;
}


int CPacMap::GetForkedCount(CMob * pMob)
{
	int x, y, cnt = 0;
	GetPos(x, y, pMob->GetRect());

	if ((x <= 0 || x >= ARR_SIZE - 1))
		return TUNNEL;
	Dir dir = pMob->GetDirection();
	for (int i = 0; i < 4; i++)
	{
		if ((dir + 2) % 4 == i)
			continue;
		if (m_Array[y + col[i]][x + row[i]] != WALL)
			cnt++;
	}	
	return cnt;
}

Dir CPacMap::GetDirOnUnableMove(CMob * pMob) const
{
	int x, y, cnt = 0;
	GetPos(x, y, pMob->GetRect());
	Dir dir = pMob->GetDirection();
	for (int i = 0; i < 4; i++)
	{
		if (dir == Dir(i) || (dir + 2) % 4 == i)
			continue;
		if (m_Array[y + col[i]][x + row[i]] != WALL)
		{
			dir = Dir(i);
			break;
		}
	}
	return dir;
}

Dir CPacMap::GetRandDir(CMob * pMob)
{
	int x, y, cnt = 0, tarX, tarY;
	GetPos(x, y, pMob->GetRect());
	GetPos(tarX, tarY, m_pPacMan->GetRect());
	Dir dir = pMob->GetDirection();
	Dir arr[3];
	for (int i = 0; i < 4; i++)
	{
		if ((dir + 2) % 4 == i)
			continue;
		if (m_Array[y + col[i]][x + row[i]] != WALL)
		{		
			if (GetRandom(0 ,3) == 0)
			{
				if ((x < tarX && i == RIGHT) | (x > tarX && i == LEFT) | (y > tarY && i == UP) | (y < tarY && i == DOWN))
					return Dir(i);
			}
			arr[cnt++] = Dir(i);
		}
	}
	return arr[GetRandom(0, cnt - 1)];
}


void CPacMap::GetRect(int x, int y, CRect &rect)
{
	int size = (WINSIZE / ARR_SIZE);
	x *= size;
	y *= size;
	rect.SetRect(x, y, x + size, y + size);
}

void CPacMap::GetPos(int & x, int & y, CRect & rect)
{
	int size = (WINSIZE / ARR_SIZE);
	x = rect.right / size - 1;
	y = rect.bottom / size - 1;
}

int CPacMap::GetRandom(int min, int max)
{
	random_device rn;
	mt19937_64 rnd(rn());
	uniform_int_distribution<int> range(min, max);
	return range(rnd);
}

void CPacMap::OnTouch(CGhost* pGhost)
{
	if (--m_nLife == 0)
	{
		OnRestartGame();
		PostMessage(m_hParent, UM_GAMEOVER, NULL, NULL);
		return;
	}
	m_bIsDead = true;
	for (int i = 0; i < 4; i++)
	{
		m_pGhost[i]->SuspendMove();
		m_pGhost[i]->InitGhost();
	}
	m_pPacMan->MoveTo(10, 15);
}

void CPacMap::OnRestartGame()
{
	m_bIsDead = false;
	for (int i = 0; i < 4; i++)
	{
		m_pGhost[i]->m_bIsDead = m_pGhost[i]->m_bRunAway = false;
		m_pGhost[i]->ResumeMove();
	}

	if (!m_pPacMan->IsCloseMouth())
		m_pPacMan->SwitchMouthState();
	m_pPacMan->SetDirection(LEFT);
}

CGhost* CPacMap::IsTouch()
{
	for (int i = 0; i < 4; i++)
	{
		if (m_pGhost[i])
			if (m_pPacMan->GetRect().PtInRect(m_pGhost[i]->GetRect().CenterPoint()))
			{
				return m_pGhost[i];
			}
	}
	return nullptr;
}


void CPacMap::OnEatBigFood()
{
	for (int i = 0; i < 4; i++)
	{
		if (!m_pGhost[i]->m_bRunAway)
			m_pGhost[i]->m_bRunAway = true;
	}
}

UINT MoveThread(void* wP, void* wL)
{
	CPacMap* pMap = (CPacMap*)wP;
	CPacMan* pPac = (CPacMan*)wL;
	unsigned int nCnt = 0;
	while (pMap->m_bFlag)
	{
		if (pMap->CanMove(pPac->GetDirection(), pPac))
		{
			pPac->Move(pPac->GetDirection());
			pPac->SwitchMouthState();
			pMap->EatFood();
		}

		if (pMap->CanTurn(pPac->GetWaitingDir(), pPac))
			pPac->SetDirection((pPac->GetWaitingDir()));

		if (nCnt++ % 15 == 0)
			pMap->SwitchBigFoodState();

		CGhost* pGhost = pMap->IsTouch();
		if (pGhost != nullptr && pGhost->m_bIsDead == false)
		{
			if (!pGhost->m_bRunAway)
			{
				pMap->OnTouch(pGhost);
				nCnt = 0;
				this_thread::sleep_for(chrono::milliseconds(1000));
				pMap->OnRestartGame();
			}
			else
			{
				pGhost->m_bRunAway = false;
				pGhost->m_bIsDead = true;
			}
		}
			this_thread::sleep_for(chrono::milliseconds(20));
	}
	return 0;
}