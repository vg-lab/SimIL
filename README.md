# SimiL - (Simulation Input/Output Library) provides and eases the access to different data formats
(c) 2015-2020. GMRV / URJC

www.gmrv.es
gmrv@gmrv.es

## Introduction

SimiL consists of a library which is used to read brain simulation datasets. It is ready to be used with BlueConfig, specific HDF5 and CSV datasets. Latest version
also loads remote streaming data using a REST API for an in-situ pipeline.

## Dependencies

### Strong dependences:

* HDF5
* Boost
* vmmlib(*)
* HDF5 1.10

(*) Note: These dependencies will be automatically downloaded and compiled with
the project.

### Weak dependences

* Qt 5.X: including Qt5Core, Qt5Widgets
* Brion: enables BlueConfig support.
* ZeroEQ: enables ZeroEQ remote data exchange

## Building

SimiL has been succesfully built with GCC 6 and used on Ubuntu 16.04 / 18.04, Mac OSX
Yosemite. The following steps should be enough to build it:

```bash
git clone --recursive https://gitlab.gmrv.es/nsviz/SimIL.git SimiL
mkdir SimiL/build && cd SimiL/build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCLONE_SUBPROJECTS=ON
make
```

# Usage

```c++
#include <simil/simil.h>
#ifdef SIMIL_USE_BRION
#include <simil/loaders/LoadblueConfigData.h>
#endif
#include <simil/loaders/LoadHDF5Data.h>
#include <simil/loaders/LoadCSVData.h>

simil::LoadSimData* importer;

if ( simtype == "-bc" )
{
  importer = new simil::LoadblueConfigData( );
}else
if ( cmdoption == "-csv" )
{
  importer = new simil::LoadCSVData( );
}
else if ( cmdoption == "-h5" )
{
  importer = new simil::LoadHDF5Data( );
}
else //.. promt usage message ...

simil::SimulationData* simData =
    importer->LoadSimulationData( path, secondaryPath );

  std::cout << "Loaded GIDS: " << simData->gids( ).size( ) << std::endl;
  std::cout << "Loaded positions: " << simData->positions( ).size( )
            << std::endl;

```
