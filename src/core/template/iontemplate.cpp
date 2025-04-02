/******************************************************************************
 * File: iontemplate.cpp                                                      *
 *----------------------------------------------------------------------------*
 * Sergey Markin, 2006 KRINC                                                  *
 ******************************************************************************/
#include "precompile.h"

#include "iontemplate.h"

///////////////////////////////////////////////////////////////////////////////
// class t_ions
//--- constructor
t_ions::t_ions( uni_template *parent, int is_active, bool showtype )
	: uni_template( parent, is_active, NULL, 0, showtype ), Z( 1.0 ), RTF( 26.54 ), 
	Adjustable( false ), In( 1.0 ), Out( 1.0 ), Eds( 1.0 ),
	Dynamics( false ), PumpR( 0.0 ), PumpT( 1.0 ), IonsR( 0.0 )
{
	TypeNameS = TypeName = "Ions";
	TypeSynonym.push_back( "Ion" );
	DList.clear();
	DList.insert( make_pair( "Setting\tConcentration inside cell (mM)\nIn", &In ));
	DList.insert( make_pair( "Setting\tConcentration outside cell (mM)\nOut", &Out ));
	DList.insert( make_pair( "Setting\tPotential (mV)\nPotential", &Eds )); 
	DList.insert( make_pair( "Dynamics\tPump rate\nPumpR", &PumpR ));
	DList.insert( make_pair( "Dynamics\tPump time constant\nPumpT", &PumpT ));
	DList.insert( make_pair( "Dynamics\tIons current rate\nIonsR", &IonsR ));
	calc_eds();
}

t_ions::t_ions( const t_ions &ions )
	: uni_template( ions ), Z( ions.Z ), RTF( ions.RTF ), 
	Adjustable( ions.Adjustable ), In( ions.In ), Out( ions.Out ), Eds( ions.Eds ),
	Dynamics( ions.Dynamics ), PumpR( ions.PumpR ), PumpT( ions.PumpT ), IonsR( ions.IonsR )
{
	DList.clear();
	DList.insert( make_pair( "Setting\tConcentration inside cell (mM)\nIn", &In ));
	DList.insert( make_pair( "Setting\tConcentration outside cell (mM)\nOut", &Out ));
	DList.insert( make_pair( "Setting\tPotential (mV)\nPotential", &Eds )); 
	DList.insert( make_pair( "Dynamics\tPump rate\nPumpR", &PumpR ));
	DList.insert( make_pair( "Dynamics\tPump time constant\nPumpT", &PumpT ));
	DList.insert( make_pair( "Dynamics\tIons current rate\nIonsR", &IonsR ));
	calc_eds();
}

t_ions::~t_ions( void )
{
}

t_ions &t_ions::operator = ( const t_ions &ions )
{
	uni_template::operator = ( ions );
	RTF = ions.RTF; 
	Z = ions.Z;
	Adjustable = ions.Adjustable;
	Eds = ions.Eds;
	In = ions.In;
	Out = ions.Out;
	Dynamics = ions.Dynamics;
	PumpR = ions.PumpR;
	PumpT = ions.PumpT;
	IonsR = ions.IonsR;
	calc_eds();
	return *this;
}

//--- overrided function
bool t_ions::get_addparlist( vector<string> &parlist )
{
	parlist.push_back( "Setting\tAdjustable" );
	parlist.push_back( "Dynamics\tActive" );
	return true;
}

bool t_ions::get_addpar( const char *path, string &name, string &element, int &type )
{
	if( path ){
		string p_path = path;
		if( p_path == "Setting\tAdjustable" ){
			name = "Data";
			element = Adjustable? "true":"false";
			type = GRID_BOOL;
			return true;
		}
		else if( p_path == "Dynamics\tActive" ){
			name = "Data";
			element = Dynamics? "true":"false";
			type = GRID_BOOL;
			return true;
		}
	}
	return false;
}

