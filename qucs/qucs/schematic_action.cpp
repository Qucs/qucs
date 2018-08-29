

#include <QAction>

#include "schematic.h"
#include "qucs.h"
#include "mouseactions.h"

void Schematic::actionSelect(bool on)
{

  // goto to insertWire mode if ESC pressed during wiring
  if(App->MouseMoveAction == &MouseActions::MMoveWire2) {
    App->MouseMoveAction = &MouseActions::MMoveWire1;
    App->MousePressAction = &MouseActions::MPressWire1;
    viewport()->update();
    App->view->drawn = false;

    selectAction()->blockSignals(true);
    selectAction()->setChecked(false);
    selectAction()->blockSignals(false);
    return;
  }

  if(performToggleAction(on, selectAction(), 0, 0, &MouseActions::MPressSelect)) {
    App->MouseReleaseAction = &MouseActions::MReleaseSelect;
    App->MouseDoubleClickAction = &MouseActions::MDoubleClickSelect;
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

void Schematic::actionEditDelete(bool on)
{
    performToggleAction(on, App->editDelete, &Schematic::deleteElements,
          &MouseActions::MMoveDelete, &MouseActions::MPressDelete);
}

void Schematic::actionSetWire(bool on)
{
	performToggleAction(on, App->insWire, 0,
			&MouseActions::MMoveWire1, &MouseActions::MPressWire1);
}

void Schematic::actionInsertLabel(bool on)
{
  performToggleAction(on, App->insLabel, 0,
		&MouseActions::MMoveLabel, &MouseActions::MPressLabel);
}

void Schematic::actionSetMarker(bool on)
{
  performToggleAction(on, App->setMarker, 0,
		&MouseActions::MMoveMarker, &MouseActions::MPressMarker);
}

void Schematic::actionMoveText(bool on)
{
  performToggleAction(on, App->moveText, 0,
		&MouseActions::MMoveMoveTextB, &MouseActions::MPressMoveText);
}

void Schematic::actionZoomIn(bool on)
{
    performToggleAction(on, App->magPlus, 0,
		&MouseActions::MMoveZoomIn, &MouseActions::MPressZoomIn);
}
