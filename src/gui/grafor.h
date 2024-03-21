#ifndef GRAFOR_H_
#define GRAFOR_H_

#include "config.h"

#ifndef __CONSOLE__

#include "../core/sptypes.h"
#include "../nsmsys.h"

class Grafor
{
public:
	LOGFONT		lfont;
	long	    Font_siz;

	COLORREF 	Line_Color,Page_col,Region_col,BkColor,TextColor,
	        	Paint_Column_Color,Axe_Color,Color_Sine,Color_Parabola;
	bool    	Draw_Ver_Line,Paint_Column_line;

	TEXTMETRIC	TM;
	int		    PenWidth,Vert_siz_Char;
	float	    font_ratio;

	RECT	    Region_Rec,Page_Rec;

	Grafor( COLORREF color = RGB( 0, 0, 0), UINT Width = 1 );
   ~Grafor( void );

	void	Gr_Default();
	void	Symbol(CDC* pDC,int x,int y,char *buf);
	void	Init_Page(CDC* pDC,RECT *pRec,bool is_Print, bool is_paint=false);
	void	Region_Gr(CDC* pDC,RECT *Rec,bool rec=FALSE,bool paint=FALSE);
	bool	Draw_rec(CDC* pDC,RECT *Reg_Rec);
	void	Limit_Gr(float *xmin,float *xmax,float *ymin,float *ymax);
	void	Math_to_Phy_Gr(double xm,double ym,int *xp,int *yp);
	void	Math_to_Phy_Gr_x(double xm,int *xp);
	void	Math_to_Phy_Gr_y(double ym,int *yp);
	void	Phy_to_Math_Gr(float *xm,float *ym,int xp,int yp);
	void	Phy_to_Math_Gr_y(float *ym,int yp);
	void	Phy_to_Math_Gr_x(float *xm,int xp);

	void	Draw_array(CDC* pDC, const nsm_vector(float) &x, const nsm_vector(float) &y, long n, long old_n );
	void    Draw_hist(CDC* pDC, const nsm_vector(float) &x, nsm_vector(lvector) &y, long n, long old_n, long bin, float norm = 1.0 );
	void	Draw_plot(CDC* pDC, const nsm_vector(float) &x, nsm_vector(lvector) &y, long n, long old_n );

	float	Osi_Gr(float left,float right,int int1,int int2);
	void	Axes_Gr(CDC* pDC,	// device context
		    char *NameX,	// X title
		    float XB,	// large scale
		    int xl,		// s scale
		    char *NameY,float YB,int yl,
		    int ixy,	// XY Grid (00 - no greed, 11 grid x and y
		    int werex,	// where to draw x axe (-1 left, 0 throu 0, 1 - right
		    int werey,
		    int show_xy		// show exes (x and y coordinat
		    );
	void	Axes_Gr_0(CDC* pDC,float XB0,int xl,double xm0,double ym0,
		    float left,float right,int num_ax,int draw_fream,
		    int draw_zero,int draw_num);
    void	Axes_Gr1(CDC* pDC,char *NameX,float XB,int xl,char *NameY,
		    float YB,int yl,int ixy,int werex,int werey,
		    int show_xy,int draw_zero_xy,int draw_num_xy);

	void	Clip_x(int *y);
	void	Clip_y(int *y);
	float	Xmin_Gr,Ymin_Gr,Xmax_Gr,Ymax_Gr;

private:
	int		Siz_big_desh,Siz_sml_desh,Where_X,Where_Y;

	int		Region_Rec_bt,Region_Rec_rl;
	float	Xmx_mi_Xmn,Ymx_mi_Ymn,R_rl_ov_Xmx_Xmn,R_bt_ov_Ymx_Ymn;
//	char	buff[256];
	
	int		Num_XB(float left,float right,double xb);
	void	Test_minmax(float *min,float *max);

	bool	Is_Print;
private:
    int old_a1;
    int old_b1;
    int old_a2;
    int old_b2;
    CPen pen_cur;
};

#endif // __CONSOLE__

#endif // GRAFOR_H_
