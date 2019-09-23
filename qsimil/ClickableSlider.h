/*
 * ClickableSlider.h
 *
 *  Created on: 11 de dic. de 2015
 *      Author: sgalindo
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
