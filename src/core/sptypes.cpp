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
#if defined( __RESPIRATION__ )
	"Excitatory2",
#endif // defined( __RESPIRATION__ )
	"Inhibitory I",
	"Inhibitory II",
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

#ifdef __MECHANICS__
const char *_ProjectName = "Spcord v2.05";
#else
const char *_ProjectName = "NSM v2.05";
#endif //__MECHANICS__

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
          if( string( name ) == _ChannelNames[i] )
              return i;
          }
      }
 return -1;
}
