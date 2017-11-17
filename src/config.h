#ifndef _CONFIG_H_
#define _CONFIG_H_

/////////////////////////////////////////////////////////////////////////////
// uncomment the definition below if you want to use
// linux version of the project (command line only)
//#define __LINUX__

/////////////////////////////////////////////////////////////////////////////
// uncomment the definition below if you want to use
// the channels for locomotion modelling
#define __LOCOMOTION__

/////////////////////////////////////////////////////////////////////////////
// uncomment the definition below if you want to use
// the channels for respiration modelling
#define __RESPIRATION__

/////////////////////////////////////////////////////////////////////////////
// uncomment the definition below if you want to include
// the bio-mechanical part of the project 
// (2D mechanical model of a cat and muscles)
//#define __MECHANICS__ 

#ifdef __MECHANICS__
/////////////////////////////////////////////////////////////////////////////
// uncomment the definition below if you want to use
// the latest model of a muscle
//#define __NEW_MUSCLE__
#endif //__MECHANICS__

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

#endif // _CONFIG_H_
