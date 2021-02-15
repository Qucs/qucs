/***************************************************************************
    copyright            : (C) 2006 by Michael Margraf
                               2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <stdlib.h>
#include <limits.h>

#include "io_trace.h"
#include "doc_actions.h"
#include "platform.h"
#include "qt_compat.h"

/*--------------------------------------------------------------------------*/

/* *******************************************************************
   *****                                                         *****
   *****                  Actions with markers                   *****
   *****                                                         *****
   ******************************************************************* */

#if 0
Marker* SchematicDoc::setMarker(int x, int y)
{
  // only diagrams ...
  for(auto pd : diagrams()){
    if(Marker* m=pd->setMarker(x,y)){
      setChanged(true, true);
      return m;
    }
  }
  return NULL;
}
#endif

// ---------------------------------------------------
// Moves the marker pointer left/right on the graph.
#if 0
void SchematicDoc::markerLeftRight(bool left, Q3PtrList<ElementGraphics> *Elements)
{
    bool acted = false;
    for(auto i : *Elements) {
        Marker* pm = prechecked_cast<Marker*>(i);
        assert(pm);
        if(pm->moveLeftRight(left))
            acted = true;
    }

    if(acted)  setChanged(true, true, 'm');
}

// ---------------------------------------------------
// Move the marker pointer up/down on the more-dimensional graph.
void SchematicDoc::markerUpDown(bool up, Q3PtrList<ElementGraphics> *Elements)
{
    Marker *pm;
    bool acted = false;
    for(pm = (Marker*)Elements->first(); pm!=0; pm = (Marker*)Elements->next())
    {
        if(pm->moveUpDown(up))
            acted = true;
    }

    if(acted)  setChanged(true, true, 'm');
}

#endif

/* *******************************************************************
   *****                                                         *****
   *****               Actions with all elements                 *****
   *****                                                         *****
   ******************************************************************* */

/* Selects a mouse action. onluy called from mouseaction.cpp
   Returns a mouse action.

   If 'flag' is true, the element can be deselected. If
   'flag' is false the element cannot be deselected. The
   purpose of this is to prevent deselection in cases such
   as right-clicking on a selected element to get a context
   menu.
*/

#if 0 // obsolete.
{ untested();
    (void) index;
   // THIS IS MISLEADING. it is also used to generate mouse actions.
   // we need something that produces actions, not Elements
   // actions will be needed to implement the undo stack... etcpp
#ifndef USE_SCROLLVIEW
    ElementGraphics *pe_sel=nullptr;
    ElementGraphics *pe_1st=nullptr;
    SchematicDoc* Doc = prechecked_cast<SchematicDoc*>(&_doc);
    assert(Doc);
   auto scenepos=Doc->mapToScene(xy);

   ElementGraphics* e=Doc->itemAt(scenepos);
   if(e){ untested();
       qDebug() << "clicked on something";
   }else{ untested();
       qDebug() << "doc says there is nothing";
   }

   // now add the mouseaction hacks...
   if(component(e)){ untested();
       ElementMouseAction pc(e);
       if(flag) { untested();
	   // The element can be deselected
	   pc->setSelected(!pc->isSelected());
	   return ElementMouseAction(pc);
       }else if(pe_sel) { untested();
	   pe_sel->setSelected(false);
	   return ElementMouseAction(pc);
       }else if(!pe_1st) { untested();
	   incomplete();
	   // give access to elements lying beneath
	   pe_1st = element(pc);
       } if(pc->isSelected()) {
	   incomplete();
	   pe_sel = element(pc);
       }else{
       }
   }else if(auto pd=diagram(e)){ untested();
       //TODO: check what legacy code does (below)
       incomplete();
       (void) pd;
       pe_1st = e;
       return ElementMouseAction(e);
   }else if(wire(e)){ untested();
       return ElementMouseAction(e);
   }else{
       incomplete(); // possibly.
	   qDebug() << "nothing at" << xy << scenepos;
       return ElementMouseAction(nullptr);
   }
    return ElementMouseAction(pe_1st);
#else

    int n;

    QPointF pos=Doc->mapToScene(xy);
    float fX=pos.x();
    float fY=pos.y();
    int x=int(fX);
    int y=int(fY);
    Element *pe_1st = 0;
    Element *pe_sel = 0;
    WireLabel *pl = 0;
    float Corr = Doc->textCorr(); // ??

    // test all nodes and their labels
    for(Node *pn = Doc->nodes().last(); pn != 0; pn = Doc->nodes().prev()) {
        if(flag) {
            // The element can be deselected
	}else if(!index) {
	    // 'index' is only true if called from MouseActions::MPressSelect()
	}else if(pn->getSelected(x, y)) {
	    // Return the node pointer, as the selection cannot change
	    return ElementMouseAction(pn);
        }

        pl = pn->Label; // Get any wire label associated with the Node
        if(!pl) {
	}else if(pl->getSelected(x, y)) {
                if(flag) {
                    // The element can be deselected, so toggle its isSelected member
                    // TODO: I don't see a need for the xor here, a simple ! on the current value
                    // would be clearer and have the same effect?
                    pl->toggleSelected();
                    return ElementMouseAction(pl);
                }else if(pe_sel) {
                    // There is another currently
                    pe_sel->setSelected(false);
                    return ElementMouseAction(pl);
                }else if(pe_1st == 0) {
                    // give access to elements lying beneath by storing this label.
                    // If no label pointer (or other element) has previously been
                    // stored, the current label pointer is stored here.
                    // pe_1st is returned if no other selected element
                    pe_1st = pl;
                }else{
		}

                if(pl->isSelected()) {
                    // if current label is already selected, store a pointer to it.
                    // This can be used to cycle through
                    pe_sel = pl;
                }
            }
    }

    // test all wires and wire labels
    for(Wire *pw=Doc->wires().last(); pw != 0; pw = Doc->wires().prev()) {
        if(pw->getSelected(x, y)) {
            if(flag) {
                // The element can be deselected
                pw->toggleSelected();
                return ElementMouseAction(pw);
            }else if(pe_sel) {
                pe_sel->setSelected(false);
                return ElementMouseAction(pw);
            }else if(pe_1st == 0) {
                pe_1st = pw;   // give access to elements lying beneath
            }else{
	    }
            if(pw->isSelected()) {
                pe_sel = pw;
            }
        }else{
	}
        pl = pw->Label; // test any label associated with the wire
        if(pl) {
            if(pl->getSelected(x, y)) {
                if(flag) {
                    // The element can be deselected
                    pl->toggleSelected();
                    return ElementMouseAction(pl);
                }else if(pe_sel) {
                    pe_sel->setSelected(false);
                    return ElementMouseAction(pl);
                }
                if(pe_1st == 0) {
                    // give access to elements lying beneath
                    pe_1st = pl;
                }
                if(pl->isSelected()) {
                    pe_sel = pl;
                }
            }
        }
    }

    // test all components
    for(Component *pc=Doc->components().last(); pc!=nullptr; pc=Doc->components().prev()) {
        if(pc->getSelected(x, y)) {
            if(flag) {
                // The element can be deselected
                pc->toggleSelected();
                return ElementMouseAction(pc);
            }else if(pe_sel) {
                pe_sel->setSelected(false);
                return ElementMouseAction(pc);
            }else if(pe_1st == 0) {
		// give access to elements lying beneath
                pe_1st = pc;
            } if(pc->isSelected()) {
                pe_sel = pc;
            }else{
	    }
        }else{
            n = pc->getTextSelected(x, y, Corr);
	    // was property text clicked ?
            if(n >= 0) {
                pc->Type = isComponentText;
                if(index)  *index = n;
                return ElementMouseAction(pc);
            }
        }
    }

    Corr = 5.0 / Doc->Scale;  // size of line select and area for resizing
    // test all diagrams
    for(Diagram *pd = Doc->diagrams().last(); pd!=nullptr; pd=Doc->diagrams().prev()) {

        foreach(Graph *pg, pd->Graphs) {
            // test markers of graphs
            foreach(Marker *pm, pg->Markers)
            {
                if(pm->getSelected(x-pd->cx_(), y-pd->cy_()))
                {
                    if(flag)
                    {
                        // The element can be deselected
                        pm->toggleSelected();
                        return ElementMouseAction(pm);
                    }
                    if(pe_sel)
                    {
                        pe_sel->setSelected(false);
                        return ElementMouseAction(pm);
                    }
                    if(pe_1st == 0)
                    {
                        pe_1st = pm;   // give access to elements beneath
                    }
                    if(pm->isSelected())
                    {
                        pe_sel = pm;
                    }
                }
            }
        }

        // resize area clicked ?
        if(!pd->isSelected()){
          // why do we visit non-selected diagrams?!
        }else if(!pd->resizeTouched(fX, fY, Corr)){
          // check: what does it mean?
        }else if(pe_1st == 0) {
	  ElementMouseAction A(pd);
	  A->setObsoleteType(isDiagramResize);
	  return A;
        }else{
        }

        if(pd->getSelected(x, y))
        {
            if(pd->name()[0] == 'T')     // tabular, timing diagram or truth table ?
            {
                if(pd->name()[1] == 'i')
                {
                    if(y > pd->cy_())
                    {
                        if(x < pd->cx_()+pd->xAxis.numGraphs) continue;
                        ElementMouseAction A(pd);
                        A->setObsoleteType(isDiagramHScroll);
                        return A;
                    }
                }
                else
                {
                    if(x < pd->cx_())        // clicked on scroll bar ?
                    {
                        ElementMouseAction A(pd);
                        A->setObsoleteType(isDiagramVScroll);
                        return A;
                    }
                }
            }

            // test graphs of diagram
            foreach(Graph *pg, pd->Graphs)
            {
                if(pg->getSelected(x-pd->cx_(), pd->cy_()-y) >= 0)
                {
                    if(flag) {
                        // The element can be deselected
                        pg->toggleSelected();

                        ElementMouseAction A(pg);
                        return A;
                    }else if(pe_sel) {
			pe_sel->setSelected(false);
                        return ElementMouseAction(pg);
                    }else if(pe_1st == 0) {
                        pe_1st = pg;   // access to elements lying beneath
                    }
                    if(pg->isSelected())
                    {
                        pe_sel = pg;
                    }
                }
            }

            if(flag)
            {
                // The element can be deselected
                pd->toggleSelected();
                return ElementMouseAction(pd);
            }
            if(pe_sel)
            {
                pe_sel->setSelected(false);
                return ElementMouseAction(pd);
            }
            if(pe_1st == 0)
            {
                pe_1st = pd;    // give access to elements lying beneath
            }
            if(pd->isSelected())
            {
                pe_sel = pd;
            }
        }
    }

    // test all paintings
    for(Painting *pp = Doc->paintings().last(); pp!=nullptr; pp=Doc->paintings().prev()) {
        if(pp->isSelected()) {
            if(pp->resizeTouched(fX, fY, Corr)) {
                if(pe_1st == 0) {
                    pp->Type = isPaintingResize;
                    return ElementMouseAction(pp);
                }
            }
        }

        if(pp->getSelected(fX, fY, Corr))
        {
            if(flag)
            {
                // The element can be deselected
                pp->toggleSelected();
                return ElementMouseAction(pp);
            }
            if(pe_sel)
            {
                pe_sel->setSelected(false);
                return ElementMouseAction(pp);
            }
            if(pe_1st == 0)
            {
                pe_1st = pp;    // give access to elements lying beneath
            }
            if(pp->isSelected())
            {
                pe_sel = pp;
            }
        }
    }

    return ElementMouseAction(pe_1st);
#endif
}

