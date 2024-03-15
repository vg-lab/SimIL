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

// Undefine to test the results of the interpolation. See test() method.
//#define TEST

// Project
#include <simil/VoltagesPlayer.h>
#include <simil/VoltageData.h>

// C++
#include <exception>
#include <cassert>
#include <fstream>
#include <iomanip>

//----------------------------------------------------------------------------
simil::VoltagesPlayer::VoltagesPlayer()
: SimulationPlayer()
, m_hasData{false}
{
  _simulationType = TSimulationType::TSimVoltages;
}

//----------------------------------------------------------------------------
void simil::VoltagesPlayer::LoadData(TDataType dataType, const std::string &, const std::string &activityFile)
{
  switch (dataType)
  {
  default:
    throw std::runtime_error("Not implemented.");
    break;
  case TDataType::TCSV:
    _simData = std::make_shared<VoltageData>(activityFile, TDataType::TCSV);
    break;
  }

  SimulationPlayer::LoadData(_simData);
  initIterators();
}

//----------------------------------------------------------------------------
void simil::VoltagesPlayer::LoadData(std::shared_ptr<SimulationData> data)
{
  if (!data || !std::dynamic_pointer_cast<VoltageData>(data))
  {
    std::cerr << "Nothing loaded: null or incorrect SimulationData." << std::endl;
    return;
  }

  SimulationPlayer::LoadData(data);
  initIterators();
}

//----------------------------------------------------------------------------
void simil::VoltagesPlayer::setInterpolationType(const InterpolationType type)
{
  std::for_each(m_iterators.begin(), m_iterators.end(), [&type](VoltageIterator &it)
                { it.setMode(type); });
}

//----------------------------------------------------------------------------
void simil::VoltagesPlayer::FrameProcess()
{
  const unsigned int finished = std::count_if(m_iterators.cbegin(), m_iterators.cbegin(), [](const VoltageIterator &it)
                                              { return it.isAtEnd(); });
  if (finished == m_iterators.size())
    Finished();
}

//----------------------------------------------------------------------------
void simil::VoltagesPlayer::PlayAtTime(float timePos)
{
  SimulationPlayer::PlayAtTime(timePos);

  std::for_each(m_iterators.begin(), m_iterators.end(), [&timePos](VoltageIterator &it)
                { it.moveAtTime(timePos); });
}

//----------------------------------------------------------------------------
void simil::VoltagesPlayer::PlayAtPercentage(float perc)
{
  SimulationPlayer::PlayAtPercentage(perc);

  std::for_each(m_iterators.begin(), m_iterators.end(), [&perc](VoltageIterator &it)
                { it.moveAtPercentage(perc); });
}

//----------------------------------------------------------------------------
void simil::VoltagesPlayer::GoTo(float timeStamp)
{
  SimulationPlayer::GoTo(timeStamp);

  std::for_each(m_iterators.begin(), m_iterators.end(), [&timeStamp](VoltageIterator &it){ it.moveAtTime(timeStamp); });
}

//----------------------------------------------------------------------------
float simil::VoltagesPlayer::value(const float time, const unsigned int group)
{
  if (group < m_iterators.size())
    return m_iterators.at(group).value(time);

  return 0.f;
}

//----------------------------------------------------------------------------
std::vector<float> simil::VoltagesPlayer::values(const float time)
{
  std::vector<float> values;
  for (unsigned int i = 0; i < m_iterators.size(); ++i)
    values.push_back(m_iterators[i].value(time));

  return values;
}

//----------------------------------------------------------------------------
void simil::VoltagesPlayer::initIterators()
{
  auto voltagesData = std::dynamic_pointer_cast<VoltageData>(_simData);
  if (!voltagesData)
    return;

  for (unsigned int i = 0; i < voltagesData->groups().size(); ++i)
    m_iterators.emplace_back(voltagesData->voltagesOf(i));

#ifdef TEST
  const std::string name = "/home/felix/Desarrollo/Code/TFM/test_";
  for (unsigned int i = 0; i < voltagesData->groups().size(); ++i)
    m_iterators[i].test(name + std::to_string(i) + ".csv", 0.0001);
#endif

  m_hasData = true;
}

