#include "precompile.h"

#include "chantemplate.h"
#include "hhnnetwork.h"

#ifndef __LINUX__ 
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif // _DEBUG
#endif //__LINUX__

CHhnNetwork *GlobalNet = NULL;

string get_gchanname( int id ) 
{
	// new code 4 control
	if( GlobalNet )
		return GlobalNet->TUniList.getname( id );
	return "";
}

int get_gchanid( const string &name )
{
	// new code 4 control
	if( GlobalNet )
		return GlobalNet->TUniList.getid( name );
	return -1;
}

bool add_gchan( const string &name )
{
	// new code 4 control
	if( GlobalNet )
		return GlobalNet->TUniList.add( name );
	return false;
}

bool del_gchan( const string &name )
{
	// new code 4 control
	if( GlobalNet )
		return GlobalNet->TUniList.del( name );
	return false;
}

bool rename_gchan( const string &oldname, const string &newname )
{
	// new code 4 control
	if( GlobalNet )
		return GlobalNet->TUniList.rename( oldname, newname );
	return false;
}

/*****************************************************************
 *   Model of channel template for Hodgkin-Haxley type neuron    *
 *****************************************************************/
//--- constructor
// soma and dendrite channels are not included. They will be inserted automatically if it is necessary
t_channel::t_channel( uni_template *parent, int is_active )
	: uni_template( parent, is_active, ( char **)_ChannelNames, _id_MAX_CHAN-2 ),
	Gmax( 0.0 ), M( 0.0 ), H( 0.0 ), Tm( 1.0 ), Th( 1.0 )
{
	TypeNameS = TypeName = "Channel";
#if defined( __RESPIRATION__ ) && !defined( __LOCOMOTION__ )
	DList.insert( make_pair( "Maximal conductance (nS)\nGmax", &Gmax ));
#else
	DList.insert( make_pair( "Maximal conductance (mS/sm\xB2)\nGmax", &Gmax ));
#endif // defined( __RESPIRATION__ ) && !defined( __LOCOMOTION__ )
}

t_channel::t_channel( const t_channel &channel )
	: uni_template( channel ),
	Gmax( channel.Gmax ), M( channel.M ), H( channel.H ), Tm( channel.Tm ), Th( channel.Th )
{
#if defined( __RESPIRATION__ ) && !defined( __LOCOMOTION__ )
	DList.insert( make_pair( "Maximal conductance (nS)\nGmax", &Gmax ));
#else
	DList.insert( make_pair( "Maximal conductance (mS/sm\xB2)\nGmax", &Gmax ));
#endif // defined( __RESPIRATION__ ) && !defined( __LOCOMOTION__ )
}

t_channel &t_channel::operator = ( const t_channel &channel )
{
	uni_template::operator = ( channel );
	Gmax = channel.Gmax;
	M = channel.M;
	H = channel.H;
	Tm = channel.Tm;
	Th = channel.Th;
	return *this;
}

bool t_channel::load_addpar( string str, istream &file )
{
 if( str == "<Help" ){
     file >> ws;
     getline( file, str, '>') ;
     if( !get_child( str.c_str() )&& !add_child( str.c_str() ))
         return false;
     return get_child( str.c_str() )->load( file );
     }
 return false;

}

void t_channel::save_addpar( ostream &file )
{
	for( unsigned int i = 0; i < Children.size(); i++ ){
		Children[i]->save( file );
	}
}

/*****************************************************************
 *  Model of generic channel template for Hodgkin-Haxley type neuron  *
 *****************************************************************/
tgen_channel::tgen_channel( uni_template *parent, int is_active ) :
	t_channel( parent, is_active ), ModI( false ), Eds("0.0" ),
	Km( 1.0 ), Hvm( 0.0 ), Slpm( 1.0 ), Tm0( 0.0 ), Hvtm( 0.0 ), Slptm( 1.0 ), PowM( 0.0 ),
	Kh( 1.0 ), Hvh( 0.0 ), Slph( 1.0 ), Th0( 0.0 ), Hvth( 0.0 ), Slpth( 1.0 ), PowH( 0.0 ), Kpump( 1.0 )
{
	Gmax = 1.; 
	UnitId = _id_generic_Chan; 
	Name = _ChannelNames[UnitId];
	DList.insert( make_pair( "Inactivation\tInit\nH", &H ));
	DList.insert( make_pair( "Inactivation\tMagitude\nHk", &Kh ));
	DList.insert( make_pair( "Inactivation\tSlope\nSlph", &Slph ));
	DList.insert( make_pair( "Inactivation\tHalf-voltage\nHvh", &Hvh ));
	DList.insert( make_pair( "Inactivation\tPower\nPowH", &PowH ));
	DList.insert( make_pair( "Inactivation\tTime constant\tShift\nTh0", &Th0 )); 
	DList.insert( make_pair( "Inactivation\tTime constant\tMagitude\nTh", &Th )); 
	DList.insert( make_pair( "Inactivation\tTime constant\tSlope\nSlpth", &Slpth ));
	DList.insert( make_pair( "Inactivation\tTime constant\tHalf-voltage\nHvth", &Hvth ));

	DList.insert( make_pair( "Activation\tInit\nM", &M ));
	DList.insert( make_pair( "Activation\tMagitude\nMk", &Km ));
	DList.insert( make_pair( "Activation\tSlope\nSlpm", &Slpm ));
	DList.insert( make_pair( "Activation\tHalf-voltage\nHvm", &Hvm ));
	DList.insert( make_pair( "Activation\tPower\nPowM", &PowM ));
	DList.insert( make_pair( "Activation\tTime constant\tShift\nTm0", &Tm0 )); 
	DList.insert( make_pair( "Activation\tTime constant\tMagitude\nTm", &Tm )); 
	DList.insert( make_pair( "Activation\tTime constant\tSlope\nSlptm", &Slptm ));
	DList.insert( make_pair( "Activation\tTime constant\tHalf-voltage\nHvtm", &Hvtm ));

	DList.insert( make_pair( "Ions dynamics rate\nKpump", &Kpump ));

	TypeNameS = "gChannel";
}

tgen_channel::tgen_channel( const tgen_channel &channel ) :
	t_channel( channel ), ModI( channel.ModI ), Eds( channel.Eds ),
	Km( channel.Km ), Hvm( channel.Hvm ), Slpm( channel.Slpm ), Tm0( channel.Tm0 ), Hvtm( channel.Hvtm ), Slptm( channel.Slptm ), PowM( channel.PowM ),
	Kh( channel.Kh ), Hvh( channel.Hvh ), Slph( channel.Slph ), Th0( channel.Th0 ), Hvth( channel.Hvth ), Slpth( channel.Slpth ), PowH( channel.PowH ), Kpump( channel.Kpump )
{
	DList.insert( make_pair( "Inactivation\tInit\nH", &H ));
	DList.insert( make_pair( "Inactivation\tMagitude\nHk", &Kh ));
	DList.insert( make_pair( "Inactivation\tSlope\nSlph", &Slph ));
	DList.insert( make_pair( "Inactivation\tHalf-voltage\nHvh", &Hvh ));
	DList.insert( make_pair( "Inactivation\tPower\nPowH", &PowH ));
	DList.insert( make_pair( "Inactivation\tTime constant\tShift\nTh0", &Th0 )); 
	DList.insert( make_pair( "Inactivation\tTime constant\tMagitude\nTh", &Th )); 
	DList.insert( make_pair( "Inactivation\tTime constant\tSlope\nSlpth", &Slpth ));
	DList.insert( make_pair( "Inactivation\tTime constant\tHalf-voltage\nHvth", &Hvth ));

	DList.insert( make_pair( "Activation\tInit\nM", &M ));
	DList.insert( make_pair( "Activation\tMagitude\nMk", &Km ));
	DList.insert( make_pair( "Activation\tSlope\nSlpm", &Slpm ));
	DList.insert( make_pair( "Activation\tHalf-voltage\nHvm", &Hvm ));
	DList.insert( make_pair( "Activation\tPower\nPowM", &PowM ));
	DList.insert( make_pair( "Activation\tTime constant\tShift\nTm0", &Tm0 )); 
	DList.insert( make_pair( "Activation\tTime constant\tMagitude\nTm", &Tm )); 
	DList.insert( make_pair( "Activation\tTime constant\tSlope\nSlptm", &Slptm ));
	DList.insert( make_pair( "Activation\tTime constant\tHalf-voltage\nHvtm", &Hvtm ));

	DList.insert( make_pair( "Ions dynamics rate\nKpump", &Kpump ));
}

