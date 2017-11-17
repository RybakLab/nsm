#ifndef __NSM_TYPES_H
#define __NSM_TYPES_H

#include "utilities.h"
#include "hhnstat.h"

#include <cmath>
#include <numeric>
//#include <functional>
#include <algorithm>
#include <functional>
#include <regex>

#include <locale>
#include <iomanip>
#include <fstream>
#include <queue>
#include <list>
#include <map>
#include <vector>

#ifdef __OMP__
#include <omp.h>
#endif //__OMP__

#ifdef __CONSOLE__
typedef unsigned long DWORD;
typedef unsigned int UINT;

struct __rect{
      long left;
      long top;
      long right;
      long bottom;
      };
#else
#include <process.h>

typedef RECT __rect;
#endif // __CONSOLE__

using std::abs;
using std::accumulate;
using std::fill;
using std::ctype;
using std::cout;
using std::endl;
using std::filebuf;
using std::fixed;
using std::getline;
using std::ios;
using std::ios_base;
using std::istream;
using std::ifstream;
using std::istringstream;
using std::list;
using std::locale;
using std::lower_bound;
using std::max;
using std::make_pair;
using std::map;
using std::multimap;
using std::ofstream;
using std::ostream;
using std::ostringstream;
using std::pair;
using std::queue;
using std::sort;
using std::swap;
using std::setprecision;
using std::setw;
using std::string;
using std::vector;
using std::upper_bound;
using std::use_facet;
using std::ws;

typedef vector<unsigned long> lvector;

enum _sumulation_errors_{
	_SIM_NOERRORS       = 0,
	_SIM_INIT_ERROR,
	_SIM_UPDATE_ERROR,
	_SIM_SHUTDOWN,
	_SIM_SYNCH_ERROR,
};

enum _ID_compartment_{
	_id_HHN_SOMA		= 0,
	_id_HHN_DENDRITE,
	_id_MAX_COMP,
};

enum _ID_connection_types_{
	_id_NN_EMPTY		= -1,
	_id_NN_OUTPUT,
	_id_NN_SOMA,
	_id_NN_DENDRITE,
	_id_NN_LAST_CONNECT,
};

enum _ID_Ion_{
     _id_Na_Ion          = 0,
     _id_K_Ion,
     _id_Ca_Ion,
     _id_Cl_Ion,
	 _id_MAX_ION,
     };

enum _ID_Pumps_{
	_id_NaK_Pump		= 0,
	_id_Ca_Pump,
	_id_MAX_PUMPS,
};

enum _ID_Channel_{
	_id_generic_Chan	=  0,// Generic channel
	_id_generic_NaPChan,    // NaP generic channel
	_id_generic_NaChan,     // Na generic channel
	_id_generic_KdrChan,    // Kdr generic channel
	_id_generic_KAChan,     // KA generic channel
	_id_generic_KCa,        // generic K(Ca)
	_id_generic_Can,        // calcium activated cation nonspecific channel
	_id_generic_HChan,      // H generic channel
#if defined( __LOCOMOTION__ ) && defined( __RESPIRATION__ )
	_id_NaChan,             // Na channel
	_id_KChan,              // K channel
	_id_NaFChan,            // NaF channel
	_id_NaPChan,            // NaP channel
	_id_KdrChan,            // K channel
	_id_KAChan,             // KA channel
	_id_CaLChan,            // CaL channel
	_id_MChan,              // M channel
	_id_CaTChan,            // CaT channel
	_id_CaNChan,            // CaN channel
	_id_KCChan,             // KC channel
	_id_KCaChan,            // KCa channel
	_id_HChan,              // H channel
	_id_LeakChan,           // Leak channel
	_id_resp_NaFChan,
	_id_resp_NaPChan,
	_id_resp_KdrChan,
	_id_resp_KAChan,
	_id_resp_CaLChan,
	_id_resp_CaTChan,
	_id_resp_KCaChan,
	_id_resp_LeakChan,
#elif defined( __LOCOMOTION__ )  && !defined( __RESPIRATION__ )
	_id_NaChan,             // K channel
	_id_KChan,              // K channel
	_id_NaFChan,            // NaF channel
	_id_NaPChan,            // NaP channel
	_id_KdrChan,            // K channel
	_id_KAChan,             // KA channel
	_id_CaLChan,            // CaL channel
	_id_MChan,              // M channel
	_id_CaTChan,            // CaT channel
	_id_CaNChan,            // CaN channel
	_id_KCChan,             // KC channel
	_id_KCaChan,            // KCa channel
	_id_HChan,              // H channel
	_id_LeakChan,           // Leak channel
#elif defined( __RESPIRATION__ ) && !defined( __LOCOMOTION__ )
	_id_resp_NaFChan,
	_id_resp_NaPChan,
	_id_KChan,              // K channel
	_id_resp_KdrChan,
	_id_NaFChan,            // NaF channel
	_id_NaPChan,            // NaP channel
	_id_KdrChan,            // K channel
	_id_resp_KAChan,
	_id_resp_CaLChan,
	_id_MChan,              // M channel
	_id_resp_CaTChan,
	_id_KCChan,             // KC channel
	_id_resp_KCaChan,
	_id_HChan,              // H channel
	_id_resp_LeakChan,
#endif // defined( __RESPIRATION__ ) && !defined( __LOCOMOTION__ )
	_id_SomaChan,           // Soma channel
	_id_DendrChan,          // Dendrite channel
	_id_MAX_CHAN,
};

