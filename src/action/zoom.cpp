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
class MouseActionZoomIn : public MouseAction{
public:
	explicit MouseActionZoomIn(MouseActions& ctx)
		: MouseAction(ctx) {itested();
	}
private: // MouseAction
	cmd* press(QEvent*) override;
	cmd* release(QMouseEvent*) override;

private:
	int _MAx1;
	int _MAx2;
	int _MAy1;
	int _MAy2;
};
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
	doc().zoomBy(1.5);

	
//  Doc->viewport()->update();
//  setDrawn(false);
	return nullptr; // zoom is not undoable (good idea?)
}
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionZoomIn::release(QMouseEvent* e)
{ untested();
  if(e->button() == Qt::LeftButton){ untested();

	  _MAx1 = e->pos().x();
	  _MAy1 = e->pos().y();
	  TODO("Sort out contentsX");
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
	  doc().releaseKeyboard();  // allow keyboard inputs again

  }else{ untested();
  }
  return nullptr;
}
