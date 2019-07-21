#include "stdafx.h"
#include "Astar.h"
#include "PacMap.h"
#include "Mob.h"
#include <algorithm>

int Row[] = { -1,  0,  1,  0 };
int Col[] = { 0,  -1,  0,  1 };

CAstar::CAstar(const byte(*ppArr)[ARR_SIZE]) : m_ppArr(ppArr)
{

}


CAstar::~CAstar()
{
}

void CAstar::GetWay(POINT startPt, POINT targetPt, vector<Dir>& way)
{
	node* current = nullptr;
	set<node*> openList, closeList;
	openList.insert(new node(startPt));
	while (!openList.empty())
	{
		current = *openList.begin();
		for (auto Node : openList)
			if (Node->F <= current->F)
				current = Node;		

		if (current->pt == targetPt)
			break;

		closeList.insert(current);
		openList.erase(find(openList.begin(), openList.end(), current));

		for (int dir = 0; dir < 4; dir++)
		{
			CPoint newPt(current->pt.x + Row[dir], current->pt.y + Col[dir]);
			if (!CPacMap::IsValidArrIndex(newPt.x, newPt.y) ||
				FindOnList(closeList, newPt) ||
				m_ppArr[newPt.y][newPt.x] == WALL)
				continue;

			node* pSuccessor = FindOnList(openList, newPt);
			if (pSuccessor == nullptr)
			{
				pSuccessor = new node(newPt, current);
				pSuccessor->F = GetCost(newPt, targetPt);
				openList.insert(pSuccessor);
			}
		}
	}

	while (current != nullptr)
	{
		if (current->pParent != nullptr)
			way.push_back(GetDir(current->pt, current->pParent->pt));
		current = current->pParent;
	}
	reverse(way.begin(), way.end());
	FreeList(openList);
	FreeList(closeList);
}

Dir CAstar::GetDir(CPoint & pt, CPoint & nextPt)
{
	for (int i = 0; i < 4; i++)
	{
		if (pt.x - nextPt.x == Row[i] && pt.y - nextPt.y == Col[i])
			return Dir(i);
	}
	return Dir();
}

int CAstar::GetCost(POINT & Pt, POINT & targetPt)
{
	return abs(Pt.x - targetPt.x) + abs(Pt.y - targetPt.y);
}

void CAstar::FreeList(set<node*>& list)
{
	for (auto itr = list.begin(); itr != list.end();)
	{
		delete *itr;
		itr = list.erase(itr);
	}
}

node * CAstar::FindOnList(set<node*>& list, CPoint& pt)
{
	for (auto Node : list)
	{
		if (pt == Node->pt)
			return Node;
	}
	return nullptr;
}