enum _ID_Synapse_{
     _id_ExSyn			= 0, // Excitatory sinapse
#if defined( __RESPIRATION__ )
     _id_ExBSyn,			// Excitatory synapse B
#endif // defined( __RESPIRATION__ )
     _id_InhASyn,			// Inhibitory synapse A
     _id_InhBSyn,			// Inhibitory synapse B
     _id_MAX_SYN,
};

enum _ID_Formats_{
	_id_ASCII_GEN		= 0,
	_id_BIN_SCRC,
	_id_MAX_FORMAT,
};

enum _ID_NN_unit_{
	_id_Network			= 0,
	_id_NNPopulat,		// sources & targets
	_id_NNFeedback,		// sources
	_id_NNDrive,		// sources
	_id_NNOutput,		// targets
	_id_NNControl,
	_id_MAX_NNUnit
};

enum _ID_Body_units{
	_id_Body			= _id_MAX_NNUnit,
	_id_Vertex,
	_id_Link,
	_id_Joint,
	_id_Muscle,
	_id_Ground,
	_id_Treadmill,
	_id_MAX_UBODY,
};

enum _ID_Body_view_{
	_id_VW_None			= -1, 
	_id_Coordinate_X	=  0,
	_id_Coordinate_Y	=  1,
	_id_Velocity_X		=  2,
	_id_Velocity_Y		=  3,

	_id_Angle			=  0,
	_id_Angle_Velocity	=  1,

	_id_MuscleForce		=  0,
	_id_TotalLength		=  1,
	_id_TotalVelocity	=  2,
	_id_MuscleLength	=  3,
	_id_MuscleVelocity	=  4,
	_id_TendonLength	=  5,
	_id_GroundForce_X	=  0,
	_id_GroundForce_Y	=  1,
	_id_MAX_VWBODY
};

enum _ID_Controlled{
	_id_CR_None			= -1, 
	
	_id_KIons			= _id_MAX_CHAN+_id_MAX_SYN,
	_id_NUM_CONTROLLED,
	_id_MAX_CTRL		= 0x040,
};

enum _ID_Out{
	_id_OUT1			= 0,
	_id_OUT12,
	_id_OUT2,
	_id_NUM_OUTS
};

enum _ID_FB{
	_id_FbV				= 0,
	_id_FbL,
	_id_FbEMG,
	_id_NUM_FBS,
	_id_FbF				= 0,
	_id_FbFmax,
	_id_FbTGX			= 0,
	_id_FbTGY
};

#define BEG_IMAGE_INDEX 14

