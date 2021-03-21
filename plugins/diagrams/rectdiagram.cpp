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

// plotting with QCustomPlot

/*--------------------------------------------------------------------------*/
#include "command.h"
#include "data.h"
#include "diagram.h"
#include "element_list.h"
#include "language.h"
#include "misc.h"
#include "module.h"
#include "output.h"
#include "painting.h"
#include "qio.h"
#include "qt_compat.h"
#include "qucs_globals.h"
/*--------------------------------------------------------------------------*/
#include <QPlainTextEdit>
#include <QLineEdit>
#include <qcustomplot.h>
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
	using qucs::CommonData;
	using qucs::Data;
	using qucs::Diagram;
	using qucs::Element;
	using qucs::Painting;
	using qucs::SimOutputData;
	using qucs::ViewPainter;
	using qucs::diagram_dispatcher;
	using qucs::language_dispatcher;
	using qucs::command_dispatcher;
	using qucs::Module;
	using qucs::ElementList;
/*--------------------------------------------------------------------------*/
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
//				trace1("var copy", x[ii]);
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
//				trace1("var copy", y[ii]);
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
		auto pp = prechecked_cast<SimOutputData const*>(p);
		assert(pp);
		assert(i<=graphCount());
		if(i==graphCount()){ untested();
			QCustomPlot::addGraph();
		}else{ untested();
		}
		auto g = graph(graphCount()-1);
		trace1("var copy", pp->numDeps());
		if(pp->numDeps()==1){
			auto dd = dynamic_cast<SimOutputData const*>(pp->dep(0));
			assert(dd);
			int num = pp->size();
			QVector<double> x(num), y(num);
			trace3("var copy", num, pp->min(), pp->max());
			trace3("var copy", num, dd->min(), dd->max());
			int ii = 0;

			double minx = dd->min();
			double maxx = dd->max();
			double miny = pp->min();
			double maxy = pp->max();
			for (auto xx : *pp){
				x[ii] = xx.first;
				y[ii] = xx.second.real();
				++ii;
			}
			assert(ii==num);

			g->setData(x, y);

			QCustomPlot::xAxis->setRange(minx, maxx);
			QCustomPlot::yAxis->setRange(miny, maxy);

		}else{
			incomplete();
		}

	}

	void add_data(Data const* d, int i){
		if(d->common()){
			trace1("var diagscope", d->short_label());
			CommonData const* p = nullptr;
			CommonData::attach(d->common(), &p);
			copy_graph_data(p, i);
			CommonData::detach(&p);


		}else{
		}
	}

	void refresh_plots(){
		assert(_diag);
		int k = 0;
		try{
			if(_diag->param_by_name("$$xaxislog")=="1"){
				xAxis->setScaleType(QCPAxis::stLogarithmic);
			}else{
				xAxis->setScaleType(QCPAxis::stLinear);
			}
		}catch(...){
			incomplete();
				// xAxis->setScaleType(QCPAxis::stLogarithmic);
		}
		try{
			if(_diag->param_by_name("$$yaxislog")=="1"){
				yAxis->setScaleType(QCPAxis::stLogarithmic);
			}else{
				yAxis->setScaleType(QCPAxis::stLinear);
			}
		}catch(...){
			incomplete();
				// yAxis->setScaleType(QCPAxis::stLogarithmic);
		}

		for(auto i : *_diag->scope()){
			if(auto d=prechecked_cast<Data const*>(i)){
				add_data(d, k);
				++k;
			}else{
				trace1("diagscope", i->short_label());
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
		trace1("DiagramWidget::paint", _diag->short_label());
		assert(_diag->scope());
		for(auto i : *_diag->scope()){
			trace1("diagscope", i->short_label());
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
/*--------------------------------------------------------------------------*/
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
	explicit RectDiagram() : Diagram(), _widget(nullptr){
		setTypeName("Rect");
	}
	~RectDiagram(){}

public:
//	bool load(const QString& Line, istream_t& stream);
	Element* clone() const override{ untested();
		return new RectDiagram(*this);
	}
	index_t param_count() const{ return 4; }

private:
	rect_t bounding_rect() const override{ itested();
		QPointF tl(0, -_height); // eek
		QPointF br(_width, 0);
		return rect_t(QRectF(tl, br));
	}
   void paint(ViewPainter* v) const override{
		Diagram const* cd = this;
		Diagram* d=const_cast<Diagram*>(cd);
		d->paintDiagram(v);
//		Element::paint(p);
	}
	void prepare() override{
		trace1("RectDiagram:prepare", short_label());
		assert(scope());
		Diagram::prepare();
	}

private:
	// does not work.
	QWidget* newWidget(){ itested();
		trace2("rect newWidget", short_label(), _height);
		if(_widget){ untested();
		}else{ untested();
			_widget = new RectDiagramWidget(this);
			_widget->move(0, -_height); // gaah. the origin must be in the top left corner.
		}
		return _widget;
	}
	diag_coordinate_t calcCoordinate(double const&, double const&) const{
		incomplete();
		return diag_coordinate_t();
	}

private:
	QWidget* _widget;
}D;
Dispatcher<Diagram>::INSTALL p(&diagram_dispatcher, "Rect", &D);
Module::INSTALL pp("diagrams", &D);
/*--------------------------------------------------------------------------*/
RectDiagram::RectDiagram(RectDiagram const& c)
  : Diagram(c), _widget(c._widget)
{ untested();
	assert(scope());
#if 0 // could use own datatype. let's see
	Element* v = v0.clone();
	v->set_label("diagramvariable");
	assert(scope());
	scope()->push_back(v);
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
class RectDiagramCmd : public Command  {
	void do_it(istream_t& cs, ElementList* scope){
		trace1("<Rect cmd" , cs.fullstring());
		auto e = prechecked_cast<RectDiagram*>(D.clone());
		assert(e);

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
// Dispatcher<Diagram>::INSTALL p1(&command_dispatcher, "<Rect", &C); TODO: proritize < in legacy lang.
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