void SchematicDoc::highlightWireLabels ()
{
    incomplete();
#if 0
    WireLabel *pltestinner = 0;
    WireLabel *pltestouter = 0;

    // First set highlighting for all wire and nodes labels to false
    for(Wire *pwouter = wires().last(); pwouter != 0; pwouter = wires().prev())
    {
        pltestouter = pwouter->Label; // test any label associated with the wire
        if (pltestouter)
        {
            pltestouter->setHighlighted (false);
        }
    }

    for(Node *pnouter = nodes().last(); pnouter != 0; pnouter = nodes().prev())
    {
        pltestouter = pnouter->Label; // test any label associated with the node
        if (pltestouter)
        {
            pltestouter->setHighlighted (false);
        }
    }
	
    // Then test every wire's label to see if we need to highlight it
    // and matching labels on wires and nodes
    for(auto it=wires().begin(); it!=wires().end(); ++it) {
	Wire* pwouter = *it;

        // get any label associated with the wire
        pltestouter = pwouter->Label;
        if (pltestouter) {
            // if (pltestouter->isSelected())
	    if(0){
                bool hiLightOuter = false;
                // Search for matching labels on wires
		for(auto iit=wires().begin(); iit!=wires().end(); ++iit) {
		    Wire* pwinner = *iit;

                    pltestinner = pwinner->Label; // test any label associated with the wire
                    if (pltestinner)
                    {
                        // Highlight the label if it has the same name as the selected label
                        // if only one wire has this label, do not highlight it
                        if (pltestinner != pltestouter) {
                            if (pltestouter->name() == pltestinner->name()){
                                pltestinner->setHighlighted (true);
                                hiLightOuter = true;
                            }else{
			    }
                        }else{
			}
                    }
                }
                // Search for matching labels on nodes

		for(auto iit=nodes().begin(); iit!=nodes().end(); ++iit) {
		    Node* pninner = *iit;

                    pltestinner = pninner->Label; // test any label associated with the node
                    if (pltestinner)
                    {
                        if (pltestouter->name() == pltestinner->name())
                        {
                            // node label matches wire label
                            pltestinner->setHighlighted (true);
                            hiLightOuter = true;
                        }
                    }
                }
                // highlight if at least two different wires/nodes with the same label found
                pltestouter->setHighlighted (hiLightOuter);
            }
        }
    }
    // Same as above but for nodes labels:
    // test every node label to see if we need to highlight it
    // and matching labels on wires and nodes
    for(auto iit=nodes().begin(); iit!=nodes().end(); ++iit) {
	Node* pnouter = *iit;

        // get any label associated with the node
        pltestouter = pnouter->Label;
        if (pltestouter) {
            //if (pltestouter->isSelected())
	    if(0){
                bool hiLightOuter = false;
                // Search for matching labels on wires
		for(auto iit=wires().begin(); iit!=wires().end(); ++iit) {
		    Wire* pwinner = *iit;

                    pltestinner = pwinner->Label; // test any label associated with the wire
                    if (pltestinner)
                    {
                        if (pltestouter->name() == pltestinner->name())
                        {
                            // wire label matches node label
                            pltestinner->setHighlighted (true);
                            hiLightOuter = true;
                        }
                    }
                }
                // Search for matching labels on nodes
		for(auto iit=nodes().begin(); iit!=nodes().end(); ++iit) {
		    Node* pninner = *iit;

                    pltestinner = pninner->Label; // test any label associated with the node
                    if (pltestinner)
                    {
                        // Highlight the label if it has the same name as the selected label
                        // if only one node has this label, do not highlight it
                        if (pltestinner != pltestouter)
                        {
                            if (pltestouter->name() == pltestinner->name())
                            {
                                pltestinner->setHighlighted (true);
                                hiLightOuter = true;
                            }
                        }
                    }
                }
                // highlight if at least two different wires/nodes with the same label found
                pltestouter->setHighlighted (hiLightOuter);
            }
        }
    }
#endif
}
#endif

