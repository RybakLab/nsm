/*****************************************************************
 ****              (C)  1997 - 2000                           ****
 **           Model of Hodgkin-Haxley type neuron               **
 **       Development by Ilia Rybak and Sergey Markin           **
 ****                   Neuron template                       ****
 *****************************************************************/
#include "precompile.h"

#ifdef __RESPIRATION__

#include "respchantemplate.h"
#include "hhn.h"

#ifndef __LINUX__ 
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif // _DEBUG
#endif //__LINUX__

//--- constructor
trnaf_channel::trnaf_channel( uni_template *parent, int is_active ) 
	: tna_channel( parent, is_active )
{
	Gmax = 150.0; Tm = 0.28; Th = 0.28;
	UnitId = _id_resp_NaFChan; 
	Name = _ChannelNames[UnitId];
}

//--- overrided function
void trnaf_channel::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new trnaf_channel( *this );
	( *unit )->set_parent( parent );
}

void trnaf_channel::copy_to( hhn_process **unit, hhn_process *parent )
{
	*unit = new hrnaf_channel(( hhn_compart *)parent );
	(( hhn_channel *)( *unit ))->init_channel( Gmax, Tm, Th );
	(( hhn_channel *)( *unit ))->Name = Name;
	(( hrnaf_channel *)*unit )->init_channel( M, H );
}

//--- constructor
trnap_channel::trnap_channel( uni_template *parent, int is_active ) 
	: tna_channel( parent, is_active )
{
	Gmax = 8; Tm = 0.28; Th = 3000.;
	UnitId = _id_resp_NaPChan; 
	Name = _ChannelNames[UnitId];
	DList.clear();
	DList.insert( make_pair( "Maximal conductance (nS)\nGmax", &Gmax ));
	DList.insert( make_pair( "Inactivation\tInit\nH\nHnap", &H )); 
	DList.insert( make_pair( "Inactivation\tTime constant\tMagnitude\nTh", &Th )); 
	DList.insert( make_pair( "Activation\tInit\nM\nMnap", &M ));
}

trnap_channel::trnap_channel( const trnap_channel &channel )
	: tna_channel( channel )
{
	DList.clear();
	DList.insert( make_pair( "Maximal conductance (nS)\nGmax", &Gmax ));
	DList.insert( make_pair( "Inactivation\tInit\nH\nHnap", &H )); 
	DList.insert( make_pair( "Inactivation\tTime constant\tMagnitude\nTh", &Th )); 
	DList.insert( make_pair( "Activation\tInit\nM\nMnap", &M ));
}

//--- overrided function
void trnap_channel::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new trnap_channel( *this );
	( *unit )->set_parent( parent );
}

void trnap_channel::copy_to( hhn_process **unit, hhn_process *parent )
{
	*unit = new hrnap_channel(( hhn_compart *)parent );
	(( hhn_channel *)( *unit ))->init_channel( Gmax, Tm, Th );
	(( hhn_channel *)( *unit ))->Name = Name;
	(( hrnap_channel *)*unit )->init_channel( M, H );
}

//--- constructor
trkdr_channel::trkdr_channel( uni_template *parent, int is_active ) 
	: tk_channel( parent, is_active )
{
	Gmax = 90; Tm = 1.0;
	UnitId = _id_resp_KdrChan;
	Name = _ChannelNames[UnitId];
}

//--- overrided function
void trkdr_channel::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new trkdr_channel( *this );
	( *unit )->set_parent( parent );
}

void trkdr_channel::copy_to( hhn_process **unit, hhn_process *parent )
{
	*unit = new hrkdr_channel(( hhn_compart *)parent );
	(( hhn_channel *)( *unit ))->init_channel( Gmax, Tm, Th );
	(( hhn_channel *)( *unit ))->Name = Name;
	(( hrkdr_channel *)*unit )->init_channel( M );
}

