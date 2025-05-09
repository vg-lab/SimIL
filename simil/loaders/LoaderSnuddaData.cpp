/*
 * Copyright (c) 2015-2025 VG-Lab/URJC.
 *
 * Authors: Felix de las Pozas <felix.delaspozas@urjc.es>
 *
 * This file is part of SimIL <https://github.com/vg-lab/SimIL>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include "LoaderSnuddaData.h"

#include <unordered_map>
#include <vector>
#include <assert.h>

constexpr int TO_MICROMETERS = 1000000;

namespace
{
    bool HDF5pathExists(const hid_t id, const std::string& path)
    {
        return H5Lexists(id, path.c_str(), H5P_DEFAULT) > 0;
    }

    std::vector<std::string> listChildGroups(const H5::Group& group)
    {
        std::vector<std::string> childGroups;
        hsize_t n_objs = group.getNumObjs();
        for (hsize_t i = 0; i < n_objs; ++i) {
            std::string name = group.getObjnameByIdx(i);
            H5G_obj_t type = group.getObjTypeByIdx(i);
            if (type == H5G_GROUP) {
                childGroups.push_back(name);
            }
        }
        return childGroups;
    }
} // namespace

namespace simil
{
    std::unordered_map<uint32_t, vmml::Vector3f> SnuddaLoader::loadNeurons() const
    {
        H5::H5File networkFile(_networkFilename, 0);

        assert(HDF5pathExists(networkFile.getId(), "network/neurons/neuron_id") && HDF5pathExists(networkFile.getId(), "network/neurons/position") &&
               HDF5pathExists(networkFile.getId(), "network/neurons/rotation") && HDF5pathExists(networkFile.getId(), "meta/voxel_size"));

        auto idsDS = networkFile.openDataSet("network/neurons/neuron_id");
        auto positionDS = networkFile.openDataSet("network/neurons/position");
        auto rotationDS = networkFile.openDataSet("network/neurons/rotation");
        auto voxelSizeDS = networkFile.openDataSet("meta/voxel_size");

        hsize_t dims;
        idsDS.getSpace().getSimpleExtentDims(&dims);

        std::vector<uint32_t> ids(dims);
        std::vector<std::array<double, 3>> position(dims);
        std::vector<std::array<double, 9>> rotation(dims);

        idsDS.read(ids.data(), H5::PredType::INTEL_I32);
        positionDS.read(position.data(), H5::PredType::IEEE_F64LE);
        rotationDS.read(rotation.data(), H5::PredType::IEEE_F64LE);

        std::unordered_map<uint32_t, vmml::Vector3f> neurons;

        for (size_t i = 0; i < ids.size(); ++i) {
            vmml::Vector3f pos(position[i][0], position[i][1], position[i][2]);
            pos *= TO_MICROMETERS;

            neurons.insert({ids[i], pos});
        }

        idsDS.close();
        positionDS.close();
        rotationDS.close();

        networkFile.close();

        return neurons;
    }

    std::unordered_map<uint32_t, vmml::Vector3f> SnuddaLoader::loadSynapses() 
    {
        std::unordered_map<uint32_t, vmml::Vector3f> data;

        H5::H5File networkFile(_networkFilename, 0);

        assert(HDF5pathExists(networkFile.getId(), "meta/voxel_size") &&
               HDF5pathExists(networkFile.getId(), "network/synapses") &&
               HDF5pathExists(networkFile.getId(), "meta/simulation_origo"));

        // From https://github.com/Hjorthmedh/Snudda/blob/master/snudda/utils/export_sonata.py : 743
        //   Convert synapse coordinates(in SWC reference frame) to SectionID
        //   and section_X(fractional distance along dendritic section)
        auto voxelSizeDs = networkFile.openDataSet("meta/voxel_size");
        auto synapsesDs = networkFile.openDataSet("network/synapses");
        auto origoDs = networkFile.openDataSet("meta/simulation_origo");

        hsize_t dims;
        synapsesDs.getSpace().getSimpleExtentDims(&dims);

        double voxelSize;
        std::vector<std::array<int32_t, 13>> synapses(dims);

        std::vector<double> simulation_origo(3, 0);

        voxelSizeDs.read(&voxelSize, H5::PredType::IEEE_F64LE);
        synapsesDs.read(synapses.data(), H5::PredType::INTEL_I32);
        origoDs.read(simulation_origo.data(), H5::PredType::IEEE_F64LE);

        const vmml::Vector3f origoVector(simulation_origo[0], simulation_origo[1], simulation_origo[2]);
        GIDVec subset;
        uint32_t idGenerator = 0;
        for (auto& synapse : synapses) {
            // https: // github.com/Hjorthmedh/Snudda/blob/master/snudda/utils/load.py#L621 : 726
            // synapse_coords = synapses[:, 2:5] * self.data["voxel_size"] + self.data["simulation_origo"]

            //  0 : source_cell_id, 1 : dest_cell_id, 2 : voxel_x, 3 : voxel_y, 4 : voxel_z,
            //  5 : hyper_voxel_id, 6 : channel_model_id,
            //  7 : source_axon_soma_dist(not SI scaled 1e6, micrometers),
            //  8 : dest_dend_soma_dist(not SI scalled 1e6, micrometers)
            //  9 : dest_seg_id, 10 : dest_seg_x(int 0 - 1000, SONATA wants float 0.0 - 1.0)
            //  11 : conductance(int, not SI scaled 1e12, in pS)
            //  12 : parameterID

            // Channel model id defines the json file where the synapse model is described (inhibitory/excitatory)

            vmml::Vector3f position(synapse[2], synapse[3], synapse[4]);
            position *= static_cast<float>(voxelSize);
            position += origoVector;
            position *= TO_MICROMETERS; // sysnapse position seems to be in meters too

            subset.push_back(idGenerator);
            data.emplace(idGenerator++, position);
        }

        _subsetEventManager.addSubset("synapses", subset);

        voxelSizeDs.close();
        synapsesDs.close();
        origoDs.close();

        networkFile.close();

        return data;
    }

    void SnuddaLoader::loadNeuronGroups()
    {
        std::map<std::string, simil::TGIDVect> groups;

        H5::H5File networkFile(_networkFilename, 0);

        const auto dsId = networkFile.getId();

        hsize_t dims;
        if (HDF5pathExists(dsId, "network/neurons/name")) {
            auto namesDs = networkFile.openDataSet("network/neurons/name");
            auto stringType = namesDs.getStrType();

            namesDs.getSpace().getSimpleExtentDims(&dims);

            auto buffer = new char[dims * 6]; // fixed size
            H5::DataSpace dataspace = namesDs.getSpace();
            namesDs.read(&buffer[0], stringType, dataspace);

            int count = 0;
            for (hsize_t idx = 0; idx < dims; ++idx) {
                auto name = std::string(&buffer[count], 6);
                auto pos = name.find('_');
                if (pos != std::string::npos) {
                    name = name.substr(0, pos);
                }
                count += 6;
                groups[name].push_back(idx);
            }
        }

        networkFile.close();

        for (auto& group : groups) {
            _subsetEventManager.addSubset(group.first, group.second);
        }
    }

    SnuddaLoader::SnuddaLoader() :
        simil::LoaderHDF5Data()
    {
    }

    SnuddaLoader::~SnuddaLoader()
    {
    }

    std::unique_ptr<simil::SimulationData> SnuddaLoader::loadSimulationData(const std::string& simFile1,
                                                                            const std::string& simFile2)
    {
        auto simData = std::make_unique<simil::SpikeData>();
        double startTime = std::numeric_limits<double>::max();
        double endTime = std::numeric_limits<double>::lowest();

        simil::TSpikes* spikes = new simil::TSpikes();
        simil::GIDVec neuronTypes[2]; // 0 virtual, 1 real.

        // input spikes
        if (!simFile1.empty()) {
            H5::H5File inputSimFile(simFile1, 0);
            auto dsId = inputSimFile.getId();

            if (!HDF5pathExists(dsId, "input")) {
                std::cerr << "Unable to open group 'input' group" << std::endl;
                std::abort();
            }

            auto inputGroup = inputSimFile.openGroup("input");

            std::vector<std::string> neuronIds = listChildGroups(inputGroup);
            inputGroup.close();

            for (auto& neuronId : neuronIds) {
                if (HDF5pathExists(dsId, "input/" + neuronId + "/activity")) {
                    neuronTypes[0].emplace_back(atoi(neuronId.c_str()));
                    auto spikesDs = inputSimFile.openDataSet("input/" + neuronId + "/activity/spikes");
                    hsize_t dims;
                    spikesDs.getSpace().getSimpleExtentDims(&dims);

                    std::vector<double> times(dims, 0);
                    spikesDs.read(times.data(), H5::PredType::IEEE_F64LE);

                    for (hsize_t i = 0; i < times.size(); ++i) {
                        startTime = std::min(startTime, times[i]);
                        endTime = std::max(endTime, times[i]);
                        spikes->emplace_back(times[i], std::stoi(neuronId));
                    }
                    spikesDs.close();
                } else {
                    neuronTypes[1].emplace_back(atoi(neuronId.c_str()));
                    for (auto subGroup :
                         {"input/" + neuronId + "/cortical/spikes", "input/" + neuronId + "/cortical_background/spikes",
                          "input/" + neuronId + "/thalamic_background/spikes"}) {
                        auto spikesDs = inputSimFile.openDataSet(subGroup);
                        hsize_t maxDims = 2;
                        hsize_t dims[2];
                        spikesDs.getSpace().getSimpleExtentDims(dims, &maxDims);

                        std::vector<float> times(dims[0] * dims[1]);
                        spikesDs.read(times.data(), H5::PredType::IEEE_F32LE);

                        for (hsize_t i = 0; i < dims[0] * dims[1]; ++i) {
                            if (times[i] < 0) {
                                continue; // -1.0 means no spike
                            }
                            startTime = std::min(startTime, static_cast<double>(times[i]));
                            endTime = std::max(endTime, static_cast<double>(times[i]));
                            spikes->emplace_back(times[i], std::stoi(neuronId));
                        }
                        spikesDs.close();
                    }
                }
            }

            _subsetEventManager.addSubset("virtual neurons", neuronTypes[0]);
            _subsetEventManager.addSubset("simulated neurons", neuronTypes[1]);

            inputSimFile.close();
        }

        // output spikes
        if (!simFile2.empty()) {
            H5::H5File outputSimFile(simFile2, 0);

            if (!HDF5pathExists(outputSimFile.getId(), "time")) {
                std::cerr << "Unable to open group 'time' group" << std::endl;
                std::abort();
            }

            hsize_t dims;
            auto idDs = outputSimFile.openDataSet("meta_data/id");
            idDs.getSpace().getSimpleExtentDims(&dims);
            idDs.close();
            const int numNeurons = dims;

            for (int idx = 0; idx < numNeurons; ++idx) {
                const std::string name = "neurons/" + std::to_string(idx);
                if (HDF5pathExists(outputSimFile.getId(), name)) {
                    hsize_t sDims[2];
                    hsize_t maxDims = 2;
                    auto spikesDs = outputSimFile.openDataSet(name + "/spikes");
                    spikesDs.getSpace().getSimpleExtentDims(sDims, &maxDims);
                    std::vector<double> times(sDims[0] * sDims[1]);
                    spikesDs.read(times.data(), H5::PredType::IEEE_F64LE);
                    spikesDs.close();

                    for (hsize_t i = 0; i < times.size(); ++i) {
                        startTime = std::min(startTime, times[i]);
                        endTime = std::max(endTime, times[i]);
                        spikes->emplace_back(times[i], idx);
                    }
                }
            }
            outputSimFile.close();
        }

        auto sortSpike = [](const simil::TSpikes::value_type& a, const simil::TSpikes::value_type& b) {
            return a.first < b.first;
        };
        std::sort(spikes->begin(), spikes->end(), sortSpike);

        simData->setStartTime(startTime);
        simData->setEndTime(endTime);
        simData->setSimulationType(simil::TSimulationType::TSimSpikes);
        simData->setSpikes(*spikes);

        return simData;
    }

    std::unique_ptr<simil::Network> SnuddaLoader::loadNetwork(const std::string& networkFile, const std::string&)
    {
        _networkFilename = networkFile;

        simil::TGIDSet ids;
        simil::TPosVect pos;

        for (auto value : loadNeurons()) {
            ids.emplace(value.first);
            pos.push_back(value.second);
        }

        if (_loadSynapses) {
            const auto neuronsSize = ids.size();
            for (auto value : loadSynapses()) {
                ids.emplace(neuronsSize + value.first);
                pos.push_back(value.second);
            }
        }

        loadNeuronGroups();

        auto network = std::make_unique<simil::Network>();
        network->setDataType(simil::TDataType::TSNUDDA);
        network->setGids(ids);
        network->setPositions(pos);
        network->setSubset(_subsetEventManager);

        return network;
    }
} // namespace visimpl