//----------------------------------------------------------------------------
simil::VoltagesPlayer::VoltageIterator::VoltageIterator(const simil::Voltages &data)
: m_data{data}
{
  begin();
  m_type = InterpolationType::LINEAR;
  assert(data.size() >= 4);
#ifdef TEST
  m_requests = 0;
  m_failed = 0;
#endif
}

//----------------------------------------------------------------------------
simil::VoltagesPlayer::VoltageIterator::~VoltageIterator()
{
#ifdef TEST
  if(m_requests > 0)
    std::cout << "iterator performance: requests " << m_requests << " failed " << m_failed 
              << " fail ratio " << static_cast<float>(m_failed * 100) / m_requests << "%" << std::endl;
#endif
}

//----------------------------------------------------------------------------
float simil::VoltagesPlayer::VoltageIterator::value(const float timePos)
{
  float value = 0;

  if (!isInTime(timePos))
  {
    // try to put the timePos between it1 and it2
    while (!isAtEnd() && timePos > (*m_it2).first)
      this->operator++();

    if (timePos > (*m_it3).first)
      return 0;

    while (!isAtBegin() && timePos < (*m_it1).first)
      this->operator--();

    if (timePos < (*m_it0).first)
      return 0;
  }

  assert((*m_it0).first <= timePos && timePos <= (*m_it3).first);

  switch (m_type)
  {
  default:
  case InterpolationType::LINEAR:
    value = linearInterpolation(timePos);
    break;
  case InterpolationType::SPLINE:
    value = catmullRom(timePos);
    break;
  }

  return value;
}

//----------------------------------------------------------------------------
void simil::VoltagesPlayer::VoltageIterator::setMode(InterpolationType type)
{
  if (m_type != type)
    m_type = type;
}

//----------------------------------------------------------------------------
simil::VoltagesPlayer::VoltageIterator &simil::VoltagesPlayer::VoltageIterator::operator++()
{
  if (!isAtEnd())
  {
    ++m_it0;
    ++m_it1;
    ++m_it2;
    ++m_it3;
    m_cache.invalid = true;
  }

  return *this;
}

//----------------------------------------------------------------------------
simil::VoltagesPlayer::VoltageIterator &simil::VoltagesPlayer::VoltageIterator::operator--()
{
  if (!isAtBegin())
  {
    --m_it0;
    --m_it1;
    --m_it2;
    --m_it3;
    m_cache.invalid = true;
  }

  return *this;
}

//----------------------------------------------------------------------------
void simil::VoltagesPlayer::VoltageIterator::begin()
{
  if (!isAtBegin())
  {
    m_it0 = m_data.cbegin();
    m_it1 = m_it0 + 1;
    m_it2 = m_it0 + 2;
    m_it3 = m_it0 + 3;
    m_cache.invalid = true;
  }
}

//----------------------------------------------------------------------------
void simil::VoltagesPlayer::VoltageIterator::end()
{
  if (!isAtEnd())
  {
    m_it3 = m_data.cend() - 1;
    m_it2 = m_it3 - 1;
    m_it1 = m_it3 - 2;
    m_it0 = m_it3 - 3;
    m_cache.invalid = true;
  }
}

//----------------------------------------------------------------------------
bool simil::VoltagesPlayer::VoltageIterator::isAtEnd() const
{
  return m_it3 == m_data.cend() - 1;
}

//----------------------------------------------------------------------------
bool simil::VoltagesPlayer::VoltageIterator::isAtBegin() const
{
  return m_it0 == m_data.cbegin();
}

