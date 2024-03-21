#include "precompile.h"

#include "sptypes.h"

const char *_CompartmentNames[_id_MAX_COMP] = {
	"Soma",
	"Dendrite"
};

const char *_ChannelNames[_id_MAX_CHAN] = {
	"Generic channel",
	"NaP channel (generic)",
	"Na channel (generic)",
	"Kdr channel (generic)",
	"KA channel (generic)",
	"KCa channel (generic)",
	"Can channel (generic)",
	"H channel (generic)",
#if defined( __LOCOMOTION__ ) && defined( __RESPIRATION__ )
	"Na channel (new)",
	"K channel (new)",

	"NaF channel (Butera)",
	"NaP channel (Butera)",
	"Kdr channel (Butera)",

	"KA channel",
	"CaL channel (new)",
	"M channel",
	"CaT channel",
	"CaN channel (new)", 
	"KC channel",
	"KCa channel (new)",
	"H channel",
	"Leak channel (new)",
	"resp Na-fast channel",
	"resp NaP channel",
	"resp Kdr channel",
	"resp KA channel",
	"resp CaL channel",
	"resp CaT channel",
	"resp KCa channel",
	"resp Leak channel",
#elif defined( __LOCOMOTION__ )  && !defined( __RESPIRATION__ )
	"Na channel (new)",
	"K channel (new)",

	"NaF channel (Butera)",
	"NaP channel (Butera)",
	"Kdr channel (Butera)",

	"KA channel",
	"CaL channel (new)",
	"M channel",
	"CaT channel",
	"CaN channel (new)", 
	"KC channel",
	"KCa channel (new)",
	"H channel",
	"Leak channel (new)",
#elif defined( __RESPIRATION__ ) && !defined( __LOCOMOTION__ )
	"Na fast",
	"NaP",
	"K",  
	"Kdr",

	"NaF channel (Butera)",
	"NaP channel (Butera)",
	"Kdr channel (Butera)",

	"KA",
	"CaL",
	"M",  
	"CaT",
	"KC", 
	"KCa",
	"H",  
	"Leak",			
#endif // defined( __LOCOMOTION__ ) || defined( __RESPIRATION__ )

	"Soma channel",               // Soma channel
	"Dendrite channel",           // Dendrite channel
};

const char *_IonsNames[_id_MAX_ION] = {
	"Na ions",
	"K ions",
	"Ca ions",
	"Cl ions",
};

const char *_PumpsNames[_id_MAX_PUMPS] ={
	"NaK pump",
	"Ca pump",
};

const char *_SynapseNames[_id_MAX_SYN] = {
	"Excitatory",
	"Excitatory2",
	"Inhibitory I",
	"Inhibitory II",
	"Additional synapse 1",
	"Additional synapse 2",
	"Additional synapse 3",
	"Additional synapse 4",
};

const char *_ConnectionTypes[_id_NN_LAST_CONNECT] = {
	"Output",
	"Soma",
	"Dendrite",
};

const char *_FormatNames[_id_MAX_FORMAT] = {
	"ASCII (generic)",
	"BINARY (SCRC format)",
};

#if defined __MECHANICS_2D__ || __MECHANICS_3D__
	const char *_ProjectName = "Spcord v2.05";
#else
	const char *_ProjectName = "NSM v2.05";
#endif //__MECHANICS_2D__

double _FileVersion = 2.05;

#ifdef __LINUX__
    locale _default_loc("en_US");
#else
    locale _default_loc("english");
#endif // LINUX

int getChannelID( const char *name )
{
	for( int i = 0; i < _id_MAX_CHAN; i++ ){
		if( _ChannelNames[i] != NULL ){
			if( string( name ) == _ChannelNames[i] ){
				return i;
			}
		}
	}
#if defined( __LOCOMOTION__ ) && defined( __RESPIRATION__ )
	const char *r_names[15] = {
		"Na fast",
		"NaP",
		"K",  
		"Kdr",

		"NaF channel (Butera)",
		"NaP channel (Butera)",
		"Kdr channel (Butera)",

		"KA",
		"CaL",
		"M",  
		"CaT",
		"KC", 
		"KCa",
		"H",  
		"Leak",			
	};
	for( int i = 0; i < 15; i++ ){
		if( string( name ) == r_names[i] ){
			switch( i ){
				case 0:
					return _id_resp_NaFChan;
				case 1:
					return _id_resp_NaPChan;
				case 2:
					return _id_KChan;
				case 3:
					return _id_resp_KdrChan;
				case 4:
					return _id_NaFChan;
				case 5:
					return _id_NaPChan;
				case 6:
					return _id_KdrChan;
				case 7:
					return _id_resp_KAChan;
				case 8:
					return _id_resp_CaLChan;
				case 9:
					return _id_MChan;
				case 10:
					return _id_resp_CaTChan;
				case 11:
					return _id_KCChan;
				case 12:
					return _id_resp_KCaChan;
				case 13:
					return _id_HChan;
				case 14:
					return _id_resp_LeakChan;
			}
		}
	}
#endif
	return -1;
}

/*
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

"Na fast",
"NaP",
"K",  
"Kdr",

"NaF channel (Butera)",
"NaP channel (Butera)",
"Kdr channel (Butera)",

"KA",
"CaL",
"M",  
"CaT",
"KC", 
"KCa",
"H",  
"Leak",			

*/