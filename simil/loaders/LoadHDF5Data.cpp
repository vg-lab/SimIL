/*
 * @file  LoadHDF5Data.h
 * @brief
 * @author Aaron Sújar <aaron.sujar@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
 */

#include "LoadHDF5Data.h"

#include "../storage/StorageSparse.h"

namespace simil
{

LoadHDF5Data::LoadHDF5Data() : LoadSimData() { _h5Network = NULL; }

LoadHDF5Data::~LoadHDF5Data()
{
    if (_h5Network != NULL)
        delete _h5Network;
}

DataSet *LoadHDF5Data::LoadNetwork(const std::string &filePath_,
                                   const std::string &target)
{
    DataSet *dataset = new DataSet(filePath_, TBlueConfig, target);

    if (_h5Network == NULL)
    {
        _h5Network = new H5Network(filePath_);
        _h5Network->load();
    }

    dataset->setGids(_h5Network->getGIDs());

    dataset->setPositions(_h5Network->getComposedPositions());

    SubsetEventManager subsetEventManager;
    auto subsetIts = _h5Network->getSubsets();
    for (simil::SubsetMapCIt it = subsetIts.first; it != subsetIts.second; ++it)
        subsetEventManager.addSubset(it->first, it->second);

    dataset->setSubset(subsetEventManager);

    return dataset;
}

SimulationData *LoadHDF5Data::LoadSimulationData(const std::string &filePath_,
                                                 const std::string &target)
{
    SimulationData *simulationdata = new SimulationData(filePath_, THDF5, target);

    if (_h5Network == NULL)
    {
        _h5Network = new H5Network(filePath_);
        _h5Network->load();
    }

    simil::H5Spikes spikeReport(*_h5Network, target);
    spikeReport.Load();

    simil::StorageSparse *newStorage = new StorageSparse("Spikes",
                                                         tTYPE_UINT,
                                                         TSimSpikes);

    newStorage->setSpikes(spikeReport.spikes());

    simulationdata->setStartTime(spikeReport.startTime());
    simulationdata->setEndTime(spikeReport.endTime());

    simulationdata->addStorage(newStorage);

    return simulationdata;
}

} // namespace simil