tgen_channel &tgen_channel::operator = ( const tgen_channel &channel )
{
	t_channel::operator = ( channel );
	Eds = channel.Eds;
	ModI = channel.ModI;
	Km = channel.Km; Hvm = channel.Hvm; Slpm = channel.Slpm;
	Kh = channel.Kh; Hvh = channel.Hvh; Slph = channel.Slph;
	Tm0 = channel.Tm0; Hvtm = channel.Hvtm; Slptm = channel.Slptm; PowM = channel.PowM;
	Th0 = channel.Th0; Hvth = channel.Hvth; Slpth = channel.Slpth; PowH = channel.PowH;
	Kpump = channel.Kpump; 
	return *this;
}

size_t tgen_channel::get_iontype( void )
{
	if( Eds == "Na ions" ){
		return _id_Na_Ion;
	}
	else if( Eds == "Ca ions" ){
		return _id_Ca_Ion;
	}
	else if( Eds == "K ions" ){
		return 0xFF;
	}
	return 0xFF;
}

size_t tgen_channel::get_pumptype( void )
{
	if( Eds == "Na ions" ){
		return _id_NaK_Pump;
	}
	else if( Eds == "Ca ions" ){
		return _id_Ca_Pump;
	}
	else if( Eds == "K ions" ){
		return 0xFF;
	}
	return 0xFF;
}

void tgen_channel::copy_to( uni_template **unit, uni_template * parent )
{
	*unit = new tgen_channel( *this );
	( *unit )->set_parent( parent );
}

void tgen_channel::copy_to( hhn_process **unit, hhn_process *parent )
{
	int id = ::get_gchanid( Name.c_str() ); // new code 4 control
	if( Eds == "Na ions" ){
		*unit = new gen_channel(( hhn_compart *)parent, (( hhn_compart *)parent )->NaIons, ModI, id );
	}
	else if( Eds == "Ca ions" ){
		*unit = new gen_channel(( hhn_compart *)parent, (( hhn_compart *)parent )->CaIons, ModI, id );
	}
	else if( Eds == "K ions" ){
		*unit = new gen_channel(( hhn_compart *)parent, (( hhn_compart *)parent )->KIons, ModI, id );
	}
	else{
		double e = atof( Eds.c_str());
		*unit = new gen_channel(( hhn_compart *)parent, e, id );
	}
	(( hhn_channel *)( *unit ))->init_channel( Gmax, Tm, Th );
	(( hhn_channel *)( *unit ))->Name = Name;
	(( gen_channel *)*unit )->init_channel( M, Km, Hvm, Slpm, Tm0, Hvtm, Slptm, size_t( PowM.Value ), H, Kh, Hvh, Slph, Th0, Hvth, Slpth, size_t( PowH.Value ));
	(( gen_channel *)*unit )->Kpump = Kpump;
}

bool tgen_channel::pre_add( void )
{
	return ::add_gchan( Name.c_str() ); // new code 4 control
}

bool tgen_channel::pre_del( void )
{
	return ::del_gchan( Name.c_str() ); // new code 4 control
}

bool tgen_channel::process( const char *path )
{
	if( path ){
		string p_path = path;
		if( p_path == "Reversal potential (mV)" ){
			return true;
		}
		else if( p_path == "Modify ions dynamics" ){
			return true;
		}
	}
	return false;
}

bool tgen_channel::get_addparlist( vector<string> &parlist )
{
	parlist.push_back( "Name" );
	parlist.push_back( "Reversal potential (mV)" );
	parlist.push_back( "Modify ions dynamics" );
	return true;
}

bool tgen_channel::get_addpar( const char *path, string &name, string &element, int &type )
{
	if( path ){
		string p_path = path;
		if( p_path == "Reversal potential (mV)" ){
			name = "Data";
			element = Eds;
			int sz = 1;
			if( Eds != "Na ions" ){
				element += "\tNa ions";
				sz++;
			}
			if( Eds != "Ca ions" ){
				element += "\tCa ions";
				sz++;
			}
			if( Eds != "K ions" ){
				element += "\tK ions";
				sz++;
			}
			if( sz < 4 ){
				element += "\t0.0";
			}
			type = GRID_COMBO;
			return true;
		}
		else if( p_path == "Name" ){
			name = "Data";
			element = Name;
			type = GRID_STRING;
			return true;
		}
		else if( p_path == "Modify ions dynamics" ){
			name = "Data";
			element = ( ModI? "true": "false" );
			type = GRID_BOOL;
			return true;
		}
	}
	return false;
}

bool tgen_channel::upd_addpar( const char *path, const char *name, string &element, bool save )
{
	if( path && name ){
		string p_path = path;
		string p_name = name;
		if( p_path == "Reversal potential (mV)" && p_name == "Data" ){
			if( save ){
				Eds = element;
			}
			else{
				element = Eds;
			}
			return true;
		}
		else if( p_path == "Name" && p_name == "Data" ){
			if( save ){
				::rename_gchan( Name.c_str(), element.c_str() ); // new code 4 control
				Name = element;
			}
			else{
				element = Name;
			}
			return true;
		}
		else if( p_path == "Modify ions dynamics" && p_name == "Data" ){
			if( save ){
				ModI = ( element == "true" )? true: false;
			}
			else{
				element = ( ModI? "true": "false" );
			}
			return true;
		}
	}
	return false;
}

int tgen_channel::get_datattr( const char *path )
{
	string p_path = path;
	if( p_path == "Reversal potential (mV)" )
		return GRID_COMBO|GRID_PROCESS;
	else if( p_path == "Name" )
		return GRID_STRING;
	else if( p_path == "Modify ions dynamics" ){
		return GRID_BOOL|GRID_PROCESS;
	}
	else if( p_path == "Ions dynamics rate" ){
		if( ModI && ( Eds == "Na ions" || Eds == "Ca ions" /*|| Eds == "K ions"*/ )){
			return GRID_DOUBLE;
		}
		return GRID_READONLY;
	}
	return t_channel::get_datattr( path );
}

bool tgen_channel::load_addpar( string str, istream &file )
{
	if( str == "Eds"){
		file >> str >> ws;
		getline( file, Eds );
		return true;
	}
	if( str == "ModI"){
		file >> str >> str;
		ModI = ( str == "true" )? true: false;
		return true;
	}
	return false;
}

void tgen_channel::save_addpar( ostream &file )
{
	file << "Eds = " << Eds << endl;
	file << "ModI = " << ( ModI? "true": "false" ) << endl;
}

/*****************************************************************
 *  Model of generic Na channel template for Hodgkin-Haxley type neuron  *
 *****************************************************************/
//--- constructor
tgna_channel::tgna_channel( uni_template *parent, int is_active ) 
	: t_channel( parent, is_active ),
	Hvm( -40.0 ), Slpm( -6.0 ), Tm0( 0.0 ), Hvtm( -40.0 ), Slptm( -12.0 ), Slptm_1( 12.0 ), PowM( 1.0 ),
	Hvh( -48.0 ), Slph(  6.0 ), Th0( 0.0 ), Hvth( -48.0 ), Slpth(  12.0 ), Slpth_1( -12.0 ), PowH( 1.0 )
{
	M = 0.02; H = 0.3; Gmax = 2.8; 
	Th = 1.;
#if defined __MECHANICS_2D__ || __MECHANICS_3D__
	Th = 0.33; // absent in merged nsm
#endif // __MECHANICS_2D__
	UnitId = _id_generic_NaChan; 
	Name = _ChannelNames[UnitId];
	DList.insert( make_pair( "Inactivation\tInit\nH\nHna", &H ));
	DList.insert( make_pair( "Inactivation\tSlope\nSlph\nKh", &Slph ));
	DList.insert( make_pair( "Inactivation\tHalf-voltage\nHvh\nV12h", &Hvh ));
	DList.insert( make_pair( "Inactivation\tPower\nPowH\nH_power", &PowH ));
	DList.insert( make_pair( "Inactivation\tTime constant\tMagnitude\nTh", &Th )); 
	DList.insert( make_pair( "Inactivation\tTime constant\tShift\nTh0\nShiftTh", &Th0 ));
	DList.insert( make_pair( "Inactivation\tTime constant\tSlope 1\nSlpth\nKth", &Slpth ));
	DList.insert( make_pair( "Inactivation\tTime constant\tSlope 2\nSlpth_1\nKth_1", &Slpth_1 ));
	DList.insert( make_pair( "Inactivation\tTime constant\tHalf-voltage\nHvth\nV12th", &Hvth ));

	DList.insert( make_pair( "Activation\tInit\nM\nMna", &M ));
	DList.insert( make_pair( "Activation\tHalf-voltage\nHvm\nV12m", &Hvm ));
	DList.insert( make_pair( "Activation\tSlope\nSlpm\nKm", &Slpm ));
	DList.insert( make_pair( "Activation\tPower\nPowM\nM_power", &PowM ));
	DList.insert( make_pair( "Activation\tTime constant\tShift\nTm0\nShiftTm", &Tm0 ));
	DList.insert( make_pair( "Activation\tTime constant\tMagnitude\nTm", &Tm )); 
	DList.insert( make_pair( "Activation\tTime constant\tSlope 1\nSlptm\nKtm", &Slptm ));
	DList.insert( make_pair( "Activation\tTime constant\tSlope 2\nSlptm_1\nKtm_1", &Slptm_1 ));
	DList.insert( make_pair( "Activation\tTime constant\tHalf-voltage\nHvtm\nV12tm", &Hvtm ));
}

