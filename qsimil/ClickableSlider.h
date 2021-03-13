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

#include <QMouseEvent>
#include <QSlider>

namespace qsimil
{
  class ClickableSlider : public QSlider
  {
  public:

    ClickableSlider( enum Qt::Orientation _orientation = Qt::Horizontal,
                  QWidget* _parent = nullptr )
    : QSlider( _orientation, _parent )
    { }

  protected:

    void mousePressEvent ( QMouseEvent * _event )
      {
        if (_event->button() == Qt::LeftButton)
        {
            if (orientation() == Qt::Vertical)
                setValue(minimum() + ((maximum()-minimum()) * 
                  (height()-_event->y())) / height() ) ;
            else
                setValue(minimum() + ((maximum()-minimum()) * 
                  _event->x()) / width() ) ;

            _event->accept();
        }
        QSlider::mousePressEvent(_event);
      }

  };  // ClickableSlider
}; // qsimil



#endif /* SRC_QT_CLICKABLESLIDER_H_ */
