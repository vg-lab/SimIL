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

#ifndef SIMIL_SIMIL_VOLTAGEDATA_H_
#define SIMIL_SIMIL_VOLTAGEDATA_H_

// SimIL
#include <simil/SimulationData.h>

namespace simil
{
  /* \class VoltageData
   * \brief Loads voltage simulation data. Tries to adhere to SimulationData api.
   */
  class SIMIL_API VoltageData
  : public SimulationData
  {
    public:
      /** \brief VoltageData class empty constructor.
       *
       */
      VoltageData();

      /** \brief VoltageData class constructor.
       * \param[in] file Data file.
       * \param[in] dataType Data file type.
       * \param[in] report Optional info.
       *
       */
      VoltageData(const std::string& file, TDataType dataType,
                  const std::string& report = "");

      /** \brief Returns the voltages at the given time using linear interpolation.
       * \param[in] time Time.
       * 
       */
      const Voltages voltagesAt( const float time ) const;

      /** \brief Returns the voltage at the given time using linear interpolation.
       * \param[in] group Group index in groups vector.
       * \param[in] time Time.
       *
       */
      float voltageAt(const unsigned int group, const float time) const;

      /** \brief Sets the voltages data.
       * \param[in] groups Groups names.
       * \param[in] voltages TVoltages vector.
       *
       */
      void setVoltages(const std::vector<std::string> groups, const simil::TVoltages &voltages );

      /** \brief Adds the given voltages to the class.
       * Warning: it's not a merge, groups are treated as different, its a concatenation.
       * \param[in] groups Groups names.
       * \param[in] voltages TVoltages vector.
       *groupRange
       */
      void addVoltages(const std::vector<std::string> groups, const simil::TVoltages &voltages );

      /** \brief Returns the group ranges, ordered.
       *
       */
      std::vector<std::pair<float, float>> ranges() const
      { return m_groupRanges; }

      /** \brief Returns the groups names.
       *
       */
      std::vector<std::string> groups() const
      { return m_groups; }

      /** \brief Returns the timestep of the data. 
       * 
      */
      float timeStep() const
      { return m_timeStep; }

      /** \brief Resets the class.
       *
       */
      void clear();

      /** \brief Returns a pointer to the object.
       *
       */
      VoltageData* get();

      /** \brief Returns the voltages of the given group. 
       * \param[in] groupIndex Group position in the vector. 
       * 
      */
      const Voltages& voltagesOf(const unsigned int groupIndex) const;

      /** \brief Returns the number of values in the given group.
       * \param[in] groupIndex Group position in the vector. 
       * 
      */
      unsigned long long sizeOfGroup(const unsigned int groupIndex) const;

    protected:
      std::vector<std::string>             m_groups;        /** groups names. */
      std::vector<Voltages>                m_groupVoltages; /** groups voltages, separated */
      std::vector<std::pair<float, float>> m_groupRanges;   /** groups voltage ranges. */
      float                                m_timeStep;      /** voltages time step. */

    private:
      /** \brief Helper method to fill gids and positions for compatibiliy. 
       * 
      */
      void insertFakeGidsAndPositions();
  };
}

#endif /* SIMIL_SIMIL_VOLTAGEDATA_H_ */
