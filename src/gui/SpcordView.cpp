/////////////////////////////////////////////////////////////////////////////
// SpcordView.cpp : implementation file
#include "precompile.h"

#ifndef __CONSOLE__

#include "SpcordView.h"
#include "Grafor.h"
#include "SpcordDoc.h"
#include "MainFrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif // _DEBUG

const int MAX_Y_LP = 5000;
const int MARGINS_SP = 2000; // x0.01mm

/////////////////////////////////////////////////////////////////////////////
// CScale view
class CScale{
	public:
		CScale() : MaxPhis( 1. ), MaxLog( 1. ){};
		CScale( const CScale &scale ) : MaxPhis( scale.MaxPhis ), MaxLog( scale.MaxLog ){};
		~CScale(){};
	public:
		CScale &operator =( const CScale &scale )
		{
			MaxPhis = scale.MaxPhis;
			MaxLog  = scale.MaxLog;
			return *this;
		};
		hhn_pair< int > PHtoLP( const hhn_pair<float> &point) const
		{
			hhn_pair< int > ret;
			ret.X = int(( point.X*MaxLog )/MaxPhis );
			ret.Y = int(( point.Y*MaxLog )/MaxPhis );
			return ret;
		};
		hhn_pair< float > LPtoPH( const hhn_pair<int> &point) const
		{
			hhn_pair< float > ret;
			ret.X = float(( point.X*MaxPhis )/MaxLog );
			ret.Y = float(( point.Y*MaxPhis )/MaxLog );
			return ret;
		};
	public:
		float MaxPhis;
		float MaxLog;
};

/////////////////////////////////////////////////////////////////////////////
// neurosim_view diagnostics
#ifdef _DEBUG
void neurosim_view::AssertValid() const
{
	CView::AssertValid();
}

void neurosim_view::Dump( CDumpContext &dc ) const
{
	CView::Dump( dc );
}

neurosim_doc* neurosim_view::GetDocument() // non-debug version is inline
{
	ASSERT( m_pDocument->IsKindOf( RUNTIME_CLASS( neurosim_doc )));
	return ( neurosim_doc *)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// neurosim_view
IMPLEMENT_DYNCREATE( neurosim_view, CView )

neurosim_view::neurosim_view( void )
{
	InitUpdate = false;
	IsUpdated = false;
	LastCounter = 0;
	NewCounter = 0;
	dcMem = NULL;
	Bitmap = NULL;
	View = NULL;
}

neurosim_view::~neurosim_view( void )
{
	if( Bitmap ){
		Bitmap->DeleteObject();
		delete Bitmap;
	}
	if( dcMem )
		delete dcMem;
}

BEGIN_MESSAGE_MAP(neurosim_view, CView)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

const char *neurosim_view::getViewName( void ) const
{
	return View? View->get_name():NULL; 
}

/////////////////////////////////////////////////////////////////////////////
// neurosim_view drawing
void neurosim_view::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	InitUpdate = true;
	IsUpdated = false;
	LastCounter = 0;
	NewCounter = LastCounter;
}

BOOL neurosim_view::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style |= WS_CLIPCHILDREN;
	return CView::PreCreateWindow(cs);
}

BOOL neurosim_view::OnPreparePrinting(CPrintInfo* pInfo) 
{
	pInfo->SetMaxPage( 1 );
	return DoPreparePrinting(pInfo);
}

void neurosim_view::OnDraw(CDC* pDC)
{
	CRect rc;
	if( pDC->IsPrinting() ){
		CSize sz( MARGINS_SP, MARGINS_SP );
		pDC->HIMETRICtoDP( &sz );
		rc.top = sz.cy;
		rc.left = sz.cx;
		sz.cy = pDC->GetDeviceCaps( VERTSIZE )*100-MARGINS_SP;
		sz.cx = pDC->GetDeviceCaps( HORZSIZE )*100-MARGINS_SP;
		pDC->HIMETRICtoDP( &sz );
		rc.bottom = sz.cy;
		rc.right = sz.cx;
		PrepareDC( pDC, rc );
		LastCounter = 0;
		NewCounter = GetMaxCounter();
		DrawPicture( pDC, rc );
	}
	else{
		GetClientRect( &rc );
		CRect rcMem( rc );
		pDC->DPtoLP( &rc );
		dcMem->DPtoLP( &rcMem );
		::StretchBlt( pDC->GetSafeHdc(), rc.left, rc.top, rc.Width(), rc.Height(),
			dcMem->GetSafeHdc(), rcMem.left, rcMem.top, rcMem.Width(), rcMem.Height(),
			SRCCOPY);
	}
	IsUpdated = true;
}

