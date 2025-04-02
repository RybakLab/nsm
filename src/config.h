#ifndef _CONFIG_H_
#define _CONFIG_H_

/////////////////////////////////////////////////////////////////////////////
// uncomment the definition below if you want to use
// linux version of the project (command line only)
//#define __LINUX__

/////////////////////////////////////////////////////////////////////////////
// uncomment the definition below if you want to use
// the channels for respiration modelling
#define __RESPIRATION__

/////////////////////////////////////////////////////////////////////////////
// uncomment the definition below if you want to include
// the bio-mechanical part of the project 
// (both 2D & 3D mechanical model of a cat)
#define __LOCOMOTION__

/////////////////////////////////////////////////////////////////////////////
// uncomment the definition below if you want to use the package in compatible
// mode (debugging only)
//#define __COMPATIBLE__

#ifdef __LOCOMOTION__
/////////////////////////////////////////////////////////////////////////////
// uncomment the definition below if you want to include
// the bio-mechanical part of the project 
// (2D mechanical model of a cat)
//#define __MECHANICS_2D__ 

/////////////////////////////////////////////////////////////////////////////
// uncomment the definition below if you want to include
// the bio-mechanical part of the project 
// (3D mechanical model of a cat based on MuJoCo engine)
//#define __MECHANICS_3D__ 

#if defined (__MECHANICS_2D__)
#undef __MECHANICS_3D__
/////////////////////////////////////////////////////////////////////////////
// uncomment the definition below if you want to use
// the latest model of a muscle
//#define __NEW_MUSCLE__
#endif //__MECHANICS_2D__
#endif /*__LOCOMOTION__*/

/////////////////////////////////////////////////////////////////////////////
// uncomment the definition below if you want to use
// the command-line version of the project
//#define __CONSOLE__ 

/////////////////////////////////////////////////////////////////////////////
// uncomment the definition below if you want to use
// MPI optimized version of the project (no GUI support)
//#define __MPI__ 

/////////////////////////////////////////////////////////////////////////////
// uncomment the definition below if you want to use
// OpenMP optimized version of the project
#define __OMP__ 

/////////////////////////////////////////////////////////////////////////////
// uncomment the definition below if you want to use
// Runge-Kutta integtration method 
#define __RK__

/////////////////////////////////////////////////////////////////////////////
// uncomment the definition below if you want to use
// multi-threading calculations
#ifdef __LINUX__
  #define __CONSOLE__
#endif // __LINUX__
/////////////////////////////////////////////////////////////////////////////
// uncomment the definition below if you want to use
// boost specific memory managements
#define __BOOST__ 

#include "nsmdebug.h"
#endif // _CONFIG_H_
