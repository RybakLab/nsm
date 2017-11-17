/////////////////////////////////////////////////////////////////////////////
// hhnconnect.h
#ifndef __HHN_CONNECT_H
#define __HHN_CONNECT_H

#include "hhnstat.h"
#include "cmatrix.h"


/////////////////////////////////////////////////////////////////////////////
// class CHhnConnect
class CHhnConnect{
	public: //--- constructor
		CHhnConnect( void );
		CHhnConnect( const CHhnConnect &connect );
virtual	~CHhnConnect( void ){};
	public:
		CHhnConnect &operator = ( const CHhnConnect &connect );
		bool operator == ( const CHhnConnect &x ) const;
	public:	//--- change of weights
		CStat &weight( int sid ){ return Weights[sid]; };
		bool load( istream &file );  // keep it to provide backward compatibility
	public:
		double Probability;
		bool Sigma[_id_MAX_SYN];
		bool Depression[_id_MAX_SYN];
		bool PreInh[_id_MAX_SYN];
		CStat Weights[_id_MAX_SYN];
};

/////////////////////////////////////////////////////////////////////////////
// class CConnect
class CConnect{
	public:
		CConnect( void ) : Connect(), Type( _id_NN_EMPTY ){};
		CConnect( const CHhnConnect &connect, int type ): Connect( connect ), Type( type ){};
		CConnect( const CConnect &connect ): Connect( connect.Connect ), Type( connect.Type ){};
virtual	~CConnect( void ){};
	public:
		CConnect &operator = ( const CConnect &connect );
		bool operator == ( const CConnect &x ) const;
	public:
		bool empty( void ){ return Type == _id_NN_EMPTY; };
		bool load( istream &file );
		void save( ostream &file ) const;
	public:
		int Type;
		CHhnConnect Connect;
};

/////////////////////////////////////////////////////////////////////////////
// class CNNConnect
class CNNConnect{
	public:
		CNNConnect( void ) : Connect(), ChainT( "" ), ChainS( "" ), Empty( true ){};
		CNNConnect( const CNNConnect &connect ) : Connect( connect.Connect ), ChainT( connect.ChainT ), ChainS( connect.ChainS ), Empty( connect.Empty ){};
virtual	~CNNConnect( void ){};
	public:
		CNNConnect &operator = ( const CNNConnect &connect );
		const CConnect &operator []( size_t index ) const{ return Connect[index]; };
		bool operator == ( const CNNConnect &x ) const;
	public:
		size_t size( void ) const{ return Connect.size(); };
		bool empty( void ) const{ return Empty||Connect.empty(); };
		const CConnect &get_connect( int type ) const;
		void add_connect( const CConnect &weight, bool replace = false );
		void del_connect( void );
		bool load( istream &file );
		void save( ostream &file ) const;
	public:
		bool Empty;
		CConnect nill;
		vector<CConnect> Connect;       // a link between pair of network elements
		string ChainT;
		string ChainS;
};

/////////////////////////////////////////////////////////////////////////////
// CConnectMatrix class
class CConnectMatrix{
	public:
		CConnectMatrix( void ){};
		CConnectMatrix( const CConnectMatrix &matrix ): Target( matrix.Target ), Source( matrix.Source ), NNConnect( matrix.NNConnect ){};
virtual	~CConnectMatrix( void ){};
	public:
		CNNConnect &operator()( size_t target, size_t source ){ return NNConnect( target, source ); };
		CNNConnect &operator()( string &target, string &source ){ return NNConnect( trg_id( target ), src_id( source )); };
		CNNConnect &operator()( const string &target, const string &source ){ return NNConnect( trg_id( target ), src_id( source )); };
		CConnectMatrix &operator =( const CConnectMatrix &matrix );
	public:
		size_t size( void ) const{ return NNConnect.size(); };
		size_t size_src( void ) const{ return NNConnect.GetNumCol(); };
		size_t size_trg( void ) const{ return NNConnect.GetNumRow(); };

		vector<string> &name_src( void ){ return Source; };
		vector<string> &name_trg( void ){ return Target; };
		string name_src( size_t inx ) const { return Source[inx]; };
		string name_trg( size_t inx ) const { return Target[inx]; };
		vector<CNNConnect> &sources( const string &target ){ return NNConnect.GetRow( trg_id( target )); };
		vector<CNNConnect> &targets( const string &source ){ return NNConnect.GetCol( src_id( source )); };

		bool rename_src( const string &oldname, const string &newname );
		bool rename_trg( const string &oldname, const string &newname );
		bool occupied( const string &name );
		void clear( void );
		bool connect( const CNNConnect &connect, const string &target, const string &source );
		bool add_src( const string &name );
		bool add_trg( const string &name );
		bool del( const string &target, const string &source );
		bool del_src( const string &name );
		bool del_trg( const string &name );

		void chain( pair<string,string> &master, pair<string,string> &slave );
		void modify( pair<string,string> &link );
		void unchain( pair<string,string> &link );
	private:
		void chain_all( list< pair<string,string> > &chain );
		int src_id( const string &name ) const;
		int trg_id( const string &name ) const;
	private:
		vector<string> Target;          // row names
		vector<string> Source;          // column names
		CMatrix<CNNConnect> NNConnect;  // format: rows are targets, columns are sources
};

#endif // __HHN_CONNECT_H
