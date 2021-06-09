
// Project_LENAView.cpp: CProjectLENAView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "Project_LENA.h"
#endif

#include "Project_LENADoc.h"
#include "Project_LENAView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CProjectLENAView

IMPLEMENT_DYNCREATE(CProjectLENAView, CView)

BEGIN_MESSAGE_MAP(CProjectLENAView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CProjectLENAView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_IMG_COMB, &CProjectLENAView::OnImgComb)
END_MESSAGE_MAP()

// CProjectLENAView 생성/소멸

CProjectLENAView::CProjectLENAView() noexcept
{
	// TODO: 여기에 생성 코드를 추가합니다.

	BmInfo = (BITMAPINFO*)malloc(sizeof(BITMAPINFO) + 256 * sizeof(RGBQUAD));
	for (int i = 0; i < 256; i++)
	{
		BmInfo->bmiColors[i].rgbRed = BmInfo->bmiColors[i].rgbGreen = BmInfo->bmiColors[i].rgbReserved = 0;
	}

}

CProjectLENAView::~CProjectLENAView()
{
	if (BmInfo) delete BmInfo;
}

BOOL CProjectLENAView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CProjectLENAView 그리기

void CProjectLENAView::OnDraw(CDC* pDC)
{
	#define WIDTHBYTES(bits) (((bits)+31)/32*4);   //이미지 가로 바이트 길이는 4바이트의 배수

	//BmInfo;
	int height;
	int width;
	int rwsize;

	CProjectLENADoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
	if (pDoc->m_InImg == NULL)
		return;


	//24비트 비트맵 파일의 영상출력
	if (pDoc->dibHi.biBitCount == 24) {
		height = pDoc->dibHi.biHeight;
		width = pDoc->dibHi.biWidth;
		rwsize = WIDTHBYTES(pDoc->dibHi.biBitCount*pDoc->dibHi.biWidth);
		BmInfo->bmiHeader = pDoc->dibHi;
		SetDIBitsToDevice(pDC->GetSafeHdc(), 0, 0, width, height, 0, 0, 0, height, pDoc->m_InImg, BmInfo, DIB_RGB_COLORS);
	}

	else	//8비트 컬러일 경우
	{
		int index;
		rwsize = WIDTHBYTES(pDoc->dibHi.biBitCount*pDoc->dibHi.biWidth);

		//팔레트를 읽어들이며 반복출력
		for (int i = 0; i < pDoc->dibHi.biHeight; i++) {
			for (int j = 0; j < pDoc->dibHi.biWidth; j++) {
				index = pDoc->m_InImg[i*rwsize + j];
				BYTE R = pDoc->palRGB[index].rgbRed;
				BYTE G = pDoc->palRGB[index].rgbGreen;
				BYTE B = pDoc->palRGB[index].rgbBlue;

				//pDC->SetPixel(j,i,RGB(R,G,B));

				//영상 반전출력
				pDC->SetPixel(j, pDoc->dibHi.biHeight - i - 1, RGB(R, G, B));
			}
		}
	}

	// output 출력
	if (pDoc->m_OutImg == NULL) {
		return;
	}

	//24비트 비트맵 파일의 영상출력
	if (pDoc->dibHi.biBitCount == 24) {
		height = pDoc->dibHi.biHeight;
		width = pDoc->dibHi.biWidth;
		rwsize = WIDTHBYTES(pDoc->dibHi.biBitCount*pDoc->dibHi.biWidth);
		BmInfo->bmiHeader = pDoc->dibHi;
		SetDIBitsToDevice(pDC->GetSafeHdc(), 512, 0, width, height, 0, 0, 0, height, pDoc->m_OutImg, BmInfo, DIB_RGB_COLORS);
	}

	else	//8비트 컬러일 경우
	{
		int index;
		rwsize = WIDTHBYTES(pDoc->dibHi.biBitCount*pDoc->dibHi.biWidth);

		//팔레트를 읽어들이며 반복출력
		for (int i = 0; i < pDoc->dibHi.biHeight; i++) {
			for (int j = 0; j < pDoc->dibHi.biWidth; j++) {
				index = pDoc->m_OutImg[i*rwsize + j];
				BYTE R = pDoc->palRGB[index].rgbRed;
				BYTE G = pDoc->palRGB[index].rgbGreen;
				BYTE B = pDoc->palRGB[index].rgbBlue;

				//pDC->SetPixel(j,i,RGB(R,G,B));

				//영상 반전출력
				pDC->SetPixel(j+512, pDoc->dibHi.biHeight - i - 1, RGB(R, G, B));
			}
		}
	}



}


// CProjectLENAView 인쇄


void CProjectLENAView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CProjectLENAView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CProjectLENAView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CProjectLENAView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}

void CProjectLENAView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CProjectLENAView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CProjectLENAView 진단

#ifdef _DEBUG
void CProjectLENAView::AssertValid() const
{
	CView::AssertValid();
}

void CProjectLENAView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CProjectLENADoc* CProjectLENAView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CProjectLENADoc)));
	return (CProjectLENADoc*)m_pDocument;
}
#endif //_DEBUG


// CProjectLENAView 메시지 처리기


void CProjectLENAView::OnImgComb()
{
	CProjectLENADoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	pDoc->OnImgComb();

	Invalidate(TRUE);

}