enum _grid_types_{
	GRID_NONE      = 0x0000,
	GRID_BOOL      = 0x0001,
	GRID_INT       = 0x0002,
	GRID_DOUBLE    = 0x0004,
	GRID_STRING    = 0x0008,
	GRID_COMBO     = 0x0010,
	GRID_COLLAPSE  = 0x0020,
	GRID_ALLTYPES  = 0x00FF, 
	GRID_READONLY  = 0x0100,
	GRID_POSITIVE  = 0x0200,
	GRID_EXCL_ZERO = 0x0400,
	GRID_NEGATIVE  = 0x0800,
	GRID_ALLNUM    = 0x0F00,
	GRID_DEL_ROW   = 0x2000,
	GRID_ADD_ROW   = 0x4000,
	GRID_PROCESS   = 0x8000,
	GRID_ALLATTR   = 0xF000,
};

extern locale _default_loc;
extern const char *_ProjectName;
extern const char *_CompartmentNames[_id_MAX_COMP];
extern const char *_ChannelNames[_id_MAX_CHAN];
extern const char *_IonsNames[_id_MAX_ION];
extern const char *_PumpsNames[_id_MAX_PUMPS];
extern const char *_SynapseNames[_id_MAX_SYN];
extern const char *_ConnectionTypes[_id_NN_LAST_CONNECT];
extern const char *_FormatNames[_id_MAX_FORMAT];
extern double _FileVersion;

class _Grid_Key{
        public:
            _Grid_Key( void )
            {
            };
            _Grid_Key( const string &path, const string &name )
                : Path( path ), Name( name )
            {
            };
            _Grid_Key( const _Grid_Key &tabkey )
                : Path( tabkey.Path ), Name( tabkey.Name )
            {
            };
            ~_Grid_Key( void )
            {
            };
        public:
            _Grid_Key &operator = ( const _Grid_Key &tabkey )
            {
             Path = tabkey.Path;
             Name = tabkey.Name;
             return *this;
            };
            bool operator < ( const _Grid_Key &tabkey ) const
            {
             if( Path < tabkey.Path )
                 return true;
             if( Path == tabkey.Path && Name < tabkey.Name )
                 return true;
             return false;
            };
        public:
            string Path;
            string Name; 
        };

class _Grid_Element{
        public:
            _Grid_Element( void )
            {
             IsStyle = false;
             Position = make_pair( -1, -1 );
             Type = GRID_NONE;
            };
            _Grid_Element( const string &data, int type, const pair<int,int> &position, char delim = '\t' )
                         : Position( position ), InitStr( data )
            {
             IsStyle = false;
             Type = type;
		 if(!( Type&GRID_COMBO ))
			Data = data;
		 else
			Data = data.substr( 0, data.find( delim, 0 ));
            };
            _Grid_Element( const _Grid_Element &x )
                         : Position( x.Position ), Data( x.Data ), InitStr( x.InitStr )
            {
             IsStyle = x.IsStyle;
             Type = x.Type;
            };
            ~_Grid_Element( void )
            {
            };
        public:
            _Grid_Element &operator = ( const _Grid_Element &x )
            {
             IsStyle = x.IsStyle;
             Position = x.Position;
             Type = x.Type;
             Data = x.Data;
		 InitStr = x.InitStr;
             return *this;
            };
        public:
            bool IsStyle;
            int Type;
            string Data;
		string InitStr;
            pair<int, int> Position; // row, col
        };

///////////////////////////////////////////////////////////////////////////////
// class t_data
class t_data{
      public:
            t_data( void )
            {
             Value = 0.0;
             Variance = 0.0;
            };
            t_data( double data )
            {
             Value = data;
             Variance = 0.0;
            };
            t_data( double data, double var )
            {
             Value = data;
             Variance = var;
            };
           ~t_data( void )
           {
           };
      public:
            t_data &operator = ( double data )
            {
             Value = data;
             Variance = 0.0;
             return *this;
            };
            t_data &operator = ( const t_data &data )
            {
             Value = data.Value;
             Variance = data.Variance;
             return *this;
            };
            operator double ( void )
            {
             CStat val( Value, Variance );
             return double( val );
            };
      public:
            double Value;
            double Variance;
      };