bool t_ions::upd_addpar( const char *path, const char *name, string &element, bool save )
{
	if( path && name ){
		string p_path = path;
		string p_name = name;
		if( p_path == "Setting\tAdjustable" && p_name == "Data" ){
			if( save ){
				Adjustable = ( element == "true" )? true:false;
			}
			else{
				element = Adjustable? "true":"false";
			}
			return true;
		}
		else if( p_path == "Dynamics\tActive" && p_name == "Data" ){
			if( save ){
				Dynamics = ( element == "true" )? true:false;
			}
			else{
				element = Dynamics? "true":"false";
			}
			return true;
		}
	}
	return false;
}

int t_ions::get_datattr( const char *_path )
{
	string path = _path;
	if( path == "Setting\tAdjustable" ){
		return GRID_PROCESS;
	}
	else if( path == "Dynamics\tActive" ){
		return GRID_PROCESS;
	}
	else if( path == "Setting\tPotential (mV)" && !Adjustable ){
		return GRID_READONLY;
	}
	else if( path == "Setting\tConcentration inside cell (mM)" || path == "Setting\tConcentration outside cell (mM)" ){
		return GRID_PROCESS|GRID_POSITIVE;
	}
	else if(( path == "Dynamics\tPump rate" || path == "Dynamics\tPump time constant" || path == "Dynamics\tIons current rate") 
			&& !Dynamics ){
		return GRID_READONLY;
	}
	return uni_template::get_datattr( _path );
}

bool t_ions::process( const char *path )
{
	calc_eds();
	return true;
}

void t_ions::copy_to( hhn_ions *ion )
{
	ion->I = 0;
	ion->Ipump = 0;
	ion->IsEds = !Adjustable;
	ion->IsDyn = Dynamics;

	ion->Z = int( Z );
	ion->RTF = double( RTF );
	ion->Eds = double( Eds );
	ion->In = double( In );
	ion->Out = double( Out );
	ion->T = double( PumpT ); 
	ion->Alpha = double( IonsR );
	//ion->Alpha = double( IonsR )/ion->T;
	ion->Beta = double( PumpR );
}

void t_ions::calc_eds( void )
{
	if( !Adjustable ){
		Eds = ( double( RTF )/double( Z ))*log( double( Out )/double( In ));
	}
}

bool t_ions::load_addpar( string str, istream &file )
{
	if( str == "Adjustable"){
		file >> str >> str;
		Adjustable = ( str == "true" )? true:false;
		calc_eds();
		return true;
	}
	else if( str == "Dynamics"){
		file >> str >> str;
		Dynamics = ( str == "true" )? true:false;
		return true;
	}
	return false;
}

void t_ions::save_addpar( ostream &file )
{
	file << "Adjustable = " << ( Adjustable? "true": "false" ) << endl;
	file << "Dynamics = " << ( Dynamics? "true": "false" ) << endl;
}

///////////////////////////////////////////////////////////////////////////////
// class tna_ions
//--- constructor
tna_ions::tna_ions( uni_template *parent, int is_active, bool showtype )
	: t_ions( parent, is_active, showtype )
{
	UnitId = _id_Na_Ion;
	Name = _IonsNames[UnitId];
	In = 15.;
	Out = 120.;
	Dynamics = true;
	PumpR = 3.0;
	IonsR = 1.0;
	calc_eds();
}

void tna_ions::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new tna_ions( *this );
	( *unit )->set_parent( parent );
}

void tna_ions::copy_to( hhn_process **unit, hhn_process *parent )
{
	*unit = new hna_ions( parent );
	t_ions::copy_to(( hhn_ions *)( *unit ));
}

///////////////////////////////////////////////////////////////////////////////
// class tk_ions
//--- constructor
tk_ions::tk_ions( uni_template *parent, int is_active, bool showtype )
       : t_ions( parent, is_active, showtype )
{
	UnitId = _id_K_Ion;
	Name = _IonsNames[UnitId];
	In = 140.;
	Out = 4.;
	calc_eds();
}