//--- constructor
trka_channel::trka_channel( uni_template *parent, int is_active ) 
	: tka_channel( parent, is_active )
{
	Gmax = 70;	Th = 300.;
	UnitId = _id_resp_KAChan; 
	Name = _ChannelNames[UnitId];
}

//--- overrided function
void trka_channel::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new trka_channel( *this );
	( *unit )->set_parent( parent );
}

void trka_channel::copy_to( hhn_process **unit, hhn_process *parent )
{
	*unit = new hrka_channel(( hhn_compart *)parent );
	(( hhn_channel *)( *unit ))->init_channel( Gmax, Tm, Th );
	(( hhn_channel *)( *unit ))->Name = Name;
	(( hrka_channel *)*unit )->init_channel( Mka1, Hka1, M, H );
}

//--- constructor
trcal_channel::trcal_channel( uni_template *parent, int is_active ) 
	: tcal_channel( parent, is_active )
{
	Gmax = 3; H = 0.01; Tm = 0.5; Th = 18.;
	UnitId = _id_resp_CaLChan; 
	Name = _ChannelNames[UnitId];
}

//--- overrided function
void trcal_channel::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new trcal_channel( *this );
	( *unit )->set_parent( parent );
}

void trcal_channel::copy_to( hhn_process **unit, hhn_process *parent )
{
	*unit = new hrcal_channel(( hhn_compart *)parent );
	(( hhn_channel *)( *unit ))->init_channel( Gmax, Tm, Th );
	(( hhn_channel *)( *unit ))->Name = Name;
	(( hrcal_channel *)*unit )->init_channel( M, H );
}

//--- constructor
trcat_channel::trcat_channel( uni_template *parent, int is_active ) 
	: tcat_channel( parent, is_active )
{
	Gmax = 3; 
	UnitId = _id_resp_CaTChan; 
	Name = _ChannelNames[UnitId];
}

//--- overrided function
void trcat_channel::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new trcat_channel( *this );
	( *unit )->set_parent( parent );
}

void trcat_channel::copy_to( hhn_process **unit, hhn_process *parent )
{
	*unit = new hrcat_channel(( hhn_compart *)parent );
	(( hhn_channel *)( *unit ))->init_channel( Gmax, Tm, Th );
	(( hhn_channel *)( *unit ))->Name = Name;
	(( hrcat_channel *)*unit )->init_channel( M, H );
}

//--- constructor
trkca_channel::trkca_channel( uni_template *parent, int is_active ) 
	: tkca_channel( parent, is_active )
{
	Gmax = 70; Tm = 2.; M = 0.3;
	UnitId = _id_resp_KCaChan; 
	Name = _ChannelNames[UnitId];
}

//--- overrided function
void trkca_channel::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new trkca_channel( *this );
	( *unit )->set_parent( parent );
}

void trkca_channel::copy_to( hhn_process **unit, hhn_process *parent )
{
	*unit = new hrkca_channel(( hhn_compart *)parent );
	(( hhn_channel *)( *unit ))->init_channel( Gmax, Tm, 1 );
	(( hhn_channel *)( *unit ))->Name = Name;
	(( hrkca_channel *)*unit )->init_channel( M );
}

//--- constructor
trleak_channel::trleak_channel( uni_template *parent, int is_active ) 
	: tleak_channel( parent, is_active ),
	Pna( 0.05 ), Pcl( 0.45 ), Adjustable( false )
{
	UnitId = _id_resp_LeakChan; 
	Name = _ChannelNames[UnitId];
	Gmax = 10.0; Eleak = -65.0;
	DList.insert( make_pair( "Leak Na:K\nPna", &Pna ));
	DList.insert( make_pair( "Leak Cl:K\nPcl", &Pcl ));
}

trleak_channel::trleak_channel( const trleak_channel &channel )
	: tleak_channel( channel )
{
	Pna = channel.Pna;
	Pcl = channel.Pcl;
	Adjustable = channel.Adjustable;
	Eleak = channel.Eleak;
	DList.insert( make_pair( "Leak Na:K\nPna", &Pna ));
	DList.insert( make_pair( "Leak Cl:K\nPcl", &Pcl ));
}

