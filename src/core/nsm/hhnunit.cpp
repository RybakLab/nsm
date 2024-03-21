//////////////////////////////////////////////////////////////////////
// hhnunit.cpp
#include "precompile.h"

#include "hhnunit.h"
#include "simulate.h"

#ifndef __LINUX__ 
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif // _DEBUG
#endif // __LINUX__

const char *vw_name( std::string &name, unit_code &code )
{
	name = "";
	switch( code.UnitId ){
		case _id_Network:
		case _id_NNPopulat:
		case _id_NNFeedback:
		case _id_NNDrive:
		case _id_NNOutput:
		case _id_NNControl:
			switch( code.Param ){
				case CViewParam::Out:
					name = "Output";
					break;
				case CViewParam::Plot:
					name = "Plot";
					break;
				case CViewParam::Hist:
					name = "Histogram";
					break;
				case CViewParam::Potent:
					name = "Potential";
					break;
				case CViewParam::NaConc:
					name = "Na Concentration";
					break;
				case CViewParam::CaConc:
					name = "Ca Concentration";
					break;
				case CViewParam::Condc:
					name = "Conductance";
					break;
				case CViewParam::Curr:
					name = "Current";
					break;
				case CViewParam::Activation:
					name = "Activation";
					break;
				case CViewParam::Inactivation:
					name = "Inactivation";
					break;
				}
			return name.c_str();
#if defined (__MECHANICS_2D__)
		case _id_Vertex:
			switch( code.Param ){
				case _id_Coordinate_X:
					name = "Coordinate X";
					break;
				case _id_Coordinate_Y:
					name = "Coordinate Y";
					break;
				case _id_Velocity_X:
					name = "Velocity X";
					break;
				case _id_Velocity_Y:
					name = "Velocity Y";
					break;
				}
			return name.c_str();
		case _id_Link:
		case _id_Joint:
			switch( code.Param ){
				case _id_Angle:
					name = "Angle";
					break;
				case _id_Angle_Velocity:
					name = "Angle velocity";
					break;
				}
			return name.c_str();
		case _id_Muscle:
			switch( code.Param ){
				case _id_MuscleForce:
					name = "Force";
					break;
				case _id_TotalLength:
					name = "Total Length";
					break;
				case _id_TotalVelocity:
					name = "Total Velocity";
					break;
				case _id_MuscleLength:
					name = "Muscle Length";
					break;
				case _id_MuscleVelocity:
					name = "Muscle Velocity";
					break;
				case _id_TendonLength:
					name = "Tendon Length";
					break;
				}
			return name.c_str();
		case _id_Ground:
			switch( code.Param ){
				case _id_GroundForce_X:
					name = "Force X";
					break;
				case _id_GroundForce_Y:
					name = "Force Y";
					break;
				}
			return name.c_str();
#elif defined (__MECHANICS_3D__)
	// TODO implementation 3d model
#endif // __MECHANICS_2D__
	}
	return name.c_str();
}

const char *cr_name( std::string &name, unit_code &code )
{
	name = "";
	if( code.UnitId == _id_NNFeedback ||
		code.UnitId == _id_NNDrive ||
		code.UnitId == _id_NNOutput )
		return "";
	else{
		if( code.Param == _id_generic_Chan ){ // dirty hack
			code.Param =_id_NUM_CONTROLLED;
		}
		if( code.Param < _id_MAX_CHAN ){
			if( code.PartIndex != 0 )
				name = "Gmax ";
			else
				name = "Eds ";
			name += _ChannelNames[code.Param];
		}
		else if( code.Param >= _id_MAX_CHAN && code.Param < _id_MAX_CHAN+_id_MAX_SYN ){
			name += _SynapseNames[code.Param-_id_MAX_CHAN];
		}
		else if( code.Param == _id_KIons){
			name = "KIons (Outside)";
		}
		else if( code.Param >= _id_NUM_CONTROLLED && code.Param < _id_MAX_CTRL ){ // hack
			name = ::get_gchanname( code.Param-_id_NUM_CONTROLLED ); // new code 4 control
		}
		else{
			name = "";
		}
	}
	return name.c_str();
}

/////////////////////////////////////////////////////////////////////////////
// unit_code class
unit_code::unit_code( void )
	: UnitId( -1 ), NNIndex( -1 ), HhnIndex( -1 ),
	PartIndex( -1 ), ChanIndex( -1 ), Param( -1 )
{
}

unit_code::unit_code( DWORD code )
{
	decode( code );
}

unit_code::unit_code( const unit_code &code )
	: UnitId( code.UnitId ), NNIndex( code.NNIndex ), HhnIndex( code.HhnIndex ),
	PartIndex( code.PartIndex ), ChanIndex( code.ChanIndex ), Param( code.Param )
{
}

