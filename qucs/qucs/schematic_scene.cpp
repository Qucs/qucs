
#include "schematic_scene.h"
#include "schematic.h"

// ---------------------------------------------------
// forward to graphicscene, once it is there.
// scene->itemAt(Doc->mapToScene(Event->pos()), QTransform()
ElementGraphics Schematic::itemAt(float x, float y)
{
    for(Component *pc = Components->first(); pc != 0; pc = Components->next())
        if(pc->getSelected(x, y))
            return ElementGraphics(pc);

    float Corr = 5.0 / Scale; // size of line select

    for(Painting *pp = Paintings->first(); pp != 0; pp = Paintings->next())
        if(pp->getSelected(x, y, Corr))
            return ElementGraphics(pp);

    incomplete(); // also select the other stuff.
    return nullptr;
}
