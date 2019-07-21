
// PacView.h : CPacView 클래스의 인터페이스
//

#pragma once
class CPacMap;
#include <thread>

class CPacView : public CView
{
protected: // serialization에서만 만들어집니다.
	CPacView();
	DECLARE_DYNCREATE(CPacView)
private:
	CPacMap* m_pMap;
	std::thread* m_pThread;
// 특성입니다.
public:
	CPacDoc* GetDocument() const;
	bool m_bFlag;
// 작업입니다.
public:

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 구현입니다.
public:
	virtual ~CPacView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg LRESULT OnGameOver(WPARAM wParam, LPARAM lParam);
};

#ifndef _DEBUG  // PacView.cpp의 디버그 버전
inline CPacDoc* CPacView::GetDocument() const
   { return reinterpret_cast<CPacDoc*>(m_pDocument); }
#endif