#if 0 // SCROLLVIEW code
    int  z=0;   // counts selected elements
    int  cx1, cy1, cx2, cy2;

    // exchange rectangle coordinates to obtain x1 < x2 and y1 < y2
    cx1 = (x1 < x2) ? x1 : x2;
    cx2 = (x1 > x2) ? x1 : x2;
    cy1 = (y1 < y2) ? y1 : y2;
    cy2 = (y1 > y2) ? y1 : y2;
    x1 = cx1;
    x2 = cx2;
    y1 = cy1;
    y2 = cy2;

    for(auto pc : components()) {
        pc->Bounding(cx1, cy1, cx2, cy2);
        if(cx1 >= x1) if(cx2 <= x2) if(cy1 >= y1) if(cy2 <= y2)
                    {
                        pc->setSelected();
                        z++;
                        continue;
                    }
        if(!flag){
	  pc->setSelected(false);
	}else if(pc->isSelected()){
	  z++;
	}
    }


    Wire *pw;
    for(pw = wires().first(); pw != 0; pw = wires().next())   // test all wires
    {
        if(  (pw->x1_() >= x1)
	  && (pw->x2_() <= x2)
	  && (pw->y1_() >= y1)
	  && (pw->y2_() <= y2) ) {
	    pw->setSelected(false);
            z++;
        }else{
	    if(!flag){
		pw->setSelected(false);
	    }else if(pw->isSelected()){
		z++;
	    }
	}
    }


    // test all wire labels *********************************
    WireLabel *pl=0;
    for(pw = wires().first(); pw != 0; pw = wires().next())
    {
        if(pw->Label)
        {
            pl = pw->Label;
            if(pl->x1_() >= x1) if((pl->x1_()+pl->x2_()) <= x2)
                    if(pl->y1_() >= y1) if((pl->y1_()+pl->y2_()) <= y2)
                        {
                            pl->setSelected();
                            z++;
                            continue;
                        }
	    if(!flag){
	      pl->setSelected(false);
	    }else if(pl->isSelected()){
	      z++;
	    }
        }
    }


    // test all node labels *************************************
    for(Node *pn = nodes().first(); pn != 0; pn = nodes().next())
    {
        pl = pn->Label;
        if(pl)
        {
            if(pl->x1_() >= x1) if((pl->x1_()+pl->x2_()) <= x2)
                    if((pl->y1_()-pl->y2_()) >= y1) if(pl->y1_() <= y2)
                        {
                            pl->setSelected();
                            z++;
                            continue;
                        }
	    if(!flag){
	      pl->setSelected(false);
	    }else if(pl->isSelected()){
	      z++;
	    }
        }
    }


    for(auto pd : diagrams()){
        foreach(Graph *pg, pd->Graphs)
        {
	    if(!flag){
	      pg->setSelected(false);
	    }else if(pg->isSelected()){
	      z++;
	    }

            // test markers of graph
            foreach(Marker *pm, pg->Markers)
            {
                pm->Bounding(cx1, cy1, cx2, cy2);
                if(cx1 >= x1) if(cx2 <= x2) if(cy1 >= y1) if(cy2 <= y2)
                            {
                                pm->setSelected();
                                z++;
                                continue;
                            }
		if(!flag){
		  pm->setSelected(false);
		}else if(pm->isSelected()){
		  z++;
		}
            }
        }

        // test diagram itself
        pd->Bounding(cx1, cy1, cx2, cy2);
        if(cx1 >= x1) if(cx2 <= x2) if(cy1 >= y1) if(cy2 <= y2)
                    {
                        pd->setSelected();
                        z++;
                        continue;
                    }
	if(!flag){
	  pd->setSelected(false);
	}else if(pd->isSelected()){
	  z++;
	}
    }

    // test all paintings *******************************************
    for(Painting *pp = paintings().first(); pp != 0; pp = paintings().next())
    {
        pp->Bounding(cx1, cy1, cx2, cy2);
        if(cx1 >= x1) if(cx2 <= x2) if(cy1 >= y1) if(cy2 <= y2)
                    {
                        pp->setSelected();
                        z++;
                        continue;
                    }
	if(!flag){
	  pp->setSelected(false);
	}else if(pp->isSelected()){
	  z++;
	}
    }

    return z;
}
#endif

// ---------------------------------------------------
// Selects all markers.
#if 0
void SchematicDoc::selectMarkers()
{
    incomplete();
    for(auto pd : diagrams()){
        foreach(Graph *pg, pd->Graphs)
            foreach(Marker *pm, pg->Markers)
                pm->setSelected();
    }
}
#endif

// ---------------------------------------------------
// For moving of elements: Copies all selected elements into the
// list 'p' and deletes them from the document.
// returns the number of "copied" _Markers_ only
#if 0
QList<ElementGraphics*> SchematicDoc::cropSelectedElements()
{
    QList<ElementGraphics*> P;
    auto p=&P;

#ifndef USE_SCROLLVIEW
    assert(scene());
    for(auto i : scene()->selectedItems()){
	if(auto x=dynamic_cast<ElementGraphics*>(i)){
	    p->append(x);
	}else{
	    unreachable();
	}
    }

    return P;
#else // does not work with ElementGraphics
    int i, count = 0;
    Component *pc;
    Wire      *pw;
    Diagram   *pd;
    Element   *pe;
    Node      *pn;

    // test all components *********************************
    // Insert components before wires in order to prevent short-cut removal.
    for(pc = components().first(); pc != 0; )
        if(pc->isSelected())
        {
            p->append(pc);
            count++;

            // delete all port connections
            foreach(Port *pp, pc->Ports)
            {
                pp->Connection->connectionsRemove((Element*)pc);
                pp->Connection->State = 4;
            }

            components().take();   // take component out of the document
            pc = components().current();
        }else{
	    pc = components().next();
	}

    // test all wires and wire labels ***********************
    for(pw = wires().first(); pw != 0; )
    {
        if(pw->Label) if(pw->Label->isSelected())
                p->append(pw->Label);

        if(pw->isSelected())
        {
            p->append(pw);

            pw->portValue(0)->connectionsRemove(pw);   // remove connection 1
            pw->portValue(0)->State = 4;
            pw->portValue(1)->connectionsRemove(pw);   // remove connection 2
            pw->portValue(1)->State = 4;
            wires().take();
            pw = wires().current();
        }
        else pw = wires().next();
    }

    // ..............................................
    // Inserts wires, if a connection to a not moving element is found.
    // The order of the "for"-loops is important to guarantee a stable
    // operation: components, new wires, old wires
    auto pi=p->begin();
    pc = component(*pi);
    for(i=0; i<count; i++) {
        foreach(Port *pp, pc->Ports){
            newMovingWires(p, pp->Connection, count);
	}

	// find something already cropped.
	++pi;
	pc = component(*pi);
    }

    // GAAH. everything is stuffed into *p, but the first few are components.
    // these could be wires or so.
    for(; pi!=p->end(); ++pi){
	pe=*pi;
	// assert wire(...)?
        if(pe->isSelected())
            break;
    }

    pw = wire(*pi);
    for(pw = (Wire*)pe; pw != 0;){
	pw = wire(*pi);
        if(pw->Type == isWire)    // not working on labels
        {
            newMovingWires(p, pw->portValue(0), count);
            newMovingWires(p, pw->portValue(1), count);
            pi = p->find(pw);   // back to the real current pointer
        }
	++pi;
    }


    // ..............................................
    // delete the unused nodes
    for(pn = nodes().first(); pn!=0; )
    {
        if(pn->State & 8)
            if(pn->degree() == 2)
                if(oneTwoWires(pn))    // if possible, connect two wires to one
                {
                    pn = nodes().current();
                    continue;
                }

        if(pn->degree() == 0)
        {
            if(pn->Label)
            {
                pn->Label->Type = isMovingLabel;
                if(pn->State & 1)
                {
                    if(!(pn->State & 2)) pn->Label->Type = isHMovingLabel;
                }
                else if(pn->State & 2) pn->Label->Type = isVMovingLabel;
                p->append(pn->Label);    // do not forget the node labels
            }
            nodes().remove();
            pn = nodes().current();
            continue;
        }

        pn->State = 0;
        pn = nodes().next();
    }

    // test all node labels
    // do this last to avoid double copying
    for(pn = nodes().first(); pn != 0; pn = nodes().next())
        if(pn->Label) if(pn->Label->isSelected())
                p->append(pn->Label);


    // test all paintings **********************************
    for(Painting *ppa = paintings().first(); ppa != 0; )
        if(ppa->isSelected())
        {
            p->append(ppa);
            paintings().take();
            ppa = paintings().current();
        }
        else ppa = paintings().next();

    count = 0;  // count markers now
    // test all diagrams **********************************
    for(pd = diagrams().first(); pd != 0; )
        if(pd->isSelected()) {
            p->append(pd);
            diagrams().take();
            pd = diagrams().current();
        } else {
            foreach(Graph *pg, pd->Graphs)
            {
                QMutableListIterator<Marker *> im(pg->Markers);
                Marker *pm;
                while (im.hasNext())
                {
                    pm = im.next();
                    if(pm->isSelected())
                    {
                        count++;
                        p->append(pm);
                    }
                }
            }

            pd = diagrams().next();
        }

    return P;
#endif
}
#endif

