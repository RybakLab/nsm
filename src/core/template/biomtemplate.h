////////////////////////////////////////////////////////////////////////
// biomtemplate.h
//---------------------------------------------------------------------
// default parameters
//---------------------------------------------------------------------
// Name			Body		Left		Right
// hip A		-			86			129
// knee A		-			147			140
// ankle A		-			140			90
// muscles properties
//-------------------
// MusBM		0.02		-			-
// MusBT		0.02		-			-
// mechanics properties
//-------------------
// JointVis		0.0			-			-
// LoinsB		0.0			-			-
// LoinsK		0.0			-			-
// JointK		0.0			-			-
// JointB		0.0			-			-
// GroundK		0.0			-			-
// GroundB		0.0			-			-
//---------------------------------------------------------------------
#ifndef __BIOM_TEMPLATE_H
#define __BIOM_TEMPLATE_H

#include "sptypes.h"

#ifdef __MECHANICS__

#include "mustemplate.h"
#include "linktemplate.h"
#include "jointemplate.h"

class walker;
/////////////////////////////////////////////////////////////////////////////
// t_limb 
class t_limb : public uni_template{
	public:
      //--- constructor
		t_limb( uni_template *parent, const char *name );
		t_limb( const t_limb &limb );
virtual	~t_limb( void );
	public:
		t_limb &operator = ( const t_limb &limb );
	public:
		void get_fblist( vector<string> &fb_list ) const;
		void get_muscleslist( vector<string> &m_list ) const;
	public:
		void upd_outlist( vector<string> &out_list );
virtual	void copy_to( uni_template **unit, uni_template *parent );
		void copy_to( walker *w, double *m, double *cm, double *I );
	protected:
		bool add_child( const char *type, int id, const char *name = NULL );
		bool add_child( const char *type, const char *name );
      //--- Load/save parameters 
virtual	bool load_addpar( string str, istream &file );
virtual	void save_addpar( ostream &file );
      protected:
static	t_link DefaultLink;
static	t_joint DefaultJoint;
static	t_muscle DefaultMuscle;
};

/////////////////////////////////////////////////////////////////////////////
// t_body 
class t_body : public uni_template{
	public:
      //--- constructor
		t_body( uni_template *parent, const char *name );
		t_body( const t_body &body );
virtual	~t_body( void );
	public:
		t_body &operator = ( const t_body &body );
	public:
		void get_fblist( vector<string> &fb_list ) const{};
virtual	void copy_to( uni_template **unit, uni_template *parent );
		void copy_to( walker *w, double *m, double *cm, double *I );
	protected:
		bool add_child( const char *type, int id, const char *name = NULL );
		bool add_child( const char *type, const char *name );
      //--- Load/save parameters 
virtual	bool load_addpar( string str, istream &file );
virtual	void save_addpar( ostream &file );
	protected:
static	t_link DefaultLink;
static	t_joint DefaultJoint;
};

/////////////////////////////////////////////////////////////////////////////
// t_biomech 
class t_biomech : public uni_template{
	public:
      //--- constructor
		t_biomech( uni_template *parent, int is_active = GRID_NONE );
		t_biomech( const t_biomech &biomech );
virtual	~t_biomech( void );
	public:
		t_biomech &operator = ( const t_biomech &biomech );
	public:
		hhn_pair<int> get_uid( const string &name ) const;
		void get_fblist( vector<string> &fb_list ) const;
		void upd_outlist( vector<string> &out_list );
		void copy_to( walker **w );
	protected:
		void get_muscleslist( vector<string> &m_list ) const;
	public:
		void copy_to( uni_template **unit, uni_template *parent );
	protected:
		bool add_child( const char *type, const char *name );
		int get_datattr( const char *path );
		bool get_addparlist( vector<string> &parlist );
		bool get_addpar( const char *path, string &name, string &element, int &type );
		bool upd_addpar( const char *path, const char *name, string &element, bool save );
      //--- Load/save parameters 
		bool load_addpar( string str, istream &file );
		void save_addpar( ostream &file );
	protected:
		bool IsFixed;
		t_data G;
		t_data Vel;
		t_data MusBM;
		t_data MusBT;
		t_data JointVis;

		t_data ForepartK;
		t_data ForepartB;
		t_data TrunkK;
		t_data TrunkB;
		t_data LoinsK;
		t_data LoinsB;
		t_data PelvisK;
		t_data PelvisB;
		t_data JointK;
		t_data JointB;
		t_data GroundK;
		t_data GroundB;
		t_data GroundLevel;
		t_data X;
		t_data Y;
		t_data Xmin;
		t_data Ymin;
		t_data A;
		t_data dX;
		t_data dY;
		t_data dA;
};

#endif //__MECHANICS__
#endif //__BIOM_TEMPLATE_H