void tk_ions::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new tk_ions( *this );
	( *unit )->set_parent( parent );
}

void tk_ions::copy_to( hhn_process **unit, hhn_process *parent )
{
	*unit = new hk_ions( parent );
	t_ions::copy_to(( hhn_ions *)( *unit ));
}

///////////////////////////////////////////////////////////////////////////////
// class tca_ions
//--- constructor
tca_ions::tca_ions( uni_template *parent, int is_active, bool showtype )
	    : t_ions( parent, is_active, showtype )
#ifndef __LOCOMOTION__
	, B( 0.03 ), K( 0.001 )
#endif
{
	UnitId = _id_Ca_Ion;
	Name = _IonsNames[UnitId];
	In = 0.00005;
	Out = 4.;
	Z = 2; 
	PumpR = 1.0;

#if defined( __RESPIRATION__ ) && !defined( __LOCOMOTION__ )
	IonsR = 1.0;
	PumpT = 500.0,
	calc_eds();
#elif defined( __LOCOMOTION__ )
	IonsR = 0.0045;
	PumpT = 50.0,
	Eds = 80.0;
	Adjustable = true;
#else
	IonsR = 0.0045;
	PumpT = 50.0,
	calc_eds();
#endif // defined( __RESPIRATION__ ) && !defined( __LOCOMOTION__ )
	Dynamics = true;

#ifndef __LOCOMOTION__
	DList.insert( make_pair( "Dynamics\tPump B\nB", &B ));
	DList.insert( make_pair( "Dynamics\tPump K\nK", &K ));
#endif //  __LOCOMOTION__
}

tca_ions::tca_ions( const tca_ions &ions )
	: t_ions( ions )
#ifndef __LOCOMOTION__
	, B( ions.B ), K( ions.K )
#endif
{
#ifndef __LOCOMOTION__
	DList.insert( make_pair( "Dynamics\tPump B\nB", &B ));
	DList.insert( make_pair( "Dynamics\tPump K\nK", &K ));
#endif //  __LOCOMOTION__
}

tca_ions &tca_ions::operator = ( const tca_ions &ions )
{
	t_ions::operator = ( ions );
#ifndef __LOCOMOTION__
	B = ions.B;
	K = ions.K;
#endif // __LOCOMOTION__
	return *this;
}

void tca_ions::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new tca_ions( *this );
	( *unit )->set_parent( parent );
}

void tca_ions::copy_to( hhn_process **unit, hhn_process *parent )
{
	*unit = new hca_ions( parent );
	t_ions::copy_to(( hhn_ions *)( *unit ));
}

///////////////////////////////////////////////////////////////////////////////
// class tcl_ions
//--- constructor
tcl_ions::tcl_ions( uni_template *parent, int is_active, bool showtype )
        : t_ions( parent, is_active, showtype )
{
	UnitId = _id_Cl_Ion;
	Name = _IonsNames[UnitId];
	In = 4.;
	Out = 110.;
	Z = -1;
	calc_eds();
}

void tcl_ions::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new tcl_ions( *this );
	( *unit )->set_parent( parent );
}

void tcl_ions::copy_to( hhn_process **unit, hhn_process *parent )
{
	*unit = new hcl_ions( parent );
	t_ions::copy_to(( hhn_ions *)( *unit ));
}

///////////////////////////////////////////////////////////////////////////////
// class t_pumps
t_pumps::t_pumps( uni_template *parent, int is_active, bool showtype )
	: uni_template( parent, is_active, NULL, 0, showtype ), PumpRV( 0.0 )
{
	TypeNameS = TypeName = "Pumps";
	TypeSynonym.push_back( "Pump" );
	DList.clear();
	DList.insert( make_pair( "Pump rate (dynamics of membrane potential)\nPumpRV", &PumpRV ));
}