// ---------------------------------------------------
// BUG: collect stuff in Qlist
#if 0

// ---------------------------------------------------
// Used in "aligning()", "distributeHorizontal()", "distributeVertical()".
int SchematicDoc::copyElements(int& x1, int& y1, int& x2, int& y2,
                            QList<Element *> *ElementCache)
{
    (void) (x1+x2+y1+y2);
    (void) ElementCache;
    incomplete();
#if 0
    int bx1, by1, bx2, by2;
    wires().setAutoDelete(false);
    components().setAutoDelete(false);

    x1=INT_MAX;
    y1=INT_MAX;
    x2=INT_MIN;
    y2=INT_MIN;
    // take components and wires out of list, check their boundings
    int number = copyComponents(x1, y1, x2, y2, ElementCache);
    number += copyWires(x1, y1, x2, y2, ElementCache);

    wires().setAutoDelete(true);
    components().setAutoDelete(true);

    // find upper most selected diagram
    for(Diagram *pd = diagrams().last(); pd != 0; pd = diagrams().prev())
        if(pd->isSelected())
        {
            pd->Bounding(bx1, by1, bx2, by2);
            if(bx1 < x1) x1 = bx1;
            if(bx2 > x2) x2 = bx2;
            if(by1 < y1) y1 = by1;
            if(by2 > y2) y2 = by2;
            ElementCache->append(pd);
            number++;
        }
    // find upper most selected painting
    for(Painting *pp = paintings().last(); pp != 0; pp = paintings().prev())
        if(pp->isSelected())
        {
            pp->Bounding(bx1, by1, bx2, by2);
            if(bx1 < x1) x1 = bx1;
            if(bx2 > x2) x2 = bx2;
            if(by1 < y1) y1 = by1;
            if(by2 > y2) y2 = by2;
            ElementCache->append(pp);
            number++;
        }

    return number;
#endif
    return 0;
}

// ---------------------------------------------------
// ---------------------------------------------------
// Deletes all selected elements.
// BUG: deletes selection
QUndoCommand* SchematicDoc::deleteElements()
{ untested();
    unreachable();
    return nullptr;
#if 0 // obsolete
#ifndef USE_SCROLLVIEW
    return new DeleteSelection(*this, scene()->selectedItems());
    // return true; // ??
#else
    bool sel = false;

    Component *pc = components().first();
    while(pc != 0)      // all selected component
        if(pc->isSelected())
        {
            deleteComp(pc);
            pc = components().current();
            sel = true;
        }
        else pc = components().next();

    Wire *pw = wires().first();
    while(pw != 0)        // all selected wires and their labels
    {
        if(pw->Label)
            if(pw->Label->isSelected())
            {
                delete pw->Label;
                pw->Label = 0;
                sel = true;
            }

        if(pw->isSelected())
        {
            deleteWire(pw);
            pw = wires().current();
            sel = true;
        }else{
	    pw = wires().next();
	}
    }

    // all selected labels on nodes ***************************
    for(Node *pn = nodes().first(); pn != 0; pn = nodes().next())
        if(pn->Label)
            if(pn->Label->isSelected())
            {
                delete pn->Label;
                pn->Label = 0;
                sel = true;
            }

    Diagram *pd = diagrams().first();
    while(pd != 0)      // test all diagrams
        if(pd->isSelected())
        {
            diagrams().remove();
            pd = diagrams().current();
            sel = true;
        }
        else
        {
            bool wasGraphDeleted = false;
            // all graphs of diagram

            QMutableListIterator<Graph *> ig(pd->Graphs);
            Graph *pg;

            while (ig.hasNext())
            {
                pg = ig.next();
                // all markers of diagram
                QMutableListIterator<Marker *> im(pg->Markers);
                Marker *pm;
                while (im.hasNext())
                {
                    pm = im.next();
                    if(pm->isSelected())
                    {
                        im.remove();
                        sel = true;
                    }
                }

                if(pg->isSelected())
                {
                    ig.remove();
                    sel = wasGraphDeleted = true;
                }
            }
            if(wasGraphDeleted)
                pd->recalcGraphData();  // update diagram (resize etc.)

            pd = diagrams().next();
        } //else


    Painting *pp = paintings().first();
    while(pp != 0)      // test all paintings
    {
        if(pp->isSelected())
            if(pp->name().at(0) != '.'){
	      // do not delete "PortSym", "ID_text"
                sel = true;
                paintings().remove();
                pp = paintings().current();
                continue;
            }
        pp = paintings().next();
    }

    if(sel)
    {
	sizeOfAll(UsedX1, UsedY1, UsedX2, UsedY2);
        setChanged(sel, true);
    }
    return sel;
#endif
#endif
}

// ---------------------------------------------------
/*!
 * \brief SchematicDoc::aligning align selected elements.
 * \param Mode: top, bottom, left, right, center vertical, center horizontal
 * \return True if aligned
 */
bool SchematicDoc::aligning(int Mode)
{
    (void) Mode;
#if 0
    int x1, y1, x2, y2;
    int bx1, by1, bx2, by2, *bx=0, *by=0, *ax=0, *ay=0;
    QList<Element *> ElementCache;
    int count = copyElements(x1, y1, x2, y2, &ElementCache);
    if(count < 1) return false;


    ax = ay = &x2;  // = 0
    switch(Mode) {
    case 0:  // align top
        bx = &x1;
        by = &by1;
        y2 = 1;
        break;
    case 1:  // align bottom
        bx = &x1;
        y1 = y2;
        by = &by2;
        y2 = 1;
        break;
    case 2:  // align left
        by = &y1;
        bx = &bx1;
        y2 = 1;
        break;
    case 3:  // align right
        by = &y1;
        x1 = x2;
        bx = &bx2;
        y2 = 1;
        break;
    case 4:  // center horizontally
        x1 = (x2+x1) / 2;
        by = &x2;  // = 0
        ax = &bx1;
        bx = &bx2;
        y1 = 0;
        y2 = 2;
        break;
    case 5:  // center vertically
        y1 = (y2+y1) / 2;
        bx = &x2;  // = 0
        ay = &by1;
        by = &by2;
        x1 = 0;
        y2 = 2;
        break;
    }
    x2 = 0;

    Wire      *pw;
    Component *pc;
    Element   *pe;
    // re-insert elements
    // Go backwards in order to insert node labels before its component.
    QListIterator<Element *> elementCacheIter(ElementCache);
    elementCacheIter.toBack();
    while (elementCacheIter.hasPrevious()) {
        pe = elementCacheIter.previous();
	auto W=dynamic_cast<WireLabel*>(pe);

        switch(pe->Type) {
        case isComponent:
        case isAnalogComponent:
        case isDigitalComponent:
            pc = (Component*)pe;
            pc->Bounding(bx1, by1, bx2, by2);
            pc->setCenter(x1-((*bx)+(*ax))/y2, y1-((*by)+(*ay))/y2, true);
            insertRawComponent(pc);
            break;

        case isWire:
            pw = (Wire*)pe;
            bx1 = pw->x1_();
            by1 = pw->y1_();
            bx2 = pw->x2_();
            by2 = pw->y2_();
            pw->setCenter(x1-((*bx)+(*ax))/y2, y1-((*by)+(*ay))/y2, true);
//        if(pw->Label) {  }
            insertWire(pw);
            break;

        case isDiagram:
            // Should the axis label be counted for ? I guess everyone
            // has a different opinion.
//        ((Diagram*)pe)->Bounding(bx1, by1, bx2, by2);

            // Take size without axis label.
            bx1 = ((Diagram*)pe)->cx();
            by2 = ((Diagram*)pe)->cy();
            bx2 = bx1 + ((Diagram*)pe)->x2_();
            by1 = by2 - ((Diagram*)pe)->y2_();
            ((Diagram*)pe)->setCenter(x1-((*bx)+(*ax))/y2, y1-((*by)+(*ay))/y2, true);
            break;

        case isPainting:
            ((Painting*)pe)->Bounding(bx1, by1, bx2, by2);
            ((Painting*)pe)->setCenter(x1-((*bx)+(*ax))/y2, y1-((*by)+(*ay))/y2, true);
            break;

        case isNodeLabel:
            if(((Element*)(((WireLabel*)pe)->pOwner))->Type & isComponent)
            {
                pc = (Component*)(((WireLabel*)pe)->pOwner);
                pc->Bounding(bx1, by1, bx2, by2);
            }
            else
            {
                pw = (Wire*)(((WireLabel*)pe)->pOwner);
                bx1 = pw->x1_();
                by1 = pw->y1_();
                bx2 = pw->x2_();
                by2 = pw->y2_();
            }
	    assert(W);
            W->cx__() += x1-((*bx)+(*ax))/y2;
            W->cx__() += y1-((*by)+(*ay))/y2;
            insertNodeLabel(W);
            break;

        default:
            ;
        }
    }

    ElementCache.clear();
    if(count < 2) return false;

    setChanged(true, true);
#endif
    return true;
}
#endif