//----------------------------------------------------------------------------
void simil::VoltagesPlayer::VoltageIterator::moveAtTime(const float time)
{
  if (isInTime(time))
    return;

  // Assumes m_data is sorted by time.
  const auto startTime = (*m_data.cbegin()).first;
  const auto endTime = (*(m_data.cend() - 1)).first;
  assert(time >= startTime && time <= endTime);

  moveAtPercentage((time - startTime) / (endTime - startTime));
}

//----------------------------------------------------------------------------
void simil::VoltagesPlayer::VoltageIterator::moveAtPercentage(const float percentage)
{
  // Assumes m_data is sorted by time.
  assert(percentage >= 0.f && percentage <= 1.f);
  const auto startTime = (*m_data.cbegin()).first;
  const auto endTime = (*(m_data.cend() - 1)).first;
  const auto timePos = startTime + (percentage * (endTime - startTime));
  assert(timePos >= startTime && timePos <= endTime);

  if (isInTime(timePos))
    return;

  // try to guess its position.
  const unsigned int itPos = (m_data.size() - 1) * percentage;

  if (itPos < 4)
    begin();
  else if (itPos > m_data.size() - 4)
    end();
  else
  {
    m_it0 = m_data.cbegin() + itPos;
    m_it1 = m_it0 + 1;
    m_it2 = m_it0 + 2;
    m_it3 = m_it0 + 3;
    m_cache.invalid = true;
  }

  // try to get the timePos in the it1 and it2 interval in catmull and it0 and it1 in linear.
  const float endVal = m_type == InterpolationType::LINEAR ? (*m_it1).first : (*m_it2).first;
  const float beginVal = m_type == InterpolationType::LINEAR ? (*m_it0).first : (*m_it1).first;
  while (!isAtEnd() && timePos > endVal)
    this->operator++();

  while (!isAtBegin() && timePos < beginVal)
    this->operator--();
}

//----------------------------------------------------------------------------
bool simil::VoltagesPlayer::VoltageIterator::isInTime(const float time) const
{
  switch (m_type)
  {
    default:
    case InterpolationType::LINEAR:
      return (*m_it0).first <= time && time <= (*m_it3).first;
    case InterpolationType::SPLINE:
      if (isAtBegin())
        return ((*m_it0).first <= time && time <= (*m_it2).first);
      if (isAtEnd())
        return ((*m_it1).first <= time && time <= (*m_it3).first);
      return ((*m_it1).first <= time && time <= (*m_it2).first);
  }

  throw std::runtime_error("Invalid isInTime execution.");
  return false;
}

//----------------------------------------------------------------------------
void simil::VoltagesPlayer::VoltageIterator::test(const std::string &csvFile, const float increment)
{
  std::ofstream file;
  file.open(csvFile, std::ios_base::trunc | std::ios_base::out);
  if (file.is_open())
  {
    file << "time, linear, catmull, diff\n";
    file << std::fixed << std::setprecision(4);
    // file << "time, catmull\n";
    for (float i = m_data.front().first; i < m_data.back().first; i += increment)
    {
      moveAtTime(i);
      const float l = linearInterpolation(i);
      const float c = catmullRom(i);
      const float diff = fabs(l - c);
      file << i << ", " << l << ", " << c << "," << diff << '\n';
      //file << i << ", " << catmullRom(i) << '\n';
    }

    file.close();
  }
  else
    std::cerr << "Unable to create/open file " << csvFile << std::endl;
}

//----------------------------------------------------------------------------
float simil::VoltagesPlayer::VoltageIterator::linear(const Voltages::const_iterator &A, const Voltages::const_iterator &B, const float value) const
{
  const auto &x0 = (*A).first;
  const auto &y0 = (*A).second;
  const auto &x1 = (*B).first;
  const auto &y1 = (*B).second;

  assert(value >= x0 && value <= x1);

  return y0 + ((y1 - y0) / (x1 - x0)) * (value - x0);
}

