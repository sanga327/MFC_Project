
// Project_LENADoc.cpp: CProjectLENADoc 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "Project_LENA.h"
#endif

#include "Project_LENADoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CProjectLENADoc

IMPLEMENT_DYNCREATE(CProjectLENADoc, CDocument)

BEGIN_MESSAGE_MAP(CProjectLENADoc, CDocument)
END_MESSAGE_MAP()


// CProjectLENADoc 생성/소멸

CProjectLENADoc::CProjectLENADoc() noexcept
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.

}

CProjectLENADoc::~CProjectLENADoc()
{
}

BOOL CProjectLENADoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 재초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.

	return TRUE;
}




// CProjectLENADoc serialization

void CProjectLENADoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 여기에 저장 코드를 추가합니다.
	}
	else
	{
		// TODO: 여기에 로딩 코드를 추가합니다.
	}
}

#ifdef SHARED_HANDLERS

// 축소판 그림을 지원합니다.
void CProjectLENADoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 문서의 데이터를 그리려면 이 코드를 수정하십시오.
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 검색 처리기를 지원합니다.
void CProjectLENADoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 문서의 데이터에서 검색 콘텐츠를 설정합니다.
	// 콘텐츠 부분은 ";"로 구분되어야 합니다.

	// 예: strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CProjectLENADoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CProjectLENADoc 진단

#ifdef _DEBUG
void CProjectLENADoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CProjectLENADoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CProjectLENADoc 명령


BOOL CProjectLENADoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	//CFile에서 처리

	CFile hFile;      //파일을 읽어들이면 이 클래스 형식으로 저장.
	int height;       //이미지의 높이를 저장할 변수
	int width;        //이미지의 가로길이를 저장할 변수

	hFile.Open(lpszPathName, CFile::modeRead | CFile::typeBinary);    //파일 열기
	hFile.Read(&dibHf, sizeof(BITMAPFILEHEADER));   //dibHf에 파일헤더를 읽어들인다.

	//이 파일이 BMP파일인지 검사 
	if (dibHf.bfType != 0x4D42)
	{
		AfxMessageBox(L"Not BMP file!"); 
		return FALSE;       
	}

	hFile.Read(&dibHi, sizeof(BITMAPINFOHEADER));  //영상정보의 header 읽기
	if (dibHi.biBitCount != 8 && dibHi.biBitCount != 24)  //8,24비트가 아닐경우
	{
		AfxMessageBox(L"Gray/True Color Possible!");
		return FALSE;
	}

	if (dibHi.biBitCount == 8)

		//8비트의 경우 팔레트를 생성해 주어야 한다. 총 256가지 색이므로 그 길이만큼 읽어들인다
		hFile.Read(palRGB, sizeof(RGBQUAD) * 256);

	//메모리 할당
	int ImgSize;
	if (dibHi.biBitCount == 8)
	{
		ImgSize = hFile.GetLength() - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER) - 256 * sizeof(RGBQUAD);    //이미지의 크기는 파일 총 길이에서, 두 헤드와 팔레트의 사이즈를 제외한다.
	}                                                                       

	else if (dibHi.biBitCount == 24) //컬러영상
	{
		ImgSize = hFile.GetLength() - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER);
	}

	//이미지를 저장, 출력할 배열생성.

	m_InImg = new unsigned char[ImgSize];
	m_OutImg = NULL;
	hFile.Read(m_InImg, ImgSize);
	hFile.Close();

	//이미지의 길이정보
	m_height = dibHi.biHeight;
	m_width = dibHi.biWidth;
	m_size = m_height * m_width;

	return TRUE;
}


unsigned char* CProjectLENADoc::OnFrameComb(int size, unsigned char* img, unsigned char* mask_img, unsigned char* background_img)
{
	m_OutImg = new unsigned char[size];
	unsigned char not_mask_img;
	not_mask_img = NULL;
	int i;
	
	for (i = 0; i < size; i++) {
		not_mask_img = 255 - mask_img[i]; // 영상의 최대값에서 마스크 영상의 값을 뺀다.
		m_OutImg[i] = (img[i] & mask_img[i]) | (background_img[i] & not_mask_img);
		// AND(입력 영상, 마스크 영상) OR  AND(배경 영상, NOT(마스크 영상))
	}
	return m_OutImg;
}


