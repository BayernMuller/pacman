#pragma once
#include <vector>
#include <set>
using namespace std;

#define NOWAY -1

struct node
{
	node(POINT& pt_, node* parent_ = nullptr) : pt(pt_), pParent(parent_), F(0) { }
	CPoint pt;
	UINT F;
	node* pParent;
};

class CAstar
{
public:
	CAstar(const byte(*ppArr)[ARR_SIZE]);
	~CAstar();
	
	void GetWay(POINT startPt, POINT targetPt, vector<Dir>& way);	

private:
	const byte(*m_ppArr)[ARR_SIZE];

	Dir GetDir(CPoint& pt, CPoint& nextPt);
	int GetCost(POINT& Pt, POINT& targetPt);
	void FreeList(set<node*>& list);
	node* FindOnList(set<node*>& list, CPoint& pt);
};