tgna_channel::tgna_channel( const tgna_channel &channel )
	: t_channel( channel ),
	Hvm( channel.Hvm ), Slpm( channel.Slpm ), Tm0( channel.Tm0 ), Hvtm( channel.Hvtm ), 
	Slptm( channel.Slptm ), Slptm_1( channel.Slptm_1 ), PowM( channel.PowM ),
	Hvh( channel.Hvh ), Slph( channel.Slph ), Th0( channel.Th0 ), Hvth( channel.Hvth ), 
	Slpth( channel.Slpth ), Slpth_1( channel.Slpth_1 ), PowH( channel.PowH )
{
	if( double( Slptm_1 ) == 0. )
		Slptm_1 = -Slptm;
	if( double( Slpth_1 ) == 0. )
		Slpth_1 = -Slpth;
	DList.insert( make_pair( "Inactivation\tInit\nH\nHna", &H ));
	DList.insert( make_pair( "Inactivation\tSlope\nSlph\nKh", &Slph ));
	DList.insert( make_pair( "Inactivation\tHalf-voltage\nHvh\nV12h", &Hvh ));
	DList.insert( make_pair( "Inactivation\tPower\nPowH\nH_power", &PowH ));
	DList.insert( make_pair( "Inactivation\tTime constant\tMagnitude\nTh", &Th )); 
	DList.insert( make_pair( "Inactivation\tTime constant\tShift\nTh0\nShiftTh", &Th0 ));
	DList.insert( make_pair( "Inactivation\tTime constant\tSlope 1\nSlpth\nKth", &Slpth ));
	DList.insert( make_pair( "Inactivation\tTime constant\tSlope 2\nSlpth_1\nKth_1", &Slpth_1 ));
	DList.insert( make_pair( "Inactivation\tTime constant\tHalf-voltage\nHvth\nV12th", &Hvth ));

	DList.insert( make_pair( "Activation\tInit\nM\nMna", &M ));
	DList.insert( make_pair( "Activation\tHalf-voltage\nHvm\nV12m", &Hvm ));
	DList.insert( make_pair( "Activation\tSlope\nSlpm\nKm", &Slpm ));
	DList.insert( make_pair( "Activation\tPower\nPowM\nM_power", &PowM ));
	DList.insert( make_pair( "Activation\tTime constant\tShift\nTm0\nShiftTm", &Tm0 ));
	DList.insert( make_pair( "Activation\tTime constant\tMagnitude\nTm", &Tm )); 
	DList.insert( make_pair( "Activation\tTime constant\tSlope 1\nSlptm\nKtm", &Slptm ));
	DList.insert( make_pair( "Activation\tTime constant\tSlope 2\nSlptm_1\nKtm_1", &Slptm_1 ));
	DList.insert( make_pair( "Activation\tTime constant\tHalf-voltage\nHvtm\nV12tm", &Hvtm ));
}

tgna_channel &tgna_channel::operator = ( const tgna_channel &channel )
{
	t_channel::operator = ( channel );
	Hvm = channel.Hvm;
	Slpm = channel.Slpm;
	PowM = channel.PowM;
	Tm0 = channel.Tm0;
	Hvtm = channel.Hvtm;
	Slptm = channel.Slptm;
	Slptm_1 = channel.Slptm_1;

	Hvh = channel.Hvh;
	Slph = channel.Slph;
	PowH = channel.PowH;
	Th0 = channel.Th0;
	Hvth = channel.Hvth;
	Slpth = channel.Slpth;
	Slpth_1 = channel.Slpth_1;

	if( double( Slptm_1 ) == 0. )
		Slptm_1 = -Slptm;
	if( double( Slpth_1 ) == 0. )
		Slpth_1 = -Slpth;
	return *this;
}

//--- overrided function
void tgna_channel::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new tgna_channel( *this );
	( *unit )->set_parent( parent );
}

void tgna_channel::copy_to( hhn_process **unit, hhn_process *parent )
{
	*unit = new hgna_channel(( hhn_compart *)parent );
	(( hhn_channel *)( *unit ))->init_channel( Gmax, Tm, Th );
	(( hhn_channel *)( *unit ))->Name = Name;
	(( hgna_channel *)*unit )->init_channel( M, Hvm, Slpm, Hvtm, Slptm, size_t( PowM.Value ),
								H, Hvh, Slph, Hvth, Slpth, size_t( PowH.Value ),
								Tm0, Slptm_1, Th0, Slpth_1 );

}

//--- constructor (generic)
tgnap_channel::tgnap_channel( uni_template *parent, int is_active ) 
                       : tgna_channel( parent, is_active )
{
	Gmax = 2.8; 
	Tm = 1.; Th = 10000.;
	UnitId = _id_generic_NaPChan; 
	Name = _ChannelNames[UnitId];

	DList.clear();
	DList.insert( make_pair( "Maximal conductance (mS/sm\xB2)\nGmax", &Gmax ));
	DList.insert( make_pair( "Inactivation\tInit\nH\nHna", &H ));
	DList.insert( make_pair( "Inactivation\tSlope\nSlph\nKh", &Slph ));
	DList.insert( make_pair( "Inactivation\tHalf-voltage\nHvh\nV12h", &Hvh ));
	DList.insert( make_pair( "Inactivation\tPower\nPowH\nH_power", &PowH ));
	DList.insert( make_pair( "Inactivation\tTime constant\tMagnitude\nTh", &Th )); 
	DList.insert( make_pair( "Inactivation\tTime constant\tSlope\nSlpth\nKth", &Slpth ));
	DList.insert( make_pair( "Inactivation\tTime constant\tHalf-voltage\nHvth\nV12th", &Hvth ));

	DList.insert( make_pair( "Activation\tInit\nM\nMna", &M ));
	DList.insert( make_pair( "Activation\tHalf-voltage\nHvm\nV12m", &Hvm ));
	DList.insert( make_pair( "Activation\tSlope\nSlpm\nKm", &Slpm ));
	DList.insert( make_pair( "Activation\tPower\nPowM\nM_power", &PowM ));
	DList.insert( make_pair( "Activation\tTime constant\tMagnitude\nTm", &Tm )); 
	DList.insert( make_pair( "Activation\tTime constant\tSlope\nSlptm\nKtm", &Slptm ));
	DList.insert( make_pair( "Activation\tTime constant\tHalf-voltage\nHvtm\nV12tm", &Hvtm ));
}

