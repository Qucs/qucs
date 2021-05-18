/***************************************************************************
    copyright            : (C) 2003 by Michael Margraf
                               2020, 2021 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// Porting legacy Cartesian diagram code
// Plan: make this one behave like RectDiagram, then do the others.
//
// No longer parsing dat files. Using Data instead (which is under
// construction). This is a plugin now, another one could use the old code, if
// so desired.

#if HAVE_CONFIG_H
# include <config.h>
#endif
#include <cmath>
#include <float.h>
#if HAVE_IEEEFP_H
# include <ieeefp.h>
#endif

#include "data.h"
#include "output.h"
#include "diagram.h"
#include "element_list.h"
#include "misc.h"
#include "module.h"
#include "obsolete_stuff.h"
#include "painting.h"
#include "qucs_app.h"
#include "qucs_globals.h"
#include "qt_compat.h"
#include "some_font_stuff.h"
#include "command.h"
#include "language.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
using qucs::Element;
using qucs::Diagram;
using qucs::Painting;
using qucs::element_dispatcher;
/*--------------------------------------------------------------------------*/
using namespace qucs; //TODO
/*--------------------------------------------------------------------------*/
// it's a painting, because the canvas is painted here.
class RectDiagram : public Diagram, public Painting{
	RectDiagram(RectDiagram const& c) : Diagram(c) {
		assert(subckt());
		assert(c.subckt());
		assert(c.subckt()->size() == subckt()->size());
		if(c.subckt()->size()){itested();
		}else{
			auto v = element_dispatcher.clone("rectdiagramvariable");
			v->set_label("diagramvariable");
			subckt()->push_back(v);
		}

		for(auto i : c.Lines){
			Lines.append(new Line(*i));
		}
		for(auto i : c.Arcs){
			Arcs.append(new Arc(*i));
		}
	}
public:
	explicit RectDiagram();
	~RectDiagram();

	Element* clone() const override {itested(); return new RectDiagram(*this);}
	std::string dev_type() const override{
		return "Rect";
	}

private:
	static Element* info(QString&, char* &, bool getNewOne=false);
	int calcDiagram();
	void calcLimits();
	bool calcAxisScale(Axis*, double&, double&, double&, double&, double);
	bool calcYAxis(Axis*, int);
	void finishMarkerCoordinates(float&, float&) const;
	bool insideDiagram(float, float) const{ incomplete(); return false; }
	void recalcGraphData();
	void updateGraphData();
	void getAxisLimits(CommonData const* g);
	bool calcAxisLogScale(Axis*, int&, double&, double&, double&, int);

	pos_t center() const override{itested();
		return Element::center();
	}

private: // Diagram
	virtual diag_coordinate_t calcCoordinate(double const& x, double const& y) const;

private: // Painting
	void paint(ViewPainter* v) const override;

	// careful: the bounding box is not where you'd expect.
	// The whole thing seems shifted, not mirrored
	rect_t bounding_rect() const override
	{itested();
		assert(_height);
		assert(_width);
		return rect_t(0, -_height, _width, _height);
	}
/*--------------------------------------------------------------------------*/

	void prepare() override{itested();
		Diagram::prepare();
		// calcData /// not sure what it did. some drawing cache, and all the rest of it?!
		// loadGraphData(); // dats were parsed here.
		                    // then called "updateGraphData".
		// updateGraphData();  // was calling CalcDiagram?
		                    // sets axis limits (if needed)?
		recalcGraphData(); // calls updateGraphData
		// calcDiagram();      // "calculates" stuff on the screen that is not data
		//                     // grids, ticks etc?
  }

public: // legacy stuff. rearrange later.
        // maybe just draw things and let Qt deal with it.
  QList<Graph *>  Graphs;
  QList<Arc *>    Arcs;
  QList<Line *>   Lines;
  QList<Text *>   Texts;
}D;
Dispatcher<Diagram>::INSTALL p(&diagram_dispatcher, "Rect", &D);
Module::INSTALL pp("diagrams", &D);

