/*
 * Copyright (c) 2015-2020 VG-Lab/URJC.
 *
 * Authors: Sergio E. Galindo <sergio.galindo@urjc.es>
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

#ifndef __QT_CLICKABLESLIDER_H__
#define __QT_CLICKABLESLIDER_H__

// Qt
#include <QMouseEvent>
#include <QSlider>
#include <QPainter>

// SimIL
#include <qsimil/api.h>

// C++
#include <cmath>

namespace qsimil
{
  class QSIMIL_API ClickableSlider : public QSlider
  {
    Q_OBJECT
  public:
    ClickableSlider(enum Qt::Orientation _orientation = Qt::Horizontal,
                    QWidget *_parent = nullptr)
    : QSlider(_orientation, _parent)
    , m_range_min{-1}
    , m_range_max{-1}
    {
    }

    /** \brief Sets the range to display. Values in [0,1] or -1 to disable.
     * \param[in] min Range minimum point.
     * \param[in] max Range maximum point.
     *
     */
    void setHighlightRange(float min, float max)
    {
      if(min < max)
      {
        m_range_min = std::max(0.f, min);
        m_range_max = std::min(1.f, max);
      }
      else
      {
        m_range_max = m_range_min = -1;
      }

      update();
    }

  protected:
    void mousePressEvent(QMouseEvent *_event) override
    {
      if (_event->button() == Qt::LeftButton)
      {
        if (orientation() == Qt::Vertical)
          setValue(minimum() + ((maximum() - minimum()) * (height() - _event->y())) / height());
        else
          setValue(minimum() + ((maximum() - minimum()) * _event->x()) / width());

        _event->accept();
        emit sliderPressed();
      }
      else
        QSlider::mousePressEvent(_event);
    }

    void paintEvent(QPaintEvent *event) override
    {
      if (m_range_min < m_range_max)
      {
        const auto qRect = rect();
        const int xMin = std::ceil(qRect.width() * m_range_min);
        const int xMax = std::ceil(qRect.width() * m_range_max); // width of the handle, to makeit in the center.

        QPainter p(this);
        p.setPen(Qt::transparent);
        p.setBrush(QColor("#909090"));
        p.drawRect(QRect{xMin, 0, xMax-xMin, qRect.height()});
        p.end();
      }

      QSlider::paintEvent(event);
    }

  private:
    float m_range_min; /** min value of displayed range in [0,1]. -1 to not in use. */
    float m_range_max; /** max value of displayed range in [0,1]. -1 to not in use. */
  };                  // ClickableSlider
};                    // qsimil

#endif /* SRC_QT_CLICKABLESLIDER_H_ */
