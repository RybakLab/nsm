////////////////////////////////////////////////////////////////////////
// mustemplate.h
//---------------------------------------------------------------------
//	default parameters 
//---------------------------------------------------------------------
//	1-joint muscles
//---------------------------------------------------------------------
//	Name		IP			BFA			VA			TA			SOL									
//	attachment scheme
//	-----------------
//	Ulink		Pelvis		Pelvis		Thigh		Shank		Shank
//	Mlink		-			-			-			-			-
//	Llink		Thigh		Thigh		Shank		Foot		Foot
//	Ujoint		Hip			Hip			Knee		Ankle		Ankle
//	Ljoint		-			-			-			-			-
//  Mlink L		-			-			-			-			-
//	Amax		0.42		0.92		0.36		0.16		0.82
//  F1			1			-1			1			1			-1
//  F2			-			-			-			-			-
//  Phi1		0			2.46091		0			0			0
//  Phi2		0			0			2.53073		0			-3.03687
//  A1			33.2737		32.5723		67.4917		9.166		45.1184
//  A2			8.27596		70.601		5.81846		31.56		23.1256
//  R1			5			5			5.81846		5			5
//  R2			-			-			-			-			-
//	muscle properties
//	-----------------
//	LT0			18.1052		39.936		53.7105		50.1514		28.9356
//	PT0			48			47.4		248.6		63.9		20.8
//	Alpha0		0			0.244346	0.23911		0.122173	0.122173
//	LT0perL0	0.5			0.5			2			1			1
//	Ladd		0			0			0			39.051614	0
//	M			17.6		28			27.5		5.5			3.5
//	Shift		0			-0.1		0.05		0			0.05
//	tendom element
//	-----------------
//	KE			69.7661		69.7661		69.7661		69.7661		69.7661
//	KL			1.84042		1.84042		1.84042		1.84042		1.84042
//	passive element
//	-----------------
//	KE			6.85764		6.85764		6.85764		6.85764		6.85764
//	KL			0.0735096	0.0735096	0.0735096	0.0735096	0.0735096
//	contractile element
//	-----------------
//	AV			3.7			9.5			2.4			5.9			1.1
//	Beta		1.55		1.55		1.55		1.55		1.55
//	Fmax		1.8			1.8			1.8			1.8			1.8
//	Omega		0.824962	0.824962	0.824962	0.824962	0.824962   !!! different
//	RHO			10.3646		10.3646		10.3646		10.3646		10.3646    !!! different
//	Vmax		0.578		0.643		0.266		0.813		0.266
//---------------------------------------------------------------------
//	2-joints muscles
//---------------------------------------------------------------------
//	Name		RF			BFP			SART		GA
//	attachment scheme
//	-----------------
//	Ulink		Pelvis		Pelvis		Pelvis		Thigh
//	Mlink		Thigh		Thigh		Thigh		Shank
//	Llink		Shank		Shank		Shank		Foot
//	Ujoint		Hip			Hip			Hip			Knee
//	Ljoint		Knee		Knee		Knee		Ankle
//  Mlink L		97			97			97			102
//	Amax		0.59		0.05		0.47		0.23
//  F1			1			-1			1			-1
//  F2			1			-1			-1			-1
//  Phi1		0			2.46091		0			0
//  Phi2		2.53073		0			0			-3.03687
//  A1			5.34781		33.2049		30.2284		3.79446
//  A2			5.81846		23.8934		18.2273		23.1256
//  R1			5			5			5			3.79446
//  R2			5.81846		3.79446		3.79446		5
//	muscle properties
//	-----------------
//	LT0			78.3195		9.80284		23.1402		88.3487
//	PT0			122			253.3		20.1		193.7
//	Alpha0		0.122173	0.122173	0			0.331613
//	LT0perL0	4			0.1			0.1			4
//	Ladd		0			0			0			0
//	M			9.5			41.3		8.5			24.8
//	Shift		0			-0.2		-0.3		0.05
//	tendom element
//	-----------------
//	KE			69.7661		69.7661		69.7661		69.7661
//	KL			1.84042		1.84042		1.84042		1.84042
//	passive element
//	-----------------
//	KE			6.85764		6.85764		6.85764		6.85764
//	KL			0.0735096	0.0735096	0.0735096	0.0735096
//	contractile element
//	-----------------
//	AV			5			9.5			3.7			2.28
//	Beta		1.55		1.55		1.55		1.55
//	Fmax		1.8			1.8			1.8			1.8
//	Omega		0.824962	0.824962	0.824962	0.824962
//	RHO			10.3646		10.3646		10.3646		10.3646
//	Vmax		0.288		0.624		1.491		0.266 
//---------------------------------------------------------------------
#ifndef __MUSCLE_TEMPLATE_H
#define __MUSCLE_TEMPLATE_H

