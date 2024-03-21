#include "precompile.h"

#ifndef __CONSOLE__

#include "grafor.h"
#include "func_lib.h"

#define X_axe 1
#define Y_axe 2

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
////#define new DEBUG_NEW
#endif // _DEBUG

/*-------------------------------------------------------------------------------*/
Grafor::Grafor(COLORREF color_pen,UINT Width)
{
	Font_siz=lfont.lfHeight		= 12;
	lfont.lfWidth			= 0;
	lfont.lfEscapement		= 0;
	lfont.lfOrientation		= 0;
	lfont.lfWeight			= FW_NORMAL;
	lfont.lfItalic			= 0;
	lfont.lfUnderline		= 0;
	lfont.lfStrikeOut		= 0;
	lfont.lfCharSet			= ANSI_CHARSET;
	lfont.lfOutPrecision		= OUT_DEVICE_PRECIS;
	lfont.lfClipPrecision		= CLIP_DEFAULT_PRECIS;
	lfont.lfQuality			= DEFAULT_QUALITY;
	lfont.lfPitchAndFamily		= DEFAULT_PITCH | FF_SWISS;
	strcpy( lfont.lfFaceName, "Arial" );

	Line_Color=color_pen;
	PenWidth=Width;
	Is_Print = false;

	SetRect(&Page_Rec,1,1,50,50);
	Vert_siz_Char=0;
	font_ratio=(float)1;
	Gr_Default();
	SetRect( &Region_Rec, 0,0,1,1);
	pen_cur.CreatePen( PS_SOLID, PenWidth, Line_Color );
}
/*-------------------------------------------------------------------------------*/
Grafor::~Grafor() {} 
/*-------------------------------------------------------------------------------*/
void Grafor::Gr_Default()
{
	TextColor=RGB(0,0,0);
	BkColor=RGB(255,255,255);
	Page_col=RGB(255,255,255);
	Region_col=RGB(255,255,255);
	Paint_Column_Color=RGB(0,255,255);
	Axe_Color=RGB(100,0,0);
	Line_Color=RGB(0,0,255);
	
	Color_Parabola=Color_Sine=RGB(255,0,0);
	Draw_Ver_Line=TRUE;
	Paint_Column_line=FALSE;
}
/*-------------------------------------------------------------------------------*/
void Grafor::Init_Page(CDC* pDC,RECT *pRec,bool is_Print, bool is_paint)
{
	Is_Print = is_Print;
	SetRect(&Page_Rec,1,1,pRec->right,pRec->bottom);

	pDC->SetBkColor(BkColor);
	pDC->SetTextColor(TextColor);

	pDC->GetTextMetrics(&TM);
	Vert_siz_Char=TM.tmHeight;
	
	if( !Is_Print ){
		font_ratio=(float)Vert_siz_Char/(float)Font_siz;
	}
	if( font_ratio==0.f ){
		font_ratio=1.f;
	}

	int font_sz=(!Is_Print) ? Font_siz :
			(int)floor(.5+Vert_siz_Char/font_ratio);
	font_sz = abs(font_sz);

	Siz_big_desh = abs(font_sz)/2; if(Siz_big_desh<2) Siz_big_desh=2;
	Siz_sml_desh = Siz_big_desh/2;  

	if( is_paint ){
		if(!Paint_rect(pDC,&Page_Rec,Page_col)) 
			return;
	}
}
/*-------------------------------------------------------------------------------*/
void Grafor::Symbol(CDC* pDC,int x,int y,char *buf)
{
	CFont draw_Font;
	pDC->SetBkColor( BkColor );
	pDC->SetTextColor( TextColor );

	int font_sz = ( !Is_Print ) ?Font_siz :
			(int)floor(.5+Vert_siz_Char/font_ratio)+1;
	
	lfont.lfHeight = font_sz+1;

	draw_Font.CreateFontIndirect(&lfont );
	CFont* pOldFont = (CFont*) pDC->SelectObject(&draw_Font);
	pDC->TextOut(x,y,buf);
	pDC->SelectObject(pOldFont);
}
/*-------------------------------------------------------------------------------*/
void Grafor::Region_Gr(CDC* pDC,RECT *Rec,bool rec,bool paint)
{
	CopyRect(&Region_Rec,Rec);

	pDC->SetBkColor(BkColor);
	pDC->SetTextColor(TextColor);
	if(paint) Paint_rect(pDC,&Region_Rec,Region_col);
	if(rec) Draw_rec(pDC,&Region_Rec);
}
/*-------------------------------------------------------------------------------*/
bool Grafor::Draw_rec(CDC* pDC,RECT *c_rect)
{  
	CPen pen_cur;

	if (!pen_cur.CreatePen(PS_SOLID,PenWidth,Line_Color)) return FALSE;
	CPen* pOldPen = pDC->SelectObject(&pen_cur);

  	pDC->MoveTo(c_rect->left,c_rect->top);
  	pDC->LineTo(c_rect->right+1,c_rect->top);
  	pDC->LineTo(c_rect->right+1,c_rect->bottom+1);
  	pDC->LineTo(c_rect->left,c_rect->bottom+1);
  	pDC->LineTo(c_rect->left,c_rect->top);

	pDC->SelectObject(pOldPen);
	return TRUE;
}
/*-------------------------------------------------------------------------------*/
void Grafor::Limit_Gr(float *xmin,float *xmax,float *ymin,float *ymax)
{  
	Test_minmax(xmin,xmax);
	Test_minmax(ymin,ymax);
	Xmin_Gr=*xmin; Ymin_Gr=*ymin;
	Xmax_Gr=*xmax; Ymax_Gr=*ymax;
 
	Region_Rec_rl=Region_Rec.right -Region_Rec.left;
	Region_Rec_bt=Region_Rec.bottom-Region_Rec.top;
	Xmx_mi_Xmn=Xmax_Gr-Xmin_Gr;
	Ymx_mi_Ymn=Ymax_Gr-Ymin_Gr;
 
	R_rl_ov_Xmx_Xmn=Region_Rec_rl/Xmx_mi_Xmn;
	R_bt_ov_Ymx_Ymn=Region_Rec_bt/Ymx_mi_Ymn;
}
/*-------------------------------------------------------------------------------*/
void Grafor::Test_minmax(float *min,float *max)
{
 float	min1,max1,a;
	min1=*min; max1=*max;
	if(min1>=max1) {
		min1=(*max)*(float).9; max1=(*min)*(float)1.1;
		if(min1==0) max1=(float).1;}
	if(min1>=max1) {a=min1; min1=max1; max1=a;}
	*min=min1; *max=max1;
}
/*-------------------------------------------------------------------------------*/
void Grafor::Math_to_Phy_Gr(double xm,double ym,int *xp,int *yp)
{  
	Math_to_Phy_Gr_x(xm,xp);
	Math_to_Phy_Gr_y(ym,yp);
}
/*-------------------------------------------------------------------------------*/
void Grafor::Math_to_Phy_Gr_x(double xm,int *xp)
{  
 *xp=Region_Rec.left+(int)/*floor*/(.5+R_rl_ov_Xmx_Xmn*(xm-Xmin_Gr));
}
/*-------------------------------------------------------------------------------*/
void Grafor::Math_to_Phy_Gr_y(double ym,int *yp)
{  
 *yp=Region_Rec.bottom-(int)/*floor*/(.5+R_bt_ov_Ymx_Ymn*(ym-Ymin_Gr));
}
/*-------------------------------------------------------------------------------*/
void Grafor::Phy_to_Math_Gr(float *xm,float *ym,int xp,int yp)
{  
	Phy_to_Math_Gr_x(xm,xp);
	Phy_to_Math_Gr_y(ym,yp);
}
/*-------------------------------------------------------------------------------*/
void Grafor::Phy_to_Math_Gr_y(float *ym,int yp)
{  
 *ym=Ymin_Gr+(Region_Rec.bottom-yp)*Ymx_mi_Ymn/Region_Rec_bt;
}
/*-------------------------------------------------------------------------------*/
void Grafor::Phy_to_Math_Gr_x(float *xm,int xp)
{  
 *xm=Xmin_Gr+(xp-Region_Rec.left)*Xmx_mi_Xmn/Region_Rec_rl;
}
/*-------------------------------------------------------------------------------*/
void Grafor::Clip_x(int *y)
{
		if(*y>Region_Rec.right) *y=Region_Rec.right;
		if(*y<Region_Rec.left) *y=Region_Rec.left;
}
/*-------------------------------------------------------------------------------*/
void Grafor::Clip_y(int *y)
{
		if(*y>Region_Rec.bottom) *y=Region_Rec.bottom-1;
		if(*y<Region_Rec.top) *y=Region_Rec.top+1;
}
/*-------------------------------------------------------------------------------*/
void Grafor::Axes_Gr1(CDC* pDC,char *NameX,float XB,int xl,char *NameY,float YB,int yl,
             int ixy,int werex,int werey,int show_xy,int draw_zero_xy,int draw_num_xy)
{  
 int x0,y0;
 long i,j;
 double xm,ym;
 float x,y;
	Where_X=werex;	Where_Y=werey;
	if((Xmax_Gr*Xmin_Gr)<=0) xm=0;
	else { j=0;
		i=(long)(Xmin_Gr/XB-1);
		while(++j<10000 && (xm=i*XB)<Xmin_Gr-.1*XB/xl) i++; }
	if((Ymax_Gr*Ymin_Gr)<=0) ym=0;
	else { j=0;
		i=(long)(Ymin_Gr/YB)-1;
		while(++j<10000 && (ym=i*YB)<Ymin_Gr-.1*YB/yl) i++; }
	if(werex!=0) {
		x0=(werex==-1) ? Region_Rec.left : Region_Rec.right;
 		Phy_to_Math_Gr(&x,&y,x0,0); xm=x; }
	if(werey!=0) {
		y0=(werey==-1) ? Region_Rec.bottom : Region_Rec.top;
 		Phy_to_Math_Gr(&x,&y,0,y0); ym=y; }
	Math_to_Phy_Gr((float)xm,(float)ym,&x0,&y0);
	if(show_xy/10==1) {  //draw X axe
		Line_R1_R2(Region_Rec.left,y0,Region_Rec.right,y0,Axe_Color,pDC);
		Axes_Gr_0(pDC,XB,xl,xm,ym,Xmin_Gr,Xmax_Gr,X_axe,ixy/10,
			draw_zero_xy/10,draw_num_xy/10);
		i=strlen(NameX);
		if(i>0) {
			Symbol(pDC,Region_Rec.right-(strlen(NameX)+1)*TM.tmAveCharWidth,y0+5,NameX); }
	}
	if(show_xy%10==1) {
		Line_R1_R2(x0,Region_Rec.bottom,x0,Region_Rec.top,Axe_Color,pDC);
		Axes_Gr_0(pDC,YB,yl,xm,ym,Ymin_Gr,Ymax_Gr,Y_axe,ixy%10,
			draw_zero_xy%10,draw_num_xy%10);
		i=strlen(NameY);
		if(i>0) {
			Symbol(pDC,x0+TM.tmAveCharWidth*2,int(Region_Rec.top-TM.tmHeight*0.7),NameY);}
	}
}
/*-------------------------------------------------------------------------------*/
void Grafor::Axes_Gr_0(CDC* pDC,float XB0,int xl,double xm0,double ym0,
	float left,float right,int num_ax,int draw_fream,int draw_zero,int draw_num)
{  
 long i,j,k;
 int x0,y0,i5,x00,y00,x1,x2,y1,y2,xo;
 double xm,xb;
 int bul;
 char buff[256];
	Math_to_Phy_Gr((float)xm0,(float)ym0,&xo,&y0);
	xb=XB0/xl;	i=(long)(left/xb)-1; k=0;  
	while(++k<10000l && (xm=i*xb)<=right+.1*xb) { i++;
		if(xm>=left-.1*xb) {
 	 		if(num_ax==X_axe)
 	 			 Math_to_Phy_Gr((float)xm,(float)ym0,&x0,&y0);
 	 		else Math_to_Phy_Gr((float)xm0,(float) xm,&x0,&y0);

	 		j=(long)floor(xm/XB0+.5);
			bul = (fabs(j*XB0-xm)<.1*xb) ? 1 : 0;
		 	if(bul==1) i5=Siz_big_desh; else i5=Siz_sml_desh;
			if(num_ax==X_axe) { 
				x1=x2=x0; y1=y0-i5; y2=y0+i5;
				if(Where_Y!=0) {if(Where_Y==-1) y1=y0; else y2=y0;}
				x00=x0; y00=y0+15; ////?????? 
 				if(draw_fream==1 && bul==1) {
 					y1=Region_Rec.bottom; y2=Region_Rec.top; }}
	 		else {
	 			x1=x0-i5; x2=x0+i5; y1=y2=y0; x00=x0; y00=y0; 
				if(Where_X!=0) {if(Where_X==-1) x2=x0; else x1=x0;}
		 		if(draw_fream==1 && bul==1) {
		 			x1=Region_Rec.right; x2=Region_Rec.left; }}
			Line_R1_R2(x1,y1,x2,y2,Axe_Color,pDC);
	 		if(bul==1) {
	 			sprintf(buff,"%g",xm);
//				CSize	siz=pDC->GetOutputTextExtent(buff,strlen(buff));
				if(num_ax==X_axe) y00=y0+i5;
	 			if(num_ax==Y_axe) {
	 				if(abs(xo-Region_Rec.left)<5)
	 					x00=x00-TM.tmAveCharWidth*(strlen(buff)+1);
	 				else x00=x00+TM.tmAveCharWidth;}
	 			if(draw_num==1) if(draw_zero==1 || xm!=0)
	 				Symbol(pDC,x00,y00,buff);
	 		}
	}}	 
}	
/*-------------------------------------------------------------------------------*/
void Grafor::Axes_Gr(CDC* pDC,char *NameX,float XB,int xl,
	char *NameY,float YB,int yl,int ixy,int werex,int werey,
	int show_xy)
{
	Axes_Gr1(pDC,NameX,XB,xl,NameY,YB,yl,ixy,werex,werey,show_xy,
		11,11);
}
/*-------------------------------------------------------------------------------*/
float Grafor::Osi_Gr(float left,float right,int int1,int int2)
{
 int i,j;
 double xb;	
 
	if( left >= right ) return 1;

	xb=right-left;
	xb=pow(10.,floor(log10(xb)+.5));
	i=Num_XB(left,right,xb);
	if(i>=int1 && i<=int2) return((float)xb);
	j=0;
	while(i<int1 || i>int2) {
		j++; 
		if(j>100) break;
		if(i<int1) xb=xb/5;
		if(i>int2) xb=xb*2;
		i=Num_XB(left,right,xb);
	}	
	return((float)xb);
}		
/*-------------------------------------------------------------------------------*/
int Grafor::Num_XB(float left,float right,double xb)
{
 int j;
 long i;
 double xm;
	i=(long)(left/xb)-1;  
	j=0;
	while(j<1000 && ((xm=i*xb)<=right+.1*xb/10)) {
		i++; 
		if(xm>=left-.1*xb/10) j++;
	}
	return(j);
}		
/*-------------------------------------------------------------------------------*/
void Grafor::Draw_array( CDC* pDC,const nsm_vector(float) &x, const nsm_vector(float) &y, long n, long old_n )
{
 if( x.size() == 0 )
     return;
 if( x.size() <= (unsigned int)n )
     n = x.size()-1;
 if( n < 2 )
     return;
	
 CPen* pOldPen = pDC->SelectObject( &pen_cur );
 long i = old_n-1>0? old_n-1: 0;
 int a = 0;
 int b = 0;
 Math_to_Phy_Gr( x[i], ( float )y[i], &a, &b );
 Clip_y( &b );
 pDC->MoveTo( a, b );
 old_a1 = old_a2; old_a2 = a;
 old_b1 = old_b2; old_b2 = b;

 for( ++i; i < n; i++ ){
      Math_to_Phy_Gr( x[i], ( float )y[i], &a, &b );
      Clip_y( &b );
      if(!( a >= old_a1 && a <= old_a2 && b >= old_b1 && b <= old_b2 ))
          pDC->LineTo( a, b );
      old_a1 = old_a2; old_a2 = a;
      old_b1 = old_b2; old_b2 = b;
      }
 pDC->SelectObject( pOldPen );
}