tgnap_channel::tgnap_channel( const tgnap_channel &channel )
	: tgna_channel( channel )
{
	DList.clear();
	DList.insert( make_pair( "Maximal conductance (mS/sm\xB2)\nGmax", &Gmax ));
	DList.insert( make_pair( "Inactivation\tInit\nH\nHna", &H ));
	DList.insert( make_pair( "Inactivation\tSlope\nSlph\nKh", &Slph ));
	DList.insert( make_pair( "Inactivation\tHalf-voltage\nHvh\nV12h", &Hvh ));
	DList.insert( make_pair( "Inactivation\tPower\nPowH\nH_power", &PowH ));
	DList.insert( make_pair( "Inactivation\tTime constant\tMagnitude\nTh", &Th )); 
	DList.insert( make_pair( "Inactivation\tTime constant\tSlope\nSlpth\nKth", &Slpth ));
	DList.insert( make_pair( "Inactivation\tTime constant\tHalf-voltage\nHvth\nV12th", &Hvth ));

	DList.insert( make_pair( "Activation\tInit\nM\nMna", &M ));
	DList.insert( make_pair( "Activation\tHalf-voltage\nHvm\nV12m", &Hvm ));
	DList.insert( make_pair( "Activation\tSlope\nSlpm\nKm", &Slpm ));
	DList.insert( make_pair( "Activation\tPower\nPowM\nM_power", &PowM ));
	DList.insert( make_pair( "Activation\tTime constant\tMagnitude\nTm", &Tm )); 
	DList.insert( make_pair( "Activation\tTime constant\tSlope\nSlptm\nKtm", &Slptm ));
	DList.insert( make_pair( "Activation\tTime constant\tHalf-voltage\nHvtm\nV12tm", &Hvtm ));
}

tgnap_channel &tgnap_channel::operator = ( const tgnap_channel &channel )
{
	tgna_channel::operator = ( channel );
	return *this;
}

//--- overrided function
void tgnap_channel::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new tgnap_channel( *this );
	( *unit )->set_parent( parent );
}

void tgnap_channel::copy_to( hhn_process **unit, hhn_process *parent )
{
	*unit = new hgnap_channel(( hhn_compart *)parent );
	(( hhn_channel *)( *unit ))->init_channel( Gmax, Tm, Th );
	(( hhn_channel *)( *unit ))->Name = Name;
	(( hgnap_channel *)*unit )->init_channel( M, Hvm, Slpm, Hvtm, Slptm, size_t( PowM.Value ), 
								H, Hvh, Slph, Hvth, Slpth, size_t( PowH.Value ));
}

/*****************************************************************
 *  Model of generic Kdr channel template for Hodgkin-Haxley type neuron  *
 *****************************************************************/
//--- constructor
tgkdr_channel::tgkdr_channel( uni_template *parent, int is_active ) 
	: t_channel( parent, is_active ),
	Hvm( -28.0 ), Slpm( -4.0 ), PowM( 4.0 ),
	Tm0( 0.0 ), Hvtm( -28.0 ), Slptm( -8.0 ), Slptm_1( 8.0 )
{
	M = 0.03; Gmax = 11.2; 
	Th = 1.;
#if defined __MECHANICS_2D__ || __MECHANICS_3D__
	Th = 0.33;  // absent in merged nsm
#endif // __MECHANICS_2D__
	UnitId = _id_generic_KdrChan; 
	Name = _ChannelNames[UnitId];
	DList.insert( make_pair( "Activation\tInit\nM\nMkdr", &M ));
	DList.insert( make_pair( "Activation\tHalf-voltage\nHvm\nV12m", &Hvm ));
	DList.insert( make_pair( "Activation\tSlope\nSlpm\nKm", &Slpm ));
	DList.insert( make_pair( "Activation\tPower\nPowM\nM_power", &PowM ));
	DList.insert( make_pair( "Activation\tTime constant\tShift\nTm0\nShiftTm", &Tm0 ));
	DList.insert( make_pair( "Activation\tTime constant\tMagnitude\nTm", &Tm ));
	DList.insert( make_pair( "Activation\tTime constant\tHalf-voltage\nHvtm\nV12tm", &Hvtm ));
	DList.insert( make_pair( "Activation\tTime constant\tSlope 1\nSlptm\nKtm", &Slptm ));
	DList.insert( make_pair( "Activation\tTime constant\tSlope 2\nSlptm_1\nKtm_1", &Slptm_1 ));
}

tgkdr_channel::tgkdr_channel( const tgkdr_channel &channel )
	: t_channel( channel ),
	Hvm( channel.Hvm ), Slpm( channel.Slpm ), PowM( channel.PowM ),
	Tm0( channel.Tm0 ), Hvtm( channel.Hvtm ), Slptm( channel.Slptm ), Slptm_1( channel.Slptm_1 )
{
	if( double( Slptm_1 ) == 0. )
		Slptm_1 = -Slptm;
	DList.insert( make_pair( "Activation\tInit\nM\nMkdr", &M ));
	DList.insert( make_pair( "Activation\tHalf-voltage\nHvm\nV12m", &Hvm ));
	DList.insert( make_pair( "Activation\tSlope\nSlpm\nKm", &Slpm ));
	DList.insert( make_pair( "Activation\tPower\nPowM\nM_power", &PowM ));
	DList.insert( make_pair( "Activation\tTime constant\tShift\nTm0\nShiftTm", &Tm0 ));
	DList.insert( make_pair( "Activation\tTime constant\tMagnitude\nTm", &Tm ));
	DList.insert( make_pair( "Activation\tTime constant\tHalf-voltage\nHvtm\nV12tm", &Hvtm ));
	DList.insert( make_pair( "Activation\tTime constant\tSlope 1\nSlptm\nKtm", &Slptm ));
	DList.insert( make_pair( "Activation\tTime constant\tSlope 2\nSlptm_1\nKtm_1", &Slptm_1 ));
}

tgkdr_channel &tgkdr_channel::operator = ( const tgkdr_channel &channel )
{
	t_channel::operator = ( channel );
	Hvm = channel.Hvm;
	Slpm = channel.Slpm;
	PowM = channel.PowM;
	Tm0 = channel.Tm0;
	Hvtm = channel.Hvtm;
	Slptm = channel.Slptm;
	Slptm_1 = channel.Slptm_1;
	if( double( Slptm_1 ) == 0. )
		Slptm_1 = -Slptm;
	return *this;
}

//--- overrided function
void tgkdr_channel::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new tgkdr_channel( *this );
	( *unit )->set_parent( parent );
}

void tgkdr_channel::copy_to( hhn_process **unit, hhn_process *parent )
{
	*unit = new hgkdr_channel(( hhn_compart *)parent );
	(( hhn_channel *)( *unit ))->init_channel( Gmax, Tm, Th );
	(( hhn_channel *)( *unit ))->Name = Name;
	(( hgkdr_channel *)*unit )-> init_channel( M, Hvm, Slpm, Hvtm, Slptm, size_t( PowM.Value ), Tm0, Slptm_1 );
}

/*****************************************************************
 *  Model of generic KA channel template for Hodgkin-Haxley type neuron  *
 *****************************************************************/
//--- constructor
tgka_channel::tgka_channel( uni_template *parent, int is_active ) 
	: t_channel( parent, is_active ),
	Hvm( -40.0 ), Slpm( -6.0 ), Tm0( 0.0 ), Hvtm( -40.0 ), Slptm( -12.0 ), Slptm_1( 12.0 ), PowM( 1 ),
	Hvh( -48.0 ), Slph(  6.0 ), Th0( 0.0 ), Hvth( -48.0 ), Slpth(  12.0 ), Slpth_1( -12.0 ), PowH( 1.0 )
{
	Gmax = 2.8;  M = 0.02; H = 0.3;
	UnitId = _id_generic_KAChan; 
	Name = _ChannelNames[UnitId];
	DList.insert( make_pair( "Inactivation\tInit\nH\nHka", &H ));
	DList.insert( make_pair( "Inactivation\tHalf-voltage\nHvh\nV12h", &Hvh ));
	DList.insert( make_pair( "Inactivation\tSlope\nSlph\nKh", &Slph ));
	DList.insert( make_pair( "Inactivation\tPower\nPowH\nH_power", &PowH ));
	DList.insert( make_pair( "Inactivation\tTime constant\tMagnitude\nTh", &Th )); 
	DList.insert( make_pair( "Inactivation\tTime constant\tShift\nTh0\nShiftTh", &Th0 ));
	DList.insert( make_pair( "Inactivation\tTime constant\tSlope 1\nSlpth\nKth", &Slpth ));
	DList.insert( make_pair( "Inactivation\tTime constant\tSlope 2\nSlpth_1\nKth_1", &Slpth_1 ));
	DList.insert( make_pair( "Inactivation\tTime constant\tHalf-voltage\nHvth\nV12th", &Hvth ));

	DList.insert( make_pair( "Activation\tInit\nM\nMka", &M ));
	DList.insert( make_pair( "Activation\tHalf-voltage\nHvm\nV12m", &Hvm ));
	DList.insert( make_pair( "Activation\tSlope\nSlpm\nKm", &Slpm ));
	DList.insert( make_pair( "Activation\tPower\nPowM\nM_power", &PowM ));
	DList.insert( make_pair( "Activation\tTime constant\tShift\nTm0\nShiftTm", &Tm0 ));
	DList.insert( make_pair( "Activation\tTime constant\tMagnitude\nTm", &Tm )); 
	DList.insert( make_pair( "Activation\tTime constant\tSlope 1\nSlptm\nKtm", &Slptm ));
	DList.insert( make_pair( "Activation\tTime constant\tSlope 2\nSlptm_1\nKtm_1", &Slptm_1 ));
	DList.insert( make_pair( "Activation\tTime constant\tHalf-voltage\nHvtm\nV12tm", &Hvtm ));
}