void neurosim_view::DrawTo(bool complete, unsigned long counter)
{
	neurosim_doc *pDoc = GetDocument(); if( !pDoc )	return;
	if( Bitmap ){
		CRect rect;
		GetClientRect( &rect );
		PrepareDC( dcMem, rect );
		if( complete ){
			EraseBackground();
			LastCounter = 0;
			NewCounter = GetMaxCounter();
			DrawPicture( dcMem, rect );
			Invalidate( FALSE );
		}
		else if( IsUpdated && LastCounter < counter ){
			IsUpdated = false;
			NewCounter = counter < GetMaxCounter()? counter:GetMaxCounter();
			DrawPicture( dcMem, rect );
			Invalidate( FALSE );
		}
		else if( IsUpdated && LastCounter > counter ){
			EraseBackground();
			LastCounter = 0;
			NewCounter = GetMaxCounter();
			DrawPicture( dcMem, rect );
			Invalidate( FALSE );
		}
	}
}

void neurosim_view::InitDraw()
{
	IsUpdated = false;
	CRect rect;
	GetClientRect( &rect );
	LastCounter = 0;
	NewCounter = 0;
	EraseBackground();
	DrawPicture( dcMem, rect );
	Invalidate( FALSE );
}

void neurosim_view::EraseBackground()
{
	CRect rc; 
	GetClientRect( &rc );
	dcMem->DPtoLP( &rc );
	dcMem->PatBlt( rc.left, rc.top, rc.Width(), rc.Height(), WHITENESS );
}

void neurosim_view::changeViewSize( void )
{
	if( !InitUpdate )
		return;
	WINDOWPLACEMENT wp;
	wp.length = sizeof wp;
	if( !GetParent()->GetWindowPlacement( &wp ))
		return;
	if( View ){
		LastCounter = View->get_1step();
		View->ShowCmd = wp.showCmd;
		View->WindowRect = wp.rcNormalPosition;
	}
}

