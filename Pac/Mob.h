#pragma once

class CMob
{	
public:
	CMob(CRect& rect);
	~CMob();

	void SetDirection(Dir dir);
	Dir GetDirection() { return m_Direction; }

	void Move(Dir dir, int range = 2);
	void MoveTo(int x, int y);
	bool IsCenter();
	CRect& GetRect() { return m_Rect; }

protected:
	CRect m_Rect;
	Dir m_Direction;	
};