unit_code &unit_code::operator = ( const unit_code &code )
{
	UnitId = code.UnitId;
	NNIndex = code.NNIndex;
	HhnIndex = code.HhnIndex;
	PartIndex = code.PartIndex;
	ChanIndex = code.ChanIndex;
	Param = code.Param;
	return *this;
}

bool unit_code::operator == ( const unit_code &code ) const
{
	return( UnitId == code.UnitId && NNIndex == code.NNIndex &&
		HhnIndex == code.HhnIndex && PartIndex == code.PartIndex && 
		ChanIndex == code.ChanIndex && Param == code.Param );
}

bool unit_code::is_stat( void ) const
{
	if( UnitId == _id_NNPopulat )
		if( Param == CViewParam::Plot || Param == CViewParam::Hist )
			return true;
 return false;
}

void unit_code::decode( DWORD code )
{
	Param = int( code&0x3F )-1;	// bits <0:5>
	code >>= 6;
	ChanIndex = int( code&0x3F )-1;	// bits <6:11>
	code >>= 6;
	PartIndex = int( code&0x03 )-1;	// bits <12:13>
	code >>= 2;
	HhnIndex = int( code&0x3F )-1;	// bits <14:19>
	code >>= 6;
	NNIndex = int( code&0xFF )-1;	// bits <20:27>
	code >>= 8;
	UnitId = int( code&0x0F )-1;	// bits <28:31>
}

DWORD unit_code::encode( void ) const
{
	DWORD code = 0;
	code = ( UnitId+1 )&0x0F;	// 4 bits to encode UnitID (16 types of network units)
	code <<= 8;
	code |= ( NNIndex+1 )&0xFF;	// 8 bits to encode NNIndex (256 populations/drives ets)
	code <<= 6;
	code |= ( HhnIndex+1 )&0x3F;	// 6 bits to encode HnnIndex (64 neurons to view)
	code <<= 2;
	code |= ( PartIndex+1 )&0x03;	// 2 bits to encode compartment number (8 compartments)
	code <<= 6;
	code |= ( ChanIndex+1 )&0x3F;	// 6 bits to encode channel index
	code <<= 6;
	code |= ( Param+1 )&0x3F;	// 6 bits to ecode additional parameter
	return code;
}

bool unit_code::load( istream &file, CSimulate *manager )
{
	string str;
	string name;
	while( file >> str){
		if( str == "Unit" ){
			file >> str;
			file >> ws;
			getline(file, name, '\n');
			hhn_pair<int> unit = manager->get_uid( name );
			UnitId = DWORD( unit.X );
			NNIndex = DWORD( unit.Y );
		}
		else if( str == "HhnIndex"){
			file >> str >> HhnIndex;
			if( HhnIndex != -1 && _FileVersion < 2 )
			    PartIndex = 0;        
		}
		else if( str == "PartIndex"){
			file >> str >> PartIndex;
		}
		else if( str == "ChanIndex"){
			file >> str >> ChanIndex;
			if( ChanIndex != -1 && _FileVersion < 2 )
			    PartIndex = 0;        
		}
		else if( str == "Param"){
			file >> str >> Param;
		}
		else if( str == "</Code>"){
			if(( UnitId == -1 || NNIndex == -1 ) && !name.empty()){
				str = "Unknown  unit name: " + name + "!";
				message( str.c_str(), "Warning!" );
				return false;
			}
			if(HhnIndex!=-1 && HhnIndex >= manager->Network.get_nnunit(*this)->size()){
				str = "Wrong neuron index (";
				str += manager->Network.get_nnunit(*this)->get_name();
				str += ")!";
				message( str.c_str(),"Warning!" );
				return false;
			}
			if(PartIndex != -1&& UnitId == _id_NNPopulat &&
				PartIndex >= manager->Network.get_pop(NNIndex).get_neuronT()->size()){
				str = "Wrong compartment index (";
				str += manager->Network.get_pop(NNIndex).get_name();
				str += ")!";
				message( str.c_str(),"Warning!" );
				return false;
			}
			if(PartIndex != -1 && UnitId == _id_NNPopulat && ChanIndex != -1 &&
				ChanIndex >= manager->Network.get_pop(NNIndex).get_neuronT()->get_compart(PartIndex)->size()){
				str = "Wrong channel index (";
				str += manager->Network.get_nnunit(*this)->get_name();
				str += ")!";
				message( str.c_str(),"Warning!" );
				return false;
			}
			return true;
		}
		else{
			unknown_string( file, str);
		}
	}
	message( "Missing tag </Code>!", "Warning!" );
	return false;
}

