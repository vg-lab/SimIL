/*
 * @file  CSVActivity.h
 * @brief
 * @author Sergio E. Galindo <sergio.galindo@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
 */
#ifndef __QSIMIL_CSVACTIVITY__
#define __QSIMIL_CSVACTIVITY__

#include "types.h"
#include "CSVNetwork.h"

namespace simil
{

  class CSVActivity
  {

  public:

    CSVActivity( const CSVNetwork& network,
                 const std::string& filename,
                 char separator = ',',
                 bool headerLine = false );

    virtual ~CSVActivity( void );

    virtual void load( void ) = 0;

  protected:

    const CSVNetwork* _network;

    std::string _fileName;
    char _separator;
    bool _headerLine;

  };

  class CSVSpikes : public CSVActivity
  {
  public:

    CSVSpikes( const CSVNetwork& network,
               const std::string& filename,
               char separator = ',',
               bool headerLine = true );

    ~CSVSpikes( void );

    void load( void );

    TSpikes spikes( void );
    float startTime( void );
    float endTime( void );

  protected:

    float _startTime;
    float _endTime;

//    std::vector< std::vector< int >> _values;
    std::multimap< float, uint32_t > _spikes;
  };

}


#endif /* __QSIMIL_CSVACTIVITY__ */