tgka_channel::tgka_channel( const tgka_channel &channel )
	: t_channel( channel ),
	Hvm( channel.Hvm ), Slpm( channel.Slpm ), Tm0( channel.Tm0 ), Hvtm( channel.Hvtm ), 
	Slptm( channel.Slptm ), Slptm_1( channel.Slptm_1 ), PowM( channel.PowM ),
	Hvh( channel.Hvh ), Slph( channel.Slph ), Th0( channel.Th0 ), Hvth( channel.Hvth ), 
	Slpth( channel.Slpth ), Slpth_1( channel.Slpth_1 ), PowH( channel.PowH )
{
	if( double( Slptm_1 ) == 0. )
		Slptm_1 = -Slptm;
	if( double( Slpth_1 ) == 0. )
		Slpth_1 = -Slpth;
	DList.insert( make_pair( "Inactivation\tInit\nH\nHka", &H ));
	DList.insert( make_pair( "Inactivation\tHalf-voltage\nHvh\nV12h", &Hvh ));
	DList.insert( make_pair( "Inactivation\tSlope\nSlph\nKh", &Slph ));
	DList.insert( make_pair( "Inactivation\tPower\nPowH\nH_power", &PowH ));
	DList.insert( make_pair( "Inactivation\tTime constant\tMagnitude\nTh", &Th )); 
	DList.insert( make_pair( "Inactivation\tTime constant\tShift\nTh0\nShiftTh", &Th0 ));
	DList.insert( make_pair( "Inactivation\tTime constant\tSlope 1\nSlpth\nKth", &Slpth ));
	DList.insert( make_pair( "Inactivation\tTime constant\tSlope 2\nSlpth_1\nKth_1", &Slpth_1 ));
	DList.insert( make_pair( "Inactivation\tTime constant\tHalf-voltage\nHvth\nV12th", &Hvth ));

	DList.insert( make_pair( "Activation\tInit\nM\nMka", &M ));
	DList.insert( make_pair( "Activation\tHalf-voltage\nHvm\nV12m", &Hvm ));
	DList.insert( make_pair( "Activation\tSlope\nSlpm\nKm", &Slpm ));
	DList.insert( make_pair( "Activation\tPower\nPowM\nM_power", &PowM ));
	DList.insert( make_pair( "Activation\tTime constant\tShift\nTm0\nShiftTm", &Tm0 ));
	DList.insert( make_pair( "Activation\tTime constant\tMagnitude\nTm", &Tm )); 
	DList.insert( make_pair( "Activation\tTime constant\tSlope 1\nSlptm\nKtm", &Slptm ));
	DList.insert( make_pair( "Activation\tTime constant\tSlope 2\nSlptm_1\nKtm_1", &Slptm_1 ));
	DList.insert( make_pair( "Activation\tTime constant\tHalf-voltage\nHvtm\nV12tm", &Hvtm ));
}

tgka_channel &tgka_channel::operator = ( const tgka_channel &channel )
{
	t_channel::operator = ( channel );

	Hvm = channel.Hvm;
	Slpm = channel.Slpm;
	PowM = channel.PowM;
	Tm0 = channel.Tm0;
	Hvtm = channel.Hvtm;
	Slptm = channel.Slptm;
	Slptm_1 = channel.Slptm_1;

	Hvh = channel.Hvh;
	Slph = channel.Slph;
	PowH = channel.PowH;
	Th0 = channel.Th0;
	Hvth = channel.Hvth;
	Slpth = channel.Slpth;
	Slpth_1 = channel.Slpth_1;

	if( double( Slptm_1 ) == 0. )
		Slptm_1 = -Slptm;
	if( double( Slpth_1 ) == 0. )
		Slpth_1 = -Slpth;
	return *this;
}

//--- overrided function
void tgka_channel::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new tgka_channel( *this );
	( *unit )->set_parent( parent );
}

void tgka_channel::copy_to( hhn_process **unit, hhn_process *parent )
{
	*unit = new hgka_channel(( hhn_compart *)parent );
	(( hhn_channel *)( *unit ))->init_channel( Gmax, Tm, Th );
	(( hhn_channel *)( *unit ))->Name = Name;
	(( hgka_channel *)*unit )->init_channel( M, Hvm, Slpm, Hvtm, Slptm, size_t( PowM.Value ), 
								H, Hvh, Slph, Hvth, Slpth, size_t( PowH.Value ),
								Tm0, Slptm_1, Th0, Slpth_1 );
}

/*****************************************************************
 * Model of generic K(Ca) channel template for Hodgkin-Haxley type neuron  *
 *****************************************************************/
//--- constructor
tgkca_channel::tgkca_channel( uni_template *parent, int is_active ) 
	: t_channel( parent, is_active ),
	Alpha( 0.5 ), Lymbda( 2.0 ), Pow( 2.0 )
{
	Gmax = 7.0; Tm = 2.0;
	UnitId = _id_generic_KCa;
	Name = _ChannelNames[UnitId];
	DList.insert( make_pair( "Activation\tInit\nM\nMkca", &M ));
	DList.insert( make_pair( "Activation\tAlpha\nAlpha", &Alpha ));
	DList.insert( make_pair( "Activation\tLymbda\nL", &Lymbda ));
	DList.insert( make_pair( "Activation\tPower\nPowM\nPow", &Pow ));
	DList.insert( make_pair( "Activation\tTime constant\tMagnitude\nTm\nTkca", &Tm )); 
}

tgkca_channel::tgkca_channel( const tgkca_channel &channel )
	: t_channel( channel ),
	Alpha( channel.Alpha ), Lymbda( channel.Lymbda ), Pow( channel.Pow )
{
	DList.insert( make_pair( "Activation\tInit\nM\nMkca", &M ));
	DList.insert( make_pair( "Activation\tAlpha\nAlpha", &Alpha ));
	DList.insert( make_pair( "Activation\tLymbda\nL", &Lymbda ));
	DList.insert( make_pair( "Activation\tPower\nPowM\nPow", &Pow ));
	DList.insert( make_pair( "Activation\tTime constant\tMagnitude\nTm\nTkca", &Tm )); 
}

tgkca_channel &tgkca_channel::operator = ( const tgkca_channel &channel )
{
	t_channel::operator = ( channel );
	Alpha = channel.Alpha;
	Lymbda = channel.Lymbda;
	Pow = channel.Pow;
	return *this;
}

//--- overrided function
void tgkca_channel::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new tgkca_channel( *this );
	( *unit )->set_parent( parent );
}

void tgkca_channel::copy_to( hhn_process **unit, hhn_process *parent )
{
	*unit = new hgkca_channel(( hhn_compart *)parent );
	(( hhn_channel *)( *unit ))->init_channel( Gmax, Tm, 1 );
	(( hhn_channel *)( *unit ))->Name = Name;
	(( hgkca_channel *)*unit )->init_channel( M, Alpha, Lymbda, size_t( Pow.Value ));
}

/*****************************************************************
 * Model of generic Ca activated cation nonspecific channel template
 * for Hodgkin-Haxley type neuron                                *
 *****************************************************************/
//--- constructor
tgcan_channel::tgcan_channel( uni_template *parent, int is_active ) 
	: tgkca_channel( parent, is_active ),
	Ecan( -30.0 )
{
	UnitId = _id_generic_Can;
	Name = _ChannelNames[UnitId];
	DList.insert( make_pair( "Reversal potential (mV)\nEds\nEcan", &Ecan ));
}

