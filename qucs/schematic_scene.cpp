
#include "schematic_scene.h"
#include "schematic.h"

// ---------------------------------------------------
// forward to graphicscene, once it is there.
// scene->itemAt(Doc->mapToScene(Event->pos()), QTransform()
ElementGraphics* Schematic::itemAt(float x, float y)
{
    for(auto pc : components()){
        if(pc->getSelected(x, y))
            return (ElementGraphics*)(pc);
	 }

    float Corr = 5.0 / Scale; // size of line select

    for(auto pp : paintings()) {
        if(pp->getSelected(x, y, Corr))
            return (ElementGraphics*)(pp);
	 }

    incomplete(); // also select the other stuff.
    return nullptr;
}
