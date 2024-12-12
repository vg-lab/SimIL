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

#ifndef __SIMIL_VOLTAGESPLAYER_H_
#define __SIMIL_VOLTAGESPLAYER_H_

// SimIL
#include <simil/SimulationPlayer.h>

// C++
#include <iterator>
#include <cstddef>

// glm
#include <glm/glm.hpp>

namespace simil
{
  enum class InterpolationType
  {
    LINEAR = 0,
    SPLINE = 1
  };

  /** \class VoltagesPlayer
   * \brief Implements a SimulationPlayer with voltages data.
   *
   */
  class SIMIL_API VoltagesPlayer
  : public SimulationPlayer
  {
    public:
      /** \brief VoltagesPlayer class constructor. 
       * 
      */
      VoltagesPlayer();

      /** \brief VoltagesPlayer class virtual destructor. 
       * 
      */
      virtual ~VoltagesPlayer()
      {};

      /** \brief Overrides the default loader to throw on other datatype but CSV. 
       * 
      */
      virtual void LoadData(TDataType dataType ,
                            const std::string& networkPath ,
                            const std::string& activityPath = "") override;

      /** \brief Overrides the default loader to throw on incorrect type. 
       * 
      */
      virtual void LoadData(std::shared_ptr<SimulationData> data) override;

      /** \brief Changes the data interpolation type to the given one. 
       * \param[in] type Interpolation type. 
       * 
      */
      void setInterpolationType(const InterpolationType type);
      
      /** \brief Process the frame. Overrided.
       * 
      */
      virtual void FrameProcess() final;

      /** \brief Moves the iterator to the time position. Overrided. 
       * \param[in] timePos Time position.
       * 
      */
      void PlayAtTime(float timePos) override;

      /** \brief Moves the iterator the given percentage of total time. Overrided.
       * \param[in] percentage Percentage in [0,1] of time range. 
       * 
      */
      void PlayAtPercentage(float percentage) override;

      /** \brief Moves the current time to the given time.
       * \param[in] timeStamp Time position.
       * 
      */
      void GoTo(float timeStamp) override;

      /** \brief Returns the interpolated value for the given time and group. 
       * \param[in] time Time position. 
       * \param[in] group Group index.
       * 
      */
      float value(const float time, const unsigned int group);

      /** \brief Returns all groups values for the given time. 
       * \param[in] time Time position. 
       * 
      */
      std::vector<float> values(const float time);

    protected:
      /** \brief Helper methods to init the iterators. 
       * 
      */
      void initIterators();

      /** \brief Helper class to iterate and interpolate voltage values. 
       * 
      */
      class SIMIL_API VoltageIterator
      {
        public:
          /** \brief VoltageIterator class constructor. 
           * \param[in] data Voltages data to iterate. 
           * 
          */
          VoltageIterator(const simil::Voltages &data);

          /** \brief VoltageIterator class destructor. 
           * 
          */
          ~VoltageIterator();

          /** \brief Returns the value computed with the current interpolation method. 
           * \param[in] timePos Time value in the data. 
           * 
          */
          float value(const float timePos);

          /** \brief Sets the interpolation method. 
           * 
          */
          void setMode(InterpolationType type);

          // increment/decrement
          VoltageIterator &operator++();
          VoltageIterator &operator--();

          /** \brief Sets the iterator the to beginning of the data. 
           * 
          */
          void begin();

          /** \brief Sets the iterator to the end of the data. 
           * 
          */
          void end();

          /** \brief True if the iterator is at the beginning of the data and false otherwise. 
           * 
          */
          bool isAtBegin() const;

          /** \brief True if the iterator is at the end of the data and false otherwise. 
           * 
          */
          bool isAtEnd() const;

          /** \brief Moves the iterator to contain the given time position. 
           * \para[in] time Time position.
           * 
          */
          void moveAtTime(const float time);

          /** \brief Moves the iterator to contain the time of the given percentage. 
           * \param[in] percentage Percentage of total time. In [0,1].
           * 
          */
          void moveAtPercentage(const float percentage);

          /** \brief Returns true if the iterator has the time in its interval and false otherwise. 
           * 
          */
          bool isInTime(const float time) const;

          /** \brief Test method that saves the contents of the output to a csv file.
           * \param[in] csvFile Filename of CSV file on disk.
           * \param[in] increment step increment.
           * 
          */
          void test(const std::string &csvFile, const float increment);

        private:
          struct Cache
          {
            glm::vec2 a;
            glm::vec2 b;
            glm::vec2 c;
            glm::vec2 d;
            bool invalid; /** true if data is invalid, false otherwise. */
            bool borders; /** true if special case for borders (t is < 0 or > 1). */

            Cache() : invalid{true}, borders{false} {};
          };

          /** Helper methods to compute interpolation values */
          float linear(const Voltages::const_iterator &A, const Voltages::const_iterator &B, const float value) const;
          float linearInterpolation(const float value) const;
          float catmullRom(const float value);
          void computeCacheData(const glm::vec2 &p0, const glm::vec2 &p1, const glm::vec2 &p2, const glm::vec2 &p3);

          Voltages::const_iterator m_it0;   /** begin iterator for current interval. */
          Voltages::const_iterator m_it1;   /** middle 1 iterator for current interval. */
          Voltages::const_iterator m_it2;   /** middle 2 iterator for current interval. */
          Voltages::const_iterator m_it3;   /** end iterator for current interval . */
          Cache                    m_cache; /** catmull-rom curves cache. */
          InterpolationType        m_type;  /** value interpolation type. */
          const simil::Voltages   &m_data;  /** data to iterate. */
#ifdef TEST
          unsigned long m_requests; /** number of interpolation requests, not linear. */
          unsigned long m_failed;   /** number if requests that needed cache computation. */
#endif          
      };

      std::vector<VoltageIterator> m_iterators; /** groups voltage iterators. */
      bool m_hasData; /** true if data has been loaded and false otherwise. */
  };
}

#endif