void neurosim_view::SaveImage( const char *filename, CSize size )
{
	CRect rect(0,0,0,0);
	rect.BottomRight() = size;
	CClientDC cDC( this );
	CDC dc;
	dc.CreateCompatibleDC( &cDC );
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap( &cDC, rect.Width(), rect.Height() ); 
	CBitmap *oldbitmap = dc.SelectObject( &bitmap );
	dc.PatBlt( rect.left, rect.top, rect.Width(), rect.Height(), WHITENESS );
	PrepareDC( &dc, rect );
	LastCounter = 0;
	NewCounter = GetMaxCounter();
	DrawPicture( &dc, rect );
	dc.SelectObject( oldbitmap );
	BITMAPINFOHEADER bih;
	memset(&bih, 0, sizeof BITMAPINFOHEADER );
	bih.biSize = sizeof BITMAPINFOHEADER;
	int ret = GetDIBits(
		dc.GetSafeHdc(),	// handle to device context
		(HBITMAP)bitmap,	// handle to bitmap
		0,					// first scan line to set in destination bitmap
		0,					// number of scan lines to copy
		NULL,				// address of array for bitmap bits
		(BITMAPINFO *)&bih,	// address of structure with bitmap data
		DIB_RGB_COLORS		// RGB or palette index
	);
	int numColorMask = 0;
	DWORD * colorsMask = NULL;
	if( ret != 0 ){
		if( bih.biBitCount == 32 ){
			bih.biBitCount = 24;
			bih.biCompression = BI_RGB;
		}
		if( bih.biCompression == BI_BITFIELDS ){
			numColorMask = 3;
			// Prepare color mask
			colorsMask = new DWORD[numColorMask];
			if( bih.biBitCount == 16 ){
				colorsMask[0] = 0xF800;
				colorsMask[1] = 0x07E0;
				colorsMask[2] = 0x001F;
			}
			else if(bih.biBitCount == 32 ){
				colorsMask[0] = 0xFF0000;
				colorsMask[1] = 0x00FF00;
				colorsMask[2] = 0x0000FF;
			}
			else{
				colorsMask[0] = 0x00;
				colorsMask[1] = 0x00;
				colorsMask[2] = 0x00;
			}
		}
		DWORD dwHeaderSize = bih.biSize + bih.biClrUsed*sizeof(RGBQUAD) + sizeof BITMAPFILEHEADER;
		if(numColorMask > 0){
			dwHeaderSize += numColorMask*sizeof DWORD;
		}
		DWORD dwFileSize = dwHeaderSize + bih.biSizeImage;
		HANDLE hFile = CreateFile(
			filename,
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL
		);
		if( hFile != INVALID_HANDLE_VALUE ){
			HANDLE hfilemap = CreateFileMapping(
				hFile,
				NULL,
				PAGE_READWRITE,
				0,
				dwFileSize,
				NULL
			);
			if( hfilemap != NULL ){
				PBYTE pbFile = (PBYTE) MapViewOfFile(
					hfilemap,
					FILE_MAP_WRITE,
					0,
					0,
					0
				);
				if( pbFile != NULL ){

					BITMAPFILEHEADER *bfh = (BITMAPFILEHEADER *)pbFile;
					PBYTE pbPos = pbFile + sizeof BITMAPFILEHEADER;
					BITMAPINFO *bi = (BITMAPINFO *)pbPos;
					memcpy(bi, &bih, sizeof BITMAPINFOHEADER);
					pbPos += sizeof BITMAPINFOHEADER;

					if(numColorMask > 0){
						memcpy(pbPos, colorsMask, numColorMask*sizeof DWORD);
						pbPos += numColorMask*sizeof DWORD;
					}
					pbPos = pbFile + dwHeaderSize;
					ret = GetDIBits(
					  dc.GetSafeHdc(),           // handle to device context
					  (HBITMAP)bitmap,      // handle to bitmap
					  0,   // first scan line to set in destination bitmap
					  bi->bmiHeader.biHeight,   // number of scan lines to copy
					  pbPos,    // address of array for bitmap bits
					  bi, // address of structure with bitmap data
					  DIB_RGB_COLORS        // RGB or palette index
					);
					// bi->bmiHeader.biSizeImage can change, therefore some correction
					dwFileSize = dwHeaderSize + bi->bmiHeader.biSizeImage;
					// Fill BITMAPFILEHEADER structure
					bfh->bfType = ((WORD) ('M' << 8) | 'B'); // "BM"
					bfh->bfSize = dwFileSize;
					bfh->bfOffBits = dwHeaderSize;
					bfh->bfReserved1 = 0;
					bfh->bfReserved2 = 0;
					UnmapViewOfFile( pbFile );
				}
				CloseHandle( hfilemap );
			}
			SetFilePointer( hFile, dwFileSize, NULL, FILE_BEGIN);
			SetEndOfFile( hFile );
			CloseHandle( hFile );
		}
	}
	if( colorsMask )
		delete[] colorsMask;
	bitmap.DeleteObject();
	dc.DeleteDC();
}

/////////////////////////////////////////////////////////////////////////////
// neurosim_view message handlers
int neurosim_view::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if( CView::OnCreate( lpCreateStruct ) == -1 )
		return -1;
	CClientDC cDC( this );
	dcMem = new CDC();
	dcMem->CreateCompatibleDC( &cDC );
	return 0;
}

void neurosim_view::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize( nType, cx, cy );
	IsUpdated = false;
	CClientDC cDC( this );
	CRect rc; GetClientRect( &rc );
	if( Bitmap ){
		Bitmap->DeleteObject();
		delete Bitmap;
	}
	Bitmap = new CBitmap();	
	Bitmap->CreateCompatibleBitmap( &cDC, rc.Width(), rc.Height() ); 
	dcMem->SelectObject( Bitmap );
	EraseBackground();
	changeViewSize();
	PrepareDC( dcMem, rc );
	DrawPicture( dcMem, rc );
}

void neurosim_view::OnDestroy() 
{
	neurosim_doc *doc = GetDocument(); ASSERT( doc );
	if( View )
		doc->Model->del_view( View->get_name());
	View = NULL;
	CView::OnDestroy();
}

void neurosim_view::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CView::OnLButtonDblClk( nFlags, point );
	neurosim_doc *pDoc = GetDocument(); if( !pDoc )	return;
	if( pDoc->SimData->IsSimComplete )
		((CMainFrame* )AfxGetMainWnd())->ViewProperies();
}

