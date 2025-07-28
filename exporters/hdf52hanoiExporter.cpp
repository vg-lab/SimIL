/*
 * Copyright (c) 2025 VG-Lab/URJC.
 *
 * Authors: Felix de las Pozas Alvarez <felix.delaspozas@urjc.es>
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

 // C++
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cassert>
#include <vector>

// Jsoncpp amalgamation
#include "../simil/loaders/jsoncpp/jsoncpp.cpp"

// hdf5
#include <H5Cpp.h>

const char CELLS_TAG[] = "cells/positions";
const char MAPS_TAG[] = "cells/type_maps";
const char CONNECTIONS_TAG[] = "cells/connections";

struct Position
{
    float x = 0.f;
    float y = 0.f;
    float z = 0.f;

    Position() {};

    Position(const float& _X, const float& _Y, const float& _Z) :
        x(_X),
        y(_Y),
        z(_Z) {};
};

using PositionsMap = std::map<uint32_t,Position>;

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " networkfile.h5\n";
        std::cout << "\tThe json output file will be written in the same directory as the input file." << std::endl;
        std::cout << "\tIf application fails, hdf5 format is incorrect. Another exporter is needed!" << std::endl;
        return EXIT_SUCCESS;
    }

    H5::H5File network(argv[1], H5F_ACC_RDONLY);
    if (H5Lexists(network.getLocId(), MAPS_TAG, H5P_DEFAULT) <= 0 ||
        H5Lexists(network.getLocId(), CONNECTIONS_TAG, H5P_DEFAULT) <= 0 ||
        H5Lexists(network.getLocId(), CELLS_TAG, H5P_DEFAULT) <= 0) {
        std::cout << "Input file is invalid, ids cells, map or connections map doesn't exist!" << std::endl;
        return EXIT_FAILURE;
    }

    Json::Value root;
    root["domain"]["name"] = "Freeform";
    root["domain"]["network"]["mode"] = "directed";
    root["domain"]["hierarchy"]["insert"] = true;
    root["domain"]["hierarchy"]["edit"] = true;

    root["hierarchy"]["name"] = "root";
    root["hierarchy"]["id"] = -1;
    std::cout << "Hierarchy..." << std::endl;

    //--------------------
    auto positionsDS = network.openDataSet(CELLS_TAG);

    hsize_t dims[2];
    positionsDS.getSpace().getSimpleExtentDims(dims);
    assert(positionsDS.getSpace().getSimpleExtentNdims() == 2);

    auto bufferType = H5::PredType::IEEE_F64LE;

    assert(positionsDS.getTypeClass() == H5T_FLOAT);
    auto floatType = positionsDS.getFloatType();
    auto byteSize = floatType.getSize();

    if (byteSize == 4) {
        bufferType = H5::PredType::IEEE_F32LE;
    } else if (byteSize == 8) {
        bufferType = H5::PredType::IEEE_F64LE;
    }

    auto posBuffer = new char[dims[0] * dims[1] * byteSize];
    std::memset(posBuffer, 0, dims[0] * dims[1] * byteSize);

    // load positions
    positionsDS.read(reinterpret_cast<void*>(posBuffer), bufferType);

    PositionsMap positions;

    if (byteSize == 4) {
        auto bufferF = reinterpret_cast<float*>(&posBuffer[0]);
        for (size_t idx = 0; idx < dims[0] * dims[1];) {
            const auto id = static_cast<uint32_t>(bufferF[idx]);
            idx += dims[1] - 3;
            const Position pos{bufferF[idx + 0], bufferF[idx + 1], bufferF[idx + 2]};
            idx += 3;
            positions.insert(std::pair<uint32_t, Position>(id, pos));
        }
    } else {
        auto bufferD = reinterpret_cast<double*>(&posBuffer[0]);
        for (size_t idx = 0; idx < dims[0] * dims[1];) {
            const auto id = static_cast<uint32_t>(bufferD[idx]);
            idx += dims[1] - 3;
            const Position pos{static_cast<float>(bufferD[idx + 0]), static_cast<float>(bufferD[idx + 1]),
                               static_cast<float>(bufferD[idx + 2])};
            idx += 3;
            positions.insert(std::pair<uint32_t, Position>(id, pos));
        }
    }
    delete[] posBuffer;
    positionsDS.close();

    auto appendChild = [&positions](const uint32_t id, Json::Value& parent) {
        assert(id < positions.size());
        Json::Value child;
        child["name"] = std::to_string(id);
        child["id"] = id;
        child["x"] = positions[id].x;
        child["y"] = positions[id].y;
        child["z"] = positions[id].z;
        parent.append(child);
    };

    std::set<uint32_t> presentIds;
    unsigned int maxId = std::numeric_limits<unsigned int>::lowest();
    std::vector<std::string> groups;
    bufferType = H5::PredType::IEEE_F64LE;
    if (H5Lexists(network.getLocId(), MAPS_TAG, H5P_DEFAULT) > 0) {
        auto group = network.openGroup(MAPS_TAG);
        const unsigned int objNum = group.getNumObjs();

        for (unsigned int i = 0; i < objNum; i++) {
            // Get object type.
            H5G_obj_t type = group.getObjTypeByIdx(i);

            // Check if type is a dataset.
            if (type != H5G_obj_t::H5G_DATASET) {
              continue;
            }

            // Get object name.
            std::string groupName = group.getObjnameByIdx(i);
            std::string name = groupName;
            auto pos = groupName.find("_");
            if (pos != std::string::npos)
                name = groupName.substr(0, pos);

            groups.emplace_back(name);

            Json::Value base;
            base["name"] = name;
            base["id"] = -1;
            base["x"] = 0;
            base["y"] = 0;
            base["z"] = 0;

            // Open dataset.
            H5::DataSet innerDs = group.openDataSet(groupName);

            memset(dims, 0, 2 * sizeof(hsize_t));
            innerDs.getSpace().getSimpleExtentDims(dims);
            assert(innerDs.getSpace().getSimpleExtentNdims() == 1);

            assert(innerDs.getTypeClass() == H5T_INTEGER);
            auto integerType = innerDs.getIntType();
            byteSize = integerType.getSize();

            if (byteSize == 4) {
                bufferType = H5::PredType::NATIVE_INT;
            } else if (byteSize == 8) {
                bufferType = H5::PredType::NATIVE_LONG;
            }

            auto buffer = new char[dims[0] * byteSize];
            std::memset(buffer, 0, dims[0] * byteSize);
            std::cout << "Group: " << groupName << " - Nodes: " << dims[0] << std::endl;

            innerDs.read(reinterpret_cast<void*>(buffer), bufferType);

            uint32_t id = std::numeric_limits<uint32_t>::max();
            if (byteSize == 4) {
                auto bufferI = reinterpret_cast<int*>(&buffer[0]);
                for (size_t idx = 0; idx < dims[0];) {
                    id = static_cast<uint32_t>(bufferI[idx]);
                    presentIds.insert(id);
                    maxId = std::max(id, maxId);
                    appendChild(id, base["children"]);
                    ++idx;
                }
            } else {
                auto bufferL = reinterpret_cast<long*>(&buffer[0]);
                for (size_t idx = 0; idx < dims[0];) {
                    id = static_cast<uint32_t>(bufferL[idx]);
                    presentIds.insert(id);
                    maxId = std::max(id, maxId);
                    appendChild(id, base["children"]);
                    ++idx;
                }
            }

            delete[] buffer;
            innerDs.close();

            root["hierarchy"]["children"].append(base);
        }

        std::cout << "Nodes: " << presentIds.size() << std::endl;

        for (auto& groupName : groups) {
            for (auto& value : root["hierarchy"]["children"]) {
                if(value["name"] == groupName)
                {
                    value["id"] = ++maxId;
                    break;
                }
            }
        }
    }

    std::cout << "Connections..." << std::endl;
    // connections value have different structure, storing it this way avoids search insertion.
    Json::Value connections;

    auto appendConnection = [&connections, &presentIds](const uint32_t from, const uint32_t to) {
        if (presentIds.find(from) == presentIds.end() || presentIds.find(to) == presentIds.end()) {
            return;
        }

        for (auto& value : connections[from]["imports"]) {
            if (value["id"] == to) {
                value["size"] = value["size"].asInt() + 1;
                return;
            }
        }

        Json::Value conn;
        conn["size"] = 1;
        conn["id"] = to;
        connections[from]["imports"].append(conn);
    };

    // groups by connections
    if (H5Lexists(network.getLocId(), CONNECTIONS_TAG, H5P_DEFAULT) > 0) {
        auto group = network.openGroup(CONNECTIONS_TAG);
        const unsigned int objNum = group.getNumObjs();

        for (unsigned int i = 0; i < objNum; i++) {
            // Get object type.
            H5G_obj_t type = group.getObjTypeByIdx(i);

            // Get object name.
            const std::string name = group.getObjnameByIdx(i);

            // Check if type is a dataset.
            if (type != H5G_obj_t::H5G_DATASET) {
                continue;
            }

            // Open dataset.
            H5::DataSet innerDs = group.openDataSet(name);
            memset(dims, 0, 2 * sizeof(hsize_t));
            innerDs.getSpace().getSimpleExtentDims(dims);
            assert(innerDs.getSpace().getSimpleExtentNdims() == 2);

            assert(innerDs.getTypeClass() == H5T_FLOAT);
            floatType = innerDs.getFloatType();
            byteSize = floatType.getSize();

            if (byteSize == 4) {
                bufferType = H5::PredType::IEEE_F32LE;
            } else if (byteSize == 8) {
                bufferType = H5::PredType::IEEE_F64LE;
            }

            if (dims[0] == 0 || dims[1] == 0) {
                continue;
            }

            auto buffer = new char[dims[0] * dims[1] * byteSize];
            std::memset(buffer, 0, dims[0] * dims[1] * byteSize);
            std::cout << "Connections group: " << name << " - Size: " << dims[0] << std::endl;

            innerDs.read(reinterpret_cast<void*>(buffer), bufferType);

            if (byteSize == 4) {
                auto bufferI = reinterpret_cast<float*>(&buffer[0]);
                for (size_t idx = 0; idx < dims[0] * dims[1];) {
                    appendConnection(static_cast<uint32_t>(bufferI[idx]), static_cast<uint32_t>(bufferI[idx + 1]));
                    idx += dims[1];
                }
            } else {
                auto bufferL = reinterpret_cast<double*>(&buffer[0]);
                for (size_t idx = 0; idx < dims[0] * dims[1];) {
                    appendConnection(static_cast<uint32_t>(bufferL[idx]), static_cast<uint32_t>(bufferL[idx + 1]));
                    idx += dims[1];
                }
            }
            delete[] buffer;
            innerDs.close();
        }
    }

    for (unsigned int idx = 0; idx < connections.size(); ++idx) {
        if(connections[idx] != Json::nullValue)
        {
            Json::Value node;
            node["id"] = idx;
            node["imports"] = connections[idx]["imports"];
            root["connections"].append(node);
        }
    }

    const std::string outputFile = std::string(argv[1]) + ".json";
    std::ofstream file_id;
    file_id.open(outputFile, std::ios_base::trunc|std::ios_base::out);
    if(!file_id.is_open())
    {
        std::cout << "Unable to create output file '" << outputFile << "'" << std::endl;
        return EXIT_FAILURE;
    }

    Json::StyledWriter styledWriter;
    file_id << styledWriter.write(root);
    file_id.close();

    std::cout << "Finished!" << std::endl;

    return EXIT_SUCCESS;
}