#include "sptypes.h"

#if defined (__MECHANICS_2D__)

#include "unitemplate.h"
#include "muscle.h"

enum __ID_Muscle{
	__id_generic_M	= 0,
	__id_IP,
	__id_BFA,
	__id_SART,
	__id_RF,
	__id_BFP,
	__id_VA,
	__id_GA,
	__id_TA,
	__id_SOL,
	__id_MAX_MUSCLES,
	};

extern const char *__MuscleNames[__id_MAX_MUSCLES];

class zmuscle;
////////////////////////////////////////////////////////////////////////
// class t_muscle
class t_muscle : public uni_template{
	public:
		t_muscle( uni_template *parent, const char *lscheme = "None", int is_active = GRID_COLLAPSE );
		t_muscle( const t_muscle &m );
virtual		~t_muscle( void ); 
	public:
		t_muscle &operator = ( const t_muscle &m );
	public:
		void upd_outlist( vector<string> &out_list );
	public:
virtual		void copy_to( uni_template **unit, uni_template *parent );
		void copy_to( zmuscle **m );
      protected:
virtual		int get_datattr( const char *path );
virtual		bool get_addparlist( vector<string> &parlist );
virtual		bool get_addpar( const char *path, string &name, string &element, int &type );
virtual		bool upd_addpar( const char *path, const char *name, string &element, bool save );
      //--- Load/save parameters 
virtual		bool load_addpar( string str, istream &file );
virtual		void save_addpar( ostream &file );
virtual		void load_dummy( void );
	private:
		void add_substr( string &str, const char *name );
		void front( string &str, const char *name );
		void del_substr( string &str, const char *name, string::size_type pos );
		void make_linkcombo( string &name, string &element );
	public:
		string MnNames;		// motoneurons
		string Mn;		// motoneurons
	protected:
		void set_default_data( void );
	protected:
		// attachment scheme
		string LScheme;
		string JScheme;
		string ULink;		// upper link
		string MLink;		// middle link (2-j)
		string LLink;		// lower link

		t_data Amax;		// maximal activation
		t_data F1;		// 1-j
		t_data F2;		// 2-j
		t_data Phi1;		// 1-j
		t_data Phi2;		// 1-j
		t_data A1;		// 1-j
		t_data A2;		// 1-j
		t_data R1;		// 1-j
		t_data R2;		// 2-j
		// muscle properties
		t_data Ltr;
		t_data LT0;
		t_data PT0;
		t_data Alpha;
		t_data LT0perL0;
		t_data M;
		t_data Shift;
		t_data Ladd;
		// tendom element
		t_data TKE;
		t_data TKL;
		// passive element
		t_data PKE;
		t_data PKL;
		// contractile element
		t_data AV;
		t_data Beta;
		t_data Fmax;
		t_data Omega;
		t_data RH0;
		t_data Vmax;
	private:
		bool IsSchDef;
#ifdef __NEW_MUSCLE__
	protected:
		t_data TdL1; // new
		t_data TF1;	 // new
		t_data TKlo; // new
		t_data TKsh; // new
		t_data CF0;		// new
		t_data CFtr;	// new
		t_data CVtr;	// new
#endif // __NEW_MUSCLE__
};

