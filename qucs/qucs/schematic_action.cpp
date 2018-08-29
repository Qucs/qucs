

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

  if(App->performToggleAction(on, selectAction(), 0, 0, &MouseActions::MPressSelect)) {
    App->MouseReleaseAction = &MouseActions::MReleaseSelect;
    App->MouseDoubleClickAction = &MouseActions::MDoubleClickSelect;
  }

}