///////////////////////////////////////////////////////////////////////////////
// class io_buffer
class io_buffer{
	public:
		io_buffer( size_t nl ) : NL( nl ){ Buffer = NULL; };
		~io_buffer( void ){	if( Buffer ) delete[] Buffer; };
	public:
		double *operator[] ( size_t layer )
		{
			layer = layer < NL? layer: NL-1;
			return Buffer+layer*Size;
		};
	public:
		size_t size( void ) const { return Size; };
		size_t size_nl( void ) const { return NL; };
		size_t create( size_t size )
		{
			if( Buffer )
				delete[] Buffer;
			Size = size;
			Buffer = new double[NL*Size];
			return Size*NL;
		};
		size_t copy( const io_buffer &src )
		{
			NL  = src.NL; create( src.Size );
			memcpy( Buffer, src.Buffer, sizeof( double )*Size*NL );
			return Size*NL;
		};
	private:
		double *Buffer;
		size_t NL;		// total number of layers
		size_t Size;	// buffer size
};

inline ostream &operator << ( ostream &s, const t_data &x )
{
 s << x.Value;
 if( x.Variance >= 0.0 ){
     s << "(" << x.Variance << ")";
     }
 return s;
}

inline istream &operator >> ( istream &s, t_data &x)
{
 char ch =0;
 double val = 0, var = 0;
 bool OK = true;

 s >> val;
 s.get(ch);
 if( ch == '(' ){
     s >> var;
	 s.get(ch);
	 OK = (ch == ')');
	 }
 else{
     s.putback(ch);
     }
 if(s && OK){
  	x.Value=val;
	x.Variance=var;
	}
 return s;
}

/////////////////////////////////////////////////////////////////////////////
// hhn_pair class
template <class T>
class hhn_pair{
	public:
		hhn_pair( void ) : X(0), Y(0){}; 
		hhn_pair( T x ) : X( x ), Y(0){};
		hhn_pair( T x, T y ) : X( x ), Y( y ){};
		hhn_pair( const hhn_pair<T> &pair ) : X( pair.X ), Y( pair.Y ){};
		~hhn_pair( void ){};
	public:
            hhn_pair<T> &operator = ( const hhn_pair<T> &point )
		{
			X = point.X;
			Y = point.Y;
			return *this;
		};
		bool operator < ( const hhn_pair<T> &a ) const
		{
			return X < a.X;
		};
	public:
		T X;
		T Y;
	};

template <class T>
inline ostream &operator << ( ostream &s, const hhn_pair<T> &point )
{
	return s << '(' << point.X << ',' << point.Y << ')';
};

template <class T>
inline istream &operator >> ( istream &s, hhn_pair<T> &point )
{
	T x = 0, y = 0; char c = 0;
	s >> c;
	if( c == '(' ){
		s >> x >> c;
		if( c == ',' )
			s >> y >> c;
		if( c != ')' ){
			s.putback( c );
			s.clear( ios_base::badbit );
		}
	}
	else{
		s.putback( c );
		s.clear( ios_base::badbit );
	}
	if( s )
		point = hhn_pair<T>( x, y );
	return s;
};

inline ostream &operator << ( ostream &s, const __rect &rect )
{
	return s << '(' << rect.left << ',' << rect.top << ',' << rect.right << ',' << rect.bottom << ')';
};

inline istream &operator >> ( istream &s, __rect &rect )
{
	float left = -1;
	float top = -1;
	float right = -1;
	float bottom =-1;
	char c = 0;
	class _BadSymbol_{};
	try{
		s >> c;
		if( c != '(' ) throw _BadSymbol_();
		s >> left >> c;
		if( c != ',' ) throw _BadSymbol_();
		s >> top >> c;
		if( c != ',' ) throw _BadSymbol_();
		s >> right >> c;
		if( c != ',' ) throw _BadSymbol_();
		s >> bottom >> c;
		if( c != ')' ) throw _BadSymbol_();
	}
	catch( _BadSymbol_ ){
		s.putback(c);
		s.clear(ios_base::badbit);
	}
	if( s ){
		rect.left = int( left );
		rect.top = int( top );
		rect.right = int( right );
		rect.bottom = int( bottom );
	}
	return s;
};

extern int getChannelID( const char *name );
#endif// __NSM_TYPES_H