/////////////////////////////////////////////////////////////////////////////
// CChartView diagnostics
#ifdef _DEBUG
void CChartView::AssertValid() const
{
	neurosim_view::AssertValid();
}

void CChartView::Dump(CDumpContext& dc) const
{
	neurosim_view::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChartView
IMPLEMENT_DYNCREATE( CChartView, neurosim_view )

CChartView::CChartView()
{
	gr = new Grafor();	// make Gragor object in order to be able to use its functions
}

CChartView::~CChartView()
{
	if( gr )
		delete gr;
}

/////////////////////////////////////////////////////////////////////////////
// CChartView drawing
void CChartView::SetView( void )
{
	hhn_pair<int> layout = GetViewLayout();
	RegionRect.CreateMatrix( layout.Y, layout.X );
}

void CChartView::DrawPicture( CDC* pDC, CRect &rect )
{
static LONG Lock = 0;

	neurosim_doc *pDoc = GetDocument(); if( !pDoc )	return;
	CChartFrameView *view = (CChartFrameView *)View;
	if( !view )
		return;
	char buff[256];
	string paramName;

	sprintf( buff, "-1234567 T_in" );
	CSize siz = pDC->GetTextExtent( buff,strlen( buff ));
	SetRegionRect( rect, siz.cy ); // calc regions to draw graphics

	gr->Init_Page( pDC , &rect, ( pDC->IsPrinting() == TRUE ), false );
	hhn_pair<double> limits_view = view->get_xrange();
	hhn_pair<size_t> limits_counter( LastCounter, NewCounter );

	float xmin = float( limits_view.X );
	float xmax = float( limits_view.Y );
	float ymin = -80.;
	float ymax = 40.;

	float rx = gr->Osi_Gr( xmin, xmax, 2, 2);	// calc big oxes step
	float ry = gr->Osi_Gr( ymin, ymax, 2, 2);

	unsigned long bin = 0;
	double norm = 0.;
	if( pDoc->Model->GetChartBuffer().step() > 0. )
		bin = unsigned long( pDoc->SimData->HistBin/pDoc->Model->GetChartBuffer().step());
	if( pDoc->SimData->HistBin > 0. )
		norm = pDoc->SimData->HistNorm/pDoc->SimData->HistBin;
	if( view->validate( limits_counter )){
		for( unsigned int i = 0, i1 = 0; i < RegionRect.GetNumRow(); i++ ){
			for( unsigned int j=0; j < RegionRect.GetNumCol(); j++, i1++ ){
				gr->Region_Gr( pDC,(LPRECT) RegionRect(i,j));
				if( i1 < view->npars()){
					const void *view_data = view->get_data( i1 );
					if( view_data ){
						if( view->get_par( i1 )->ParCode.is_stat() ){
							::lock_data();
							vector<lvector> *data = (vector<lvector> *)view_data;
							switch( view->get_par( i1 )->ParCode.Param ){
								case CViewParam::Plot:
									ymin = 0.;
									ymax = ( float )data->size();
									ry = gr->Osi_Gr( ymin, ymax, 2, 2 );  // calc big oxes step
									gr->Limit_Gr( &xmin, &xmax, &ymin, &ymax );
									sprintf( buff, "", i+1, j+1 );
									gr->Draw_plot( pDC, pDoc->Model->GetTimeScale(), *data, limits_counter.Y, limits_counter.X );
								break;
								case CViewParam::Hist:
									ymin = ( float )view->ymin( i1 );
									ymax = ( float )view->ymax( i1 );
									ry = gr->Osi_Gr( ymin, ymax, 2, 2 );  // calc big oxes step
									gr->Limit_Gr( &xmin, &xmax, &ymin, &ymax );
									sprintf( buff, "", i+1, j+1 );
									if( bin )
										gr->Draw_hist( pDC, pDoc->Model->GetTimeScale(), *data, limits_counter.Y, limits_counter.X, bin, float( norm ));
									break;
							}
							::unlock_data();
						}
						else{
							const vector<float> *data = (const vector<float> *)view_data;
							ymin = (float)view->ymin(i1);
							ymax = (float)view->ymax(i1);
							ry=gr->Osi_Gr(ymin,ymax,2,2);	// calc big oxes step
							gr->Limit_Gr(&xmin,&xmax,&ymin,&ymax);
							sprintf(buff,"", i+1, j+1);
							gr->Draw_array(pDC, pDoc->Model->GetTimeScale(), *data, limits_counter.Y, limits_counter.X );
						}
						CViewParam *par = view->get_par(i1);
						if( par ){
							par->ParCode.get_fullname( pDoc->Model, paramName );
						}
						gr->Axes_Gr( pDC, "" ,rx , 4, ( char *)paramName.c_str(), ry, 1, 00, -1, 0, 11 );
					}
				}
			}
		}
	}
	LastCounter = NewCounter;
}

void CChartView::SetRegionRect( const CRect &rect, int charsize )
{
	int	x0 = (int) rect.Width()/20;//6	// x coordinate of the 1-st column
	int	y0 = (int) rect.Height()/20;//9	// y coordinate of the 1-st row
	int	xs = charsize*4;            // distance between columns
	int	ys = charsize*2;	// distance between rows
	int	xf = (int) (rect.Width())/20;//9	// distance between final column and window edge
	// calc the RegionRect hight
	int hight = (int) floor(.5 + (rect.Height()-2*y0-(RegionRect.GetNumRow()-1)*(double)ys)/(double)RegionRect.GetNumRow());
	// calc the RegionRect width
	int width = (int) floor(.5 + (rect.Width()-xf-x0-(RegionRect.GetNumCol()-1)*(double)xs)/(double)RegionRect.GetNumCol());
	// set region rect
	for( unsigned int i = 0;i<RegionRect.GetNumRow();i++)
		for( unsigned int j = 0;j<RegionRect.GetNumCol();j++){
			int x1 = rect.left + x0 + j*(width+xs);
			int y1 = rect.top  + y0 + i*(hight+ys);
			RegionRect( i, j ).SetRect(x1, y1, x1+width, y1+hight);
		}
}

hhn_pair<int> CChartView::GetViewLayout( void )
{
	hhn_pair<int> layout;
	neurosim_doc *pDoc = GetDocument(); if( !pDoc )	return layout;
	if( View ){
		CChartFrameView *view = ( CChartFrameView * )View;
		if( view ){
			layout.X = view->ncol();
			layout.Y = view->nrow();
		}
	}
	return layout;
}

unsigned long CChartView::GetMaxCounter( void )
{
	neurosim_doc *pDoc = GetDocument(); if( !pDoc )	return 0;
	return pDoc->Model->GetChartBuffer().nsteps();
}

/////////////////////////////////////////////////////////////////////////////
// CChartView message handlers
void CChartView::OnInitialUpdate() 
{
	neurosim_view::OnInitialUpdate();
	SetView();
}

#ifdef __MECHANICS__
/////////////////////////////////////////////////////////////////////////////
// CLimbView diagnostics
#ifdef _DEBUG
void CLimbView::AssertValid() const
{
	neurosim_view::AssertValid();
}

void CLimbView::Dump(CDumpContext& dc) const
{
	neurosim_view::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CLimbView
IMPLEMENT_DYNCREATE(CLimbView, neurosim_view)

CLimbView::CLimbView()
{
	IsSliceEmpty = true;
	LeftLegColor = RGB( 0x00, 0x00, 0xFF );
	RightLegColor = RGB( 0xAA, 0xAA, 0xAA );
	WalkerColorL = RGB( 0xFF, 0x00, 0x00 );
	WalkerColorR = RGB( 0x90, 0x00, 0x00 );
	EraseColor = RGB( 0xFF, 0xFF, 0xFF );
	GroundColor = RGB( 0x21, 0x6E,0x21);
}

CLimbView::~CLimbView()
{
}

/////////////////////////////////////////////////////////////////////////////
// CLimbView drawing
void CLimbView::InitDraw( void )
{
	neurosim_view::InitDraw();
	IsSliceEmpty = true;
	LastSlice = CWalkerVertex();
}

void CLimbView::DrawPicture( CDC* pDC, CRect &rect )
{
 	neurosim_doc *pDoc = GetDocument(); if( !pDoc || !pDoc->Model->Walker )	return;
	CWalkerFrameView *view = ( CWalkerFrameView *)View;
	if( !view )	return;
	pDC->DPtoLP( rect );
	hhn_pair<size_t> limits_counter( LastCounter, NewCounter );
	if( view->validate( limits_counter )){
		size_t increment = pDoc->SimData->LimbSkip;
		size_t i = ( limits_counter.X/increment )*increment;
		if( i < limits_counter.X )
			i += increment;
		for( ; i < limits_counter.Y; i += increment ){
			DrawWalker( pDC, pDoc->Model->GetWalkerBuffer().get_buffer(),
					   pDoc->SimData->IsStickDiagram, i );
		}
	}
	DrawGround( pDC, rect );
	LastCounter = NewCounter;
}

void CLimbView::DrawGround( CDC* pDC, CRect &rect )
{
// transformation for MM_HIMETRIC (0.01 mm)
	neurosim_doc *pDoc = GetDocument();	if( !pDoc || !pDoc->Model->Walker )	return;
	CScale sc; 
	sc.MaxLog = 100.;
	sc.MaxPhis = float( pDoc->SimData->LimbScale );
	hhn_pair<int> LpX = hhn_pair<int>( rect.left, rect.right );
	hhn_pair<float> PhX = sc.LPtoPH( LpX );
	hhn_pair<float> PhY = hhn_pair<float>( float( pDoc->Model->Walker->ground( PhX.X )), float( pDoc->Model->Walker->ground( PhX.Y )));
	hhn_pair<int> LpY = sc.PHtoLP( PhY );
	CPen pen(PS_SOLID, 20, GroundColor );
	CPen *oldpen = pDC->SelectObject( &pen );
	pDC->MoveTo( LpX.X, LpY.X );
	pDC->LineTo( LpX.Y, LpY.Y ); 
	pDC->SelectObject( oldpen );
}

void CLimbView::DrawWalker( CDC* pDC, const void *buf, bool isstick, unsigned long counter )
{
	vector<CWalkerVertex> *data = (vector<CWalkerVertex> *)buf;
	if( !IsSliceEmpty ){
		if( isstick ){
			DrawLimb( pDC, &LastSlice.Right, RightLegColor );
			DrawLimb( pDC, &LastSlice.Left, LeftLegColor );
		}
		else{
			DrawLimb( pDC, &LastSlice.Right, EraseColor );
			DrawLimb( pDC, &LastSlice.Left, EraseColor );
		}
	}
	DrawLimb( pDC, &(*data)[counter].Left, WalkerColorL );
	DrawLimb( pDC, &(*data)[counter].Right, WalkerColorR );
	LastSlice = (*data)[counter];
	IsSliceEmpty = false;
}

void CLimbView::DrawLimb( CDC* pDC, const void *limb, COLORREF color )
{
	neurosim_doc *pDoc = GetDocument();	if( !pDoc || !pDoc->Model->Walker )	return;
	vector< hhn_pair<float> > *data = (vector< hhn_pair<float> > *)limb;
	if(( *data ).size() < 1 )
		return;
	CScale sc;
	sc.MaxLog = 100.;
	sc.MaxPhis = float( pDoc->SimData->LimbScale );
	CPen pen(PS_SOLID, 10, color );
	CPen *oldpen = pDC->SelectObject( &pen );
	hhn_pair<int> point = sc.PHtoLP( (*data)[0] );
	pDC->MoveTo( point.X, point.Y );
	for( unsigned int i = 1; i < (*data).size(); i++){
		point = sc.PHtoLP( (*data)[i] );
		pDC->LineTo( point.X, point.Y );
	}
	pDC->SelectObject( oldpen );
}

unsigned long CLimbView::GetMaxCounter( void )
{
	neurosim_doc *pDoc = GetDocument();	if( !pDoc || !pDoc->Model->Walker )	return 0;
	return pDoc->Model->GetWalkerBuffer().nsteps();
}

void CLimbView::PrepareDC( CDC* pDC, CRect &rect )
{
	neurosim_view::PrepareDC( pDC, rect);
	neurosim_doc *pDoc = GetDocument();	if( !pDoc )	return;
	if( rect.Height() == 0 )
		return;
	pDC->SetMapMode( MM_HIMETRIC );
	CRect lp_rect = rect;
	pDC->DPtoLP(lp_rect);
	pDC->SetWindowOrg( -int( lp_rect.Width()*pDoc->SimData->LimbOriginX ), -int( lp_rect.Height()*(1-pDoc->SimData->LimbOriginY )));
}

#endif // __MECHANICS__
#endif // __CONSOLE__