void Grafor::Draw_hist( CDC* pDC, const nsm_vector(float) &x, nsm_vector(lvector) &y, long n, long old_n, long bin, float norm )
{
 static long BIN = 0;
 if( y.size() == 0 )
     return;
 if( x.size() == 0 )
     return;
 if( x.size() <= (unsigned int)n )
     n = x.size()-1;
 CPen* pOldPen = pDC->SelectObject( &pen_cur );
 float y_norm = float( 1./y.size());
 for( long n_bin = old_n/bin-1, step = 0; (n_bin+1)*bin <= n; n_bin++ )
      if( n_bin >= 0 ){
          long n_spikes = 0;
          for( unsigned int neuron = 0; neuron < y.size(); neuron++ ){
               lvector::iterator fpos = lower_bound( y[neuron].begin(), y[neuron].end(), ( unsigned int )n_bin*bin );
			   lvector::iterator lpos = lower_bound( fpos, y[neuron].end(), ( unsigned int )( n_bin+1 )*bin );
               if( fpos != y[neuron].end() ){
				   if( lpos == y[neuron].end() && lpos > y[neuron].begin() )
                       lpos--;
				   if( lpos >= fpos ){
					   n_spikes += lpos-fpos;
					   if( *lpos < (unsigned int)( n_bin+1 )*bin )
                           n_spikes += 1;
				       }
                   }
               }
          int a, b;
          Math_to_Phy_Gr( x[n_bin*bin], norm*n_spikes*y_norm, &a, &b );
          Clip_y( &b );
          Clip_x( &a );
		  if( step == 0 ) //first step
              pDC->MoveTo( a, b );
		  else
              pDC->LineTo( a, b );

		  BIN = bin;
		  Math_to_Phy_Gr( x[n_bin*bin+BIN], norm*n_spikes*y_norm, &a, &b ); // <=>  Math_to_Phy_Gr( x[(n_bin+1)*bin], norm*n_spikes*y_norm, &a, &b );

		  Clip_y( &b );
		  Clip_x( &a );
		  pDC->LineTo( a, b );
		  step++;
          }
 pDC->SelectObject( pOldPen );
}

void Grafor::Draw_plot(CDC* pDC,const nsm_vector(float) &x, nsm_vector(lvector) &y, long n, long old_n )
{
 if( x.size() == 0 )
     return;
 if( x.size() <= ( unsigned int )n )
     n = x.size()-1;
 if( n < 2 )
     return;
 for( unsigned int neuron = 0; neuron < y.size(); neuron++ ){
      old_a1 = 0; old_b1 = 0;
      for( lvector::iterator pos = upper_bound( y[neuron].begin(), y[neuron].end(),(  unsigned int )old_n ); pos != y[neuron].end() && *pos < (unsigned int)n; pos++ )
           if( *pos >= ( unsigned int )old_n ){
               int a, b;
               Math_to_Phy_Gr( x[*pos], ( float )neuron+1, &a, &b );
               Clip_y( &b );
               if( a != old_a1 || b != old_b1 ){
                   pDC->SetPixelV( a, b, Line_Color );
/*
                   pDC->SetPixelV( a+1, b, Line_Color );
                   pDC->SetPixelV( a, b+1, Line_Color );
                   pDC->SetPixelV( a+1, b+1, Line_Color );
*/
                   old_a1 = a; old_b1 = b;
                   }
               }
      }
}
#endif // __CONSOLE__
