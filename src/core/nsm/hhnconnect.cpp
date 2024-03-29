/////////////////////////////////////////////////////////////////////////////
// hhnconnect.cpp
#include "precompile.h"

#include "hhnconnect.h"
#include "hhnsynapse.h"

#ifndef __LINUX__ 
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif // _DEBUG
#endif // __LINUX__

using std::find_if;
using std::stringstream;

/////////////////////////////////////////////////////////////////////////////
// class CHhnConnect
//--- constructor
CHhnConnect::CHhnConnect( void )
	: Probability( 1. ), Modulate( 0 )
{
	reset();
}

CHhnConnect::CHhnConnect( const CHhnConnect &connect )
	: Probability( connect.Probability ), Modulate( connect.Modulate )
{
	for( size_t i = 0; i < _id_MAX_SYN; Sigma[i] = connect.Sigma[i], ModSigma[i] = connect.ModSigma[i], Linear[i] = connect.Linear[i], Weights[i] = connect.Weights[i], ++i );
}

CHhnConnect &CHhnConnect::operator = ( const CHhnConnect &connect )
{
	Probability = connect.Probability;
	Modulate = connect.Modulate;
	for( size_t i = 0; i < _id_MAX_SYN; Sigma[i] = connect.Sigma[i], ModSigma[i] = connect.ModSigma[i], Linear[i] = connect.Linear[i], Weights[i] = connect.Weights[i], ++i );
	return *this;
}

bool CHhnConnect::operator == ( const CHhnConnect &x ) const
{
	bool res = true;
	for( size_t i = 0; i < _id_MAX_SYN; ++i ){
		res &= ( Sigma[i] == x.Sigma[i] );
		res &= ( ModSigma[i] == x.ModSigma[i] );
		res &= ( Linear[i] == x.Linear[i] );
		res &= ( Weights[i] == x.Weights[i] );
	}
	return res;
}

bool CHhnConnect::load( istream &file ) // keep it to provide backward compatibility
{
	string str;
	while( file >> str){
		auto get_synid = [](auto s ){
			if( s == "Ex" ) return _id_ExSyn;
			if( s == "Ex2" ) return _id_ExBSyn;
			if( s == "InhI" ) return _id_InhASyn;
			if( s == "InhII" ) return _id_InhBSyn;
			if( s == "Syn1" ) return _id_Syn1;
			if( s == "Syn2" ) return _id_Syn2;
			if( s == "Syn3" ) return _id_Syn3;
			if( s == "Syn4" ) return _id_Syn4;
			return _id_MAX_SYN;
		};
		size_t id = get_synid( str );
		if( id < _id_MAX_SYN ){ file >> str >> Weights[id]; }
		else{ unknown_string( file, str); }
	}
	message( "Missing tag </Target>!", "Warning!" );
	return false;
}

/////////////////////////////////////////////////////////////////////////////
// class CConnect
CConnect &CConnect::operator = ( const CConnect &target )
{
	Type = target.Type;
	Connect = target.Connect;
	return *this;
}

bool CConnect::operator == ( const CConnect &x ) const
{
	if( Type == x.Type ){
		return bool( Connect == x.Connect );
	}
	return false;
}

bool CConnect::load( istream &file )
{
	string str;
	Connect.reset();
	while( file >> str){
		if( str == "Type" ){
			file >> str >> Type;
		} else if( str == "Probability"){
			file >> str >> Connect.Probability;
		} else if( str == "Modulate"){
			file >> str >> Connect.Modulate;
		} else if( str == "</Connect>"){
			return true;
		} else{
			auto get_synid = [](auto s ){
				if( s == "Ex" ) return _id_ExSyn;
				if( s == "Ex2" ) return _id_ExBSyn;
				if( s == "InhI" ) return _id_InhASyn;
				if( s == "InhII" ) return _id_InhBSyn;
				if( s == "Syn1" ) return _id_Syn1;
				if( s == "Syn2" ) return _id_Syn2;
				if( s == "Syn3" ) return _id_Syn3;
				if( s == "Syn4" ) return _id_Syn4;
				return _id_MAX_SYN;
			};
			size_t id = get_synid( str );
			if( id < _id_MAX_SYN ){
				getline( file, str );
				stringstream line;
				line << str;
				line >> str >> Connect.Weights[id] >> str;
				if( str.find( 's' ) != string::npos ){ // sigma
					Connect.Sigma[id] = true;
				} else if( str.find( 'm' ) != string::npos ){ // modified sigma
					Connect.ModSigma[id] = true;
				} else if( str.find( 'l' ) != string::npos ){ // linear
					Connect.Linear[id] = true;
				}
			} else{ unknown_string( file, str); }
		}
	}
	message( "Missing tag </Connect>!", "Warning!" );
	return false;
}