////////////////////////////////////////////////////////////////////////
// class t_ip
class t_ip : public t_muscle{
	public:
		t_ip( uni_template *parent, const char *lscheme = "None" );
		t_ip( const t_ip &m ) : t_muscle( m ){};
		~t_ip( void ){};
	public:
		void copy_to( uni_template **unit, uni_template *parent )
		{
			*unit = new t_ip( *this );
			( *unit )->set_parent( parent );
		};
	protected:
		int get_datattr( const char *path )
		{
			string p_path = path;
			if( p_path == "Joint scheme" || p_path == "Middle link" || 
				p_path == "Upper link" || p_path == "Lower link" || p_path == "Name" ){
				return GRID_READONLY;
			}
			return t_muscle::get_datattr( path );
		};
		bool get_addpar( const char *path, string &name, string &element, int &type )
		{
			if( path ){
				string p_path = path;
				if( p_path == "Joint scheme" ){
					name = "Data";
					element = JScheme;
					type = GRID_STRING;
					return true;
				}
				if( p_path == "Middle link" ){
					name = "Data";
					element = MLink;
					type = GRID_STRING;
					return true;
				}
				if( p_path == "Upper link" ){
					name = "Data";
					element = ULink;
					type = GRID_STRING;
					return true;
				}
				if( p_path == "Lower link" ){
					name = "Data";
					element = LLink;
					type = GRID_STRING;
					return true;
				}
			}
			return t_muscle::get_addpar( path, name, element, type );
		};
		bool upd_addpar( const char *path, const char *name, string &element, bool save )
		{
			if( path ){
				string p_path = path;
				if( p_path == "MN input" ){
					return t_muscle::upd_addpar( path, name, element, save );
				}
			}
			return false;
		};
		void load_dummy( void );
};

////////////////////////////////////////////////////////////////////////
// class t_bfa
class t_bfa : public t_muscle{
	public:
		t_bfa( uni_template *parent, const char *lscheme = "None" );
		t_bfa( const t_bfa &m ) : t_muscle( m ){};
		~t_bfa( void ){};
	public:
		void copy_to( uni_template **unit, uni_template *parent )
		{
			*unit = new t_bfa( *this );
			( *unit )->set_parent( parent );
		};
	protected:
		int get_datattr( const char *path )
		{
			string p_path = path;
			if( p_path == "Joint scheme" || p_path == "Middle link" || 
				p_path == "Upper link" || p_path == "Lower link" || p_path == "Name" ){
				return GRID_READONLY;
			}
			return t_muscle::get_datattr( path );
		};
		bool get_addpar( const char *path, string &name, string &element, int &type )
		{
			if( path ){
				string p_path = path;
				if( p_path == "Joint scheme" ){
					name = "Data";
					element = JScheme;
					type = GRID_STRING;
					return true;
				}
				if( p_path == "Middle link" ){
					name = "Data";
					element = MLink;
					type = GRID_STRING;
					return true;
				}
				if( p_path == "Upper link" ){
					name = "Data";
					element = ULink;
					type = GRID_STRING;
					return true;
				}
				if( p_path == "Lower link" ){
					name = "Data";
					element = LLink;
					type = GRID_STRING;
					return true;
				}
			}
			return t_muscle::get_addpar( path, name, element, type );
		};
		bool upd_addpar( const char *path, const char *name, string &element, bool save )
		{
			if( path ){
				string p_path = path;
				if( p_path == "MN input" ){
					return t_muscle::upd_addpar( path, name, element, save );
				}
			}
			return false;
		};
		void load_dummy( void );
};

