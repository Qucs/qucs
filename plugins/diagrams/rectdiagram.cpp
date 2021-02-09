/***************************************************************************
    copyright            : (C) 2020, 2021 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "diagram.h"
#include "misc.h"
#include "qucs_globals.h"
#include "module.h"
#include "qio.h"
#include "qt_compat.h"
#include "language.h"
#include "data.h"
#include "output.h"

#include <QPlainTextEdit>
#include <QLineEdit>
#include <qcustomplot.h>

namespace {

	// todo: share?
// class DiagramWidget : public QWidget
class DiagramWidget : public QCustomPlot {
public:
	DiagramWidget(Diagram* d)
	: _diag(d){ untested();
		assert(d);
		Element const*e = d;
		if(auto p=dynamic_cast<Painting const*>(e)){
			auto br(p->bounding_rect());
			trace2("DiagramWidget", br.tl(), br.br());
			setGeometry(QRectF_(br).toRect()); // this only sets the SIZE.
											  // origin is in topleft corner.
		}else{
		}
		refresh_plots();
	}

private:
	void copy_graph_data_(CommonData const* p, int i){
		auto pp=prechecked_cast<SimOutputData const*>(p);
		assert(pp);
		assert(i<=graphCount());
		if(i==graphCount()){ untested();
			QCustomPlot::addGraph();
		}else{ untested();
		}
		auto g = graph(graphCount()-1);
		trace1("var copy", pp->numDeps());
		if(pp->numDeps()==1){
			auto d = pp->dep(0);
			auto dd = dynamic_cast<SimOutputData const*>(d);
			assert(dd);

			int num = pp->size();
			QVector<double> x(num), y(num);
			trace3("var copy", num, pp->min(), pp->max()); // does not work
			int ii = 0;
			double minx = 1e99;
			double maxx = -1e99;
			for (auto xx : *dd){
				x[ii] = xx.second.real();
				trace1("var copy", x[ii]);
				minx = std::min(x[ii], minx);
				maxx = std::max(x[ii], maxx);
				++ii;
			}
			assert(ii==num);
			ii = 0;
			double miny = 1e99;
			double maxy = -1e99;
			for (auto yy : *pp){
				y[ii] = yy.second.real();
				trace1("var copy", y[ii]);
				miny = std::min(y[ii], miny);
				maxy = std::max(y[ii], maxy);
				++ii;
			}

			g->setData(x, y);

			trace2("range", minx, maxx);
			trace2("range", miny, maxy);
			QCustomPlot::xAxis->setRange(minx, maxx);
			QCustomPlot::yAxis->setRange(miny, maxy);


		}else{
			incomplete();
		}

	}
	void copy_graph_data(CommonData const* p, int i){
		auto pp=prechecked_cast<SimOutputData const*>(p);
		assert(pp);
		assert(i<=graphCount());
		if(i==graphCount()){ untested();
			QCustomPlot::addGraph();
		}else{ untested();
		}
		auto g = graph(graphCount()-1);
		trace1("var copy", pp->numDeps());
		if(pp->numDeps()==1){

			int num = pp->size();
			QVector<double> x(num), y(num);
			trace3("var copy", num, pp->min(), pp->max()); // does not work
			int ii = 0;
			double minx = 1e99;
			double maxx = -1e99;
			double miny = 1e99;
			double maxy = -1e99;
			for (auto xx : *pp){
				x[ii] = xx.first;
				y[ii] = xx.second.real();
				trace1("var copy", x[ii]);
				minx = std::min(x[ii], minx);
				maxx = std::max(x[ii], maxx);
				miny = std::min(y[ii], miny);
				maxy = std::max(y[ii], maxy);
				++ii;
			}
			assert(ii==num);

			g->setData(x, y);

			trace2("range", minx, maxx);
			trace2("range", miny, maxy);
			QCustomPlot::xAxis->setRange(minx, maxx);
			QCustomPlot::yAxis->setRange(miny, maxy);


		}else{
			incomplete();
		}

	}

	void add_data(Data const* d, int i){
		if(d->common()){
			trace1("var diagscope", d->label());
			CommonData const* p=nullptr;
			CommonData::attach(d->common(), &p);
			copy_graph_data(p, i);
			CommonData::detach(&p);
		}
	}

	void refresh_plots(){
		int k = 0;
		for(auto i : *_diag->scope()){
			if(auto d=prechecked_cast<Data const*>(i)){
				add_data(d, k);
				++k;
			}else{
				trace1("diagscope", i->label());
			}
		}
	}

	void mousePressEvent(QMouseEvent* e) override{
		QWidget::mousePressEvent(e);
	}
	void mouseMoveEvent(QMouseEvent* e) override{
		QWidget::mouseMoveEvent(e);
	}
#if 0
	void  paintEvent(QPaintEvent *ev) override{
		assert(_diag);
		trace1("DiagramWidget::paint", _diag->label());
		assert(_diag->scope());
		for(auto i : *_diag->scope()){
			trace1("diagscope", i->label());
		}
		QCustomPlot::paintEvent(ev);
	}
#endif
// 	QSize sizeHint() const override{
// 		return QSize(30,30);
// 	}

private:
	Diagram* _diag;
};

class RectDiagramWidget : public DiagramWidget{
public:
	explicit RectDiagramWidget(Diagram* d) : DiagramWidget(d){
	}
};
/*--------------------------------------------------------------------------*/
// must be a factory, because cant instanciate QtObject statically
class RectDiagram : public Diagram, public Painting  {
private:
	RectDiagram(RectDiagram const& c);
public:
	explicit RectDiagram() : Diagram(), _widget(nullptr){}
	~RectDiagram(){}

public:
	bool load(const QString& Line, istream_t& stream);
	Element* clone() const override{
		return new RectDiagram(*this);
	}
	index_t param_count() const{ return 4; }

private:
	rect_t bounding_rect() const override{
//		Diagram const* cd = this;
//		Diagram* d=const_cast<Diagram*>(cd);
//		d->Bounding(x1_, y1_, x2_, y2_); // eek

		QPointF tl(0, -y2); // eek
		QPointF br(x2, 0);
		return rect_t(QRectF(tl, br));
	}
   void paint(ViewPainter* v) const override{
		Diagram const* cd = this;
		Diagram* d=const_cast<Diagram*>(cd);
		d->paintDiagram(v);
//		Element::paint(p);
	}
	void prepare() override{
		trace1("RectDiagram:prepare", label());
		assert(scope());
		Diagram::prepare();
	}

private:
	// does not work.
	QWidget* newWidget(){ itested();
		trace2("rect newWidget", label(), y2);
		if(_widget){ untested();
		}else{ untested();
			_widget = new RectDiagramWidget(this);
			_widget->move(0, -y2); // gaah. the origin must be in the top left corner.
		}
		return _widget;
	}

private:
	QWidget* _widget;
}D;
Dispatcher<Diagram>::INSTALL p(&diagram_dispatcher, "Rect", &D);
Module::INSTALL pp("diagrams", &D);
/*--------------------------------------------------------------------------*/
RectDiagram::RectDiagram(RectDiagram const& c)
  : Diagram(c), _widget(c._widget)
{
	assert(scope());
#if 0 // could use own datatype. let's see
	Element* v = v0.clone();
	v->setLabel("diagramvariable");
	assert(scope());
	scope()->push_back(v);
#endif
}
/*--------------------------------------------------------------------------*/
// obsolete.
bool RectDiagram::load(const QString& Line, istream_t& stream)
{
	incomplete();
	bool ok;
	QString s = Line;

	if(s.at(0) != '<') return false;
	if(s.at(s.length()-1) != '>'){ untested();
		return false;
	}else{
	}
	s = s.mid(1, s.length()-2);   // cut off start and end character

	QString n;



	n = s.section(' ',9,9);   // xAxis.autoScale
	if(n.at(0) == '"') {
		// seems to be a label.
	}else{
		// backward compatible
		if(n == "1") {
			xAxis.autoScale = true;
		}else{
			xAxis.autoScale = false;
		}

		n = s.section(' ',10,10);    // xAxis.limit_min
		xAxis.limit_min = n.toDouble(&ok);
		if(!ok) return false;

		n = s.section(' ',11,11);  // xAxis.step
		xAxis.step = n.toDouble(&ok);
		if(!ok) return false;

		n = s.section(' ',12,12);  // xAxis.limit_max
		xAxis.limit_max = n.toDouble(&ok);
		if(!ok) return false;

		n = s.section(' ',13,13);    // yAxis.autoScale
		if(n == "1")  yAxis.autoScale = true;
		else  yAxis.autoScale = false;

		n = s.section(' ',14,14);    // yAxis.limit_min
		yAxis.limit_min = n.toDouble(&ok);
		if(!ok) return false;

		n = s.section(' ',15,15);    // yAxis.step
		yAxis.step = n.toDouble(&ok);
		if(!ok) return false;

		n = s.section(' ',16,16);    // yAxis.limit_max
		yAxis.limit_max = n.toDouble(&ok);
		if(!ok) return false;

		n = s.section(' ',17,17);    // zAxis.autoScale
		if(n == "1")  zAxis.autoScale = true;
		else  zAxis.autoScale = false;

		n = s.section(' ',18,18);    // zAxis.limit_min
		zAxis.limit_min = n.toDouble(&ok);
		if(!ok) return false;

		n = s.section(' ',19,19);    // zAxis.step
		zAxis.step = n.toDouble(&ok);
		if(!ok) return false;

		n = s.section(' ',20,20);    // zAxis.limit_max
		zAxis.limit_max = n.toDouble(&ok);
		if(!ok) return false;

		n = s.section(' ',21,21); // rotX
		if(n.at(0) != '"') {
			// backward compatible
			rotX = n.toInt(&ok);
			if(!ok) return false;

			n = s.section(' ',22,22); // rotY
			rotY = n.toInt(&ok);
			if(!ok) return false;

			n = s.section(' ',23,23); // rotZ
			rotZ = n.toInt(&ok);
			if(!ok) return false;
		}else{ untested();
		}
	}

	trace3("load diag", label(), Element::cx(), Element::cy());


	xAxis.Label = s.section('"',1,1);   // xLabel
	yAxis.Label = s.section('"',3,3);   // yLabel left
	zAxis.Label = s.section('"',5,5);   // yLabel right

	return true; // use new__instance, below.

//	Graph *pg;
	// .......................................................
	// load graphs of the diagram
	while(!stream.atEnd()) {
		s = QString_(stream.read_line());
		s = s.trimmed();
		if(s.isEmpty()) continue;

		//trace2("diagram::load", Name, label());
		if(s == ("</"+QString_(label())+">")){
			return true;  // found end tag ?
		}else if(s.section(' ', 0,0) == "<Mkr") { untested();

			// .......................................................
			// load markers of the diagram
#if 0
			pg = Graphs.last();
			if(!pg){ untested();
				return false;
			}else{ untested();
			}
			assert(pg->parentDiagram() == this);
			Marker *pm = new Marker(pg);
			if(!pm->load(s)) { untested();
				delete pm;
				return false;
			}
			pg->Markers.append(pm);
			continue;
		}else{
			pg = new Graph(this);
			trace2("graph load", s, Name);
			if(!pg->load(s)) { untested();
				delete pg;
				return false;
			}
			Graphs.append(pg);
#endif
		}
	}
	return false;   // end tag missing
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
class RectDiagramCmd : public Command  {
	void do_it(istream_t& cs, SchematicModel* scope){
		trace1("<Rect cmd" , cs.fullstring());
		auto e = prechecked_cast<RectDiagram*>(D.clone());
		assert(e);

#if 1
		parseDiagram(e, cs);
#endif
#if 1
		auto lang = languageDispatcher["leg_sch"];
		assert(lang);
		while(true){
			cs.read_line();
			trace1("<Rect cmd" , cs.fullstring());
			if(cs.umatch("</Rect>")){
				break;
			}else{
				trace2("Rect parse", label(), cs.fullstring());
				cs.skipbl();
				lang->new__instance(cs, e, e->scope());
			}
		}
#endif
		scope->push_back(e);
	}
}C;
Dispatcher<Diagram>::INSTALL p1(&commandDispatcher, "Rect", &C);
// Dispatcher<Diagram>::INSTALL p1(&commandDispatcher, "<Rect", &C); TODO: proritize < in legacy lang.
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