/*!
 * \brief SchematicDoc::distributeHorizontal sort selection horizontally
 * \return True if sorted
 */
#if 0
bool SchematicDoc::distributeHorizontal()
{
    int x1, y1, x2, y2;
    int bx1, by1, bx2, by2;
    QList<Element *> ElementCache;
    int count = copyElements(x1, y1, x2, y2, &ElementCache);
    if(count < 1) return false;

    Element *pe;
    WireLabel *pl;
    // Node labels are not counted for, so put them to the end.
    /*  for(pe = ElementCache.last(); pe != 0; pe = ElementCache.prev())
        if(pe->Type == isNodeLabel) {
          ElementCache.append(pe);
          ElementCache.removeRef(pe);
        }*/

    // using bubble sort to get elements x ordered
    QListIterator<Element *> elementCacheIter(ElementCache);
    if(count > 1)
        for(int i = count-1; i>0; i--)
        {
            pe = ElementCache.first();
            for(int j=0; j<i; j++)
            {
                pe->getCenter(bx1, by1);
                pe=elementCacheIter.peekNext();
                pe->getCenter(bx2, by2);
                if(bx1 > bx2)    // change two elements ?
                {
                    ElementCache.replace(j+1, elementCacheIter.peekPrevious());
                    ElementCache.replace(j, pe);
                    pe = elementCacheIter.next();
                }
            }
        }

    ElementCache.last()->getCenter(x2, y2);
    ElementCache.first()->getCenter(x1, y1);
    int x = x2;
    int dx=0;
    if(count > 1) dx = (x2-x1)/(count-1);
    // re-insert elements and put them at right position
    // Go backwards in order to insert node labels before its component.
    elementCacheIter.toBack();

    while (elementCacheIter.hasPrevious())
    {
        pe = elementCacheIter.previous();
	auto C=dynamic_cast<Component*>(pe);
	auto pw=dynamic_cast<Wire*>(pe);
	auto pd=dynamic_cast<Diagram*>(pe);

        switch(pe->Type) {
        case isComponent:
        case isAnalogComponent:
        case isDigitalComponent:
            assert(C);

            C->setCenter(x, C->cy());
            insertRawComponent(C);
            break;

        case isWire:
	    assert(pw);
	    incomplete();
//            if(pw->isHorizontal()) {
//                x1 = pw->x2_() - pw->x1_();
//                pw->x1__() = x - (x1 >> 1);
//                pw->x2__() = pw->x1_() + x1;
//            }else{
//	      pw->x1__() = pw->x2__() = x;
//	    }
//        if(pw->Label) {	}
            insertWire(pw);
            break;

        case isDiagram:
            pd->cx__() = x - (pe->x2_() >> 1);
            break;

        case isPainting:
            pe->getCenter(bx1, by1);
            pe->setCenter(x, by1, false);
            break;

        case isNodeLabel:
            pl = (WireLabel*)pe;
            if(auto oc=dynamic_cast<Component*>(pl->pOwner)){
		//pe->cx += x - ((Component*)(pl->pOwner))->cx;
		pl->setCenter(x - oc->cx(), 0, true);
	
	    }else if((pw=dynamic_cast<Wire*>(pl->pOwner))){
                if(pw->isHorizontal()) {
                    x1 = pw->x2_() - pw->x1_();
                    pl->cx__() += x - (x1 >> 1) - pw->x1_();
                } else {
		  pl->cx__() += x - pw->x1_();
		}
            }else{
	      // unreachable();
	    }
            insertNodeLabel(pl);
            x += dx;
            break;

        default:
            ;
        }
        x -= dx;
    }

    ElementCache.clear();
    if(count < 2) return false;

    setChanged(true, true);
    return true;
}

/*!
 * \brief SchematicDoc::distributeVertical sort selection vertically.
 * \return True if sorted
 */
bool SchematicDoc::distributeVertical()
{
    int x1, y1, x2, y2;
    int bx1, by1, bx2, by2;
    QList<Element *> ElementCache;
    int count = copyElements(x1, y1, x2, y2, &ElementCache);
    if(count < 1) return false;

    // using bubble sort to get elements y ordered
    QListIterator<Element *> elementCacheIter(ElementCache);
    Element *pe;
    if(count > 1)
        for(int i = count-1; i>0; i--)
        {
            pe = ElementCache.first();
            for(int j=0; j<i; j++)
            {
                pe->getCenter(bx1, by1);
                pe = elementCacheIter.peekNext();
                pe->getCenter(bx2, by2);
                if(by1 > by2)    // change two elements ?
                {
                    ElementCache.replace(j+1, elementCacheIter.peekPrevious());
                    ElementCache.replace(j, pe);
                    pe = elementCacheIter.next();
                }
            }
        }

    ElementCache.last()->getCenter(x2, y2);
    ElementCache.first()->getCenter(x1, y1);
    int y  = y2;
    int dy=0;
    if(count > 1) dy = (y2-y1)/(count-1);
    // re-insert elements and put them at right position
    // Go backwards in order to insert node labels before its component.
    elementCacheIter.toBack();
    while (elementCacheIter.hasPrevious())
    {
        pe = elementCacheIter.previous();
        auto C=dynamic_cast<Component*>(pe);
        auto pw=dynamic_cast<Wire*>(pe);
        auto pd=dynamic_cast<Diagram*>(pe);
	auto pl=dynamic_cast<WireLabel*>(pe);

        switch(pe->Type) { // FIXME.
        case isComponent:
        case isAnalogComponent:
        case isDigitalComponent:
            assert(C);
            C->setCenter(C->cx(), y);
            insertRawComponent(C);
            break;

        case isWire:
	    incomplete();
//            if(pw->isHorizontal())  pw->y1__() = pw->y2__() = y;
//            else
//            {
//                y1 = pw->y2_() - pw->y1_();
//                pw->y1__() = y - (y1 >> 1);
//                pw->y2__() = pe->y1_() + y1;
//            }
//        if(pw->Label) {	}
            insertWire(pw);
            break;

        case isDiagram:
	    assert(pd);
            pd->cx__() = y + (pe->y2_() >> 1);
            break;

        case isPainting:
            pe->getCenter(bx1, by1);
            pe->setCenter(bx1, y, false);
            break;

        case isNodeLabel:
            // if(((Element*)(((WireLabel*)pe)->pOwner))->Type & isComponent)
            if(auto oc=dynamic_cast<Component*>(pl->pOwner)){
	        oc->setCenter(y - oc->cx(), 0, true);
	    }else if((pw=dynamic_cast<Wire*>(pl->pOwner))){
                if(!pw->isHorizontal())
                {
                    y1 = pw->y2_() - pw->y1_();
                    pl->cx__() += y - (y1 >> 1) - pw->y1_();
                }else{
                    pl->cx__() += y - pw->y1_();
		}
            }else{
	      // unreachable
	    }
            insertNodeLabel((WireLabel*)pe);
            y += dy;
            break;

        default:
            ;
        }
        y -= dy;
    }

    ElementCache.clear();
    if(count < 2) return false;

    setChanged(true, true);
    return true;
}
#endif