//----------------------------------------------------------------------------
float simil::VoltagesPlayer::VoltageIterator::linearInterpolation(const float value) const
{
  if ((*m_it1).first >= value)
    return linear(m_it0, m_it1, value);

  if ((*m_it2).first >= value)
    return linear(m_it1, m_it2, value);

  return linear(m_it2, m_it3, value);
}

//----------------------------------------------------------------------------
float simil::VoltagesPlayer::VoltageIterator::catmullRom(const float value)
{
#ifdef TEST
  ++m_requests;
#endif

  // try to ensure the middle interval. [m_it1, m_it2];
  if (!isAtBegin() && value < (*m_it1).first)
    operator--();

  if(!isAtEnd() && value > (*m_it2).first)
    operator++();

  glm::vec2 p0 = {(*m_it0).first, (*m_it0).second};
  glm::vec2 p1 = {(*m_it1).first, (*m_it1).second};
  glm::vec2 p2 = {(*m_it2).first, (*m_it2).second};
  glm::vec2 p3 = {(*m_it3).first, (*m_it3).second};

  float t = (value - (*m_it1).first) / ((*m_it2).first - (*m_it1).first);

  // handle t < 0 at the beginning.
  if(t < 0 && isAtBegin())
  {
    if(m_cache.borders == false)
    {
      m_cache.borders = true;
      m_cache.invalid = true;
      p3 = p2;
      p2 = p1;
      p1 = p0;
      p0 = {(*m_it0).first - ((*m_it1).first-(*m_it0).first), (*m_it0).second};
    }

    t += 1.f;
  }
  else
  {
    if(isAtBegin() && m_cache.borders == true)
    {
      m_cache.borders = false;
      m_cache.invalid = true;
    }
  }

  // handle t > 1 at the end.
  if(t > 1 && isAtEnd())
  {
    if(m_cache.borders == false)
    {
      m_cache.borders = true;
      m_cache.invalid = true;
      p0 = p1;
      p1 = p2;
      p2 = p3;
      p3 = {(*m_it3).first + ((*m_it3).first-(*m_it2).first), (*m_it3).second};
    }

    t -= 1.f;
  }
  else
  {
    if(isAtEnd() && m_cache.borders == true)
    {
      m_cache.borders = false;
      m_cache.invalid = true;
    }
  }


  if (m_cache.invalid)
  {
    computeCacheData(p0, p1, p2, p3);
#ifdef TEST
    ++m_failed;
#endif
  }

  glm::vec2 result = m_cache.a * t * t * t +
                     m_cache.b * t * t +
                     m_cache.c * t +
                     m_cache.d;

  return result.y;
}

//----------------------------------------------------------------------------
void simil::VoltagesPlayer::VoltageIterator::computeCacheData(const glm::vec2 &p0, const glm::vec2 &p1, 
                                                              const glm::vec2 &p2, const glm::vec2 &p3)
{
  constexpr float alpha = 0.5f;
  // constexpr float tension = 0.f;

  const float t01 = pow(distance(p0, p1), alpha);
  const float t12 = pow(distance(p1, p2), alpha);
  const float t23 = pow(distance(p2, p3), alpha);

  // const glm::vec2 m1 = (1.0f - tension) * (p2 - p1 + t12 * ((p1 - p0) / t01 - (p2 - p0) / (t01 + t12)));
  // const glm::vec2 m2 = (1.0f - tension) * (p2 - p1 + t12 * ((p3 - p2) / t23 - (p3 - p1) / (t12 + t23)));
  const glm::vec2 m1 = (p2 - p1 + t12 * ((p1 - p0) / t01 - (p2 - p0) / (t01 + t12)));
  const glm::vec2 m2 = (p2 - p1 + t12 * ((p3 - p2) / t23 - (p3 - p1) / (t12 + t23)));

  m_cache.a = 2.f * (p1 - p2) + m1 + m2;
  m_cache.b = -3.f * (p1 - p2) - m1 - m1 - m2;
  m_cache.c = m1;
  m_cache.d = p1;

  m_cache.invalid = false;
}
