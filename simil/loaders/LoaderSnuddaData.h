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

#ifndef SNUDDALOADER_H
#define SNUDDALOADER_H

#include <simil/types.h>
#include <memory>

#include <H5Cpp.h>
#include <simil/loaders/LoaderHDF5Data.h>

namespace simil
{
    class SpikeData;
}

namespace simil
{
    class SnuddaLoader : public simil::LoaderHDF5Data
    {
      public:
        SnuddaLoader();
        virtual ~SnuddaLoader();

        virtual std::unique_ptr<simil::SimulationData> loadSimulationData(
            const std::string& networkFile, const std::string& activityFile = "") override;

        virtual std::unique_ptr<simil::Network> loadNetwork(const std::string& networkFile,
                                                            const std::string& aux = "") override;

        /** \brief Sets the loading of synapses. To be called before the load network/activity
         * \param[in] value True to load synapses and false otherwise.
         * 
         */
        void setLoadSynapses(const bool value)
        {
            _loadSynapses = value;
        }

      private:
        std::unordered_map<uint32_t, vmml::Vector3f> loadNeurons() const;
        std::unordered_map<uint32_t, vmml::Vector3f> loadSynapses();
        void loadNeuronGroups();
        std::string _networkFilename;
        std::shared_ptr<simil::SpikeData> _simData = nullptr;
        bool _loadSynapses = false;
        simil::SubsetEventManager _subsetEventManager;
    };
} // namespace visimpl

#endif // SNUDDALOADER_H