t_pumps::t_pumps( const t_pumps &pump )
	: uni_template( pump ), PumpRV( pump.PumpRV )
{
	DList.clear();
	DList.insert( make_pair( "Pump rate (dynamics of membrane potential)\nPumpRV", &PumpRV ));
}

t_pumps::~t_pumps( void )
{
}

t_pumps &t_pumps::operator = ( const t_pumps &pump )
{
	uni_template::operator = ( pump );
	PumpRV = pump.PumpRV;
	return *this;
}

///////////////////////////////////////////////////////////////////////////////
// class tnak_pump
tnak_pump::tnak_pump( uni_template *parent, int is_active, bool showtype )
	: t_pumps( parent, is_active, showtype ), Na0( 8.0 ), Kp( 15.0 ), Rp( 0.0006 )
{
	UnitId = _id_NaK_Pump;
	Name = _PumpsNames[UnitId];
	DList.insert( make_pair( "Setting\tEquilibrium Na Concentration\nNa0", &Na0 ));
	DList.insert( make_pair( "Setting\tNa pump rate\nRp", &Rp ));
	DList.insert( make_pair( "Setting\tKp\nKp", &Kp ));
}

tnak_pump::tnak_pump( const tnak_pump &pump )
	: t_pumps( pump ), Na0( pump.Na0 ), Kp( pump.Kp ), Rp( pump.Rp )
{
	DList.insert( make_pair( "Setting\tEquilibrium Na Concentration\nNa0", &Na0 ));
	DList.insert( make_pair( "Setting\tNa pump rate\nRp", &Rp ));
	DList.insert( make_pair( "Setting\tKp\nKp", &Kp ));
}

tnak_pump::~tnak_pump( void )
{
}

tnak_pump &tnak_pump::operator = ( const tnak_pump &pump )
{
	t_pumps::operator = ( pump );
	Na0 = pump.Na0;
	Kp = pump.Kp;
	Rp = pump.Rp;
	return *this;
}

void tnak_pump::copy_to( uni_template **unit, uni_template *parent )
{
 *unit = new tnak_pump( *this );
 ( *unit )->set_parent( parent );
}

void tnak_pump::copy_to( hhn_process **unit, hhn_process *parent )
{
	*unit = new hnak_pump( parent );
	(( hnak_pump *)*unit )->PumpRV = double( PumpRV );
	(( hnak_pump *)*unit )->Na0 = double( Na0 );
	(( hnak_pump *)*unit )->Kp = double( Kp );
	(( hnak_pump *)*unit )->Rp = double( Rp );
}

///////////////////////////////////////////////////////////////////////////////
// class tca_pump
tca_pump::tca_pump( uni_template *parent, int is_active, bool showtype )
	: t_pumps( parent, is_active, showtype ), Ca0( 0. )
{
	UnitId = _id_Ca_Pump;
	Name = _PumpsNames[UnitId];
#ifndef __LOCOMOTION__
	Ca0 = 0.00005;
#endif //  __LOCOMOTION__
	DList.insert( make_pair( "Setting\tEquilibrium Ca Concentration\nCa0", &Ca0 ));
}

tca_pump::tca_pump( const tca_pump &pump )
	: t_pumps( pump ), Ca0( pump.Ca0 )
{
	DList.insert( make_pair( "Setting\tEquilibrium Ca Concentration\nCa0", &Ca0 ));
}

tca_pump::~tca_pump( void )
{
}

tca_pump &tca_pump::operator = ( const tca_pump &pump )
{
	t_pumps::operator = ( pump );
	Ca0 = pump.Ca0;
	return *this;
}

void tca_pump::copy_to( uni_template **unit, uni_template *parent )
{
 *unit = new tca_pump( *this );
 ( *unit )->set_parent( parent );
}

void tca_pump::copy_to( hhn_process **unit, hhn_process *parent )
{
	*unit = new hca_pump( parent );
	(( hca_pump *)*unit )->PumpRV = double( PumpRV );
	(( hca_pump *)*unit )->Ca0 = double( Ca0 );
}
