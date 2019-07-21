#include "stdafx.h"
#include "Mob.h"

short g_row[] = { -1,  0,  1,  0 };
short g_col[] = { 0, -1,  0,  1 };

CMob::CMob(CRect & rect) : m_Rect(rect), m_Direction(LEFT)
{
}


CMob::~CMob()
{
}

void CMob::SetDirection(Dir dir)
{
	m_Direction = dir;
}

void CMob::Move(Dir dir, int range)
{
	m_Rect.OffsetRect(g_row[dir] * range, g_col[dir] * range);
}

void CMob::MoveTo(int x, int y)
{
	int size = (WINSIZE / ARR_SIZE);
	x *= size;
	y *= size;	
	m_Rect.SetRect(x, y, x + size, y + size);
}

bool CMob::IsCenter()
{
	return (m_Rect.left % (WINSIZE / ARR_SIZE) == 0 && m_Rect.top % (WINSIZE / ARR_SIZE) == 0);
}