/* *******************************************************************
   *****                                                         *****
   *****                Actions with components                  *****
   *****                                                         *****
   ******************************************************************* */

#if 0
// Finds the correct number for power sources, subcircuit ports and
// digital sources and sets them accordingly.
void SchematicDoc::setComponentNumber(Component *)
{
    Property *pp = c->Props.getFirst();
    if(!pp) return;
    if(pp->Name != "Num") return;

    int n=1;
    QString s = pp->Value;
    QString cSign = c->obsolete_model_hack();
    Component *pc=nullptr;
    // First look, if the port number already exists.
    incomplete();
#if 0
    for(pc = components().first(); pc!=0; pc = components().next()){
        if(pc->obsolete_model_hack() == cSign)
            if(pc->Props.getFirst()->Value == s) break;
    }
#endif
    if(!pc) return;   // was port number not yet in use ?

    incomplete();
#if 0 // does not work
    // Find the first free number.
    do
    {
        s  = QString::number(n);
        // look for existing ports and their numbers
        for(pc = components().first(); pc != 0; pc = components().next())
            if(pc->obsolete_model_hack() == cSign)
                if(pc->Props.getFirst()->Value == s) break;

        n++;
    }
    while(pc);     // found not used component number
    pp->Value = s; // set new number
#endif
}
#endif

// ---------------------------------------------------
// connect?
#if 0
void SchematicDoc::insertComponentNodes(Component *c, bool noOptimize)
{
    (void) sch;
    (void) noOptimize;
    Component* c=this;
    // simulation components do not have ports
    if (c->Ports.empty()) return;

    // connect every node of the component to corresponding schematic node
    foreach(Port *pp, c->Ports){
        pp->Connection = sch->insertNode(pp->x+c->cx_(), pp->y+c->cy_(), c);
    }

    if(noOptimize)  return;

    Node    *pn;
    Element *pe, *pe1;
    Q3PtrList<Element> *pL;
    // if component over wire then delete this wire
    QListIterator<Port *> iport(c->Ports);
    // omit the first element
    Port *pp = iport.next();
    while (iport.hasNext())
    {
        pp = iport.next();
        pn = pp->Connection;
        for(pe = pn->Connections.first(); pe!=0; pe = pn->Connections.next())
            if(pe->Type == isWire)
            {
                if(((Wire*)pe)->portValue(0) == pn)  pL = &(((Wire*)pe)->portValue(1)->Connections);
                else  pL = &(((Wire*)pe)->portValue(0)->Connections);

                for(pe1 = pL->first(); pe1!=0; pe1 = pL->next()){
                    if(pe1 == c) {
			// ?!?!
                        sch->deleteWire((Wire*)pe);
                        break;
                    }else{
		    }
		}
            }
    }
}
#endif

// ---------------------------------------------------
#if 0 // does not work. manipulate objects in scene through scene()
// Used for example in moving components.
void SchematicDoc::insertRawComponent(Component *c, bool noOptimize)
{ untested();
    (void) c;
    (void) noOptimize;

    // connect every node of component to corresponding schematic node
    insertComponentNodes(c, noOptimize);
    components().append(c);

#ifndef USE_SCROLLVIEW
    // huh, where is the original EG? (memory leak)
    scene()->addItem(new ElementGraphics(c));
#endif

    // a ground symbol erases an existing label on the wire line
    if(c->obsolete_model_hack() == "GND") { untested();
	// BUG.
        c->gnd_obsolete_model_override_hack("x");
        Element *pe = getWireLabel(c->Ports.first()->Connection);
        if(pe) if((pe->Type & isComponent) == 0)
            {
                delete ((Conductor*)pe)->Label;
                ((Conductor*)pe)->Label = 0;
            }
        c->gnd_obsolete_model_override_hack("GND");
    }else{
	untested();
    }
}
#endif

// ---------------------------------------------------
// was void SchematicDoc::recreateComponent(Component *Comp)
// what does it do?! only call symbol->recreate.
//
// apparently label are owned by nodes,
// FIXME: store labels properly, and get rid of this mess.
#if 0 // does not work like this.
void SchematicModel::recreateSymbol(Symbol *Comp)
{
    incomplete();
    WireLabel **plMem=0, **pl;
    int PortCount = Comp->numPorts();

    if(PortCount > 0) {
        // Save the labels whose node is not connected to somewhere else.
        // Otherwise the label would be deleted.
        pl = plMem = (WireLabel**)malloc(PortCount * sizeof(WireLabel*));
        foreach(Port *pp, Comp->Ports)
            if(pp->Connection->degree() < 2)
            {
                *(pl++) = pp->Connection->Label;
                pp->Connection->Label = 0;
            }
            else  *(pl++) = 0;
    }else{
    }
#endif


#if 0
    // move component back to where it was.
    // BUG: why are components moved around??!
    int x = Comp->tx, y = Comp->ty;
    int x1 = Comp->x1_(), x2 = Comp->x2_(), y1 = Comp->y1_(), y2 = Comp->y2_();
    QString tmp = Comp->name();    // is sometimes changed by "recreate"
#endif
//    Comp->recreate(); // *this);
#if 0
    Comp->obsolete_name_override_hack(tmp);
    if(x < x1)
        x += Comp->x1_() - x1;
    else if(x > x2)
        x += Comp->x2_() - x2;
    if(y < y1)
        y += Comp->y1_() - y1;
    else if(y > y2)
        y += Comp->y2_() - y2;
    Comp->tx = x;
    Comp->ty = y;


    if(PortCount > 0) {
        // restore node labels
        pl = plMem;
        foreach(Port *pp, Comp->Ports)
        {
            if(*pl != 0)
            {
                (*pl)->cx__() = pp->Connection->cx_();
                (*pl)->cx__() = pp->Connection->cx_();
                placeNodeLabel(*pl);
            }
            pl++;
            if((--PortCount) < 1)  break;
        }
        for( ; PortCount > 0; PortCount--)
        {
            delete (*pl);  // delete not needed labels
            pl++;
        }
        free(plMem);
    }
}
#endif
// ---------------------------------------------------

// ---------------------------------------------------