void CProjectLENADoc::OnImgComb()
{
	CFile FileMask;
	CFile FileBackground;
	CFileDialog OpenDlgMask(TRUE);
	CFileDialog OpenDlgBackground(TRUE);

	int i;
	unsigned char *mask = nullptr;
	unsigned char *background = nullptr;


	// mask
	AfxMessageBox(L"마스크 이미지를 입력하세요.");
	if (OpenDlgMask.DoModal() == IDOK) {
		FileMask.Open(OpenDlgMask.GetPathName(), CFile::modeRead | CFile::typeBinary);
		FileMask.Read(&dibHf, sizeof(BITMAPFILEHEADER));   //dibHf에 파일헤더를 읽어들인다.

		//이 파일이 BMP파일인지 검사 
		if (dibHf.bfType != 0x4D42)
		{
			AfxMessageBox(L"Not BMP file!");
			return;
		}

		FileMask.Read(&dibHi, sizeof(BITMAPINFOHEADER));  //영상정보의 header 읽기
		if (dibHi.biBitCount != 8 && dibHi.biBitCount != 24)  //8,24비트가 아닐경우
		{
			AfxMessageBox(L"Gray/True Color Possible!");
			return;
		}

		if (dibHi.biBitCount == 8)
			//8비트의 경우 팔레트를 생성해 주어야 한다. 총 256가지 색이므로 그 길이만큼 읽어들인다
			FileMask.Read(palRGB, sizeof(RGBQUAD) * 256);

		//메모리 할당
		int ImgSize;
		if (dibHi.biBitCount == 8)
		{
			ImgSize = FileMask.GetLength() - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER) - 256 * sizeof(RGBQUAD);    //이미지의 크기는 파일 총 길이에서, 두 헤드와 팔레트의 사이즈를 제외한다.
		}

		else if (dibHi.biBitCount == 24) //컬러영상
		{
			ImgSize = FileMask.GetLength() - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER);
		}

		//이미지를 저장, 출력할 배열생성.

		mask = new unsigned char[ImgSize];
		FileMask.Read(mask, ImgSize);
		FileMask.Close();
	}

	// background
	AfxMessageBox(L"배경 이미지를 입력하세요.");
	if (OpenDlgBackground.DoModal() == IDOK) {
		FileBackground.Open(OpenDlgBackground.GetPathName(), CFile::modeRead | CFile::typeBinary);
		FileBackground.Read(&dibHf, sizeof(BITMAPFILEHEADER));   //dibHf에 파일헤더를 읽어들인다.

		//이 파일이 BMP파일인지 검사 
		if (dibHf.bfType != 0x4D42)
		{
			AfxMessageBox(L"Not BMP file!");
			return;
		}

		FileBackground.Read(&dibHi, sizeof(BITMAPINFOHEADER));  //영상정보의 header 읽기
		if (dibHi.biBitCount != 8 && dibHi.biBitCount != 24)  //8,24비트가 아닐경우
		{
			AfxMessageBox(L"Gray/True Color Possible!");
			return;
		}

		if (dibHi.biBitCount == 8)
			//8비트의 경우 팔레트를 생성해 주어야 한다. 총 256가지 색이므로 그 길이만큼 읽어들인다
			FileBackground.Read(palRGB, sizeof(RGBQUAD) * 256);

		//메모리 할당
		int ImgSize;
		if (dibHi.biBitCount == 8)
		{
			ImgSize = FileBackground.GetLength() - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER) - 256 * sizeof(RGBQUAD);    //이미지의 크기는 파일 총 길이에서, 두 헤드와 팔레트의 사이즈를 제외한다.
		}

		else if (dibHi.biBitCount == 24) //컬러영상
		{
			ImgSize = FileBackground.GetLength() - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER);
		}

		//이미지를 저장, 출력할 배열생성.

		background = new unsigned char[ImgSize];
		FileBackground.Read(background, ImgSize);
		FileBackground.Close();

		// 결과 이미지
		m_OutImg = new unsigned char[ImgSize];
		m_OutImg = OnFrameComb(ImgSize, m_InImg, mask, background);
	}

}


BOOL CProjectLENADoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	CFile hFile;
	CFileDialog SaveDlg(FALSE, L"bmp", NULL, OFN_HIDEREADONLY);

	if (SaveDlg.DoModal() == IDOK) {
		if (!hFile.Open(SaveDlg.GetPathName(), CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
			return FALSE;

		//정보저장
		hFile.Write(&dibHf, sizeof(BITMAPFILEHEADER));
		hFile.Write(&dibHi, sizeof(BITMAPINFOHEADER));

		if (dibHi.biBitCount == 8)
			hFile.Write(palRGB, sizeof(RGBQUAD) * 256);

		hFile.Write(m_OutImg, dibHi.biSizeImage);
		hFile.Close();
	}

	return TRUE;
}