void unit_code::save( ostream &file, CSimulate *manager )
{
	file << "\n<Code>\n";
	if( !manager->GetUnitName( *this ).empty()){
		string name = manager->GetUnitName( *this );
		file << "Unit = " << name << endl;
		file << "HhnIndex = " << HhnIndex << "\n";
		file << "PartIndex = " << PartIndex << "\n";
		file << "ChanIndex = " << ChanIndex << "\n";
		file << "Param = " << Param << "\n";
	}
	file << "</Code>\n";
}

bool unit_code::get_fullname( CSimulate *manager, std::string &str )
{
	std::ostringstream s;
	std::string name = manager->GetUnitName( *this );
	if( name.empty() )
		return false;
	s << name;
	if( UnitId == _id_NNPopulat ){
		if( Param == CViewParam::Plot || Param == CViewParam::Hist ){
			string sname;
			vw_name( sname, *this );
			s << " / " << sname;
			str = s.str();
			return true;
		}
		hhn_populat *populat = (hhn_populat *)manager->Network.get_nnunit( *this );
		if( !(HhnIndex >= 0 && HhnIndex < populat->size()) )
			return false;
		if( populat->size() > 1 ){
			s << " / Neuron " << (HhnIndex);
		}
		if( PartIndex != -1 ){
			if(!(PartIndex >= 0 && PartIndex < ( *populat )[HhnIndex].size()))
				return false;
			const hhn_compart *cmp = ( *populat )[HhnIndex].get_compart( PartIndex );
			s << " / " << cmp->get_name();
			if( ChanIndex != -1 ){
				if(!( ChanIndex >= 0 && ChanIndex < cmp->size() ))
					return false;
				s << " / " << ( *cmp )[ChanIndex]->get_name();
			}
		}
	}
	string sname;
	vw_name( sname, *this );
	if( sname.empty() )
		return false;
	if( sname != "Output")
		s << " / " << sname;
	if( s.str() == "" )
		return false;
	str = s.str();
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// class hhn_input
hhn_input::hhn_input( void )
	: hhn_process(), Trg( NULL )
{
}

hhn_input::hhn_input( double *trg, size_t src_sz )
	: hhn_process(), Trg( trg )
{
	Src.reserve( src_sz );
}

hhn_input::hhn_input( const hhn_input &inp )
	: hhn_process( inp ), Src( inp.Src ), Trg( inp.Trg )
{
}

hhn_input::~hhn_input( void )
{
}

hhn_input &hhn_input::operator = ( const hhn_input &input )
{
	hhn_process::operator = ( input );
	Src = input.Src; Trg = input.Trg;
	return *this;
}

void hhn_input::add_src( double *s, double w )
{
	Src.push_back( winp_( s, w ));
}

void hhn_input::reg_unit( runman *man )
{
	if( Trg != NULL && !Src.empty()){
		::reg_unit( this, hhn_input::calc_g, _id_PASS_G, -1, man );
	}
}

void hhn_input::calc_g( double step )
{
	double trg = 0;
	for( size_t i = 0; i < Src.size(); ++i ){
		trg += double( Src[i] );
	}
	*Trg = trg;
}

///////////////////////////////////////////////////////////////////////////////
// class nn_unit
nn_unit::nn_unit( void )
	: Name( "" ), UnitId( 1 ), NNIndex( -1 ), ConnectType()
{
}

nn_unit::nn_unit( const nn_unit &unit )
	:  Name( unit.Name ), UnitId( unit.UnitId ), NNIndex( unit.NNIndex ), ConnectType( unit.ConnectType ), Inputs( unit.Inputs )
{
}

nn_unit::~nn_unit( void )
{
}

nn_unit &nn_unit::operator = ( const nn_unit &unit )
{
	Name = unit.Name;
	UnitId = unit.UnitId;
	NNIndex = unit.NNIndex;
	ConnectType = unit.ConnectType;
	Inputs = unit.Inputs;
	return *this;
}

void nn_unit::connect( const vector<nn_unit *> &source, const vector<CNNConnect> &connect )
{
	vector<nn_unit *> src;
	vector<CConnect> w;
	src.reserve( ConnectType.size()*source.size());
	w.reserve( ConnectType.size()*source.size());
	for( size_t i = 0; i < ConnectType.size(); ++i ){
		int type = ConnectType[i].Type;
		for( size_t j = 0; j < source.size(); ++j ){
			CConnect nn = connect[j].get_connect( type );
			if( !nn.empty() ){
				src.push_back( source[j] );
				w.push_back( nn );
			}
		}
	}
	Inputs.clear();
	add_connection( src, w );
}

bool nn_unit::load( istream &file )
{
	file >> ws;
	getline( file, Name, '>' );
	return true;
}

void nn_unit::save( ostream &file )
{
	file << Name << ">" << endl;
	file << "Size = " << size() << endl;
}