////////////////////////////////////////////////////////////////////////
// class t_sart
class t_sart : public t_muscle{
	public:
		t_sart( uni_template *parent, const char *lscheme = "None" );
		t_sart( const t_sart &m ) : t_muscle( m ){};
		~t_sart( void ){};
	public:
		void copy_to( uni_template **unit, uni_template *parent )
		{
			*unit = new t_sart( *this );
			( *unit )->set_parent( parent );
		};
	protected:
		int get_datattr( const char *path )
		{
			string p_path = path;
			if( p_path == "Joint scheme" || p_path == "Middle link" || 
				p_path == "Upper link" || p_path == "Lower link" || p_path == "Name" ){
				return GRID_READONLY;
			}
			return t_muscle::get_datattr( path );
		};
		bool get_addpar( const char *path, string &name, string &element, int &type )
		{
			if( path ){
				string p_path = path;
				if( p_path == "Joint scheme" ){
					name = "Data";
					element = JScheme;
					type = GRID_STRING;
					return true;
				}
				if( p_path == "Middle link" ){
					name = "Data";
					element = MLink;
					type = GRID_STRING;
					return true;
				}
				if( p_path == "Upper link" ){
					name = "Data";
					element = ULink;
					type = GRID_STRING;
					return true;
				}
				if( p_path == "Lower link" ){
					name = "Data";
					element = LLink;
					type = GRID_STRING;
					return true;
				}
			}
			return t_muscle::get_addpar( path, name, element, type );
		};
		bool upd_addpar( const char *path, const char *name, string &element, bool save )
		{
			if( path ){
				string p_path = path;
				if( p_path == "MN input" ){
					return t_muscle::upd_addpar( path, name, element, save );
				}
			}
			return false;
		};
		void load_dummy( void );
};

////////////////////////////////////////////////////////////////////////
// class t_rf
class t_rf : public t_muscle{
	public:
		t_rf( uni_template *parent, const char *lscheme = "None" );
		t_rf( const t_rf &m ) : t_muscle( m ){};
		~t_rf( void ){};
	public:
		void copy_to( uni_template **unit, uni_template *parent )
		{
			*unit = new t_rf( *this );
			( *unit )->set_parent( parent );
		};
      protected:
		int get_datattr( const char *path )
		{
			string p_path = path;
			if( p_path == "Joint scheme" || p_path == "Middle link" || 
				p_path == "Upper link" || p_path == "Lower link" || p_path == "Name" ){
				return GRID_READONLY;
			}
			return t_muscle::get_datattr( path );
		};
		bool get_addpar( const char *path, string &name, string &element, int &type )
		{
			if( path ){
				string p_path = path;
				if( p_path == "Joint scheme" ){
					name = "Data";
					element = JScheme;
					type = GRID_STRING;
					return true;
				}
				if( p_path == "Middle link" ){
					name = "Data";
					element = MLink;
					type = GRID_STRING;
					return true;
				}
				if( p_path == "Upper link" ){
					name = "Data";
					element = ULink;
					type = GRID_STRING;
					return true;
				}
				if( p_path == "Lower link" ){
					name = "Data";
					element = LLink;
					type = GRID_STRING;
					return true;
				}
			}
			return t_muscle::get_addpar( path, name, element, type );
		};
		bool upd_addpar( const char *path, const char *name, string &element, bool save )
		{
			if( path ){
				string p_path = path;
				if( p_path == "MN input" ){
					return t_muscle::upd_addpar( path, name, element, save );
				}
			}
			return false;
		};
		void load_dummy( void );
};

////////////////////////////////////////////////////////////////////////
// class t_bfp
class t_bfp : public t_muscle{
	public:
		t_bfp( uni_template *parent, const char *lscheme = "None" );
		t_bfp( const t_bfp &m ) : t_muscle( m ){};
		~t_bfp( void ){};
	public:
		void copy_to( uni_template **unit, uni_template *parent )
		{
			*unit = new t_bfp( *this );
			( *unit )->set_parent( parent );
		};
	protected:
		int get_datattr( const char *path )
		{
			string p_path = path;
			if( p_path == "Joint scheme" || p_path == "Middle link" || 
				p_path == "Upper link" || p_path == "Lower link" || p_path == "Name" ){
				return GRID_READONLY;
			}
			return t_muscle::get_datattr( path );
		};
		bool get_addpar( const char *path, string &name, string &element, int &type )
		{
			if( path ){
				string p_path = path;
				if( p_path == "Joint scheme" ){
					name = "Data";
					element = JScheme;
					type = GRID_STRING;
					return true;
				}
				if( p_path == "Middle link" ){
					name = "Data";
					element = MLink;
					type = GRID_STRING;
					return true;
				}
				if( p_path == "Upper link" ){
					name = "Data";
					element = ULink;
					type = GRID_STRING;
					return true;
				}
				if( p_path == "Lower link" ){
					name = "Data";
					element = LLink;
					type = GRID_STRING;
					return true;
				}
			}
			return t_muscle::get_addpar( path, name, element, type );
		};
		bool upd_addpar( const char *path, const char *name, string &element, bool save )
		{
			if( path ){
				string p_path = path;
				if( p_path == "MN input" ){
					return t_muscle::upd_addpar( path, name, element, save );
				}
			}
			return false;
		};
		void load_dummy( void );
};