tgcan_channel::tgcan_channel( const tgcan_channel &channel )
	: tgkca_channel( channel ),
	Ecan( channel.Ecan )
{
	DList.insert( make_pair( "Reversal potential (mV)\nEds\nEcan", &Ecan ));
}

tgcan_channel &tgcan_channel::operator = ( const tgcan_channel &channel )
{
	tgkca_channel::operator = ( channel );
	Ecan = channel.Ecan;
	return *this;
}

//--- overrided function
void tgcan_channel::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new tgcan_channel( *this );
	( *unit )->set_parent( parent );
}

void tgcan_channel::copy_to( hhn_process **unit, hhn_process *parent )
{
	*unit = new hgcan_channel(( hhn_compart *)parent );
	(( hhn_channel *)( *unit ))->init_channel( Gmax, Tm, 1 );
	(( hhn_channel *)( *unit ))->Name = Name;
	(( hgcan_channel *)*unit )->init_channel( M, Alpha, Lymbda, size_t( Pow.Value ));
	(( hgcan_channel *)*unit )->Egcan = Ecan;
}

/*****************************************************************
 *  Model of H generic channel template for Hodgkin-Haxley type neuron   *
 *****************************************************************/
//--- constructor
tgh_channel::tgh_channel( uni_template *parent, int is_active ) 
	: t_channel( parent, is_active ),
	Eh( -43.0 ), Hvh( -75.0 ),
	Slph( 5.5 ), Hvth( -80.0 ), Slpth( 13. )
{
	Gmax = 1; Th = 1.;
	UnitId = _id_generic_HChan;
	Name = _ChannelNames[UnitId];
	DList.insert( make_pair( "Inactivation\tInit\nH\nHh", &H ));
	DList.insert( make_pair( "Inactivation\tSlope\nSlph\nKh", &Slph ));
	DList.insert( make_pair( "Inactivation\tHalf-voltage\nHvh\nV12h", &Hvh ));
	DList.insert( make_pair( "Inactivation\tTime constant\tMagnitude\nTh", &Th ));
	DList.insert( make_pair( "Inactivation\tTime constant\tSlope\nSlpth\nKth", &Slpth ));
	DList.insert( make_pair( "Inactivation\tTime constant\tHalf-voltage\nHvth\nV12tm", &Hvth ));
	DList.insert( make_pair( "Reversal potential (mV)\nEds\nEh", &Eh ));
}

tgh_channel::tgh_channel( const tgh_channel &channel )
	: t_channel( channel ),
	Eh( channel.Eh ), Hvh( channel.Hvh ),
	Slph( channel.Slph ), Hvth( channel.Hvth ), Slpth( channel.Slpth )
{
	DList.insert( make_pair( "Inactivation\tInit\nH\nHh", &H ));
	DList.insert( make_pair( "Inactivation\tSlope\nSlph\nKh", &Slph ));
	DList.insert( make_pair( "Inactivation\tHalf-voltage\nHvh\nV12h", &Hvh ));
	DList.insert( make_pair( "Inactivation\tTime constant\tMagnitude\nTh", &Th ));
	DList.insert( make_pair( "Inactivation\tTime constant\tSlope\nSlpth\nKth", &Slpth ));
	DList.insert( make_pair( "Inactivation\tTime constant\tHalf-voltage\nHvth\nV12tm", &Hvth ));
	DList.insert( make_pair( "Reversal potential (mV)\nEds\nEh", &Eh ));
}

tgh_channel &tgh_channel::operator = ( const tgh_channel &channel )
{
	t_channel::operator = ( channel );
	Eh = channel.Eh;
	Hvh = channel.Hvh;
	Slph = channel.Slph;
	Hvth = channel.Hvth;
	Slpth = channel.Slpth;
	return *this;
}

//--- overrided function
void tgh_channel::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new tgh_channel( *this );
	( *unit )->set_parent( parent );
}

void tgh_channel::copy_to( hhn_process **unit, hhn_process *parent )
{
	*unit = new hgh_channel(( hhn_compart *)parent );
	(( hhn_channel *)( *unit ))->init_channel( Gmax, Tm, Th );
	(( hhn_channel *)( *unit ))->Name = Name;
	(( hgh_channel *)*unit )-> init_channel( Eh, H, Hvh, Slph, Hvth, Slpth );
}

#if defined( __LOCOMOTION__ ) || defined( __RESPIRATION__ )\
/*****************************************************************
 *  Model of Na channel template for Hodgkin-Haxley type neuron  *
 *****************************************************************/
//--- constructor
tna_channel::tna_channel( uni_template *parent, int is_active ) 
	: t_channel( parent, is_active )
{
	M = 0.02; H = 0.3;
#ifdef __LOCOMOTION__
	Gmax = 120.; 
	UnitId = _id_NaChan; 
	Name = _ChannelNames[UnitId];
#endif // __LOCOMOTION__
	Th = 1.;
#if defined __MECHANICS_2D__ || __MECHANICS_3D__
	Th = 0.33;  // absent in merged nsm
#endif // __MECHANICS_2D__
	DList.insert( make_pair( "Activation\tInit\nM\nMna", &M ));
	DList.insert( make_pair( "Inactivation\tInit\nH\nHna", &H ));
}

tna_channel::tna_channel( const tna_channel &channel )
	: t_channel( channel )
{
	DList.insert( make_pair( "Activation\tInit\nM\nMna", &M ));
	DList.insert( make_pair( "Inactivation\tInit\nH\nHna", &H ));
}

//--- overrided function
void tna_channel::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new tna_channel( *this );
	( *unit )->set_parent( parent );
}

void tna_channel::copy_to( hhn_process **unit, hhn_process *parent )
{
	*unit = new hna_channel(( hhn_compart *)parent );
	(( hhn_channel *)( *unit ))->init_channel( Gmax, Tm, Th );
	(( hhn_channel *)( *unit ))->Name = Name;
	(( hna_channel *)*unit )->init_channel( M, H );
}

/*****************************************************************
 *  Model of K channel template for Hodgkin-Haxley type neuron   *
 *****************************************************************/
//--- constructor
tk_channel::tk_channel( uni_template *parent, int is_active ) 
	: t_channel( parent, is_active )
{
	M = 0.03;
#ifdef __LOCOMOTION__
	Gmax = 100.; 
#elif !defined( __LOCOMOTION__ )
	Gmax = 90; 
#endif // __LOCOMOTION__
	Tm = 1.;
#if defined __MECHANICS_2D__ || __MECHANICS_3D__
	Tm = 0.33;  // absent in merged nsm
#endif // __MECHANICS_2D__
	UnitId = _id_KChan; 
	Name = _ChannelNames[UnitId];
	DList.insert( make_pair( "Activation\tInit\nM\nMk", &M ));
}

tk_channel::tk_channel( const tk_channel &channel )
	: t_channel( channel )
{
	DList.insert( make_pair( "Activation\tInit\nM\nMk", &M ));
}

//--- overrided function
void tk_channel::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new tk_channel( *this );
	( *unit )->set_parent( parent );
}

void tk_channel::copy_to( hhn_process **unit, hhn_process *parent )
{
	*unit = new hk_channel(( hhn_compart *)parent );
	(( hhn_channel *)( *unit ))->init_channel( Gmax, Tm, Th );
	(( hhn_channel *)( *unit ))->Name = Name;
	(( hk_channel *)*unit )->init_channel( M );
}

//--- constructor (butera)
tnaf_channel::tnaf_channel( uni_template *parent, int is_active ) 
	: tna_channel( parent, is_active )
{
	Gmax = 150; Tm = 0.28; Th = 0.28; M = 0.02; H = 0.3;
	UnitId = _id_NaFChan; 
	Name = _ChannelNames[UnitId];
	DList.insert( make_pair( "Inactivation\tTime constant\tMagnitude\nTh", &Th )); 
	DList.insert( make_pair( "Activation\tTime constant\tMagnitude\nTm", &Tm )); 
}

tnaf_channel::tnaf_channel( const tnaf_channel &channel )
	: tna_channel( channel )
{
	DList.insert( make_pair( "Inactivation\tTime constant\tMagnitude\nTh", &Th )); 
	DList.insert( make_pair( "Activation\tTime constant\tMagnitude\nTm", &Tm )); 
}

