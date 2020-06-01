/***************************************************************************
    copyright            : (C) 2020 Felix Salfelder

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// user interaction, undo commands
// derived from various Qucs "MouseAction" etc.

#include <QAction>
#include <QMessageBox> // BUG
#include <QLineEdit> // BUG?
#include <QRegExp> // BUG?
#include <QRegExpValidator>
#include <QFileDialog>
#include <QUndoCommand>
#include <QMouseEvent>

#include "schematic_doc.h"
#include "qucs.h"
#include "misc.h"
#include "globals.h"
#include "schematic_action.h"
#include "globals.h"

#include "changedialog.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class MouseActionDelete : public MouseAction{
public:
	explicit MouseActionDelete(MouseActions& ctx)
		: MouseAction(ctx){}

private:
	cmd* move(QMouseEvent*);
	cmd* press(QMouseEvent*);
	cmd* release(QMouseEvent*);
};
/*--------------------------------------------------------------------------*/
//   was Mouseactions::MMove or so.
QUndoCommand* MouseActionDelete::move(QMouseEvent *e)
{ untested();
  // TODO: inactive initially?
  ctx().Set3(e); // BUG: use a variable with a name.

  // cannot draw on the viewport, it is displaced by the size of dock and toolbar
  //
//   Paint a cross under the mouse cursor to show the delete mode.
//  there are no postpaint events. change the mouse appearance some other way?
  incomplete();
//  Doc->PostPaintEvent (_Line, MAx3-15, MAy3-15, MAx3+15, MAy3+15,0,0,false);
//  Doc->PostPaintEvent (_Line, MAx3-15, MAy3+15, MAx3+15, MAy3-15,0,0,false);
//
  e->ignore(); // handle in QGraphicsView?

  return nullptr;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class MouseActionSelect : public MouseAction{
public:
	explicit MouseActionSelect(MouseActions& ctx)
		: MouseAction(ctx), focusElement(nullptr) {}

private:
	cmd* move(QMouseEvent*);
	cmd* press(QMouseEvent*); //was MouseActions::MPressSelect
	cmd* release(QMouseEvent*);
	//	cmd* release2(QMouseEvent*); // what is this?

private: // rectangles?
	int MAx1;
	int MAy1;
	int MAx2;
	int MAy2;
private: // more decoupling
	ElementMouseAction focusElement;
	bool isMoveEqual; //?
};
/*--------------------------------------------------------------------------*/
// was MouseActions::MMoveSelect
QUndoCommand* MouseActionSelect::move(QMouseEvent *Event)
{ untested();
	//qDebug() << "MMoveSelect " << "select area";
	ctx().Set2(Event); // BUG
	if(isMoveEqual) {    // x and y size must be equal ?
		if(abs(MAx2) > abs(MAy2)) { untested();
			if(MAx2<0){ untested();
				MAx2 = -abs(MAy2);
			}else{ untested();
				MAx2 = abs(MAy2);
			}
		} else { untested();
			if(MAy2<0) { untested();
				MAy2 = -abs(MAx2);
			} else { untested();
				MAy2 = abs(MAx2);
			}
		}
	}else{ untested();
	}

	doc().PostPaintEvent (_Rect, MAx1, MAy1, MAx2, MAy2);
	return nullptr;
}
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionSelect::press(QMouseEvent* e)
{ untested();
	auto Event = e;
	SchematicDoc* Doc = &doc();
	assert(Doc);
	QPointF pos=Doc->mapToScene(Event->pos());
	float fX=pos.x();
	float fY=pos.y();

	bool Ctrl = Event->modifiers().testFlag(Qt::ControlModifier);

	int No=0;
	MAx1 = int(fX);
	MAy1 = int(fY);
	focusElement = ctx().selectElement(Event->pos(), Ctrl, &No);
	isMoveEqual = false;   // moving not neccessarily square

	incomplete(); //this does not add up.
#if 0
	if(!focusElement){ untested();
	}else if(focusElement->Type == isDiagramHScroll){ untested();
		// BUG: move to selectElement? what is MAy1?!
		MAy1 = MAx1;
	}else{ untested();
	}
#endif

	if(!focusElement){ untested();
		qDebug() << "MPressSelect miss" << Event->pos() << pos;
	}else if(focusElement->Type == isPaintingResize){ untested();
		incomplete(); // delegate. how?
		focusElement->Type = isPainting;
		QucsMain->MouseReleaseAction = &MouseActions::MReleaseResizePainting;
		QucsMain->MouseMoveAction = &MouseActions::MMoveResizePainting;
		QucsMain->MousePressAction = 0;
		QucsMain->MouseDoubleClickAction = 0;
		Doc->grabKeyboard();  // no keyboard inputs during move actions
		// Update matching wire label highlighting
		Doc->highlightWireLabels ();
		return nullptr;
	}else if(focusElement->Type == isDiagramResize){ untested();
		incomplete();

#if 0
		if(((Diagram*)focusElement)->name().left(4) != "Rect")
			if(((Diagram*)focusElement)->name().at(0) != 'T')
				if(((Diagram*)focusElement)->name() != "Curve")
					/* if(((Diagram*)focusElement)->name() != "Waveac")
						if(((Diagram*)focusElement)->name() != "Phasor")*/
					isMoveEqual = true;  // diagram must be square
#endif

		focusElement->Type = isDiagram;
		MAx1 = focusElement->cx_();
		MAx2 = focusElement->x2_();

#if 0
		focusElement->someDiagramStateCallback()

			// old:
			// if(diagram(focusElement)->State & 1) { untested();
			//   MAx1 += MAx2;
			//   MAx2 *= -1;
			// }

			MAy1 =  focusElement->cy_();
		MAy2 = -focusElement->y2_();
		if(((Diagram*)focusElement)->State & 2) { untested();
			MAy1 += MAy2;
			MAy2 *= -1;
		}
#endif

		 // diagram_action?
		QucsMain->MouseReleaseAction = &MouseActions::MReleaseResizeDiagram;
		QucsMain->MouseMoveAction = &MouseActions::MMoveSelect;
		QucsMain->MousePressAction = 0;
		QucsMain->MouseDoubleClickAction = 0;
		Doc->grabKeyboard(); // no keyboard inputs during move actions
		// Update matching wire label highlighting
		Doc->highlightWireLabels ();
		return nullptr;

#if 0
	}else if(focusElement->Type == isDiagramHScroll
			|| focusElement->Type == isDiagramVScroll){ untested();
		incomplete();

		focusElement->Type = isDiagram; // reset happens here. FIXME.

		auto d=diagram(focusElement); // is this necessary?!
		assert(d);
		No = d->scroll(MAy1);

		switch(No) {
		case 1:
			Doc->setChanged(true, true, 'm'); // 'm' = only the first time
			break;
		case 2:  // move scroll bar with mouse cursor
			QucsMain->MouseMoveAction = &MouseActions::MMoveScrollBar;
			QucsMain->MousePressAction = 0;
			QucsMain->MouseDoubleClickAction = 0;
			Doc->grabKeyboard();  // no keyboard inputs during move actions

			// Remember inital scroll bar position.
			MAx2 = int(d->xAxis_limit_min());
			// Update matching wire label highlighting
			Doc->highlightWireLabels ();
			return;
		}
		// Update matching wire label highlighting
		Doc->highlightWireLabels ();
		Doc->viewport()->update();
		drawn = false;
		return;

	}else if(focusElement->Type == isComponentText){ untested();
		incomplete();

		focusElement->Type &= (~isComponentText) | isComponent;

		MAx3 = No;
		QucsMain->slotApplyCompText();
		// Update matching wire label highlighting
		Doc->highlightWireLabels ();
		return;

	}else if(auto n=node(focusElement)){ untested();
		(void)n;
		if (QucsSettings.NodeWiring) { untested();
			incomplete();

			MAx1 = 0;   // paint wire corner first up, then left/right
			MAx3 = focusElement->cx_();  // works even if node is not on grid
			MAy3 = focusElement->cy_();
			QucsMain->MouseMoveAction = &MouseActions::MMoveWire2;
			QucsMain->MousePressAction = &MouseActions::MPressWire2;
			QucsMain->MouseReleaseAction = 0; // if function is called from elsewhere
			QucsMain->MouseDoubleClickAction = 0;

			formerAction = QucsMain->select; // to restore action afterwards
			QucsMain->activeAction = QucsMain->insWire;

			QucsMain->select->blockSignals(true);
			QucsMain->select->setChecked(false);
			QucsMain->select->blockSignals(false);

			QucsMain->insWire->blockSignals(true);
			QucsMain->insWire->setChecked(true);
			QucsMain->insWire->blockSignals(false);
			// Update matching wire label highlighting
			Doc->highlightWireLabels ();
			return;
		}else{ untested();
		}
#endif
	}else{ untested();
		// default case
		// unreachable?
	}

	QucsMain->MousePressAction = 0;
	QucsMain->MouseDoubleClickAction = 0;
	Doc->grabKeyboard();  // no keyboard inputs during move actions
	Doc->viewport()->update();
	//setDrawn(false);

	if(!focusElement) { untested();
		MAx2 = 0;  // if not clicking on an element => open a rectangle
		MAy2 = 0;
		// QucsMain->MouseReleaseAction = &MouseActions::MReleaseSelect2;
		// QucsMain->MouseMoveAction = &MouseActions::MMoveSelect;
		//state = drag_rectangle;
	}else{ untested();
		// element could be moved
		if(Ctrl) { untested();
		}else{ untested();
			if(!focusElement->isSelected()) { untested();
				// Don't move selected elements if clicked
				// deselectElements(focusElement); // element was not selected.
			}else{ untested();
			}
			focusElement->setSelected();
		}
		Doc->setOnGrid(MAx1, MAy1);
		QucsMain->MouseMoveAction = &MouseActions::MMoveMoving;
	}
	// Update matching wire label highlighting
	assert(Doc);
	// Doc->highlightWireLabels ();
	e->ignore(); // handle in QGraphicsView?
	return nullptr;
} // select::press
/*--------------------------------------------------------------------------*/
// was MouseActions::MReleaseSelect(SchematicDoc *Doc, QMouseEvent *Event)
QUndoCommand* MouseActionSelect::release(QMouseEvent *Event)
{ untested();
	bool ctrl = Event->modifiers().testFlag(Qt::ControlModifier);

	if(!ctrl) { untested();
		incomplete();
//		ctx().deselectElements(focusElement);
	}else{ untested();
	}

	if(focusElement && Event->button() == Qt::LeftButton){ untested();
		if(auto w=wire(focusElement)) { untested();
			incomplete();
			(void)w;
#if 0
			Doc->selectWireLine(element(focusElement), w->portValue(0), ctrl);
			Doc->selectWireLine(element(focusElement), w->portValue(1), ctrl);
#endif
		}else{ untested();
		}
	}else{ untested();
	}

	doc().releaseKeyboard();

#if 0
	// is it not in "select" mode already?
	QucsMain->MouseMoveAction = 0;
	QucsMain->MousePressAction = &MouseActions::MPressSelect;
	QucsMain->MouseReleaseAction = &MouseActions::MReleaseSelect;
	QucsMain->MouseDoubleClickAction = &MouseActions::MDoubleClickSelect;
#endif

//	Doc->highlightWireLabels ();
	updateViewport();
	// drawn = false;
	return nullptr;
} // select::release
/*--------------------------------------------------------------------------*/
class deleteCommand : public QUndoCommand {
public:
  explicit deleteCommand(ElementMouseAction e){ untested();
    incomplete();
  }
  void undo() override{ untested();
    incomplete();
  }
  void redo() override{ untested();
    incomplete();
  }
};
/*--------------------------------------------------------------------------*/
// void MouseActions::MPressDelete(Schematic *Doc, QMouseEvent* Event)
// press a mouse while delete action is active.
// Event tells us where...
QUndoCommand* MouseActionDelete::press(QMouseEvent* Event)
{ untested();
  // QPointF pos=Doc->mapToScene(Event->pos());

  ElementMouseAction pe = ctx().selectElement(Event->pos(), false); // BUG
  QUndoCommand* d = nullptr;

  if(pe) { untested();
    d = new deleteCommand(pe);

    // pe->setSelected();
    // Doc->deleteElements();

    // Doc->sizeOfAll(Doc->UsedX1, Doc->UsedY1, Doc->UsedX2, Doc->UsedY2);
    // Doc->viewport()->update();
    // drawn = false;
  }else{ untested();

  }
  return d;
} // delete::press
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionDelete::release(QMouseEvent*)
{ untested();
  incomplete();
  return nullptr;
} // delete::release
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SchematicActions::SchematicActions(SchematicDoc& ctx)
  : MouseActions(ctx)
{ untested();
  maDelete = new MouseActionDelete(*this);
  maSelect = new MouseActionSelect(*this);
//  maMove = new MouseActionMove(*this);

  // TODO
  maActivate = new MouseAction(*this);
  maMirrorX = new MouseAction(*this);
  maMirrorY = new MouseAction(*this);
  maRotate = new MouseAction(*this);

  // this was in App previously, and scattered across a couple of pointer hacks.
  // possibly initialised to "select". recheck.
  _maCurrent = maSelect;
}
/*--------------------------------------------------------------------------*/
SchematicActions::~SchematicActions()
{ untested();
	delete maDelete;
	delete maSelect;
}
/*--------------------------------------------------------------------------*/
SchematicDoc* SchematicActions::doc()
{ untested();
	auto d=dynamic_cast<SchematicDoc*>(&_doc);
	assert(d);
	return d;
}
/*--------------------------------------------------------------------------*/
void SchematicActions::updateViewport()
{ untested();
	doc()->updateViewport();	
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

QRegExp Expr_CompProp;
QRegExpValidator Val_CompProp(Expr_CompProp, 0);

// forward to mouseActions...
void SchematicDoc::actionSelect(QAction* sender)
{ untested();

  // goto to insertWire mode if ESC pressed during wiring
  if(App->MouseMoveAction == &MouseActions::MMoveWire2) { untested();
	  incomplete();
#if 0
    App->MouseMoveAction = &MouseActions::MMoveWire1;
    App->MousePressAction = &MouseActions::MPressWire1;
    viewport()->update();
    mouseActions().setDrawn(false);


    // see EditDelete
//  }else if(performToggleAction(on, selectAction(), 0, 0, &MouseActions::MPressSelect)) { untested();
//    App->MouseReleaseAction = &MouseActions::MReleaseSelect;
//    App->MouseDoubleClickAction = &MouseActions::MDoubleClickSelect;
//
//  }else{ untested();
#endif
  }else{ untested();
  }

  // sender is a button. maSelect is an action. connect the two.
  // this looks a bit redundant
  // if(on)..?
  possiblyToggleAction(schematicActions().maSelect, sender);

} // SchematicDoc::actionSelect

void SchematicDoc::actionOnGrid(QAction* sender)
{ untested();
  possiblyToggleAction(schematicActions().maOnGrid, sender);
//  performToggleAction(on, App->onGrid, &SchematicDoc::elementsOnGrid,
//		&MouseActions::MMoveOnGrid, &MouseActions::MPressOnGrid);
}

void SchematicDoc::actionEditRotate(QAction* sender)
{ untested();
  possiblyToggleAction(schematicActions().maRotate, sender);
//  performToggleAction(on, App->editRotate, &SchematicDoc::rotateElements,
//		&MouseActions::MMoveRotate, &MouseActions::MPressRotate);
}

void SchematicDoc::actionEditMirrorX(QAction* sender)
{ untested();
  possiblyToggleAction(schematicActions().maMirrorX, sender);
//  performToggleAction(on, App->editMirror, &SchematicDoc::mirrorXComponents,
//		&MouseActions::MMoveMirrorX, &MouseActions::MPressMirrorX);
}

void SchematicDoc::actionEditMirrorY(QAction* sender)
{ untested();
  possiblyToggleAction(schematicActions().maMirrorY, sender);
//  performToggleAction(on, App->editMirrorY, &SchematicDoc::mirrorYComponents,
//		&MouseActions::MMoveMirrorY, &MouseActions::MPressMirrorY);
}

void SchematicDoc::actionEditActivate(QAction* sender)
{ untested();
  possiblyToggleAction(schematicActions().maActivate, sender);
}

// getting here after somebody presses "del"
// on looks obsolete...?
void SchematicDoc::actionEditDelete(QAction* sender)
{ untested();
#if 0 // old code
  performToggleAction(on, App->editDelete, &SchematicDoc::deleteElements,
      &MouseActions::MMoveDelete, &MouseActions::MPressDelete);
#else // approximately this.
  possiblyToggleAction(schematicActions().maDelete, sender);
#endif

  updateViewport();
  assert(mouseActions());
//  mouseActions()->setDrawn(false);
}

void SchematicDoc::actionSetWire(QAction* on)
{ untested();
  incomplete();
//	performToggleAction(on, App->insWire, 0,
//			&MouseActions::MMoveWire1, &MouseActions::MPressWire1);
//
  // mouseAction = mouseActions().maSetWire;
}

void SchematicDoc::actionInsertLabel(QAction* on)
{ untested();
  incomplete();
//  performToggleAction(on, App->insLabel, 0,
//		&MouseActions::MMoveLabel, &MouseActions::MPressLabel);
  // mouseAction = mouseActions().maInsLabel;
}

void SchematicDoc::actionSetMarker(QAction* on)
{ untested();
  incomplete();
//  performToggleAction(on, App->setMarker, 0,
//		&MouseActions::MMoveMarker, &MouseActions::MPressMarker);
  // mouseAction = mouseActions().maSetMarker;
}

void SchematicDoc::actionMoveText(QAction* on)
{ untested();
  incomplete();
//  performToggleAction(on, App->moveText, 0,
//		&MouseActions::MMoveMoveTextB, &MouseActions::MPressMoveText);
  // mouseAction = mouseActions().maMoveText;
}

void SchematicDoc::actionZoomIn(QAction* on)
{ untested();
  incomplete();
//    performToggleAction(on, App->magPlus, 0,
//		&MouseActions::MMoveZoomIn, &MouseActions::MPressZoomIn);
  // mouseAction = mouseActions().maZoomIn;
}

void SchematicDoc::actionInsertEquation(QAction* on)
{ untested();
	incomplete();
#if 0
  App->hideEdit(); // disable text edit of component property
  App->MouseReleaseAction = 0;
  App->MouseDoubleClickAction = 0;

  if(!on) { untested();
    App->MouseMoveAction = 0;
    App->MousePressAction = 0;
    App->activeAction = 0;   // no action active
    return;
  }
  if(App->activeAction) { untested();
    App->activeAction->blockSignals(true); // do not call toggle slot
    App->activeAction->setChecked(false);       // set last toolbar button off
    App->activeAction->blockSignals(false);
  }
  App->activeAction = App->insEquation;

  if(mouseActions().hasElem()){ untested();
    incomplete(); // undo??
    delete mouseActions().getElem();  // delete previously selected component
  }else{ untested();
  }

  Symbol* sym=symbol_dispatcher.clone("Eqn");
  assert(sym);
  mouseActions().setElem(prechecked_cast<Component*>(sym));
  assert(mouseActions().hasElem());

  if(mouseActions().wasDrawn()){ untested();
    updateViewport();
  }else{ untested();
  }
  mouseActions().setDrawn(false);
  App->MouseMoveAction = &MouseActions::MMoveElement;
  App->MousePressAction = &MouseActions::MPressElement;
#endif
}

void SchematicDoc::actionEditPaste(QAction* on)
{ untested();
#if 0
	// if it's not a text doc, prevent the user from editing
	// while we perform the paste operation
	App->hideEdit();

	if(!on) { untested();
		App->MouseMoveAction = 0;
		App->MousePressAction = 0;
		App->MouseReleaseAction = 0;
		App->MouseDoubleClickAction = 0;
		App->activeAction = 0;   // no action active
		if(mouseActions().wasDrawn()) { untested();
			viewport()->update();
		}
		return;
	}else{ untested();
        }

	if(!mouseActions()->pasteElements(this)) { untested();
		App->editPaste->blockSignals(true); // do not call toggle slot
		App->editPaste->setChecked(false);       // set toolbar button off
		App->editPaste->blockSignals(false);
		return;   // if clipboard empty
	}else{ untested();
        }

	uncheckActive();
	App->activeAction = App->editPaste;

        mouseActions().setDrawn(false)
	App->MouseMoveAction = &MouseActions::MMovePaste;
	mouseActions()->movingRotated = 0;
	App->MousePressAction = 0;
	App->MouseReleaseAction = 0;
	App->MouseDoubleClickAction = 0;

#endif
}

void SchematicDoc::actionInsertGround(QAction* sender)
{ untested();
#if 0
  App->hideEdit(); // disable text edit of component property
  App->MouseReleaseAction = 0;
  App->MouseDoubleClickAction = 0;

  if(!on) { untested();
    App->MouseMoveAction = 0;
    App->MousePressAction = 0;
    App->activeAction = 0;   // no action active
    return;
  }

  uncheckActive();
  App->activeAction = App->insGround;

  if(mouseActions().hasElem()){ untested();
    incomplete(); // undo.
//    delete mouseActions()->selElem;  // delete previously selected component
  }

  Symbol* sym=symbol_dispatcher.clone("GND");
  assert(sym);
  mouseActions()->selElem = prechecked_cast<Component*>(sym);
  assert(mouseActions()->selElem);

  if(mouseActions().wasDrawn()){ untested();
    updateViewport();
  }else{ untested();
  }
  mouseActions().setDrawn(false)
  App->MouseMoveAction = &MouseActions::MMoveElement;
  App->MousePressAction = &MouseActions::MPressElement;
#endif
}

void SchematicDoc::actionInsertPort(QAction* sender)
{ untested();
#if 0
  App->hideEdit(); // disable text edit of component property
  App->MouseReleaseAction = 0;
  App->MouseDoubleClickAction = 0;

  if(!on) { untested();
    App->MouseMoveAction = 0;
    App->MousePressAction = 0;
    App->activeAction = 0;   // no action active
    return;
  }
  if(App->activeAction) { untested();
    App->activeAction->blockSignals(true); // do not call toggle slot
    App->activeAction->setChecked(false);       // set last toolbar button off
    App->activeAction->blockSignals(false);
  }
  App->activeAction = App->insPort;

  if(mouseActions()->selElem)
    delete mouseActions()->selElem;  // delete previously selected component

  Symbol* sym=symbol_dispatcher.clone("Port");
  assert(sym);
  mouseActions()->selElem = prechecked_cast<Component*>(sym);
  assert(mouseActions()->selElem);

  if(mouseActions()->drawn) viewport()->update();
  mouseActions()->drawn = false;
  App->MouseMoveAction = &MouseActions::MMoveElement;
  App->MousePressAction = &MouseActions::MPressElement;
#endif
}

void SchematicDoc::actionEditUndo(QAction* sender)
{ untested();
  // really?
  App->hideEdit(); // disable text edit of component property

  undo();
  updateViewport();
  assert(mouseActions());
  setDrawn(false);
}

void SchematicDoc::actionEditRedo(QAction* sender)
{ untested();
  App->hideEdit(); // disable text edit of component property

  redo();
  updateViewport();
  setDrawn(false);
}

void SchematicDoc::actionAlign(int what)
{ untested();
  App->hideEdit(); // disable text edit of component property

  if(!aligning(what)){ untested();
    QMessageBox::information(this, tr("Info"),
		      tr("At least two elements must be selected !"));
  }
  updateViewport();
  assert(mouseActions());
  setDrawn(false);
}

void SchematicDoc::actionDistrib(int d)
{ untested();
  App->hideEdit(); // disable text edit of component property

  if (d==0){ untested();
	  distributeHorizontal();
  }else if(d==1){ untested();
	  distributeVertical();
  }else{ untested();
	  unreachable();
  }
  updateViewport();
  setDrawn(false);
}

void SchematicDoc::actionSelectAll(QAction*)
{ untested();
    selectElements(INT_MIN, INT_MIN, INT_MAX, INT_MAX, true);
    updateViewport();
}

void SchematicDoc::actionSelectMarker()
{ untested();
  App->hideEdit(); // disable text edit of component property

  selectMarkers();
  updateViewport();
  setDrawn(false);
}

void SchematicDoc::actionChangeProps(QAction*)
{ untested();
	ChangeDialog *d = new ChangeDialog(this);
	if(d->exec() == QDialog::Accepted) { untested();
		setChanged(true, true);
		updateViewport();
	}
}

void SchematicDoc::actionCursor(arrow_dir_t dir)
{ untested();
#ifdef USE_SCROLLVIEW
	int sign = 1;

	if(dir==arr_left){ untested();
		sign = -1;
	}

	if(App->editText->isHidden()) { untested();
		// for edit of component property ?
		auto movingElements = cropSelectedElements();
		int markerCount=0;
		for(auto const& i : movingElements){ untested();
			if(marker(i)){ untested();
				++markerCount;
			}
		}

		if((movingElements.count() - markerCount) < 1) { // all selections are markers
			incomplete();
#if 0
			if(markerCount > 0) {  // only move marker if nothing else selected
				markerMove(dir, &movingElements);
			} else if(dir==arr_up) { untested();
				// nothing selected at all
				if(scrollUp(verticalScrollBar()->singleStep()))
					scrollBy(0, -verticalScrollBar()->singleStep());
			} else if(dir==arr_down) { untested();
				if(scrollDown(-verticalScrollBar()->singleStep()))
					scrollBy(0, verticalScrollBar()->singleStep());
			} else if(dir==arr_left) { untested();
				if(scrollLeft(horizontalScrollBar()->singleStep()))
					scrollBy(-horizontalScrollBar()->singleStep(), 0);
			} else if(dir==arr_right) { untested();
				if(scrollRight(-horizontalScrollBar()->singleStep()))
					scrollBy(horizontalScrollBar()->singleStep(), 0);
			}else{ untested();
				// unreachable. TODO: switch
			}

			updateViewport();
			mouseActions()->drawn = false;
#endif
		}else if(dir==arr_up || dir==arr_down){ untested();
			// some random selection, put it back
			mouseActions()->moveElements(movingElements, 0, ((dir==arr_up)?-1:1) * GridY);
			mouseActions()->MAx3 = 1;  // sign for moved elements
			mouseActions()->endElementMoving(this, &movingElements);
		}else if(dir==arr_left || dir==arr_right){ untested();
			mouseActions()->moveElements(movingElements, sign*GridX, 0);
			mouseActions()->MAx3 = 1;  // sign for moved elements
			mouseActions()->endElementMoving(this, &movingElements);
		}else{ untested();
			//unreachable(); //TODO: switch.
		}

	}else if(dir==arr_up){ // BUG: redirect.
		if(mouseActions()->MAx3 == 0) return;  // edit component namen ?
		Component *pc = component(mouseActions()->focusElement);
		Property *pp = pc->Props.at(mouseActions()->MAx3-1);  // current property
		int Begin = pp->Description.indexOf('[');
		if(Begin < 0) return;  // no selection list ?
		int End = pp->Description.indexOf(App->editText->text(), Begin); // current
		if(End < 0) return;  // should never happen
		End = pp->Description.lastIndexOf(',', End);
		if(End < Begin) return;  // was first item ?
		End--;
		int Pos = pp->Description.lastIndexOf(',', End);
		if(Pos < Begin) Pos = Begin;   // is first item ?
		Pos++;
		if(pp->Description.at(Pos) == ' ') Pos++; // remove leading space
		App->editText->setText(pp->Description.mid(Pos, End-Pos+1));
		App->editText->selectAll();
	}else if(dir==arr_down) { // BUG: redirect.
		if(mouseActions()->MAx3 == 0) return;  // edit component namen ?
		Component *pc = component(mouseActions()->focusElement);
		Property *pp = pc->Props.at(mouseActions()->MAx3-1);  // current property
		int Pos = pp->Description.indexOf('[');
		if(Pos < 0) return;  // no selection list ?
		Pos = pp->Description.indexOf(App->editText->text(), Pos); // current list item
		if(Pos < 0) return;  // should never happen
		Pos = pp->Description.indexOf(',', Pos);
		if(Pos < 0) return;  // was last item ?
		Pos++;
		if(pp->Description.at(Pos) == ' ') Pos++; // remove leading space
		int End = pp->Description.indexOf(',', Pos);
		if(End < 0) {  // is last item ?
			End = pp->Description.indexOf(']', Pos);
			if(End < 0) return;  // should never happen
		}
		App->editText->setText(pp->Description.mid(Pos, End-Pos));
		App->editText->selectAll();
	}else{ untested();

	}
#endif
} // actionCursor

void SchematicDoc::actionApplyCompText()
{ untested();
#if 0 // what is focusElement??
	auto Doc=this;
	auto editText=App->editText;

	QString s;
	QFont f = QucsSettings.font;
	f.setPointSizeF(Scale * float(f.pointSize()) );
	editText->setFont(f);

	Property  *pp = 0;
	Component *pc = component(mouseActions()->focusElement);
	if(!pc) return;  // should never happen
	mouseActions()->MAx1 = pc->cx_() + pc->tx;
	mouseActions()->MAy1 = pc->cy_() + pc->ty;

	int z, n=0;  // "n" is number of property on screen
	pp = pc->Props.first();
	for(z=mouseActions()->MAx3; z>0; z--) {  // calculate "n"
		if(!pp) {  // should never happen
			App->hideEdit();
			return;
		}
		if(pp->display) n++;   // is visible ?
		pp = pc->Props.next();
	}

	pp = 0;
	if(mouseActions()->MAx3 > 0)  pp = pc->Props.at(mouseActions()->MAx3-1); // current property
	else s = pc->name();

	if(!editText->isHidden()) {   // is called the first time ?
		// no -> apply value to current property
		if(mouseActions()->MAx3 == 0) {   // component name ?
			if(!editText->text().isEmpty())
				if(pc->name() != editText->text()) { untested();
					auto pc2=Doc->find_component( editText->text());
					if(!pc2) { untested();
						pc->obsolete_name_override_hack( editText->text() );
						setChanged(true, true);  // only one undo state
					}
				}
		}
		else if(pp) {  // property was applied
			if(pp->Value != editText->text()) { untested();
				pp->Value = editText->text();
				recreateSymbol(pc);  // because of "Num" and schematic symbol
				setChanged(true, true); // only one undo state
			}
		}

		n++;     // next row on screen
		(mouseActions()->MAx3)++;  // next property
		pp = pc->Props.at(mouseActions()->MAx3-1);  // search for next property

		viewport()->update();
                mouseActions().setDrawn(false);

		if(!pp) {     // was already last property ?
			App->hideEdit();
			return;
		}


		while(!pp->display) {  // search for next visible property
			(mouseActions()->MAx3)++;  // next property
			pp = pc->Props.next();
			if(!pp) {     // was already last property ?
				App->hideEdit();
				return;
			}
		}
	}

	// avoid seeing the property text behind the line edit
	if(pp)  // Is it first property or component name ?
		s = pp->Value;
	editText->setMinimumWidth(editText->fontMetrics().width(s)+4);

  incomplete();
  /// Doc->contentsToViewport(int(Doc->Scale * float(view->MAx1 - Doc->ViewX1)),
  ///			 int(Doc->Scale * float(view->MAy1 - Doc->ViewY1)),
  ///	 view->MAx2, view->MAy2);
	editText->setReadOnly(false);
	if(pp) {  // is it a property ?
		s = pp->Value;
		mouseActions()->MAx2 += editText->fontMetrics().width(pp->Name+"=");
		if(pp->Description.indexOf('[') >= 0)  // is selection list ?
			editText->setReadOnly(true);
		Expr_CompProp.setPattern("[^\"]*");
		if(!pc->showName) n--;
	}
	else   // it is the component name
		Expr_CompProp.setPattern("[\\w_]+");
	Val_CompProp.setRegExp(Expr_CompProp);
	editText->setValidator(&Val_CompProp);

	z = editText->fontMetrics().lineSpacing();
	mouseActions()->MAy2 += n*z;
	editText->setText(s);
	misc::setWidgetBackgroundColor(editText, QucsSettings.BGColor);
	editText->setFocus();
	editText->selectAll();
	// make QLineEdit editable on mouse click
	QPoint p = QPoint(mouseActions()->MAx2, mouseActions()->MAy2);
	editText->setParent(Doc->viewport());
	App->editText->setGeometry(p.x(), p.y(), App->editText->width(), App->editText->height());
	App->editText->show();
#endif
}

// BUG: this is a diagram function.
void SchematicDoc::actionExportGraphAsCsv()
{ untested();
  // focusElement->exportGraphAsCsv? or so.
#if 0
  for(;;) { untested();
    if(!mouseActions()->focusElement){ untested();
    }else if(graph(mouseActions()->focusElement)){ untested();
        break;
    }else{ untested();
    }

    QMessageBox::critical(this, tr("Error"), tr("Please select a diagram graph!"));
    return;
  }

  /*QString s = Q3FileDialog::getSaveFileName(
     lastDir.isEmpty() ? QString(".") : lastDir,
     tr("CSV file")+" (*.csv);;" + tr("Any File")+" (*)",
     this, 0, tr("Enter an Output File Name"));
     */
  QString s = QFileDialog::getSaveFileName(this, tr("Enter an Output File Name"),
    lastDir.isEmpty() ? QString(".") : lastDir, tr("CSV file")+" (*.csv);;" + tr("Any File")+" (*)");

  if(s.isEmpty())
    return;

  QFileInfo Info(s);
  lastDir = Info.absolutePath();  // remember last directory
  if(Info.suffix().isEmpty())
    s += ".csv";

  QFile File(s);
  if(File.exists())
    if(QMessageBox::information(this, tr("Info"),
          tr("Output file already exists!")+"\n"+tr("Overwrite it?"),
          tr("&Yes"), tr("&No"), 0,1,1))
      return;

  if(!File.open(QIODevice::WriteOnly)) { untested();
    QMessageBox::critical(this, QObject::tr("Error"),
                          QObject::tr("Cannot create output file!"));
    return;
  }

  QTextStream Stream(&File);


  DataX const *pD;
  Graph const*g = graph(mouseActions()->focusElement);
  // First output the names of independent and dependent variables.
  for(unsigned ii=0; (pD=g->axis(ii)); ++ii){ untested();
    Stream << '\"' << pD->Var << "\";";
  }
  Stream << "\"r " << g->Var << "\";\"i " << g->Var << "\"\n";


  int n, m;
  double *py = g->cPointsY;
  int Count = g->countY * g->axis(0)->count;
  for(n = 0; n < Count; n++) { untested();
    m = n;
	 for(unsigned ii=0; (pD=g->axis(ii)); ++ii) { untested();
      Stream << *(pD->Points + m%pD->count) << ';';
      m /= pD->count;
    }

    Stream << *(py) << ';' << *(py+1) << '\n';
    py += 2;
  }

  File.close();
#endif
}

// -----------------------------------------------------------------------
/*!
 * \brief QucsApp::performToggleAction
 * \param on
 * \param Action
 * \param Function
 * \param MouseMove
 * \param MousePress
 * \return
 * This function is called from all toggle actions.
 * Used in combination with slots to set function pointers to the methods
 * that serve the mouse actions, ie. press, move, release, double click.
 */

#if 0
bool SchematicDoc::performToggleAction(bool on, QAction *Action,
	pToggleFunc Function, pMouseFunc MouseMove, pMouseFunc2 MousePress)
{ untested();
  assert(false); // obsolete
  assert(App);
  App->hideEdit(); // disable text edit of component property

  assert(on == Action.isChecked()); // why both?!

  if(!on) { untested();

    // toolbar button has been switched off.
    App->MouseMoveAction = 0;
    App->MousePressAction = 0;
    App->MouseReleaseAction = 0;
    App->MouseDoubleClickAction = 0;
    App->activeAction = 0;

  }else{ untested();

    if(Function && (this->*Function)()) { untested();
      // some action "done"?? dont switch over to this one.
		// TODO
		//
      Action->blockSignals(true);
      Action->setChecked(false);  // release toolbar button
      Action->blockSignals(false);
      viewport()->update();
    }else{ untested();
      // action not "done"? leave active.

      if(App->activeAction) { untested();
        App->activeAction->blockSignals(true); // do not call toggle slot
        App->activeAction->setChecked(false);       // set last toolbar button off
        App->activeAction->blockSignals(false);
      }else{ untested();
      }

      App->activeAction = Action; /// this is a toolbar button
      App->MouseMoveAction = MouseMove;
      App->MousePressAction = MousePress;
      App->MouseReleaseAction = 0;
      App->MouseDoubleClickAction = 0;
    }

    viewport()->update();
    setDrawn(false);
    return true;
  }

  untested();
  return false; // really?
} // performtoggleAction
#endif
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
