

#include <QAction>
#include <QMessageBox> // BUG
#include <QLineEdit> // BUG?
#include <QRegExp> // BUG?
#include <QRegExpValidator>
#include <QFileDialog>
#include <QUndoCommand>

#include "schematic.h"
#include "qucs.h"
#include "misc.h"
#include "globals.h"
#include "schematic_action.h"
#include "globals.h"

#include "changedialog.h"

// TODO: different file.
class MouseActionDelete : public MouseAction{
public:
	explicit MouseActionDelete(MouseActions& ctx)
		: MouseAction(ctx){}

private:
	cmd* move(QMouseEvent*);
	cmd* press(QMouseEvent*);
	cmd* release(QMouseEvent*);
};
//   Paint a cross under the mouse cursor to show the delete mode.
QUndoCommand* MouseActionDelete::move(QMouseEvent *Event)
{
  // TODO: inactive initially?
  _ctx.Set3(Event); // BUG: use a variable with a name.

  // cannot draw on the viewport, it is displaced by the size of dock and toolbar
  //
//  there are no postpaint events. change the mouse appearance some other way?
  incomplete();
//  Doc->PostPaintEvent (_Line, MAx3-15, MAy3-15, MAx3+15, MAy3+15,0,0,false);
//  Doc->PostPaintEvent (_Line, MAx3-15, MAy3+15, MAx3+15, MAy3-15,0,0,false);

  return nullptr;
}

QUndoCommand* MouseActionDelete::release(QMouseEvent *Event){
  incomplete();
}

class deleteCommand : public QUndoCommand {
public:
  explicit deleteCommand(ElementMouseAction e){
    incomplete();
  }
};
// void MouseActions::MPressDelete(Schematic *Doc, QMouseEvent* Event)
// press a mouse while delete action is active.
// Event tells us where...
QUndoCommand* MouseActionDelete::press(QMouseEvent* Event)
{
  // QPointF pos=Doc->mapToScene(Event->pos());

  ElementMouseAction pe = _ctx.selectElement(Event->pos(), false); // BUG
  QUndoCommand* d = nullptr;

  if(pe) { untested();
    d = new deleteCommand(pe);

    // pe->setSelected();
    // Doc->deleteElements();

    // Doc->sizeOfAll(Doc->UsedX1, Doc->UsedY1, Doc->UsedX2, Doc->UsedY2);
    // Doc->viewport()->update();
    // drawn = false;
  }else{

  }
  return d;
}
/*==================================================================================*/

SchematicActions::SchematicActions(Schematic& ctx)
  : MouseActions(ctx)
{
  maDelete = new MouseActionDelete(*this);
}

SchematicActions::~SchematicActions()
{
  delete maDelete;
}

QRegExp Expr_CompProp;
QRegExpValidator Val_CompProp(Expr_CompProp, 0);

void Schematic::actionSelect(bool on)
{

  // goto to insertWire mode if ESC pressed during wiring
  if(App->MouseMoveAction == &MouseActions::MMoveWire2) {
    App->MouseMoveAction = &MouseActions::MMoveWire1;
    App->MousePressAction = &MouseActions::MPressWire1;
    viewport()->update();
    mouseActions().setDrawn(false);

    // this is implemented somewhere else
    {
    selectAction()->blockSignals(true);
    selectAction()->setChecked(false);
    selectAction()->blockSignals(false);
    }

    // see EditDelete
//  }else if(performToggleAction(on, selectAction(), 0, 0, &MouseActions::MPressSelect)) {
//    App->MouseReleaseAction = &MouseActions::MReleaseSelect;
//    App->MouseDoubleClickAction = &MouseActions::MDoubleClickSelect;
//
//  }else{ untested();
  }
}

void Schematic::actionOnGrid(bool on)
{
  performToggleAction(on, App->onGrid, &Schematic::elementsOnGrid,
		&MouseActions::MMoveOnGrid, &MouseActions::MPressOnGrid);
}

void Schematic::actionEditRotate(bool on)
{
  performToggleAction(on, App->editRotate, &Schematic::rotateElements,
		&MouseActions::MMoveRotate, &MouseActions::MPressRotate);
}

void Schematic::actionEditMirrorX(bool on)
{
  performToggleAction(on, App->editMirror, &Schematic::mirrorXComponents,
		&MouseActions::MMoveMirrorX, &MouseActions::MPressMirrorX);
}