trleak_channel &trleak_channel::operator = ( const trleak_channel &channel )
{
	tleak_channel::operator = ( channel );
	Pna = channel.Pna;
	Pcl = channel.Pcl;
	Adjustable = channel.Adjustable;
	return *this;
}

//--- overrided function
bool trleak_channel::process( const char *path )
{
	calc_eds();
	return true;
}

void trleak_channel::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new trleak_channel( *this );
	( *unit )->set_parent( parent );
}

void trleak_channel::copy_to( hhn_process **unit, hhn_process *parent )
{
	*unit = new hrleak_channel(( hhn_compart *)parent );
	(( hhn_channel *)( *unit ))->init_channel( Gmax, Tm, Th );
	(( hhn_channel *)( *unit ))->Name = Name;
	(( hrleak_channel *)*unit )->init_channel( Adjustable, Pna, Pcl, Eleak );
}

bool trleak_channel::get_addparlist( vector<string> &parlist )
{
	parlist.push_back( "Leak GHK Eq" );
	return true;
}

bool trleak_channel::get_addpar( const char *path, string &name, string &element, int &type )
{
	if( path ){
		string p_path = path;
		if( p_path == "Leak GHK Eq" ){
			name = "Data";
			element = Adjustable? "true":"false";
			type = GRID_BOOL;
			return true;
		}
	}
	return false;
}

bool trleak_channel::upd_addpar( const char *path, const char *name, string &element, bool save )
{
	if( path && name ){
		string p_path = path;
		string p_name = name;
		if( p_path == "Leak GHK Eq" && p_name == "Data" ){
			if( save ){
				Adjustable = ( element == "true" )? true:false;
			}
			else{
				element = Adjustable? "true":"false";
			}
			return true;
		}
	}
	return false;
}

int trleak_channel::get_datattr( const char *path )
{
	string p_path = path;
	if( p_path == "Leak Na:K" || p_path == "Leak Cl:K" ){
		if( Adjustable ){
			return GRID_PROCESS|GRID_POSITIVE;
		}
		return GRID_READONLY;
	}
	else if( p_path == "Leak GHK Eq" ){
		return GRID_PROCESS;
	}
	else if( p_path == "Reversal potential (mV)" && Adjustable ){
		return GRID_READONLY;
	}
	return tleak_channel::get_datattr( path );
}

void trleak_channel::calc_eds(void) 
{
	if( Adjustable ){
		double pna = Pna.Value;
		double pcl = Pcl.Value;
		t_data *par = find_data( "Na\tInside" );
		if( !par )
			return;
		double inNa = par->Value;
		par = find_data( "Na\tOutside" );
		if( !par )
			return;
		double outNa = par->Value;
		par = find_data( "K\tInside" );
		if( !par )
			return;
		double inK = par->Value;
		par = find_data( "K\tOutside" );
		if( !par )
			return;
		double outK = par->Value;
		par = find_data( "Cl\tInside" );
		if( !par )
			return;
		double inCl = par->Value;
		par = find_data( "Cl\tOutside" );
		if( !par )
			return;
		double outCl = par->Value;
		par = find_data( "RTF\tRTF" );
		if( !par )
			return;
		double rtf = par->Value;
		Eleak.Value = rtf*log(( outK+pna*outNa+pcl*inCl )/( inK+pna*inNa+pcl*outCl ));
	}
}

bool trleak_channel::load_addpar( string str, istream &file )
{
	if( str == "AdjustableLeak"){
		file >> str >> str;
		Adjustable = ( str == "true" )? true:false;
		return true;
	}
	return false;
}

void trleak_channel::save_addpar( ostream &file )
{
	file << "AdjustableLeak = " << ( Adjustable? "true": "false" ) << endl;
}

#endif //__RESPIRATION__