////////////////////////////////////////////////////////////////////////
// class t_va
class t_va : public t_muscle{
	public:
		t_va( uni_template *parent, const char *lscheme = "None" );
		t_va( const t_va &m ) : t_muscle( m ){};
		~t_va( void ){};
	public:
		void copy_to( uni_template **unit, uni_template *parent )
		{
			*unit = new t_va( *this );
			( *unit )->set_parent( parent );
		};
	protected:
		int get_datattr( const char *path )
		{
			string p_path = path;
			if( p_path == "Joint scheme" || p_path == "Middle link" || 
				p_path == "Upper link" || p_path == "Lower link" || p_path == "Name" ){
				return GRID_READONLY;
			}
			return t_muscle::get_datattr( path );
		};
		bool get_addpar( const char *path, string &name, string &element, int &type )
		{
			if( path ){
				string p_path = path;
				if( p_path == "Joint scheme" ){
					name = "Data";
					element = JScheme;
					type = GRID_STRING;
					return true;
				}
				if( p_path == "Middle link" ){
					name = "Data";
					element = MLink;
					type = GRID_STRING;
					return true;
				}
				if( p_path == "Upper link" ){
					name = "Data";
					element = ULink;
					type = GRID_STRING;
					return true;
				}
				if( p_path == "Lower link" ){
					name = "Data";
					element = LLink;
					type = GRID_STRING;
					return true;
				}
			}
			return t_muscle::get_addpar( path, name, element, type );
		};
		bool upd_addpar( const char *path, const char *name, string &element, bool save )
		{
			if( path ){
				string p_path = path;
				if( p_path == "MN input" ){
					return t_muscle::upd_addpar( path, name, element, save );
				}
			}
			return false;
		};
		void load_dummy( void );
};

////////////////////////////////////////////////////////////////////////
// class t_ga
class t_ga : public t_muscle{
	public:
		t_ga( uni_template *parent, const char *lscheme = "None" );
		t_ga( const t_ga &m ) : t_muscle( m ){};
		~t_ga( void ){};
	public:
		void copy_to( uni_template **unit, uni_template *parent )
		{
			*unit = new t_ga( *this );
			( *unit )->set_parent( parent );
		};
	protected:
		int get_datattr( const char *path )
		{
			string p_path = path;
			if( p_path == "Joint scheme" || p_path == "Middle link" || 
				p_path == "Upper link" || p_path == "Lower link" || p_path == "Name" ){
				return GRID_READONLY;
			}
			return t_muscle::get_datattr( path );
		};
		bool get_addpar( const char *path, string &name, string &element, int &type )
		{
			if( path ){
				string p_path = path;
				if( p_path == "Joint scheme" ){
					name = "Data";
					element = JScheme;
					type = GRID_STRING;
					return true;
				}
				if( p_path == "Middle link" ){
					name = "Data";
					element = MLink;
					type = GRID_STRING;
					return true;
				}
				if( p_path == "Upper link" ){
					name = "Data";
					element = ULink;
					type = GRID_STRING;
					return true;
				}
				if( p_path == "Lower link" ){
					name = "Data";
					element = LLink;
					type = GRID_STRING;
					return true;
				}
			}
			return t_muscle::get_addpar( path, name, element, type );
		};
		bool upd_addpar( const char *path, const char *name, string &element, bool save )
		{
			if( path ){
				string p_path = path;
				if( p_path == "MN input" ){
					return t_muscle::upd_addpar( path, name, element, save );
				}
			}
			return false;
		};
		void load_dummy( void );
};