void CConnect::save( ostream &file ) const
{
	file << "Type = " << Type << endl;
	for( int i = 0; i < _id_MAX_SYN; ++i ){
		if( Connect.Weights[i] != 0. ){
			switch( i ){
				case _id_ExSyn: file << "Ex = "; break;
				case _id_ExBSyn: file << "Ex2 = "; break;
				case _id_InhASyn: file << "InhI = "; break;
				case _id_InhBSyn: file << "InhII = "; break;
				case _id_Syn1: file << "Syn1 = "; break;
				case _id_Syn2: file << "Syn2 = "; break;
				case _id_Syn3: file << "Syn3 = "; break;
				case _id_Syn4: file << "Syn4 = "; break;
			}
			file << Connect.Weights[i] << ( Connect.Sigma[i]? "s":   Connect.ModSigma[i]? "m": Connect.Linear[i]? "l": "" ) << endl;
		}
	}
	file << "Probability = " << Connect.Probability << endl;
	file << "Modulate = " << Connect.Modulate << endl;
	file << "</Connect>" << endl;
}

/////////////////////////////////////////////////////////////////////////////
// class CNNConnect
CNNConnect &CNNConnect::operator = ( const CNNConnect &connect )
{
	Empty = connect.Empty;
	Connect = connect.Connect;
	ChainT = connect.ChainT;
	ChainS = connect.ChainS;
	return *this;
}

bool CNNConnect::operator == ( const CNNConnect &x ) const
{
	if( !Empty && Connect.size() == x.Connect.size()){
		bool res = true;
		for( size_t i = 0; i < Connect.size(); ++i ){
			res &= bool( Connect[i] == x.Connect[i] );
		}
		return res;
	}
	return false;
}

const CConnect &CNNConnect::get_connect( int type ) const
{
	for( size_t i = 0; i < Connect.size(); i++ )
		if( type == Connect[i].Type ){
			return Connect[i];
		}
	return nill;
}

void CNNConnect::add_connect( const CConnect &weight, bool replace )
{
	for( size_t i = 0; i < Connect.size(); ++i )
		if( weight.Type == Connect[i].Type ){
			if( replace ){
				Connect[i].Connect = weight.Connect;
			}
			return;
		}
	Empty = false;
	Connect.push_back( weight );
}

void CNNConnect::del_connect( void )
{
	Empty = true;
	Connect.clear();
}

bool CNNConnect::load( istream &file )
{
	string str;
	if( _FileVersion >= 2 ){
		while( file >> str){
			if( str == "<Connect>" ){
				CConnect connect;
				if( connect.load( file )){
					add_connect( connect );
				}
			} else if( str == "ChainT" ){
				file >> str; file >> ws;
				getline( file, ChainT );
			} else if( str == "ChainS"){
				file >> str; file >> ws;
				getline( file, ChainS );
			} else if( str == "</Target>"){
				return true;
			} else{ unknown_string( file, str ); }
		}
		message( "Missing tag </Target>!", "Warning!" );
	} else{
		CHhnConnect connect;
		if( connect.load( file )){
			add_connect( CConnect( connect, _id_NN_SOMA ));
			return true;
		}
	}
	return false;
}

void CNNConnect::save( ostream &file ) const
{
	for( size_t i = 0; i < Connect.size(); ++i ){
		file << "<Connect>" << endl;
		Connect[i].save( file );
	}
	if( !ChainS.empty() && !ChainT.empty() ){
		file << "ChainT = " << ChainT << endl;
		file << "ChainS = " << ChainS << endl;
	}
	file << "</Target>" << endl;
}

/////////////////////////////////////////////////////////////////////////////
// CConnectMatrix class
typedef pair<string,string> _link;
typedef list<_link>::iterator _it;
typedef CConnectMatrix _matrix;
class _add_chain{
	class _link_eq{
		public:
			_link_eq( const _link &link ) : Link( link ){};
		public:
			bool operator ()( const _link &l ) const
			{
				return Link == l;
			};
		public:
			_link Link;
		};
	public:
		_add_chain( _matrix &m, list<_link> &chain, _link &l )
		{
			for( _link link = l; !( link.first.empty() || link.second.empty()); ){
				_it pos = find_if( chain.begin(), chain.end(), _link_eq( link ));
				if( pos == chain.end() ){
					chain.push_back( link );
				} else{ break; }
				link = make_pair( m( link.first, link.second ).ChainT, m( link.first, link.second ).ChainS );
			}
		};
		_add_chain( _matrix &m, list<_link> &chain, _link &l, _link &exclude )
		{
			for( _link link = l; !( link.first.empty() || link.second.empty()); ){
				_it pos = find_if( chain.begin(), chain.end(), _link_eq( link ));
				if( pos == chain.end() ){
					if( link != exclude ){
						chain.push_back( link );
					}
				} else{ break; }
				link = make_pair( m( link.first, link.second ).ChainT, m( link.first, link.second ).ChainS );
			}
		};
};

CConnectMatrix &CConnectMatrix::operator =( const CConnectMatrix &matrix )
{
	Target = matrix.Target;
	Source = matrix.Source;
	NNConnect = matrix.NNConnect;
	return *this;
}

bool CConnectMatrix::occupied( const string &name )
{
	if(( trg_id( name ) == -1 )&&( src_id( name ) == -1 ))
		return false;
	return true;
}