/*!
  Calculations for logarithmical Cartesian diagrams
  (RectDiagram and  Rect3DDiagram).

 \param       Axis   - pointer to the axis to scale
 \param       len    - length of axis in pixel on the screen
 \return value: "true" if axis runs from largest to smallest value

 \param[out]  z      - screen coordinate where the first grid is placed
 \param[out]  zD     - number where the first grid is placed
 \param[out]  zDstep - number increment from one grid to the next
 \param[out]  coor   - scale factor for calculate screen coordinate

 \todo use this as example to document other methods
*/
bool RectDiagram::calcAxisLogScale(Axis *Axis, int& z, double& zD,
				double& zDstep, double& corr, int len)
{itested();
  if(fabs(Axis->max-Axis->min) < 1e-200) { // if max = min, double difference
    Axis->max *= 10.0;
    Axis->min /= 10.0;
  }
  Axis->low = Axis->min; Axis->up = Axis->max;

  if(!Axis->autoScale) { untested();
    Axis->low = Axis->limit_min;
    Axis->up  = Axis->limit_max;
  }


  bool mirror=false, mirror2=false;
  double tmp;
  if(Axis->up < 0.0) {   // for negative values
    tmp = Axis->low;
    Axis->low = -Axis->up;
    Axis->up  = -tmp;
    mirror = true;
  }

  double Base, Expo;
  if(Axis->autoScale) {itested();
    if(mirror) {   // set back values ?
      tmp = Axis->min;
      Axis->min = -Axis->max;
      Axis->max = -tmp;
    }

    Expo = floor(log10(Axis->max));
    Base = Axis->max/pow(10.0,Expo);
    if(Base > 3.0001) Axis->up = pow(10.0,Expo+1.0);
    else  if(Base < 1.0001) Axis->up = pow(10.0,Expo);
	  else Axis->up = 3.0 * pow(10.0,Expo);

    Expo = floor(log10(Axis->min));
    Base = Axis->min/pow(10.0,Expo);
    if(Base < 2.999) Axis->low = pow(10.0,Expo);
    else  if(Base > 9.999) Axis->low = pow(10.0,Expo+1.0);
	  else Axis->low = 3.0 * pow(10.0,Expo);

    corr = double(len) / log10(Axis->up / Axis->low);

    z = 0;
    zD = Axis->low;
    zDstep = pow(10.0,Expo);

    if(mirror) {   // set back values ?
      tmp = Axis->min;
      Axis->min = -Axis->max;
      Axis->max = -tmp;
    }
  }
  else {   // user defined limits
    if(Axis->up < Axis->low) { untested();
      tmp = Axis->low;
      Axis->low = Axis->up;
      Axis->up  = tmp;
      mirror2 = true;
    }

    Expo = floor(log10(Axis->low));
    Base = ceil(Axis->low/pow(10.0,Expo));
    zD = Base * pow(10.0, Expo);
    zDstep = pow(10.0,Expo);
    if(zD > 9.5*zDstep)  zDstep *= 10.0;

    corr = double(len) / log10(Axis->up / Axis->low);
    z = int(corr*log10(zD / Axis->low) + 0.5); // int(..) implies floor(..)

    if(mirror2) {   // set back values ?
      tmp = Axis->low;
      Axis->low = Axis->up;
      Axis->up  = tmp;
    }
  }

  if(mirror) {   // set back values ?
    tmp = Axis->low;
    Axis->low = -Axis->up;
    Axis->up  = -tmp;
  }

  if(mirror == mirror2)  return false;
  else  return true;
}

RectDiagram::RectDiagram() : Diagram()
{itested();
  x1 = 10;      // position of label text
  y1 = y3 = 33;
  _width = 240;    // initial size of diagram
  _height = 160;
  x3 = 247;    // with some distance for right axes text

  Name = "Rect"; // BUG in lang_sch?
}

