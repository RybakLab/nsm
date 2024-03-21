/////////////////////////////////////////////////////////////////////////////
// frameview.h
#ifndef __FRAME_VIEW_H
#define __FRAME_VIEW_H

#include "dataview.h"

class CSimulate;

/////////////////////////////////////////////////////////////////////////////
// CViewParam class
class CViewParam{
	public:
		enum _view_parameters_{
			None				= -1, 
			Out				=  0,
			Plot				=  1,
			Hist				=  2,
			Potent				=  3,
			CaConc				=  4,
			Condc				=  5,
			Curr				=  6,
			Activation			=  7,
			Inactivation			=  8,
			NaConc				=  9,
		};
	public:
		CViewParam( DWORD code = 0 ) : ParCode( code ), Ymax( 10 ), Ymin( -80 ){};
		CViewParam( const unit_code &parcode ) : ParCode( parcode ), Ymax( 10 ), Ymin( -80 ){};
		CViewParam( const CViewParam &param ) : ParCode( param.ParCode ), Ymax( param.Ymax ), Ymin( param.Ymin ){};
		~CViewParam(){};
	public:
		CViewParam &operator = ( const CViewParam &param )
		{
			ParCode = param.ParCode; Ymax = param.Ymax; Ymin = param.Ymin; return *this;
		};
	public:
		bool is_plot( void )
		{
			return( ParCode.UnitId == _id_NNPopulat && ParCode.Param == CViewParam::Plot );
		};
		bool load( istream &file, CSimulate *manager );
		void save( ostream &file, CSimulate *manager );
	public:
		unit_code ParCode;
		double Ymax;
		double Ymin;
};

/////////////////////////////////////////////////////////////////////////////
// CFrameView class
class CFrameView{
	public:
		CFrameView( void );
		CFrameView( const CFrameView &view );
virtual		~CFrameView( void ){};
	public:
		CFrameView &operator = ( const CFrameView &view);
	public:
		const char *get_name( void ) const{ return ViewName.c_str(); };
		void set_name( const char *name ){ ViewName = name; };
		hhn_pair<double> get_xrange( void ){ return LimitView; };
		size_t get_1step( void ){ return LimitStep.X; };
		size_t nsteps( void ) const{ return ( BufferManager )? BufferManager->nsteps(): 0; };
		bool is_resize( void ) const{ return !( WindowRect.left == -1 && WindowRect.top == -1 && WindowRect.right == -1 && WindowRect.bottom == -1 ); };
		bool validate( hhn_pair<size_t> &limits );
		void set_xrange( double begin, double end );
	public:
virtual		const char *prefix( void ) const { return ""; };
virtual		CViewParam *get_par( size_t inx ){ return NULL; };
virtual		size_t npars( void ) const { return 0; };
virtual		void add_view( const CViewParam &param ){};
virtual		void del_view( unit_code &code ){};
virtual		void init_view( CSimulate *manager ){};
virtual		void alloc_view( void ){};
virtual		void copy_to( CFrameView **view ) const{};
virtual		bool load( istream &file, CSimulate *manager );
virtual		void save( ostream &file, CSimulate *manager );
#ifndef __CONSOLE__
virtual		bool create_view( CDocument* pDoc, CMDIChildWnd *pChild ){ return false; };
	protected:
		bool create_view( CDocTemplate *tpl, CDocument* doc, CMDIChildWnd *child );
#endif //__CONSOLE__
	protected:
		void init_buf( CBufferManager *man ){	BufferManager = man; set_range(); };
		bool load_pos( istream &file );
		void save_pos( ostream &file );
		void set_range( void );
	public:
		UINT  ShowCmd;
		__rect WindowRect;
	protected:
		string ViewName;
		CBufferManager *BufferManager;
		hhn_pair<double> LimitView;		// x - begin_view, y - end_view /*msec*/
		hhn_pair<size_t> LimitStep;	// x - begin_step, y - end_step
};

/////////////////////////////////////////////////////////////////////////////
// CChartFrameView class
class CChartFrameView : public CFrameView{
	public:
		CChartFrameView( void ) : CFrameView(), NumRow( 0 ), NumCol( 0 ){};
		CChartFrameView( const CChartFrameView &view ) : CFrameView( view ), NumRow( view.NumRow ), NumCol( view.NumCol ), NNParam( view.NNParam ){};
		~CChartFrameView( void ){};
	public:
		CChartFrameView &operator = ( CChartFrameView &view);
	public:
		const void *get_data( size_t inx ) const
		{ 
			return ( BufferManager && inx < NNParam.size())? (( CChartBuffer *)BufferManager )->get_buffer( NNParam[inx].ParCode ): NULL; 
		};
		size_t nrow( void ) const{ return NumRow; };
		size_t ncol( void ) const{ return NumCol; };
		void set_geometry( size_t row, size_t col ){ NumRow = row; NumCol = col; };
		double &ymin( size_t index ){ return NNParam[index].Ymin; };
		double &ymax( size_t index ){ return NNParam[index].Ymax; };
		void swap( size_t inx1, size_t inx2 );
	public:	// overrided
		const char *prefix( void ) const{ return "Chart"; };
		CViewParam *get_par( size_t inx )
		{ 
			return ( inx < NNParam.size())? &NNParam[inx]: NULL; 
		};
		size_t npars( void ) const
		{ 
			return NNParam.size(); 
		};
		void add_view( const CViewParam &param ) final;
		void del_view( unit_code &code );
		void init_view( CSimulate *manager );
		void alloc_view( void );
		void copy_to( CFrameView **view ) const;
		bool load( istream &file, CSimulate *manager );
		void save( ostream &file, CSimulate *manager );
#ifndef __CONSOLE__
		bool create_view( CDocument* pDoc, CMDIChildWnd *pChild );
#endif // __CONSOLE__
	private:
		void clear( void );
		void remove( size_t inx );
	private:
		size_t NumRow;
		size_t NumCol;
		vector<CViewParam> NNParam;
};

#if defined (__MECHANICS_2D__)
/////////////////////////////////////////////////////////////////////////////
// CWalkerFrameView class
class CWalkerFrameView : public CFrameView{
	public:
		CWalkerFrameView( void ) : CFrameView(){};
		CWalkerFrameView( const CWalkerFrameView &view ) : CFrameView( view ){};
		~CWalkerFrameView( void ){};
	public:	// overrided
		const char *prefix( void ) const { return "Limb"; };
		void init_view( CSimulate *manager );
		void alloc_view( void );
		void copy_to( CFrameView **view ) const;
		void save( ostream &file, CSimulate *manager );
#ifndef __CONSOLE__
		bool create_view( CDocument* pDoc, CMDIChildWnd *pChild );
#endif // __CONSOLE__
};
#elif defined (__MECHANICS_3D__)
// TODO implementation for 3d model
#endif // __MECHANICS_2D__
#endif // __FRAME_VIEW_H
