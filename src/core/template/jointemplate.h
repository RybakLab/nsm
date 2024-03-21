////////////////////////////////////////////////////////////////////////
// jointemplate.h
//---------------------------------------------------------------------
// default parameters
//---------------------------------------------------------------------
// Name			Loins	Hip		Knee	Ankle
// angle		146		0		0		0
// MinA			120		5		0		0
// MaxA			240		180		180		180
// velocity		0		0		0		0
//---------------------------------------------------------------------

#ifndef __JOINT_TEMPLATE_H
#define __JOINT_TEMPLATE_H

#include "sptypes.h"

#if defined (__MECHANICS_2D__)

#include "unitemplate.h"

enum __ID_Joint{
	__id_generic_J	= 0,
	__id_LOINS,
	__id_HIP,
	__id_KNEE,
	__id_ANKLE,
	__id_MAX_JOINTS,
	};

extern const char *__JointNames[__id_MAX_JOINTS];

class walker;
/////////////////////////////////////////////////////////////////////////////
// t_joint 
class t_joint : public uni_template{
	public:
		t_joint( uni_template *parent, int is_active = GRID_COLLAPSE );
		t_joint( const t_joint &j );
virtual		~t_joint( void );
	public:
		t_joint &operator = ( const t_joint &j );
	public:
virtual		void copy_to( uni_template **unit, uni_template *parent );
virtual		void copy_to( walker *w, int side ){};
	protected:
virtual		int get_datattr( const char *path );
virtual		bool get_addparlist( vector<string> &parlist );
virtual		bool get_addpar( const char *path, string &name, string &element, int &type );
virtual		bool upd_addpar( const char *path, const char *name, string &element, bool save );
      //--- Load/save parameters 
virtual		bool load_addpar( string str, istream &file );
virtual		void save_addpar( ostream &file );
	public:
		bool IsFixed;
		t_data A;
		t_data V;
		t_data MinA;
		t_data MaxA;
};

////////////////////////////////////////////////////////////////////////
// class t_loins
class t_loins : public t_joint{
	public:
		t_loins( uni_template *parent, int is_active = GRID_COLLAPSE );
		t_loins( const t_loins &j ) : t_joint( j ){};
		~t_loins( void ) {};
	public:
		void copy_to( uni_template **unit, uni_template *parent );
		void copy_to( walker *w, int /*side*/ );
	protected:
		int get_datattr( const char *path )
		{
			string p_path = path;
			if( p_path == "Name" ){
				return GRID_READONLY;
			}
			return t_joint::get_datattr( path );
		};
};

////////////////////////////////////////////////////////////////////////
// class t_hip
class t_hip : public t_joint{
	public:
		t_hip( uni_template *parent, int is_active = GRID_COLLAPSE );
		t_hip( const t_hip &j ) : t_joint( j ){};
		~t_hip( void ) {};
	public:
		void copy_to( uni_template **unit, uni_template *parent );
		void copy_to( walker *w, int side );
	protected:
		int get_datattr( const char *path )
		{
			string p_path = path;
			if( p_path == "Name" ){
				return GRID_READONLY;
			}
			return t_joint::get_datattr( path );
		};
};

////////////////////////////////////////////////////////////////////////
// class t_knee
class t_knee : public t_joint{
	public:
		t_knee( uni_template *parent, int is_active = GRID_COLLAPSE );
		t_knee( const t_knee &j ) : t_joint( j ){};
		~t_knee( void ) {};
	public:
		void copy_to( uni_template **unit, uni_template *parent );
		void copy_to( walker *w, int side );
	protected:
		int get_datattr( const char *path )
		{
			string p_path = path;
			if( p_path == "Name" ){
				return GRID_READONLY;
			}
			return t_joint::get_datattr( path );
		};
};

////////////////////////////////////////////////////////////////////////
// class t_ankle
class t_ankle : public t_joint{
	public:
		t_ankle( uni_template *parent, int is_active = GRID_COLLAPSE );
		t_ankle( const t_ankle &j ) : t_joint( j ){};
		~t_ankle( void ) {};
	public:
		void copy_to( uni_template **unit, uni_template *parent );
		void copy_to( walker *w, int side );
	protected:
		int get_datattr( const char *path )
		{
			string p_path = path;
			if( p_path == "Name" ){
				return GRID_READONLY;
			}
			return t_joint::get_datattr( path );
		};
};

#elif defined (__MECHANICS_3D__)
// TODO implementation 3d model
#endif //__MECHANICS_2D__
#endif //__JOINT_TEMPLATE_H