RectDiagram::~RectDiagram()
{itested();
}
/*--------------------------------------------------------------------------*/
// map graph point to local scene coordinates
Diagram::diag_coordinate_t RectDiagram::calcCoordinate(double const& x_, double const& y_) const
{itested();
	double x = x_;
	double y = y_;

	float xret;
	float yret;

	if(xAxis.log) {itested();
		x /= xAxis.low;
		if(x <= 0.0) {
			xret = -1e5;   // "negative infinity"
		}else{
			xret = float(log10(x)/log10(xAxis.up / xAxis.low) * double(_width));
		}
	}else{ untested();
		xret = float((x-xAxis.low)/(xAxis.up-xAxis.low)*double(_width));
	}

	if(yAxis.log) { untested();
		// yr = sqrt(yr*yr + yi*yi);
		//if(yr <= 0.0)  *py = -1e5;   // "negative infinity"
		//
		yret = float(log10(y/fabs(yAxis.low)) /
				log10(yAxis.up/yAxis.low) * double(_height));
	}else{itested();
		//    if(fabs(yi) > 1e-250) {
		//      // preserve negative values if not complex number
		//      yr = sqrt(yr*yr + yi*yi);
		//    }else{
		//    }
		yret = float((y-yAxis.low)/(yAxis.up-yAxis.low)*double(_height));
	}

	if(!std::isfinite(xret)){ untested();
		xret = 0.0;
	}else{itested();
	}
	if(!std::isfinite(yret)){ untested();
		yret = 0.0;
	}else{itested();
	}

	// mirror up, because origin is in lower left corner.
	return diag_coordinate_t(xret, -yret);

	// move up, because origin is in lower left corner?
	return diag_coordinate_t(xret, yret-_height);
}

// --------------------------------------------------------------
void RectDiagram::finishMarkerCoordinates(float& fCX, float& fCY) const
{ untested();
  if(!insideDiagram(fCX, fCY)) { untested();
	  fCX = fCY = 0.0;
  }else{
  }
}

// --------------------------------------------------------------
void RectDiagram::calcLimits()
{itested();
  int i;
  double a, b, c;

  if(xAxis.autoScale) {// check before, to preserve limit exchange (max < min)
    if(xAxis.log) {itested();
      calcAxisLogScale(&xAxis, i, a, b, c, _width);
      xAxis.step = 1.0;
    }else{
      calcAxisScale(&xAxis, a, b, c, xAxis.step, double(_width));
    }
    xAxis.limit_min = xAxis.low;
    xAxis.limit_max = xAxis.up;
  }else{
  }

  if(yAxis.autoScale) {
    // check before, to preserve limit exchange (max < min)
    if(yAxis.log) { untested();
      calcAxisLogScale(&yAxis, i, a, b, c, _height);
      yAxis.step = 1.0;
    }else {
      calcAxisScale(&yAxis, a, b, c, yAxis.step, double(_height));
    }
    yAxis.limit_min = yAxis.low;
    yAxis.limit_max = yAxis.up;
  }else{
  }
}

                    // axes, ticks.// --------------------------------------------------------------