//--- overrided function
void tnaf_channel::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new tnaf_channel( *this );
	( *unit )->set_parent( parent );
}

void tnaf_channel::copy_to( hhn_process **unit, hhn_process *parent )
{
	*unit = new hnaf_channel(( hhn_compart *)parent );
	(( hhn_channel *)( *unit ))->init_channel( Gmax, Tm, Th );
	(( hhn_channel *)( *unit ))->Name = Name;
	(( hnaf_channel *)*unit )->init_channel( M, H );
}

//--- constructor (butera)
tnap_channel::tnap_channel( uni_template *parent, int is_active ) 
	: tna_channel( parent, is_active )
{
	Gmax = 8; Tm = 0.28; Th = 3000.; M = 0.02; H = 0.3;
	UnitId = _id_NaPChan; 
	Name = _ChannelNames[UnitId];
	DList.clear();
	DList.insert( make_pair( "Maximal conductance (nS)\nGmax", &Gmax ));
	DList.insert( make_pair( "Inactivation\tInit\nH\nHnap", &H )); 
	DList.insert( make_pair( "Inactivation\tTime constant\tMagnitude\nTh", &Th )); 
	DList.insert( make_pair( "Activation\tInit\nM\nMnap", &M ));
	DList.insert( make_pair( "Activation\tTime constant\tMagnitude\nTm", &Tm )); 
}

tnap_channel::tnap_channel( const tnap_channel &channel )
	: tna_channel( channel )
{
	DList.clear();
	DList.insert( make_pair( "Maximal conductance (nS)\nGmax", &Gmax ));
	DList.insert( make_pair( "Inactivation\tInit\nH\nHnap", &H )); 
	DList.insert( make_pair( "Inactivation\tTime constant\tMagnitude\nTh", &Th )); 
	DList.insert( make_pair( "Activation\tInit\nM\nMnap", &M ));
	DList.insert( make_pair( "Activation\tTime constant\tMagnitude\nTm", &Tm )); 
}

//--- overrided function
void tnap_channel::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new tnap_channel( *this );
	( *unit )->set_parent( parent );
}

void tnap_channel::copy_to( hhn_process **unit, hhn_process *parent )
{
	*unit = new hnap_channel(( hhn_compart *)parent );
	(( hhn_channel *)( *unit ))->init_channel( Gmax, Tm, Th );
	(( hhn_channel *)( *unit ))->Name = Name;
	(( hnap_channel *)*unit )->init_channel( M, H );
}

//--- constructor (butera)
tkdr_channel::tkdr_channel( uni_template *parent, int is_active ) 
	: tk_channel( parent, is_active )
{
	Gmax = 90; Tm = 4.0;
	M = 0.03;
	UnitId = _id_KdrChan;
	Name = _ChannelNames[UnitId];
}

tkdr_channel::tkdr_channel( const tkdr_channel &channel )
	: tk_channel( channel )
{
}

//--- overrided function
void tkdr_channel::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new tkdr_channel( *this );
	( *unit )->set_parent( parent );
}

void tkdr_channel::copy_to( hhn_process **unit, hhn_process *parent )
{
	*unit = new hkdr_channel(( hhn_compart *)parent );
	(( hhn_channel *)( *unit ))->init_channel( Gmax, Tm, Th );
	(( hhn_channel *)( *unit ))->Name = Name;
	(( hkdr_channel *)*unit )->init_channel( M );
}

/*****************************************************************
 *  Model of KA channel template for Hodgkin-Haxley type neuron  *
 *****************************************************************/
//--- constructor
tka_channel::tka_channel( uni_template *parent, int is_active ) 
	: t_channel( parent, is_active ),
	Mka1( 0.47 ), Hka1( 0.05 )
{
	M = 0.25; H = 0.05;
#ifdef __LOCOMOTION__
	Gmax = 0.07; Th = 1.;
	UnitId = _id_KAChan;
	Name = _ChannelNames[UnitId];
#endif // __LOCOMOTION__
	DList.insert( make_pair( "Inactivation\tInit 2\nH1\nHka1", &Hka1 ));
	DList.insert( make_pair( "Inactivation\tInit 1\nH\nHka2", &H ));
	DList.insert( make_pair( "Inactivation\tTime constant\tMagnitude 2\nTh1\nTh2_0", &Th ));
	DList.insert( make_pair( "Activation\tInit 2\nM1\nMka1", &Mka1 ));
	DList.insert( make_pair( "Activation\tInit 1\nM\nMka2", &M ));
}

tka_channel::tka_channel( const tka_channel &channel )
	: t_channel( channel ),
	Mka1( channel.Mka1 ), Hka1( channel.Hka1 )
{
	DList.insert( make_pair( "Inactivation\tInit 2\nH1\nHka1", &Hka1 ));
	DList.insert( make_pair( "Inactivation\tInit 1\nH\nHka2", &H ));
	DList.insert( make_pair( "Inactivation\tTime constant\tMagnitude 2\nTh1\nTh2_0", &Th ));
	DList.insert( make_pair( "Activation\tInit 2\nM1\nMka1", &Mka1 ));
	DList.insert( make_pair( "Activation\tInit 1\nM\nMka2", &M ));
}

tka_channel &tka_channel::operator = ( const tka_channel &channel )
{
	t_channel::operator = ( channel );
	Mka1 = channel.Mka1;
	Hka1 = channel.Hka1;
	return *this;
}

//--- overrided function
void tka_channel::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new tka_channel( *this );
	( *unit )->set_parent( parent );
}

void tka_channel::copy_to( hhn_process **unit, hhn_process *parent )
{
	*unit = new hka_channel(( hhn_compart *)parent );
	(( hhn_channel *)( *unit ))->init_channel( Gmax, Tm, Th );
	(( hhn_channel *)( *unit ))->Name = Name;
	(( hka_channel *)*unit )->init_channel( Mka1, Hka1, M, H );
}

/*****************************************************************
 * Model of CaL channel template for Hodgkin-Haxley type neuron  *
 *****************************************************************/
//--- constructor
tcal_channel::tcal_channel( uni_template *parent, int is_active ) 
	: t_channel( parent, is_active )
{
	M = 0.01;
#ifdef __LOCOMOTION__
	Gmax = 0.4; Tm = 40.;
	UnitId = _id_CaLChan;
	Name = _ChannelNames[UnitId];
#endif // __LOCOMOTION__
	DList.insert( make_pair( "Activation\tInit\nM\nMcal", &M ));
}

tcal_channel::tcal_channel( const tcal_channel &channel )
	: t_channel( channel )
{
	DList.insert( make_pair( "Activation\tInit\nM\nMcal", &M ));
}

//--- overrided function
void tcal_channel::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new tcal_channel( *this );
	( *unit )->set_parent( parent );
}

void tcal_channel::copy_to( hhn_process **unit, hhn_process *parent )
{
	*unit = new hcal_channel(( hhn_compart *)parent );
	(( hhn_channel *)( *unit ))->init_channel( Gmax, Tm, Th );
	(( hhn_channel *)( *unit ))->Name = Name;
	(( hcal_channel *)*unit )->init_channel( M );
}

/*****************************************************************
 * Model of M channel template for Hodgkin-Haxley type neuron    *
 *****************************************************************/
//--- constructor
tm_channel::tm_channel( uni_template *parent, int is_active ) 
	: t_channel( parent, is_active )
{
	UnitId = _id_MChan;
	Name = _ChannelNames[UnitId];
	DList.insert( make_pair( "Activation\tInit\nM\nMm", &M ));
}

tm_channel::tm_channel( const tm_channel &channel )
	: t_channel( channel )
{
	DList.insert( make_pair( "Activation\tInit\nM\nMm", &M ));
}

//--- overrided function
void tm_channel::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new tm_channel( *this );
	( *unit )->set_parent( parent );
}

void tm_channel::copy_to( hhn_process **unit, hhn_process *parent )
{
	*unit = new hm_channel(( hhn_compart *)parent );
	(( hhn_channel *)( *unit ))->init_channel( Gmax, Tm, Th );
	(( hhn_channel *)( *unit ))->Name = Name;
	(( hm_channel *)*unit )->init_channel( M );
}

/*****************************************************************
 * Model of CaTchannel template for Hodgkin-Haxley type neuron  *
 *****************************************************************/