int CConnectMatrix::trg_id( const string &name ) const
{
	for( size_t i = 0; i < Target.size(); i++ )
		if( name == Target[i] )
			return i;
	return -1;
}

bool CConnectMatrix::add_trg( const string &name )
{
	if( trg_id( name ) == -1 ){
		Target.push_back( name );
		NNConnect.AddRow();
		return true;
	}
	return false;
}

void CConnectMatrix::chain( _link master, _link slave )
{
	if( master != slave ){
		// to copy the properties of 'master' link to 'slave' link
		CNNConnect connect = operator()( master.first, master.second );
		operator()( slave.first, slave.second ).Empty = connect.Empty;
		operator()( slave.first, slave.second ).Connect = connect.Connect;
		// to add all elements of the chain into the same list
		list<_link> chain;
		_add_chain( *this, chain, master );
		_add_chain( *this, chain, slave );
		// to chain all links
		chain_all( chain );
	}
}

void CConnectMatrix::modify( _link link )
{
	if( !( link.first.empty() || link.second.empty())){
		typedef pair<string,string> _link;
		CNNConnect connect = operator()( link.first, link.second );
		_link head = link;
		do{
			link = make_pair( operator()( link.first, link.second ).ChainT, operator()( link.first, link.second ).ChainS );
			if( !( link.first.empty() || link.second.empty())){
				operator()( link.first, link.second ).Empty = connect.Empty;
				operator()( link.first, link.second ).Connect = connect.Connect;
			} else{ break; }
		} while( link != head );
	}
}

void CConnectMatrix::unchain( _link link )
{
	if( !( link.first.empty() || link.second.empty())){
		// to chain all but 'link'
		_link head = link;
		list<_link> chain;
		link = make_pair( operator()( link.first, link.second ).ChainT, operator()( link.first, link.second ).ChainS );
		_add_chain( *this, chain, link, head );
		chain_all( chain );
		// to unchain 'link'
		operator()( head.first, head.second ).ChainT.clear();
		operator()( head.first, head.second ).ChainS.clear();
	}
}

void CConnectMatrix::chain_all( list<_link> &chain )
{
	for( _it pos = chain.begin(), next = pos, head = pos; pos != chain.end(); pos = next ){
		++next;
		if( next == chain.end()){
			operator()( pos->first, pos->second ).ChainT = head->first;
			operator()( pos->first, pos->second ).ChainS = head->second;
		} else{
			operator()( pos->first, pos->second ).ChainT = next->first;
			operator()( pos->first, pos->second ).ChainS = next->second;
		}
	}
}

bool CConnectMatrix::rename_trg( const string &oldname, const string &newname )
{
	int index = trg_id( oldname );
	if( index != -1 ){
		Target[index] = newname;
		for( size_t i = 0; i < NNConnect.size(); ++i ){
			if( !NNConnect.GetMatrix()[i].empty() && NNConnect.GetMatrix()[i].ChainT == oldname ){
				NNConnect.GetMatrix()[i].ChainT = newname;
			}
		}
		return true;
	}
	return false;
}

bool CConnectMatrix::rename_src( const string &oldname, const string &newname )
{
	int index = src_id( oldname );
	if( index != -1 ){
		Source[index] = newname;
		for( size_t i = 0; i < NNConnect.size(); ++i ){
			if( !NNConnect.GetMatrix()[i].empty() && NNConnect.GetMatrix()[i].ChainS == oldname ){
				NNConnect.GetMatrix()[i].ChainS = newname;
			}
		}
		return true;
	}
	return false;
}


bool CConnectMatrix::del_trg( const string &name )
{
	int inx = trg_id( name );
	if( inx != -1 ){
		Target.erase( Target.begin()+inx );
		NNConnect.DelRow( inx );
		return true;
	}
	return false;
}

int CConnectMatrix::src_id( const string &name ) const
{
	for( size_t i = 0; i < Source.size(); i++ )
		if( name == Source[i] )
			return i;
	return -1;
}

bool CConnectMatrix::add_src( const string &name )
{
	if( src_id( name ) == -1 ){
		Source.push_back( name );
		NNConnect.AddCol();
		return true;
	}
	return false;
}

bool CConnectMatrix::del_src( const string &name )
{
	int inx = src_id( name );
	if( inx != -1 ){
		Source.erase( Source.begin()+inx );
		NNConnect.DelCol( inx );
		return true;
	}
	return false;
}

void CConnectMatrix::clear( void )
{
	Target.clear();
	Source.clear();
	NNConnect.ClearMatrix();
}

bool CConnectMatrix::connect( const CNNConnect &connect, const string &target, const string &source )
{
	int tinx = trg_id( target );
	int sinx = src_id( source );
	if( tinx != -1 && sinx != -1){
		NNConnect( tinx, sinx ) = connect;
		return true;
	}
	return false;
}

bool CConnectMatrix::del( const string &target, const string &source )
{
	int tinx = trg_id( target );
	int sinx = src_id( source );
	if( tinx != -1 && sinx != -1){
		NNConnect( tinx, sinx ).del_connect();
		return true;
	}
	return false;
}
