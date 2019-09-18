/*
 * @file  LoadInsituData.h
 * @brief
 * @author Aaron Sújar <aaron.sujar@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
 */


#ifndef __SIMIL__LOADINSITUDATA_H__
#define __SIMIL__LOADINSITUDATA_H__

#include "LoadSimData.h"

namespace simil
{
    class LoadInsituData : LoadSimData
    {

        virtual SimulationData LoadSimulationData();
        virtual DataSet LoadNetwork();


    }


}//namespace



#endif /* __SIMIL__LOADINSITUDATA_H__ */