void Schematic::actionEditMirrorY(bool on)
{
  performToggleAction(on, App->editMirrorY, &Schematic::mirrorYComponents,
		&MouseActions::MMoveMirrorY, &MouseActions::MPressMirrorY);
}

void Schematic::actionEditActivate(bool on)
{
    performToggleAction (on, App->editActivate,
        &Schematic::activateSelectedComponents,
        &MouseActions::MMoveActivate, &MouseActions::MPressActivate);
}



// getting here after somebody presses "del"
// on looks obsolete...?
void Schematic::actionEditDelete(bool on)
{
  trace1("actionEditDelete", on);
  incomplete();

#if 0 // old code
  performToggleAction(on, App->editDelete, &Schematic::deleteElements,
      &MouseActions::MMoveDelete, &MouseActions::MPressDelete);
#else // approximately this.
  incomplete();
  // why is this not done by the caller?
  // Toolbar->uncheck_current();
  //
  // if(App->activeAction) {
  //   // // App->activeAction->blockSignals(true); // do not call toggle slot
  //   // // App->activeAction->setChecked(false);       // set last toolbar button off
  //   // // App->activeAction->blockSignals(false);
  // }else{
  // }

  // App->activeAction = Action; /// another toolbar issue?

  mouseAction = mouseActions().maDelete;
  assert(mouseAction);

  viewport()->update();
  mouseActions().setDrawn(false);
#endif
}

void Schematic::actionSetWire(bool on)
{
	performToggleAction(on, App->insWire, 0,
			&MouseActions::MMoveWire1, &MouseActions::MPressWire1);

  // mouseAction = mouseActions().maSetWire;
}

void Schematic::actionInsertLabel(bool on)
{
  performToggleAction(on, App->insLabel, 0,
		&MouseActions::MMoveLabel, &MouseActions::MPressLabel);
  // mouseAction = mouseActions().maInsLabel;
}

void Schematic::actionSetMarker(bool on)
{
  performToggleAction(on, App->setMarker, 0,
		&MouseActions::MMoveMarker, &MouseActions::MPressMarker);
  // mouseAction = mouseActions().maSetMarker;
}

void Schematic::actionMoveText(bool on)
{
  performToggleAction(on, App->moveText, 0,
		&MouseActions::MMoveMoveTextB, &MouseActions::MPressMoveText);
  // mouseAction = mouseActions().maMoveText;
}

void Schematic::actionZoomIn(bool on)
{
    performToggleAction(on, App->magPlus, 0,
		&MouseActions::MMoveZoomIn, &MouseActions::MPressZoomIn);
  // mouseAction = mouseActions().maZoomIn;
}

void Schematic::actionInsertEquation(bool on)
{
  App->hideEdit(); // disable text edit of component property
  App->MouseReleaseAction = 0;
  App->MouseDoubleClickAction = 0;

  if(!on) {
    App->MouseMoveAction = 0;
    App->MousePressAction = 0;
    App->activeAction = 0;   // no action active
    return;
  }
  if(App->activeAction) {
    App->activeAction->blockSignals(true); // do not call toggle slot
    App->activeAction->setChecked(false);       // set last toolbar button off
    App->activeAction->blockSignals(false);
  }
  App->activeAction = App->insEquation;

  if(mouseActions().hasElem()){
    incomplete(); // undo??
    delete mouseActions().getElem();  // delete previously selected component
  }else{
  }

  Symbol* sym=symbol_dispatcher.clone("Eqn");
  assert(sym);
  mouseActions().setElem(prechecked_cast<Component*>(sym));
  assert(mouseActions().hasElem());

  if(mouseActions().wasDrawn()){
    viewport()->update();
  }else{
  }
  mouseActions().setDrawn(false);
  App->MouseMoveAction = &MouseActions::MMoveElement;
  App->MousePressAction = &MouseActions::MPressElement;
}

