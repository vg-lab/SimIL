/*
 * Copyright (c) 2015-2024 VG-Lab/URJC.
 *
 * Authors: Félix de las Pozas Álvarez <felix.delaspozas@urjc.es>
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

// Project
#include <simil/VoltageData.h>
#include <simil/loaders/auxiliar/CSVActivity.h>

// C++
#include <vector>
#include <cassert>
#include "VoltageData.h"

//----------------------------------------------------------------------------
simil::VoltageData::VoltageData()
: SimulationData()
, m_timeStep(-1.f)
{
  _simulationType = TSimulationType::TSimVoltages;
}

//----------------------------------------------------------------------------
simil::VoltageData::VoltageData(const std::string &, TDataType, const std::string &)
: SimulationData()
, m_timeStep(-1.f)
{
  _simulationType = TSimulationType::TSimVoltages;
  // Simulation data tries to load it as a network file, but because we're not
  // going to use we can ignore the 'loaded' gids.
}

//----------------------------------------------------------------------------
const simil::Voltages simil::VoltageData::voltagesAt(const float time) const
{
  Voltages result;

  for(unsigned int i = 0; i < m_groupVoltages.size(); ++i)
    result.emplace_back(time, voltageAt(i, time));

  return result;
}

//----------------------------------------------------------------------------
void simil::VoltageData::setVoltages(const std::vector<std::string> groups,
                                     const simil::TVoltages &voltages )
{
  clear();
  addVoltages(groups, voltages);
}

//----------------------------------------------------------------------------
void simil::VoltageData::addVoltages(const std::vector<std::string> groups,
                                     const simil::TVoltages &voltages )
{
  // NOTE: doesn't take into cosideration repeated groups.
  auto previous = m_groups.size();
  m_groups.insert(m_groups.end(), groups.cbegin(), groups.cend());
  m_groupRanges.resize(previous + groups.size(), {std::numeric_limits<float>::max(), std::numeric_limits<float>::min()});
  m_groupVoltages.resize(previous + groups.size());

  float minTime = previous > 0 ? startTime() : std::numeric_limits<float>::max();
  float maxTime = previous > 0 ? endTime() : std::numeric_limits<float>::min();
  auto insertValues = [&](const std::tuple<float, float, int> &t)
  {
    const auto group = std::get<2>(t) + previous;
    const auto value = std::get<1>(t);
    const auto time = std::get<0>(t);
    m_groupVoltages[group].emplace_back(time, value);
    m_groupRanges[group] = { std::min(m_groupRanges[group].first, value),
                            std::max(m_groupRanges[group].second, value) };

    minTime = std::min(minTime, time);
    maxTime = std::max(maxTime, time);
  };
  std::for_each(voltages.cbegin(), voltages.cend(), insertValues);

  // Update timestep.
  if (m_timeStep < 0.f)
    m_timeStep = std::numeric_limits<float>::max();

  for (unsigned int i = 0; i < groups.size(); ++i)
      m_timeStep = std::min(m_timeStep, simil::CSVVoltages::groupTimeStep(voltages, i));

  // Update times.
  assert(maxTime - minTime > 0);
  setStartTime(minTime);
  setEndTime(maxTime);

  insertFakeGidsAndPositions();
}

//----------------------------------------------------------------------------
void simil::VoltageData::clear()
{
  m_groups.clear();
  m_groupVoltages.clear();
  m_groupRanges.clear();
  m_timeStep = -1.f;
}

//----------------------------------------------------------------------------
simil::VoltageData* simil::VoltageData::get(void)
{
  return this;
}

//----------------------------------------------------------------------------
const simil::Voltages &simil::VoltageData::voltagesOf(const unsigned int groupIndex) const
{
  assert(groupIndex < m_groupVoltages.size());

  return m_groupVoltages[groupIndex];
}

//----------------------------------------------------------------------------
unsigned long long simil::VoltageData::sizeOfGroup(const unsigned int groupIndex) const
{
  if(groupIndex < m_groupVoltages.size())
    return m_groupVoltages[groupIndex].size();

  return 0;
}

//----------------------------------------------------------------------------
void simil::VoltageData::insertFakeGidsAndPositions()
{
  TGIDSet gids;
  TPosVect positions;
  for (unsigned int i = 0; i < m_groupVoltages.size(); ++i)
  {
    gids.emplace(i);
    positions.emplace_back(0, 0, 0);
  }

  setGids(gids);
  setPositions(positions);
} 

//----------------------------------------------------------------------------
float simil::VoltageData::voltageAt(const unsigned int group, const float time) const
{
  if(group >= m_groups.size()) return std::numeric_limits<float>::min();

  const auto &groupVoltage = m_groupVoltages[group];
  if(groupVoltage.empty()) return std::numeric_limits<float>::min();
  assert(m_timeStep > 0.f);

  auto firstVoltage = groupVoltage.front();
  if(time <= firstVoltage.first)
    return firstVoltage.second;

  auto lastVoltage = groupVoltage.back();
  if(time >= lastVoltage.first)
    return lastVoltage.second;

  // interpolation between values in an interval
  auto findInterval = [&time](const std::pair<float, float> &p){ return p.first > time; };
  auto it = std::find_if(groupVoltage.cbegin(), groupVoltage.cend(), findInterval);
  assert(it != groupVoltage.cend());
  auto valueA = (*(it-1));
  auto valueB = (*it);

  const auto weightA = 1 - (std::abs(time - valueA.first)/m_timeStep);
  const auto weightB = 1 - weightA;

  return (weightA * valueA.second) + (weightB * valueB.second);
}
