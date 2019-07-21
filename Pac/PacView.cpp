
// PacView.cpp : CPacView 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "Pac.h"
#endif

#include "PacDoc.h"
#include "PacView.h"

#include "PacMap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
using namespace std;

UINT DrawThread(void* p);

// CPacView

IMPLEMENT_DYNCREATE(CPacView, CView)

BEGIN_MESSAGE_MAP(CPacView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_KEYDOWN()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(UM_GAMEOVER, &CPacView::OnGameOver)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// CPacView 생성/소멸

CPacView::CPacView() : m_pMap(nullptr), m_bFlag(true), m_pThread(nullptr)
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

CPacView::~CPacView()
{
	
}

BOOL CPacView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.
	cs.style &= ~WS_BORDER;
	return CView::PreCreateWindow(cs);
}

// CPacView 그리기

void CPacView::OnDraw(CDC* pDC)
{
	CPacDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
	CDC MemDC;
	CRect rect;
	GetClientRect(&rect);
	MemDC.CreateCompatibleDC(pDC);
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
	CBitmap *pOldBitmap = (CBitmap*)MemDC.SelectObject(&bitmap);

	if (m_pMap)
		m_pMap->Draw(&MemDC);	

	pDC->BitBlt(0, 0, rect.Width(), rect.Height(), &MemDC, 0, 0, SRCCOPY);
	MemDC.SelectObject(pOldBitmap);
	bitmap.DeleteObject();	
}


// CPacView 인쇄

BOOL CPacView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CPacView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CPacView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}


// CPacView 진단

#ifdef _DEBUG
void CPacView::AssertValid() const
{
	CView::AssertValid();
}

void CPacView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CPacDoc* CPacView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPacDoc)));
	return (CPacDoc*)m_pDocument;
}
#endif //_DEBUG


// CPacView 메시지 처리기


void CPacView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	m_pThread = new thread(DrawThread, this);
	m_pMap = new CPacMap(m_hWnd);
	
}


void CPacView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (m_pMap)
	{
		m_pMap->OnKeyDown(nChar);
	}
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}


BOOL CPacView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

UINT DrawThread(void* p)
{
	CPacView* pView = (CPacView*)p;
	while (pView->m_bFlag)
	{
		pView->Invalidate(0);
		this_thread::sleep_for(chrono::milliseconds(50));
	}
	return 0;
}

void CPacView::OnDestroy()
{
	CView::OnDestroy();

	m_bFlag = false;
	if (m_pThread)
	{
		m_pThread->join();
		delete m_pThread;
		m_pThread = nullptr;
	}
	delete m_pMap;
}

LRESULT CPacView::OnGameOver(WPARAM wParam, LPARAM lParam)
{
	delete m_pMap;
	m_pMap = new CPacMap(m_hWnd);
	return LRESULT();
}