int RectDiagram::calcDiagram()
{itested();
	assert(scope());


	Lines.clear();
	Texts.clear();
	Arcs.clear();

	double GridStep, corr, zD, zDstep, GridNum;
	// get size of text using the screen-compatible metric
	FontMetrics metrics;
	y1 = 18; // was: QucsSettings.fontSize() + 6;

	x1 = 10;      // position of label text
	x3 = _width + 7;
	QString tmp;
	bool back = false;
	int  z, w, valid = 0;

	// =====  give "step" the right sign (if user made it wrong)  ==============
	xAxis.step = fabs(xAxis.step);
	if(xAxis.limit_min > xAxis.limit_max){
		xAxis.step *= -1.0;
	}else{
	}

	yAxis.step = fabs(yAxis.step);
	if(yAxis.limit_min > yAxis.limit_max){
		yAxis.step *= -1.0;
	}else{
	}

	// ====  x grid  =======================================================
	if(xAxis.log) {itested();
		if(xAxis.autoScale) {itested();
			if(xAxis.max*xAxis.min < 1e-200){itested();
				trace2("RectDiagram::calcDiagram", xAxis.max, xAxis.min);
				goto Frame;  // invalid
			}else{itested();
			}
		}else if(xAxis.limit_min*xAxis.limit_max < 1e-200){
			goto Frame;  // invalid
		}else{
		}

		back = calcAxisLogScale(&xAxis, z, zD, zDstep, corr, _width);

		if(back){ untested();
		  	z = _width;
		}else{itested();
		}
		while((z <= _width) && (z >= 0)) {itested();
	  		// create grid lines
			if(!xAxis.GridOn){ untested();
			}else if(z >= _width){itested();
			}else if(z > 0){itested();
				Lines.prepend(new Line(z, _height, z, 0, GridPen));  // x grid
			}else{itested();
			}

			if((zD < 1.5*zDstep) || (z == 0) || (z == _width)) {itested();
				tmp = misc::StringNiceNum(zD);
				if(xAxis.up < 0.0)  tmp = '-'+tmp;
				w = metrics.width(tmp);  // width of text
				// center text horizontally under the x tick mark
				Texts.append(new Text(z-(w>>1), -y1, tmp));
				Lines.append(new Line(z, 5, z, -5, QPen(Qt::black,0)));  // x tick marks
			}else{itested();
			}

			zD += zDstep;
			if(zD > 9.5*zDstep)  zDstep *= 10.0;
			if(back) { untested();
				z = int(corr*log10(zD / fabs(xAxis.up)) + 0.5); // int() implies floor()
				z = _width - z;
			}else{itested();
				z = int(corr*log10(zD / fabs(xAxis.low)) + 0.5);// int() implies floor()
			}
		}
	}else{
		// not logarithmical
		calcAxisScale(&xAxis, GridNum, zD, zDstep, GridStep, double(_width));
		trace1("RectDiagram::calcDiagram done axisScele", GridNum);

		double Expo;
		if(xAxis.up == 0.0){ untested();
			Expo = log10(fabs(xAxis.up-xAxis.low));
		}else{
			Expo = log10(fabs(xAxis.up));
		}

		zD += 0.5;     // perform rounding
		z = int(zD);   //  "int(...)" implies "floor(...)"
		while((z <= _width) && (z >= 0)) {
			// create grid lines
			if(fabs(GridNum) < 0.01*pow(10.0, Expo)){
			  	GridNum = 0.0;// make 0 really 0
			}else{
			}
			tmp = misc::StringNiceNum(GridNum);
			w = metrics.width(tmp);  // width of text
			// center text horizontally under the x tick mark
			Texts.append(new Text(z-(w>>1), -y1, tmp)); // Text(x, y, str, ...)
			GridNum += GridStep;

			if(!xAxis.GridOn){
			}else if(z >= _width){
			}else	if(z > 0){
				Lines.prepend(new Line(z, _height, z, 0, GridPen)); // x grid
			}else{
			}

			Lines.append(new Line(z, 5, z, -5, QPen(Qt::black,0)));   // x tick marks
			zD += zDstep;
			z = int(zD);
		}
	} // of "if(xlog) ... else ..."


	// ====  y grid  =======================================================
	if(yAxis.numGraphs > 0){ // ?
	}else{
	}
	if(1){
		if(calcYAxis(&yAxis, 0)){
		 	valid |= 1;
		}
	}else{
	}

Frame:
	// outer frame
	Lines.append(new Line(0,  _height, _width, _height, QPen(Qt::black,0)));
	Lines.append(new Line(_width, _height, _width,  0, QPen(Qt::black,0)));
	Lines.append(new Line(0,   0, _width,  0, QPen(Qt::black,0)));
	Lines.append(new Line(0,  _height,  0,  0, QPen(Qt::black,0)));
	trace3("rectdiagram", Lines.size(), _height, _width);
	trace2("rectdiagram", Texts.size(), Arcs.size());
	return valid;
}

