/*
 * @file	GDFActivity.h
 * @brief
 * @author Sergio E. Galindo <sergio.galindo@urjc.es> 
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *					Do not distribute without further notice.
 */

#ifndef GDFACTIVITY_H_
#define GDFACTIVITY_H_

#include "types.h"


namespace simil
{
  class GDFActivity
  {
  public:

    GDFActivity( void );

    ~GDFActivity( void );

    void load( const std::string& filePath,
               const std::string& separator = "\t",
               bool readFirstLine = true );

    const std::vector< std::string >& fileName( void ) const;
    const std::vector< std::string >& pattern( void ) const;

    TSpikes spikes( void );

    float startTime( void ) const;
    float endTime( void ) const;

  protected:

    TSpikes _spikes;

    std::vector< std::string > _fileNames;
    std::vector< std::string > _separators;

    unsigned int _totalRecords;

    float _startTime;
    float _endTime;

  };

}



#endif /* GDFACTIVITY_H_ */