////////////////////////////////////////////////////////////////////////
// class t_ta
class t_ta : public t_muscle{
	public:
		t_ta( uni_template *parent, const char *lscheme = "None" );
		t_ta( const t_ta &m ) : t_muscle( m ){};
		~t_ta( void ){};
	public:
		void copy_to( uni_template **unit, uni_template *parent )
		{
			*unit = new t_ta( *this );
			( *unit )->set_parent( parent );
		};
	protected:
		int get_datattr( const char *path )
		{
			string p_path = path;
			if( p_path == "Joint scheme" || p_path == "Middle link" || 
				p_path == "Upper link" || p_path == "Lower link" || p_path == "Name" ){
				return GRID_READONLY;
			}
			return t_muscle::get_datattr( path );
		};
		bool get_addpar( const char *path, string &name, string &element, int &type )
		{
			if( path ){
				string p_path = path;
				if( p_path == "Joint scheme" ){
					name = "Data";
					element = JScheme;
					type = GRID_STRING;
					return true;
				}
				if( p_path == "Middle link" ){
					name = "Data";
					element = MLink;
					type = GRID_STRING;
					return true;
				}
				if( p_path == "Upper link" ){
					name = "Data";
					element = ULink;
					type = GRID_STRING;
					return true;
				}
				if( p_path == "Lower link" ){
					name = "Data";
					element = LLink;
					type = GRID_STRING;
					return true;
				}
			}
			return t_muscle::get_addpar( path, name, element, type );
		};
		bool upd_addpar( const char *path, const char *name, string &element, bool save )
		{
			if( path ){
				string p_path = path;
				if( p_path == "MN input" ){
					return t_muscle::upd_addpar( path, name, element, save );
				}
			}
			return false;
		};
		void load_dummy( void );
};

////////////////////////////////////////////////////////////////////////
// class t_sol
class t_sol : public t_muscle{
	public:
		t_sol( uni_template *parent, const char *lscheme = "None" );
		t_sol( const t_sol &m ) : t_muscle( m ){};
		~t_sol( void ){};
	public:
		void copy_to( uni_template **unit, uni_template *parent )
		{
			*unit = new t_sol( *this );
			( *unit )->set_parent( parent );
		};
	protected:
		int get_datattr( const char *path )
		{
			string p_path = path;
			if( p_path == "Joint scheme" || p_path == "Middle link" || 
				p_path == "Upper link" || p_path == "Lower link" || p_path == "Name" ){
				return GRID_READONLY;
			}
			return t_muscle::get_datattr( path );
		};
		bool get_addpar( const char *path, string &name, string &element, int &type )
		{
			if( path ){
				string p_path = path;
				if( p_path == "Joint scheme" ){
					name = "Data";
					element = JScheme;
					type = GRID_STRING;
					return true;
				}
				if( p_path == "Middle link" ){
					name = "Data";
					element = MLink;
					type = GRID_STRING;
					return true;
				}
				if( p_path == "Upper link" ){
					name = "Data";
					element = ULink;
					type = GRID_STRING;
					return true;
				}
				if( p_path == "Lower link" ){
					name = "Data";
					element = LLink;
					type = GRID_STRING;
					return true;
				}
			}
			return t_muscle::get_addpar( path, name, element, type );
		};
		bool upd_addpar( const char *path, const char *name, string &element, bool save )
		{
			if( path ){
				string p_path = path;
				if( p_path == "MN input" ){
					return t_muscle::upd_addpar( path, name, element, save );
				}
			}
			return false;
		};
		void load_dummy( void );
};

#elif defined (__MECHANICS_3D__)
// TODO implementation 3d model
#endif //__MECHANICS_2D__
#endif //__MUSCLE_TEMPLATE_H