// ------------------------------------------------------------


#if 0
bool RectDiagram::insideDiagram(float x, float y) const
{ untested();
  return (regionCode(x, y) == 0);
}

// ------------------------------------------------------------
#endif

// ------------------------------------------------------------
#if 0
Element* RectDiagram::info(QString& Name, char* &BitmapFile, bool getNewOne)
{ untested();
  unreachable();
  Name = QObject::tr("Cartesian");
  BitmapFile = (char *) "rect";

  if(getNewOne)  return new RectDiagram();
  return 0;
}
#endif
void RectDiagram::paint(ViewPainter* v) const
{

	// move the view down. that moves the drawing up...
	// v->translate(0, -_height);
 	v->scale(1.0, -1.0);

	Diagram const* cd = this;
	Diagram* d=const_cast<Diagram*>(cd);
	d->paintDiagram(v); // really?
	auto p = v;

	{ // legacy stuff from from diagram.cpp
		// paint arcs (1 pixel larger to compensate for strange circle method)
		foreach(Arc *pa, Arcs) { untested();
			p->setPen(pa->style);
			p->drawArc(pa->x, pa->y, pa->w, pa->h, pa->angle, pa->arclen);
		}
		foreach(Line *pl, Lines) {itested();
			p->setPen(pl->style);
			p->drawLine(pl->x1, pl->y1, pl->x2, pl->y2);
		}
	}

	assert(subckt());

	// v->translate(0, _height);
	v->scale(1.0, -1.0);

	//		Element::paint(p);
}
/*--------------------------------------------------------------------------*/
/*!
 Calculations for Cartesian diagrams (RectDiagram and Rect3DDiagram).
 \param   Axis - pointer to the axis to scale
 \param   Dist - length of axis in pixel on the screen
 \return value: "true" if axis runs from largest to smallest value

  \param[out]             GridNum  - number where the first numbered grid is placed
  \param[out]             GridStep - distance from one grid to the next
  \param[out]             zD     - screen coordinate where the first grid is placed
  \param[out]             zDstep - distance on screen from one grid to the next
*/
bool RectDiagram::calcAxisScale(Axis *Axis, double& GridNum, double& zD,
				double& zDstep, double& GridStep, double Dist)
{
	trace0("RectDiagram::calcAxisScale");
	bool back = false;
	double numGrids, Base, Expo, corr;
	if(Axis->autoScale) {

		if(fabs(Axis->max-Axis->min) < 1e-200) { untested();
			if((Axis->max == 0.0) && (Axis->min == 0.0)) { untested();
				Axis->up  =  1.0;
				Axis->low = -1.0;
			}
			else {   // if max = min, double difference
				Axis->up  = Axis->max + fabs(Axis->max);
				Axis->low = Axis->min - fabs(Axis->min);
			}
		}else if((Axis != &xAxis)) {itested();
			// keep a small bounding between graph and  diagram limit
			Axis->up  = Axis->max + 0.1*(Axis->max-Axis->min);
			Axis->low = Axis->min - 0.1*(Axis->max-Axis->min);
		} else {
			Axis->up  = Axis->max;   // normal case for x axis
			Axis->low = Axis->min;
		}

		numGrids = floor(Dist/60.0);   // minimal grid is 60 pixel
		if(numGrids < 1.0){
			Base = Axis->up-Axis->low;
		} else {
			Base = (Axis->up-Axis->low)/numGrids;
		}
		Expo = floor(log10(Base));
		Base = Base/pow(10.0,Expo);        // separate first significant digit
		if(Base < 3.5) {     // use only 1, 2 and 5, which ever is best fitted
			if(Base < 1.5) {
				Base = 1.0;
			} else { Base = 2.0;
			}
		} else {itested();
			if(Base < 7.5){
				Base = 5.0;
			} else {
				Base = 1.0; Expo++; 
			}
		}
		GridStep = Base * pow(10.0,Expo);   // grid distance in real coordinates
		corr = floor((Axis->up-Axis->low)/GridStep - numGrids);
		if(corr < 0.0){
			corr++;
		}else{
		}
		numGrids += corr;     // correct rounding faults


		// upper y boundery ...........................
		zD = fabs(fmod(Axis->up, GridStep));// expand grid to upper diagram edge ?
		GridNum = zD/GridStep;
		if((1.0-GridNum) < 1e-10) GridNum = 0.0;  // fix rounding errors
		if(Axis->up <= 0.0) { untested();
			if(GridNum < 0.3) {
				Axis->up += zD;  zD = 0.0; 
			}else{
			}
		} else if(GridNum > 0.7){
			Axis->up += GridStep-zD;
		} else if(GridNum < 0.1){
			// more than 1 pixel above ?
			if(GridNum*Dist >= 1.0){
				// beauty correction
				Axis->up += 0.3*GridStep;
			}else{
			}
		}else{
		}

		// lower y boundery ...........................
		zD = fabs(fmod(Axis->low, GridStep));// expand grid to lower diagram edge ?
		GridNum = zD/GridStep;
		if((1.0-GridNum) < 1e-10) zD = GridNum = 0.0;  // fix rounding errors
		if(Axis->low <= 0.0) {
			if(GridNum > 0.7) { Axis->low -= GridStep-zD;  zD = 0.0; }
			else if(GridNum < 0.1)
				if(GridNum*Dist >= 1.0) { // more than 1 pixel above ?
					Axis->low -= 0.3*GridStep;   // beauty correction
					zD += 0.3*GridStep;
				}
		}
		else { untested();
			if(GridNum > 0.3) { untested();
				zD = GridStep-zD;
				if(GridNum > 0.9) { untested();
					if((1.0-GridNum)*Dist >= 1.0) { // more than 1 pixel above ?
						Axis->low -= 0.3*GridStep;    // beauty correction
						zD += 0.3*GridStep;
					}
				}
			}
			else { Axis->low -= zD; zD = 0.0; }
		}

		GridNum = Axis->low + zD;
		zD /= (Axis->up-Axis->low)/Dist;
	}
	else {   // user defined limits
		zD = 0.0;
		Axis->low = GridNum = Axis->limit_min;
		Axis->up  = Axis->limit_max;
		if(Axis->limit_max < Axis->limit_min)
			back = true;
		GridStep  = Axis->step;
	}

	zDstep = GridStep/(Axis->up-Axis->low)*Dist; // grid in pixel

	if(fabs(zDstep) < 2.0) {  // if grid too small, then no grid
		zDstep = Dist;
		GridStep = Axis->step = Axis->up-Axis->low;
	}

	return back;
}
/*--------------------------------------------------------------------------*/
bool RectDiagram::calcYAxis(Axis *Axis, int x0)
{itested();
  int z, w;
  double GridStep, corr, zD, zDstep, GridNum;

  QString tmp;
  // get size of text using the screen-compatible metric
  FontMetrics metrics;
  int maxWidth = 0;

  bool back = false;
if(Axis->log) {
  if(Axis->autoScale) {
    if(Axis->max*Axis->min <= 0.0)  return false;  // invalid
  }
  else  if(Axis->limit_min*Axis->limit_max <= 0.0)  return false;  // invalid

  back = calcAxisLogScale(Axis, z, zD, zDstep, corr, _height);

  if(back) z = _height;
  while((z <= _height) && (z >= 0)) {    // create all grid lines
#if 0
    if(Axis->GridOn)  if(z < y2)  if(z > 0)
      Lines.prepend(new Line(0, z, x2, z, GridPen));  // y grid
#endif

    if((zD < 1.5*zDstep) || (z == 0)) { untested();
      tmp = misc::StringNiceNum(zD);
      if(Axis->up < 0.0)  tmp = '-'+tmp;

      w = metrics.width(tmp);  // width of text
      if(maxWidth < w) maxWidth = w;
#if 0
      if(x0 > 0)
        Texts.append(new Text(x0+7, z-6, tmp)); // text aligned left
      else
        Texts.append(new Text(-w-7, z-6, tmp)); // text aligned right

      // y marks
      Lines.append(new Line(x0-5, z, x0+5, z, QPen(Qt::black,0)));
#endif
    }

    zD += zDstep;
    if(zD > 9.5*zDstep)  zDstep *= 10.0;
    if(back) { untested();
      z = int(corr*log10(zD / fabs(Axis->up)) + 0.5); // int() implies floor()
      z = _height - z;
    }
    else
      z = int(corr*log10(zD / fabs(Axis->low)) + 0.5);// int() implies floor()
  }
}
else {  // not logarithmical
  back = calcAxisScale(Axis, GridNum, zD, zDstep, GridStep, double(_height));

  double Expo;
  if(Axis->up == 0.0)  Expo = log10(fabs(Axis->up-Axis->low));
  else  Expo = log10(fabs(Axis->up));

  zD += 0.5;     // perform rounding
  z = int(zD);   //  "int(...)" implies "floor(...)"
  while((z <= _height) && (z >= 0)) {  // create all grid lines
    if(fabs(GridNum) < 0.01*pow(10.0, Expo)) GridNum = 0.0;// make 0 really 0
    tmp = misc::StringNiceNum(GridNum);

     w = metrics.width(tmp);  // width of text
      if(maxWidth < w) maxWidth = w;
///       if(x0 > 0)
/// 	Texts.append(new Text(x0+8, z-6, tmp));  // text aligned left
///       else
/// 	Texts.append(new Text(-w-7, z-6, tmp));  // text aligned right
      GridNum += GridStep;


///     if(Axis->GridOn && z < _height && z > 0){
///       Lines.prepend(new Line(0, z, x2, z, GridPen));  // y grid
///     }else{
///      }
///    Lines.append(new Line(x0-5, z, x0+5, z, QPen(Qt::black,0))); // y marks
    zD += zDstep;
    z = int(zD);
  }
} // of "if(ylog) ... else ..."
  if(x0 == 0)  x1 = maxWidth + 14;
  else  x3 = _width + maxWidth + 14;
  return true;
}
/*--------------------------------------------------------------------------*/
// Calculate diagram again without reading dataset from file.
void RectDiagram::recalcGraphData()
{itested();
  yAxis.min = zAxis.min = xAxis.min =  DBL_MAX;
  yAxis.max = zAxis.max = xAxis.max = -DBL_MAX;
  yAxis.numGraphs = zAxis.numGraphs = 0;

  // get maximum and minimum values
#if 0
  foreach(Graph *pg, Graphs){
    getAxisLimits(pg);
  }
#else
	// look around for data.
	for(auto i : *scope()){
		if(auto d=prechecked_cast<Data const*>(i)){
			trace2("RectDiagram::calcDiagram data", i->short_label(), d->common());
			CommonData const* p = nullptr;;
			CommonData::attach(d->common(), &p);
			incomplete();
			getAxisLimits(p);
			CommonData::detach(&p);
		}else{
			trace1("RectDiagram::calcDiagram not data", i->short_label());
		}
	}
#endif

  if(xAxis.min > xAxis.max) {itested();
    xAxis.min = 0.0;
    xAxis.max = 1.0;
  }else{
  }
  if(yAxis.min > yAxis.max) {itested();
    yAxis.min = 0.0;
    yAxis.max = 1.0;
  }else{
  }
  if(zAxis.min > zAxis.max) {itested();
    zAxis.min = 0.0;
    zAxis.max = 1.0;
  }else{
  }

  updateGraphData();
}
/*--------------------------------------------------------------------------*/
// set axis limits
void RectDiagram::getAxisLimits(CommonData const* g)
{itested();
	trace0("RectDiagram::getAxisLimits");
//	int z = 0;
//	double x, y, *p;
	QString var, find;
	auto pg = dynamic_cast<SimOutputData const*>(g);

	if(pg){
		trace2("RectDiagram::getAxisLimits y", pg->min(), pg->max());
		yAxis.fit_in_data(pg->min(), pg->max());
	}else{itested();
	}

	if(!pg){
		incomplete();
	}else if(pg->numDeps()){
		auto const* pD = pg->dep(0);
		if(auto pd = dynamic_cast<SimOutputData const*>(pD)){
			trace2("RectDiagram::getAxisLimits x", pd->min(), pd->max());
			xAxis.fit_in_data(pd->min(), pd->max());
		}else if(pD == 0){
			incomplete();
			return;
		}else{
		}
	}else{ untested();
	}
}
/*--------------------------------------------------------------------------*/
void RectDiagram::updateGraphData()
{itested();
	// int valid =
	calcDiagram();

#if 0
  foreach(Graph *pg, Graphs) {itested();
    pg->clear();
	// do not calculate graph data if invalid
    if((valid & (pg->yAxisNo+1)) != 0){
      calcData(pg);   // calculate screen coordinates
	 } else if(pg->cPointsY) { untested();
      delete[] pg->cPointsY;
      pg->cPointsY = 0;
    }
  }
#endif

  createAxisLabels();  // virtual function

  // Setting markers must be done last, because in 3D diagram "Mem"
  // is released in "createAxisLabels()".
#if 0
  foreach(Graph *pg, Graphs){itested();
    pg->createMarkerText();
  }
#endif
}
/*--------------------------------------------------------------------------*/
static Diagram* parseDiagram(RectDiagram* d, istream_t& cmd)
{
	if(!cmd.atEnd()) {
		QString Line = QString::fromStdString(cmd.fullString());
		trace1("diagram?", Line);
		if(Line.at(0) == '<' && Line.at(1) == '/'){ untested();
			return nullptr;
		}else{
		}
		Line = Line.trimmed();
		if(Line.isEmpty()){ untested();
			return nullptr;
		}else{
		}

		std::string p;
		for(unsigned i=0; i<d->param_count(); ++i){
			cmd >> p;
			d->set_param_by_index(i, p);
		}

		if(!d->load(Line, cmd)) { untested();
			trace1("loadfail", Line);
			incomplete();
			// delete d;
			// return nullptr; // no. just use the half parsed thing.
		}else{
		}
		return d;
	}

	return nullptr;
}
/*--------------------------------------------------------------------------*/
// TODO: generalise & lang_sch?
class RectDiagramCmd : public Command  {
	void do_it(istream_t& cs, ElementList* scope){
		trace1("<Rect cmd" , cs.fullstring());
		auto e = prechecked_cast<RectDiagram*>(D.clone());
		assert(e);
		assert(e->dev_type()=="Rect");

#if 1
		parseDiagram(e, cs);
#endif
#if 1
		auto lang = language_dispatcher["leg_sch"];
		assert(lang);
		while(true){
			cs.read_line();
			trace1("<Rect cmd" , cs.fullstring());
			if(cs.umatch("</Rect>")){
				break;
			}else{
				trace2("Rect parse", short_label(), cs.fullstring());
				cs.skipbl();
				lang->new__instance(cs, e, e->scope());
			}
		}
#endif
		scope->push_back(e);
	}
}C;
Dispatcher<Diagram>::INSTALL p1(&command_dispatcher, "Rect", &C);
} // namespace
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
