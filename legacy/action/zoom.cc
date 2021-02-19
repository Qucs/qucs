/* Copyright (C) 2020 Felix Salfelder
 *
 * Qucs is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Qucs.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
/*--------------------------------------------------------------------------*/

#include <QEvent>
#include <QGraphicsSceneMouseEvent>
#include <QUndoCommand>
#include <QGraphicsView>

#include "qucsdoc.h"
#include "qucs_globals.h"
#include "platform.h"
#include "mouseaction.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class ActionZoomIn : public QAction{
public:
	explicit ActionZoomIn(QObject* parent) : QAction(parent) { untested();
		setIcon(QIcon(":/bitmaps/viewmag+.png"));
		setText(tr("Zoom in"));
		setShortcut(Qt::Key_Plus);
		setStatusTip(tr("Zoom into the current view"));
		setWhatsThis(tr("Zoom in\n\nZoom the current view"));
		setCheckable(true);
	}
};
 // connect(magPlus, &QAction:: triggered??, this, &QucsApp::slotZoomIn);
/*--------------------------------------------------------------------------*/
class MouseActionZoomIn : public MouseAction{
public:
	explicit MouseActionZoomIn(QObject* parent=nullptr)
	    : MouseAction(parent){ itested(); }

private: // Action
	Action* clone() const{
		return new MouseActionZoomIn(nullptr); // this?
	}
	QAction* createAction(QObject* parent) const override{ untested();
		auto x = new ActionZoomIn(parent);
		connect(x, &QAction::triggered, this, &MouseAction::slotTrigger);
		return x;
	}

private: // MouseAction
	cmd* press(QEvent*) override;
	cmd* release(QEvent*) override;

private:
	int _MAx1;
	int _MAx2;
	int _MAy1;
	int _MAy2;
}a;
static Dispatcher<Widget>::INSTALL p1(&action_dispatcher, "ZoomIn", &a);
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionZoomIn::press(QEvent* e)
{ untested();
	QPointF pos;
	if(auto se=dynamic_cast<QGraphicsSceneMouseEvent*>(e)){ untested();
		pos = se->scenePos();
	}else{ untested();
		incomplete();
	}
	float fX = pos.x();
	float fY = pos.y();

	qDebug() << "zoom into box";
	_MAx1 = int(fX);
	_MAy1 = int(fY);
	_MAx2 = 0;  // rectangle size
	_MAy2 = 0;

//  QucsMain->MouseMoveAction = &MouseActions::MMoveSelect;
//  QucsMain->MouseReleaseAction = &MouseActions::MReleaseZoomIn;
	
	//grabKeyboard();  // no keyboard inputs during move actions
	                 // why not redirect events to mode?!
	doc()->zoomBy(1.5);

	
//  Doc->viewport()->update();
//  setDrawn(false);
	return nullptr; // zoom is not undoable (good idea?)
}
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionZoomIn::release(QEvent* e)
{ untested();
	auto* m = prechecked_cast<QGraphicsSceneMouseEvent*>(e);
  if(m->button() == Qt::LeftButton){ untested();

	  _MAx1 = m->pos().x();
	  _MAy1 = m->pos().y();
	  incomplete(); // ("Sort out contentsX");
	  return nullptr; // not undoable.
	  /**
	  float DX = float(MAx2);
	  float DY = float(MAy2);

	  float initialScale = Doc->Scale;
	  float scale = 1;
	  float xShift = 0;
	  float yShift = 0;
	  if((Doc->Scale * DX) < 6.0) { untested();
		 // a simple click zooms by constant factor
		 scale = Doc->zoom(1.5)/initialScale;

		 xShift = scale * Event->pos().x();
		 yShift = scale * Event->pos().y();
	  } else { untested();
		 float xScale = float(Doc->visibleWidth())  / std::abs(DX);
		 float yScale = float(Doc->visibleHeight()) / std::abs(DY);
		 scale = qMin(xScale, yScale)/initialScale;
		 scale = Doc->zoom(scale)/initialScale;

		 xShift = scale * (MAx1 - 0.5*DX);
		 yShift = scale * (MAy1 - 0.5*DY);
	  }
	  xShift -= (0.5*Doc->visibleWidth() + Doc->contentsX());
	  yShift -= (0.5*Doc->visibleHeight() + Doc->contentsY());
	  Doc->scrollBy(xShift, yShift);
	  */
#if 0
	  QucsMain->MouseMoveAction = &MouseActions::MMoveZoomIn;
	  QucsMain->MouseReleaseAction = 0;
#endif
	  if(auto v=dynamic_cast<QGraphicsView*>(doc())){
		  v->releaseKeyboard();  // allow keyboard inputs again
	  }else{
	  }

  }else{ untested();
  }
  return nullptr;
}

}