// ---------------------------------------------------
// TODO: actions & rectangle select?
#if 0
void SchematicDoc::activateCompsWithinRect(int, int, int, int)
{
    bool changed = false;
    int  cx1, cy1, cx2, cy2, a;
    // exchange rectangle coordinates to obtain x1 < x2 and y1 < y2
    cx1 = (x1 < x2) ? x1 : x2;
    cx2 = (x1 > x2) ? x1 : x2;
    cy1 = (y1 < y2) ? y1 : y2;
    cy2 = (y1 > y2) ? y1 : y2;
    x1 = cx1;
    x2 = cx2;
    y1 = cy1;
    y2 = cy2;


    for(auto pc : components()) {
        pc->Bounding(cx1, cy1, cx2, cy2);
        if(cx1 >= x1) if(cx2 <= x2) if(cy1 >= y1) if(cy2 <= y2)
                    {
                        a = pc->isActive - 1;

                        if(pc->Ports.count() > 1)
                        {
                            if(a < 0)  a = 2;
                            pc->isActive = a;    // change "active status" BUG
                           // pc->activate();
                        }
                        else
                        {
                            a &= 1;
                            pc->isActive = a;    // change "active status"
                            if(a == COMP_IS_ACTIVE)  // only for active (not shorten)
                                if(pc->obsolete_model_hack() == "GND"){
				  // if existing, delete label on wire line
                                    oneLabel(pc->Ports.first()->value());
				}else{
				}
                        }
                        changed = true;
                    }
    }

    if(changed)  setChanged(true, true);
}

// ---------------------------------------------------
bool SchematicDoc::activateSpecifiedComponent(int x, int y)
{
    incomplete();
    return 0;

    int x1, y1, x2, y2, a;
    for(auto pc : components()) {
        pc->Bounding(x1, y1, x2, y2);
        if(x >= x1) if(x <= x2) if(y >= y1) if(y <= y2)
                    {
                        a = pc->isActive - 1;

                        if(pc->Ports.count() > 1)
                        {
                            if(a < 0)  a = 2;
                            pc->isActive = a;    // change "active status"
                        }
                        else
                        {
                            a &= 1;
                            pc->isActive = a;    // change "active status"
                            if(a == COMP_IS_ACTIVE)  // only for active (not shorten)
                                if(pc->obsolete_model_hack() == "GND"){
				  // if existing, delete label on wire line
				  incomplete(); // labels are simpler now.
                                  //  oneLabel(pc->Ports.first()->Connection);
				}
                        }
                        setChanged(true, true);
                        return true;
                    }
    }
    return false;
}

// ---------------------------------------------------
bool SchematicDoc::activateSelectedComponents()
{
    int a;
    bool sel = false;
    for(auto x : selectedItems()) {
	// x.activate(); // that should do it.
        if(auto pc=component(x)) {
            a = pc->isActive - 1;

            if(pc->Ports.count() > 1)
            {
                if(a < 0)  a = 2;
                pc->isActive = a;    // change "active status"
            }
            else
            {
                a &= 1;
                pc->isActive = a;    // change "active status"
#if 0 // does not make any sense
                if(a == COMP_IS_ACTIVE)  // only for active (not shorten)
                    if(pc->obsolete_model_hack() == "GND"){
		      // if existing, delete label on wire line
                        oneLabel(pc->Ports.first()->value());
		    }
#endif
            }
            sel = true;
        }
    }

    if(sel) setChanged(true, true);
    return sel;
}
#endif

// ---------------------------------------------------
#if 0
// Sets the component ports anew. Used after rotate, mirror etc.
// // obsolete
void SchematicDoc::setCompPorts(Component *pc)
{
    incomplete();
    Symbol* sym=pc;
    WireLabel *pl;
    Q3PtrList<WireLabel> LabelCache;

    foreach(Port *pp, pc->Ports) {
        pp->value()->connectionsRemove(pc);
        switch(pp->value()->degree()) {
        case 0:
            pl = pp->value()->Label; // a node can have a Label... why is it not just a "connection"?!
            if(pl) {
                LabelCache.append(pl);
                pl->cx__() = pp->x + pc->cx_();
                pl->cx__() = pp->y + pc->cx_();
            }else{
	    }
            DocModel.removeNode(pp->value()); // erase?!
            break;
        case 2:
            oneTwoWires(pp->value()); // try to merge two wires to one
        default:
            ;
        }
    }

    // Re-connect component node to schematic node. This must be done completely
    // after the first loop in order to avoid problems with node labels.
    for(unsigned i=0; i<sym->numPorts(); ++i){
	auto pp=sym->port(i);

	// BUG: duplicate connection routines.
        Node* nn = insertNode(pp->x+pc->cx_(), pp->y+pc->cx_(), pc);
        sym->setPort(i, nn);
    }

    for(pl = LabelCache.first(); pl != 0; pl = LabelCache.next()){
	incomplete();
        insertNodeLabel(pl);
    }
}
#endif

// ---------------------------------------------------
#if 0
// Returns a pointer of the component on whose text x/y points.
Component* MouseActions::selectCompText(SchematicDoc* Doc, int x_, int y_, int& w, int& h)
{
    incomplete();
    int a, b, dx, dy;
    for(auto *pc : Doc->components()) {
        a = pc->cx() + pc->tx;
        if(x_ < a)  continue;
        b = pc->cx() + pc->ty;
        if(y_ < b)  continue;

        pc->textSize(dx, dy);
        if(x_ > a+dx)  continue;
        if(y_ > b+dy)  continue;

        w = dx;
        h = dy;
        return pc;
    }

    return 0;
}
#endif

// ---------------------------------------------------
#if 0
//  what does this do?!
Component* SchematicDoc::searchSelSubcircuit()
{
    Component *sub=0;
    for(auto pc : components()) {
        if(!pc->isSelected()) continue;

        if(pc->obsolete_model_hack() != "Sub"){
            if(pc->obsolete_model_hack() != "VHDL")
                if(pc->obsolete_model_hack() != "Verilog") continue;
	}

        if(sub != 0) return 0;    // more than one subcircuit selected
        sub = pc;
    }
    return sub;
}
#endif

// ---------------------------------------------------
#if 0 // this does not work
int SchematicDoc::copyComponents(int& x1, int& y1, int& x2, int& y2,
                              QList<Element *> *ElementCache)
{
    assert(0);
    return 0;
      // and there is no need to "copy components".
      // anyway, this function does not seem to do what the name suggests?!

    Component *pc;
    int bx1, by1, bx2, by2, count=0;
    // find bounds of all selected components
    for(pc = components().first(); pc != 0; )
    {
        if(pc->isSelected())
        {
            pc->Bounding(bx1, by1, bx2, by2);  // is needed because of "distribute
            if(bx1 < x1) x1 = bx1;             // uniformly"
            if(bx2 > x2) x2 = bx2;
            if(by1 < y1) y1 = by1;
            if(by2 > y2) y2 = by2;

            count++;
            ElementCache->append(pc);

            // rescue non-selected node labels
            foreach(Port *pp, pc->Ports)
                if(pp->Connection->Label)
                    if(pp->Connection->degree() < 2)
                    {
                        ElementCache->append(pp->Connection->Label);

                        // Don't set pp->Connection->Label->pOwner=0,
                        // so text position stays unchanged, but
                        // remember component for align/distribute.
                        pp->Connection->Label->pOwner = (Node*)pc;

                        pp->Connection->Label = 0;
                    }

            deleteComp(pc);
            pc = components().current();
            continue;
        }
        pc = components().next();
    }
    return count;
}

// ---------------------------------------------------
// ???
void SchematicDoc::copyComponents2(int& x1, int& y1, int& x2, int& y2,
                                QList<Element *> *ElementCache)
{
    assert(false);
    Component *pc;
    // find bounds of all selected components
    for(pc = components().first(); pc != 0; ) {
        if(pc->isSelected()) {
            // is better for unsymmetrical components
            if(pc->cx_() < x1)  x1 = pc->cx_();
            if(pc->cx_() > x2)  x2 = pc->cx_();
            if(pc->cx_() < y1)  y1 = pc->cx_();
            if(pc->cx_() > y2)  y2 = pc->cx_();

            ElementCache->append(pc);

            // rescue non-selected node labels
            foreach(Port *pp, pc->Ports)
                if(pp->Connection->Label)
                    if(pp->Connection->degree() < 2)
                    {
                        ElementCache->append(pp->Connection->Label);
                        pp->Connection->Label = 0;
                        // Don't set pp->Connection->Label->pOwner=0,
                        // so text position stays unchanged.
                    }

            deleteComp(pc);
            pc = components().current();
            continue;
        }
        pc = components().next();
    }
}
#endif