//--- constructor
tcat_channel::tcat_channel( uni_template *parent, int is_active ) 
	: t_channel( parent, is_active )
{
	M = 0.5;
#ifdef __LOCOMOTION__
	Gmax = 0.003;
	UnitId = _id_CaTChan;
	Name = _ChannelNames[UnitId];
#endif // __LOCOMOTION__
	DList.insert( make_pair( "Inactivation\tInit\nH\nHcat", &H ));
	DList.insert( make_pair( "Activation\tInit\nM\nMcat", &M ));
}

tcat_channel::tcat_channel( const tcat_channel &channel )
	: t_channel( channel )
{
	DList.insert( make_pair( "Inactivation\tInit\nH\nHcat", &H ));
	DList.insert( make_pair( "Activation\tInit\nM\nMcat", &M ));
}

//--- overrided function
void tcat_channel::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new tcat_channel( *this );
	( *unit )->set_parent( parent );
}

void tcat_channel::copy_to( hhn_process **unit, hhn_process *parent )
{
	*unit = new hcat_channel(( hhn_compart *)parent );
	(( hhn_channel *)( *unit ))->init_channel( Gmax, Tm, Th );
	(( hhn_channel *)( *unit ))->Name = Name;
	(( hcat_channel *)*unit )->init_channel( M, H );
}

/*****************************************************************
 * Model of CaN channel template for Hodgkin-Haxley type neuron  *
 *****************************************************************/
//--- constructor
tcan_channel::tcan_channel( uni_template *parent, int is_active ) 
	: t_channel( parent, is_active )
{
	M = 0.5;
#ifdef __LOCOMOTION__
	Gmax = 0.3;
	Tm = 4.;
	Th = 40.;
	UnitId = _id_CaNChan;
	Name = _ChannelNames[UnitId];
#endif // __LOCOMOTION__
	DList.insert( make_pair( "Inactivation\tInit\nH\nHcan", &H ));
	DList.insert( make_pair( "Activation\tInit\nM\nMcan", &M ));
}

tcan_channel::tcan_channel( const tcan_channel &channel )
	: t_channel( channel )
{
	DList.insert( make_pair( "Inactivation\tInit\nH\nHcan", &H ));
	DList.insert( make_pair( "Activation\tInit\nM\nMcan", &M ));
}

//--- overrided function
void tcan_channel::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new tcan_channel( *this );
	( *unit )->set_parent( parent );
}

void tcan_channel::copy_to( hhn_process **unit, hhn_process *parent )
{
	*unit = new hcan_channel(( hhn_compart *)parent );
	(( hhn_channel *)( *unit ))->init_channel( Gmax, Tm, Th );
	(( hhn_channel *)( *unit ))->Name = Name;
	(( hcan_channel *)*unit )->init_channel( M, H );
}

/*****************************************************************
 *  Model of KC channel template for Hodgkin-Haxley type neuron  *
 *****************************************************************/
//--- constructor
tkc_channel::tkc_channel( uni_template *parent, int is_active ) 
	: t_channel( parent, is_active )
{
	UnitId = _id_KCChan;
	Name = _ChannelNames[UnitId];
	DList.insert( make_pair( "Activation\tInit\nM\nMkc", &M ));
}

tkc_channel::tkc_channel( const tkc_channel &channel )
	: t_channel( channel )
{
	DList.insert( make_pair( "Activation\tInit\nM\nMkc", &M ));
}

//--- overrided function
void tkc_channel::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new tkc_channel( *this );
	( *unit )->set_parent( parent );
}

void tkc_channel::copy_to( hhn_process **unit, hhn_process *parent )
{
	*unit = new hkc_channel(( hhn_compart *)parent );
	(( hhn_channel *)( *unit ))->init_channel( Gmax, Tm, Th );
	(( hhn_channel *)( *unit ))->Name = Name;
	(( hkc_channel *)*unit )->init_channel( M );
}

/*****************************************************************
 * Model of KCa channel template for Hodgkin-Haxley type neuron  *
 *****************************************************************/
//--- constructor
tkca_channel::tkca_channel( uni_template *parent, int is_active ) 
	: t_channel( parent, is_active )
{
#ifdef __LOCOMOTION__
	Gmax = 5.;
	Tm = 0.2;
	UnitId = _id_KCaChan;
	Name = _ChannelNames[UnitId];
#endif // __LOCOMOTION__
	DList.insert( make_pair( "Activation\tInit\nM\nMkca", &M ));
	DList.insert( make_pair( "Activation\tTime constant\tMagnitude\nTm\nTkca", &Tm )); 
}

tkca_channel::tkca_channel( const tkca_channel &channel )
	: t_channel( channel )
{
	DList.insert( make_pair( "Activation\tInit\nM\nMkca", &M ));
	DList.insert( make_pair( "Activation\tTime constant\tMagnitude\nTm\nTkca", &Tm )); 
}

//--- overrided function
void tkca_channel::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new tkca_channel( *this );
	( *unit )->set_parent( parent );
}

void tkca_channel::copy_to( hhn_process **unit, hhn_process *parent )
{
	*unit = new hkca_channel(( hhn_compart *)parent );
	(( hhn_channel *)( *unit ))->init_channel( Gmax, Tm, 1 );
	(( hhn_channel *)( *unit ))->Name = Name;
	(( hkca_channel *)*unit )->init_channel( M );
}

/*****************************************************************
 *  Model of H channel template for Hodgkin-Haxley type neuron   *
 *****************************************************************/
//--- constructor
th_channel::th_channel( uni_template *parent, int is_active ) 
	: t_channel( parent, is_active ), Eh( -43.0 )
{
	M = 0.0; Tm = 1.;
	UnitId = _id_HChan;
	Name = _ChannelNames[UnitId];
	DList.insert( make_pair( "Activation\tInit\nM\nMh", &M ));
	DList.insert( make_pair( "Reversal potential (mV)\nEds\nEh", &Eh ));
}

th_channel::th_channel( const th_channel &channel )
	: t_channel( channel ), Eh( channel.Eh )
{
	DList.insert( make_pair( "Activation\tInit\nM\nMh", &M ));
	DList.insert( make_pair( "Reversal potential (mV)\nEds\nEh", &Eh ));
}

th_channel &th_channel::operator = ( const th_channel &channel )
{
	t_channel::operator = ( channel );
	Eh = channel.Eh;
	return *this;
}

//--- overrided function
void th_channel::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new th_channel( *this );
	( *unit )->set_parent( parent );
}

void th_channel::copy_to( hhn_process **unit, hhn_process *parent )
{
	*unit = new hh_channel(( hhn_compart *)parent );
	(( hhn_channel *)( *unit ))->init_channel( Gmax, Tm, Th );
	(( hhn_channel *)( *unit ))->Name = Name;
	(( hh_channel *)*unit )->init_channel( M, Eh );
}

/*****************************************************************
 * Model of Leak channel template for Hodgkin-Haxley type neuron  *
 *****************************************************************/
//--- constructor
tleak_channel::tleak_channel( uni_template *parent, int is_active ) 
	: t_channel( parent, is_active ), Eleak( -60.0 )
{
#ifdef __LOCOMOTION__
	Gmax = 1,0;
	UnitId = _id_LeakChan;
	Name = _ChannelNames[UnitId];
#endif // __LOCOMOTION__
	DList.insert( make_pair( "Reversal potential (mV)\nEds\nEleak", &Eleak ));
}

tleak_channel::tleak_channel( const tleak_channel &channel )
	: t_channel( channel ), Eleak( channel.Eleak )
{
	DList.insert( make_pair( "Reversal potential (mV)\nEds\nEleak", &Eleak ));
}

tleak_channel &tleak_channel::operator = ( const tleak_channel &channel )
{
	t_channel::operator = ( channel );
	Eleak = channel.Eleak;
	return *this;
}

//--- overrided function
void tleak_channel::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new tleak_channel( *this );
	( *unit )->set_parent( parent );
}

void tleak_channel::copy_to( hhn_process **unit, hhn_process *parent )
{
	*unit = new hleak_channel(( hhn_compart *)parent );
	(( hhn_channel *)( *unit ))->init_channel( Gmax, Tm, Th );
	(( hhn_channel *)( *unit ))->Name = Name;
	(( hleak_channel *)*unit )->init_channel( Eleak );
}
#endif // __LOCOMOTION__