void Schematic::actionEditPaste(bool on)
{
#if 0
	// if it's not a text doc, prevent the user from editing
	// while we perform the paste operation
	App->hideEdit();

	if(!on) {
		App->MouseMoveAction = 0;
		App->MousePressAction = 0;
		App->MouseReleaseAction = 0;
		App->MouseDoubleClickAction = 0;
		App->activeAction = 0;   // no action active
		if(mouseActions().wasDrawn()) {
			viewport()->update();
		}
		return;
	}else{
        }

	if(!mouseActions()->pasteElements(this)) {
		App->editPaste->blockSignals(true); // do not call toggle slot
		App->editPaste->setChecked(false);       // set toolbar button off
		App->editPaste->blockSignals(false);
		return;   // if clipboard empty
	}else{
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

void Schematic::actionInsertGround(bool on)
{
#if 0
  App->hideEdit(); // disable text edit of component property
  App->MouseReleaseAction = 0;
  App->MouseDoubleClickAction = 0;

  if(!on) {
    App->MouseMoveAction = 0;
    App->MousePressAction = 0;
    App->activeAction = 0;   // no action active
    return;
  }

  uncheckActive();
  App->activeAction = App->insGround;

  if(mouseActions().hasElem()){
    incomplete(); // undo.
//    delete mouseActions()->selElem;  // delete previously selected component
  }

  Symbol* sym=symbol_dispatcher.clone("GND");
  assert(sym);
  mouseActions()->selElem = prechecked_cast<Component*>(sym);
  assert(mouseActions()->selElem);

  if(mouseActions().wasDrawn()){
    updateViewport();
  }else{
  }
  mouseActions().setDrawn(false)
  App->MouseMoveAction = &MouseActions::MMoveElement;
  App->MousePressAction = &MouseActions::MPressElement;
#endif
}

void Schematic::actionInsertPort(bool on)
{
#if 0
  App->hideEdit(); // disable text edit of component property
  App->MouseReleaseAction = 0;
  App->MouseDoubleClickAction = 0;

  if(!on) {
    App->MouseMoveAction = 0;
    App->MousePressAction = 0;
    App->activeAction = 0;   // no action active
    return;
  }
  if(App->activeAction) {
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

void Schematic::actionEditUndo()
{
  // really?
  App->hideEdit(); // disable text edit of component property

  undo();
  viewport()->update();
  mouseActions().setDrawn(false);
}

void Schematic::actionEditRedo()
{
  App->hideEdit(); // disable text edit of component property

  redo();
  viewport()->update();
  mouseActions().setDrawn(false);
}

void Schematic::actionAlign(int what)
{
  App->hideEdit(); // disable text edit of component property

  if(!aligning(what)){
    QMessageBox::information(this, tr("Info"),
		      tr("At least two elements must be selected !"));
  }
  viewport()->update();
  mouseActions().setDrawn(false);
}

void Schematic::actionDistrib(int d)
{
  App->hideEdit(); // disable text edit of component property

  if (d==0){
	  distributeHorizontal();
  }else if(d==1){
	  distributeVertical();
  }else{
	  unreachable();
  }
  viewport()->update();
  mouseActions().setDrawn(false);
}

void Schematic::actionSelectAll()
{
    selectElements(INT_MIN, INT_MIN, INT_MAX, INT_MAX, true);
    viewport()->update();
}

void Schematic::actionSelectMarker()
{
  App->hideEdit(); // disable text edit of component property

  selectMarkers();
  viewport()->update();
  mouseActions().setDrawn(false);
}

void Schematic::actionChangeProps()
{
	ChangeDialog *d = new ChangeDialog(this);
	if(d->exec() == QDialog::Accepted) {
		setChanged(true, true);
		viewport()->update();
	}
}

void Schematic::actionCursor(arrow_dir_t dir)
{
#ifdef USE_SCROLLVIEW
	int sign = 1;

	if(dir==arr_left){
		sign = -1;
	}

	if(App->editText->isHidden()) {
		// for edit of component property ?
		auto movingElements = cropSelectedElements();
		int markerCount=0;
		for(auto const& i : movingElements){
			if(marker(i)){
				++markerCount;
			}
		}

		if((movingElements.count() - markerCount) < 1) { // all selections are markers
			incomplete();
#if 0
			if(markerCount > 0) {  // only move marker if nothing else selected
				markerMove(dir, &movingElements);
			} else if(dir==arr_up) {
				// nothing selected at all
				if(scrollUp(verticalScrollBar()->singleStep()))
					scrollBy(0, -verticalScrollBar()->singleStep());
			} else if(dir==arr_down) {
				if(scrollDown(-verticalScrollBar()->singleStep()))
					scrollBy(0, verticalScrollBar()->singleStep());
			} else if(dir==arr_left) {
				if(scrollLeft(horizontalScrollBar()->singleStep()))
					scrollBy(-horizontalScrollBar()->singleStep(), 0);
			} else if(dir==arr_right) {
				if(scrollRight(-horizontalScrollBar()->singleStep()))
					scrollBy(horizontalScrollBar()->singleStep(), 0);
			}else{
				// unreachable. TODO: switch
			}

			viewport()->update();
			mouseActions()->drawn = false;
#endif
		}else if(dir==arr_up || dir==arr_down){
			// some random selection, put it back
			mouseActions()->moveElements(movingElements, 0, ((dir==arr_up)?-1:1) * GridY);
			mouseActions()->MAx3 = 1;  // sign for moved elements
			mouseActions()->endElementMoving(this, &movingElements);
		}else if(dir==arr_left || dir==arr_right){
			mouseActions()->moveElements(movingElements, sign*GridX, 0);
			mouseActions()->MAx3 = 1;  // sign for moved elements
			mouseActions()->endElementMoving(this, &movingElements);
		}else{
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
	}else{

	}
#endif
} // actionCursor

void Schematic::actionApplyCompText()
{
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
				if(pc->name() != editText->text()) {
					auto pc2=Doc->find_component( editText->text());
					if(!pc2) {
						pc->obsolete_name_override_hack( editText->text() );
						setChanged(true, true);  // only one undo state
					}
				}
		}
		else if(pp) {  // property was applied
			if(pp->Value != editText->text()) {
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
void Schematic::actionExportGraphAsCsv()
{
  // focusElement->exportGraphAsCsv? or so.
#if 0
  for(;;) {
    if(!mouseActions()->focusElement){
    }else if(graph(mouseActions()->focusElement)){
        break;
    }else{
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

  if(!File.open(QIODevice::WriteOnly)) {
    QMessageBox::critical(this, QObject::tr("Error"),
                          QObject::tr("Cannot create output file!"));
    return;
  }

  QTextStream Stream(&File);


  DataX const *pD;
  Graph const*g = graph(mouseActions()->focusElement);
  // First output the names of independent and dependent variables.
  for(unsigned ii=0; (pD=g->axis(ii)); ++ii){
    Stream << '\"' << pD->Var << "\";";
  }
  Stream << "\"r " << g->Var << "\";\"i " << g->Var << "\"\n";


  int n, m;
  double *py = g->cPointsY;
  int Count = g->countY * g->axis(0)->count;
  for(n = 0; n < Count; n++) {
    m = n;
	 for(unsigned ii=0; (pD=g->axis(ii)); ++ii) {
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
bool Schematic::performToggleAction(bool on, QAction *Action,
	pToggleFunc Function, pMouseFunc MouseMove, pMouseFunc2 MousePress)
{
  assert(App);
  App->hideEdit(); // disable text edit of component property

  if(!on) {

    // toolbar button has been switched off.
    App->MouseMoveAction = 0;
    App->MousePressAction = 0;
    App->MouseReleaseAction = 0;
    App->MouseDoubleClickAction = 0;
    App->activeAction = 0;

  }else{

    if(Function && (this->*Function)()) {
    // if(Function && Function->do_it(this)) {
      // some action "done"??
      Action->blockSignals(true);
      Action->setChecked(false);  // release toolbar button
      Action->blockSignals(false);
      viewport()->update();
    }else{
      // action not "done"? still active.
      //    assign some of supplementary mouseactions

      if(App->activeAction) {
        App->activeAction->blockSignals(true); // do not call toggle slot
        App->activeAction->setChecked(false);       // set last toolbar button off
        App->activeAction->blockSignals(false);
      }else{
      }

      App->activeAction = Action; /// this is a toolbar button
      App->MouseMoveAction = MouseMove;
      App->MousePressAction = MousePress;
      App->MouseReleaseAction = 0;
      App->MouseDoubleClickAction = 0;
    }

    viewport()->update();
    mouseActions().setDrawn(false);
    return true;
  }

  untested();
  return false; // really?
}

// vim:ts=8:sw=2:et