/* *******************************************************************
   *****                                                         *****
   *****                  Actions with labels                    *****
   *****                                                         *****
   ******************************************************************* */

#if 0 // this is obsolete. (hooray!) just use the net label
// Test, if wire connects wire line with more than one label and delete
// all further labels. Also delete all labels if wire line is grounded.
void SchematicDoc::oneLabel(Node *)
{

    Wire *pw;
    Node *pn, *pNode;
    Element *pe;
    WireLabel *pl = 0;
    bool named=false;   // wire line already named ?
    Q3PtrList<Node> Cons;

    for(auto pn : nodes()){
        pn->markUnChecked(); // y1__() = 0;
    }

    Cons.append(n1);
    n1->markChecked(); // y1_() = 1; 
    for(pn = Cons.first(); pn!=0; pn = Cons.next())
    {
        if(pn->Label)
        {
            if(named)
            {
                delete pn->Label;
                pn->Label = 0;    // erase double names
            }
            else
            {
                named = true;
                pl = pn->Label;
            }
        }

#if 0 // deferred
        for(pe = pn->Connections.first(); pe!=0; pe = pn->Connections.next())
        {
            if(pe->Type != isWire)
            {
                if(((Component*)pe)->isActive == COMP_IS_ACTIVE)
                    if(((Component*)pe)->obsolete_model_hack() == "GND")
                    {
                        named = true;
                        if(pl)
                        {
                            pl->pOwner->Label = 0;
                            delete pl;
                        }
                        pl = 0;
                    }
                continue;
            }
            pw = (Wire*)pe;

            if(pn != pw->portValue(0)) pNode = pw->portValue(0);
            else pNode = pw->portValue(1);

            if(pNode->y1_()) continue;
            pNode->markChecked(); // y1_() = 1;
            Cons.append(pNode);
            Cons.findRef(pn);

            if(pw->Label)
            {
                if(named)
                {
                    delete pw->Label;
                    pw->Label = 0;    // erase double names
                }
                else
                {
                    named = true;
                    pl = pw->Label;
                }
            }
        }
#endif
    }
}

// ---------------------------------------------------
int SchematicDoc::placeNodeLabel(WireLabel *)
{
#if 0
    Node *pn=nullptr;
    int x = pl->cx_();
    int y = pl->cx_();

    // check if new node lies upon an existing node
    for(auto n : nodes()){
        if(pn->cx_() == x && pn->cy_() == y){
	    break;
	}else{
	    pn=n;
	}
    }

    if(!pn)  return -1;

    Element *pe = getWireLabel(pn);
    if(pe)      // name found ?
    {
        if(pe->Type & isComponent)
        {
            delete pl;
            return -2;  // ground potential
        }

        delete ((Conductor*)pe)->Label;
        ((Conductor*)pe)->Label = 0;
    }

    pn->Label = pl;   // insert node label
    pl->Type = isNodeLabel;
    pl->pOwner = pn;
#endif
    return 0;
}

// ---------------------------------------------------
// Test, if wire line is already labeled and returns a pointer to the
// labeled element.
// possibly obsolete.
Element* SchematicDoc::getWireLabel(Node *)
{
    // ...  pn->getNet()->label() or so.
#if 0 // obsolete. just use the net label.
    Wire *pw;
    Node *pn, *pNode;
    Element *pe;
    Q3PtrList<Node> Cons;

    for(auto pn : nodes()){
        pn->markUnChecked();
    }

    Cons.append(pn_);
    pn_->markChecked();
    for(pn = Cons.first(); pn!=0; pn = Cons.next())
        if(pn->Label) return pn;
        else{
#if 0 // obsolete?
            for(pe = pn->Connections.first(); pe!=0; pe = pn->Connections.next())
            {
                if(pe->Type != isWire)
                {
                    if(((Component*)pe)->isActive == COMP_IS_ACTIVE)
                        if(((Component*)pe)->obsolete_model_hack() == "GND") return pe;
                    continue;
                }

                pw = (Wire*)pe;
                if(pw->Label) return pw;

                if(pn != pw->portValue(0)) pNode = pw->portValue(0);
                else pNode = pw->portValue(1);

                if(pNode->y1_()) continue;
		pNode->markChecked();
                Cons.append(pNode);
                Cons.findRef(pn);
            }
#endif
	}
#endif
    return 0;   // no wire label found
}

// ---------------------------------------------------
// Inserts a node label.
void SchematicDoc::insertNodeLabel(WireLabel *)
{
#if 0 // hopefully obsolete.

    if(placeNodeLabel(pl) != -1)
        return;

    // Go on, if label don't lie on existing node.

    Wire *pw = selectedWire(pl->cx_(), pl->cy_());
    if(pw){
    	// label on existing wire ?
        if(getWireLabel(pw->portValue(0)) == 0)  // wire not yet labeled ?
            pw->setName(pl->name(), pl->initValue, 0, pl->cx_(), pl->cx_());

        delete pl; // yikes.
    }else{

	// yikes.
	Node *pn = &DocModel.nodes().new_at(pl->cx_(), pl->cy_());

	pn->Label = pl;
	pl->Type  = isNodeLabel;
	pl->pOwner = pn;
    }
#endif
}
#endif

// ---------------------------------------------------
// why?!
#if 0
void SchematicDoc::copyLabels(int& x1, int& y1, int& x2, int& y2,
                           QList<Element *> *ElementCache)
{
    WireLabel *pl;
    // find bounds of all selected wires
    incomplete();
    for(auto pw : wires()){
        pl = pw->Label;
        if(pl) if(pl->isSelected())
            {
                if(pl->x1_() < x1) x1 = pl->x1_();
                if(pl->y1_()-pl->y2_() < y1) y1 = pl->y1_()-pl->y2_();
                if(pl->x1_()+pl->x2_() > x2) x2 = pl->x1_()+pl->x2_();
                if(pl->y1_() > y2) y2 = pl->y1_();
                ElementCache->append(pl);
            }
    }

    for(Node *pn = nodes().first(); pn != 0; pn = nodes().next())
    {
        pl = pn->Label;
        if(pl) if(pl->isSelected())
            {
                if(pl->x1_() < x1) x1 = pl->x1_();
                if(pl->y1_()-pl->y2_() < y1) y1 = pl->y1_()-pl->y2_();
                if(pl->x1_()+pl->x2_() > x2) x2 = pl->x1_()+pl->x2_();
                if(pl->y1_() > y2) y2 = pl->y1_();
                ElementCache->append(pl);
                pl->pOwner->Label = 0;   // erase connection
                pl->pOwner = 0;
            }
    }
}
#endif


/* *******************************************************************
   *****                                                         *****
   *****                Actions with paintings                   *****
   *****                                                         *****
   ******************************************************************* */

// don't use this.
#if 0
Painting* SchematicDoc::selectedPainting(float fX, float fY)
{
    float Corr = 5.0 / Scale; // size of line select

    for(Painting *pp = paintings().first(); pp != 0; pp = paintings().next())
        if(pp->getSelected(fX, fY, Corr))
            return pp;

    return 0;
}
#endif

// ---------------------------------------------------
// BUG: does not copy
#if 0
void SchematicDoc::copyPaintings(int& x1, int& y1, int& x2, int& y2,
                              QList<Element *> *ElementCache)
{
    incomplete();
    Painting *pp;
    int bx1, by1, bx2, by2;
    // find boundings of all selected paintings
    for(pp = paintings().first(); pp != 0; )
        if(pp->isSelected())
        {
            pp->Bounding(bx1, by1, bx2, by2);
            if(bx1 < x1) x1 = bx1;
            if(bx2 > x2) x2 = bx2;
            if(by1 < y1) y1 = by1;
            if(by2 > y2) y2 = by2;

            ElementCache->append(pp);
            paintings().take();
            pp = paintings().current();
        }
        else pp = paintings().next();
}
#endif

// vim:ts=8:sw=4:noet
