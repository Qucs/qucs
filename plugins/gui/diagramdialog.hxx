/***************************************************************************
    copyright            : (C) 2003 by Michael Margraf
	                            2018, 2020, 2021 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QUCS_QTPLUGIN_H
#define QUCS_QTPLUGIN_H

#include "component.h"
#include "data.h"
#include "output.h" // really?
#include "diagram.h"
#include "element_graphics.h"
#include "element_list.h"
#include "misc.h"
#include "node.h"
#include "platform.h"
#include "qt_compat.h"
#include "qucs_app.h"
#include "qucs_globals.h"
#include "qucsdoc.h"
#include "schematic_dialog.h"

#include <cmath>
#include <assert.h>

#include <QDialog>
#include <QRegExp>
#include <QPaintEvent>
#include <QPushButton>
#include <QTabWidget>
#include <QLabel>
#include <QStringList>
#include <QColorDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QSlider>
#include <QComboBox>
#include <QListWidget>
#include <QTableWidget>
#include <QPainter>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QDir>
#include <QDebug>


#define CROSS3D_SIZE   30
#define WIDGET3D_SIZE  2*CROSS3D_SIZE

// this is needed to be able to attach CommonData to
// dropdown menu items
Q_DECLARE_METATYPE(qucs::CommonData const*)
Q_DECLARE_METATYPE(qucs::Data*)

namespace {
using namespace qucs;

// This widget class paints a small 3-dimensional coordinate cross.
class Cross3D : public QWidget  {
public:
	Cross3D(float rx_, float ry_, float rz_, QWidget *parent = 0)
		: QWidget(parent) { untested();
			rotX = rx_;
			rotY = ry_;
			rotZ = rz_;
			setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
			setMinimumSize(WIDGET3D_SIZE, WIDGET3D_SIZE);
			resize(WIDGET3D_SIZE, WIDGET3D_SIZE);
		};
	~Cross3D() {};

	double rotX, rotY, rotZ;   // in radians !!!!

private:
	void  paintEvent(QPaintEvent*) { untested();
		QPainter Painter(this);
		float cxx = cos(rotZ);
		float cxy = sin(rotZ);
		float cxz = sin(rotY);
		float cyz = sin(rotX);
		float cyy = cos(rotX);
		float cyx = cyy * cxy + cyz * cxz * cxx;
		cyy = cyy * cxx - cyz * cxz * cxy;
		cyz *= cos(rotY);
		cxx *= cos(rotY);
		cxy *= cos(rotY);

		Painter.setPen(QPen(Qt::red,2));
		Painter.drawLine(CROSS3D_SIZE, CROSS3D_SIZE,
				int(CROSS3D_SIZE * (1.0+cxx)),
				int(CROSS3D_SIZE * (1.0-cyx)));
		Painter.setPen(QPen(Qt::green,2));
		Painter.drawLine(CROSS3D_SIZE, CROSS3D_SIZE,
				int(CROSS3D_SIZE * (1.0-cxy)),
				int(CROSS3D_SIZE * (1.0-cyy)));
		Painter.setPen(QPen(Qt::blue,2));
		Painter.drawLine(CROSS3D_SIZE, CROSS3D_SIZE,
				int(CROSS3D_SIZE * (1.0+cxz)),
				int(CROSS3D_SIZE * (1.0+cyz)));
	};
};

class DataChooser : public QComboBox{
public:
	explicit DataChooser() { untested();
		setMinimumWidth(300);
	}

public:
	void setScope(ElementList const* s){ _scope = s; }

	void refresh(){ untested();
		for(Element const* elt : *_scope){ untested();
			trace1("refresh chooser", elt->short_label());
			CommonData const* cd = nullptr;;
			if(auto dd=dynamic_cast<Data const*>(elt)){ untested();
				dd->refresh();
				CommonData::attach(dd->common(), &cd);
				add_data(cd, elt->short_label());
				CommonData::detach(&cd);
			}else{ untested();
			}
		}
#if 0
		= Elements.begin(); it != Elements.end(); ++it) { untested();
			ChooseData->addItem((*it).left((*it).length()-4));
			if((*it) == Info.fileName()){ untested();
				ChooseData->setCurrentIndex(ChooseData->count()-1);
			}else{ untested();
			}
		}
#endif
	}

private:
	// this is completely ad-hoc, but uses the legacy diagramdialog widgets
	void add_data(CommonData const* cd, std::string prefix=""){ untested();
		if(auto dd=dynamic_cast<SimOutputData const*>(cd)){ untested();
		}else if(auto dir=dynamic_cast<SimOutputDir const*>(cd)){ untested();
			bool nonempty = false;
			for(auto n : *dir){ untested();
				if(dynamic_cast<SimOutputData const*>(n)){ untested();
					nonempty = true;
				}else{ untested();
					add_data(n, prefix+"."+n->short_label());
				}
			}
			if(nonempty){ untested();
				std::string label = dir->short_label();
				QVariant shared = QVariant::fromValue<CommonData const*>(dir); // BUG: *tach.

				addItem(QString_(prefix), shared);
				//				addItem(QString_(prefix));
			}else{ untested();
			}
		}else{ untested();
		}
	}

public:
	ElementList const* _scope{nullptr};
};


class DiagramDialog : public QDialog, public SchematicDialog {
	Q_OBJECT;
public:
	explicit DiagramDialog();
	~DiagramDialog();

	bool loadVarData(const QString&);
	void copyDiagramGraphs();

private: // SchematicDialog
	Widget* clone() const override{return new DiagramDialog();}
	virtual void attach(ElementGraphics* gfx) override;

private slots:
	virtual void slotReadVars(int);
	virtual void slotTakeVar(QTableWidgetItem *item);
	//virtual   void slotSelectGraph(int index);
	virtual void slotSelectGraph(QListWidgetItem*);
	virtual void slotNewGraph();
	virtual void slotDeleteGraph();
	virtual void slotButtOK();
	virtual void slotButtApply();
	virtual void slotButtCancel();
	virtual void slotSetColor();
	virtual void slotSetGridColor();
	virtual void slotResetToTake(const QString&);
	virtual void slotSetProp2(const QString&);
	virtual void slotSetNumMode(int);
	virtual void slotSetGridBox(int);
	virtual void slotSetGraphStyle(int);
	virtual void slotSetYAxis(int);
	virtual void slotManualX(int);
	virtual void slotManualY(int);
	virtual void slotManualZ(int);
	virtual void slotChangeTab(QWidget*);

	virtual void slotNewRotX(int);
	virtual void slotNewRotY(int);
	virtual void slotNewRotZ(int);
	virtual void slotEditRotX(const QString&);
	virtual void slotEditRotY(const QString&);
	virtual void slotEditRotZ(const QString&);
	/* RELATED TO WAVEAC AND PHASOR DIAGRAMS
		void PhasorvalV(int);
		void PhasorvalI(int);
		void PhasorvalP(int);
		void PhasorvalZ(int);
		void addvar(QString);
		void remvar(QString);
		bool testvar(QString);*/

	protected slots:
		virtual void reject();

protected:
	virtual void SelectGraph(Data*);

	QString defaultDataSet;
	QString Var2;
	int loc;
	QString Name;

	QRegExp Expr;
	QDoubleValidator *ValDouble;
	QIntValidator    *ValInteger;
	QRegExpValidator *Validator;

	DataChooser *ChooseData;
	QTableWidget *ChooseVars;
	QListWidget  *GraphList;

	QVBoxLayout *all;   // the mother of all widgets
	QLineEdit   *GraphInput, *Property2, *xLabel, *ylLabel, *yrLabel;
	QCheckBox   *GridOn, *GridLogX, *GridLogY, *GridLogZ;
	QCheckBox   *manualX, *manualY, *manualZ, *hideInvisible;
	QCheckBox   *inputV, *inputI, *inputP, *inputZ;
	QLineEdit   *startX, *stepX, *stopX;
	QLineEdit   *startY, *stepY, *stopY;
	QLineEdit   *startZ, *stepZ, *stopZ;
	QLineEdit   *rotationX, *rotationY, *rotationZ;
	QLineEdit   *freq;
	QLabel      *GridLabel1, *GridLabel2, *Label1, *Label2, *Label3, *Label4;
	QComboBox   *PropertyBox, *GridStyleBox, *yAxisBox;
	QPushButton *ColorButt, *GridColorButt;
	QSlider     *SliderRotX, *SliderRotY, *SliderRotZ;
	Cross3D     *DiagCross;
	bool changed, transfer, toTake;
	//Q3PtrList<Graph>  Graphs;

private:
	ElementGraphics /*const??*/ * _orig;
	ElementGraphics* _clone;
};




// standard colors: blue, red, magenta, green, cyan, yellow, grey, black
static const QRgb DefaultColors[]
	= {0x0000ff, 0xff0000, 0xff00ff, 0x00ff00, 0x00ffff, 0xffff00,
		0x777777, 0x000000};

static const int NumDefaultColors = 8;

DiagramDialog::DiagramDialog()
	: SchematicDialog()
{ untested();
	if(auto w=dynamic_cast<QDialog*>(this)){
		w->setAttribute(Qt::WA_DeleteOnClose);
	}else{
	}
}

// BUG: disentangle different diagram types
void DiagramDialog::attach(ElementGraphics* g)
{ untested();
	// edit a copy of the element,
	// display the copy in the schematic while the dialog is open.
	_orig = g;
	_clone = g->clone();
	auto d = element(_clone);
	assert(d);

	Diagram* diag = prechecked_cast<Diagram*>(d);
	assert(diag);

	// what?
	if(doc()){ untested();
		QFileInfo Info(QString_(doc()->short_label()));
		defaultDataSet = Info.path() + QDir::separator() + doc()->DataSet;
	}else{ untested();
		defaultDataSet = "unknown";
	}

	setWindowTitle(tr("Edit Diagram Properties"));
	changed = false;
	transfer = false;  // have changes be applied ? (used by "Cancel")
	toTake = false;   // double-clicked variable be inserted into graph list ?

	Expr.setPattern("[^\"]+");
	Validator  = new QRegExpValidator(Expr, this);
	ValInteger = new QIntValidator(0, 360, this);
	ValDouble  = new QDoubleValidator(-1e200, 1e200, 6, this);

	QString NameY, NameZ;
#if 1
	NameY = tr("left Axis");
	NameZ = tr("right Axis");
	// whatever->getnames()
#else // does not work
	if((Diag->name() == "Rect") || (Diag->name() == "Curve") || (Diag->name() == "Waveac")) {
		NameY = tr("left Axis");
		NameZ = tr("right Axis");
	} else if(Diag->name() == "Polar") { untested();
		NameY = tr("y-Axis");
	} else if((Diag->name() == "Smith") || (Diag->name() == "ySmith")) { untested();
		NameY = tr("y-Axis");
	} else if(Diag->name() == "PS") { untested();
		NameY = tr("smith Axis");
		NameZ = tr("polar Axis");
	} else if(Diag->name() == "SP") { untested();
		NameY = tr("polar Axis");
		NameZ = tr("smith Axis");
	}else{ untested();
		NameY = tr("y-Axis");
		NameZ = tr("z-Axis");
	}
#endif

	all = new QVBoxLayout(this); // to provide neccessary size
	QTabWidget *t = new QTabWidget();
	all->addWidget(t);

	// Tab #1 - Data ...........................................................
	QWidget *Tab1 = new QWidget();
	QVBoxLayout *Tab1Layout = new QVBoxLayout();
	Tab1Layout->setSpacing(0);
	Tab1->setLayout(Tab1Layout);

	Label4 = 0;     // different types with same content
	yrLabel = 0;
	yAxisBox = 0;
	Property2 = 0;
	ColorButt = 0;
	hideInvisible = 0;
	rotationX = rotationY = rotationZ = 0;

	QGroupBox *InputGroup = new QGroupBox(tr("Graph Input"));
	QVBoxLayout *InputGroupLayout = new QVBoxLayout();
	InputGroup->setLayout(InputGroupLayout);
	Tab1Layout->addWidget(InputGroup);
	GraphInput = new QLineEdit();
	InputGroupLayout->addWidget(GraphInput);
	GraphInput->setValidator(Validator);
	connect(GraphInput, SIGNAL(textChanged(const QString&)), SLOT(slotResetToTake(const QString&)));
	QWidget *Box2 = new QWidget();
	QHBoxLayout *Box2Layout = new QHBoxLayout();
	Box2->setLayout(Box2Layout);
	InputGroupLayout->addWidget(Box2);
	Box2Layout->setSpacing(5);

	// init_stuff()
#if 0
	if(Diag->typeName() == "Tab") { untested();
		Label1 = new QLabel(tr("Number Notation: "));
		Box2Layout->addWidget(Label1);
		PropertyBox = new QComboBox();
		Box2Layout->addWidget(PropertyBox);
		PropertyBox->addItem(tr("real/imaginary"));
		PropertyBox->addItem(tr("magnitude/angle (degree)"));
		PropertyBox->addItem(tr("magnitude/angle (radian)"));
		PropertyBox->setCurrentIndex(1);
		connect(PropertyBox, SIGNAL(activated(int)), SLOT(slotSetNumMode(int)));
		Box2Layout->setStretchFactor(new QWidget(Box2), 5); // stretchable placeholder

		Label2 = new QLabel(tr("Precision:"));
		Box2Layout->addWidget(Label2);
		Property2 = new QLineEdit();
		Box2Layout->addWidget(Property2);
		Property2->setValidator(ValInteger);
		Property2->setMaxLength(2);
		Property2->setMaximumWidth(25);
		Property2->setText("3");
	}else if(Diag->name() == "Truth") { untested();
		// ??
	}else{ untested();
#endif
		Label1 = new QLabel(tr("Color:"));
		Box2Layout->addWidget(Label1);
		ColorButt = new QPushButton("");
		Box2Layout->addWidget(ColorButt);
		ColorButt->setMinimumWidth(50);
		ColorButt->setEnabled(false);
		connect(ColorButt, SIGNAL(clicked()), SLOT(slotSetColor()));
		Box2Layout->setStretchFactor(new QWidget(Box2), 5); // stretchable placeholder

		Label3 = new QLabel(tr("Style:"));
		Box2Layout->addWidget(Label3);
		Label3->setEnabled(false);
		PropertyBox = new QComboBox();
		Box2Layout->addWidget(PropertyBox);
		PropertyBox->addItem(tr("solid line"));

		PropertyBox->addItem(tr("dash line"));
		PropertyBox->addItem(tr("dot line"));
		//    if(Diag->name() != "Time") { untested();
		PropertyBox->addItem(tr("long dash line"));
		PropertyBox->addItem(tr("stars"));
		PropertyBox->addItem(tr("circles"));
		PropertyBox->addItem(tr("arrows"));
		//    }

		connect(PropertyBox, SIGNAL(activated(int)),
				SLOT(slotSetGraphStyle(int)));
		Box2Layout->setStretchFactor(new QWidget(Box2), 5); // stretchable placeholder

		Label2 = new QLabel(tr("Thickness:"));
		Box2Layout->addWidget(Label2);
		Property2 = new QLineEdit();
		Box2Layout->addWidget(Property2);
		Property2->setValidator(ValInteger);
		Property2->setMaximumWidth(25);
		Property2->setMaxLength(2);
		Property2->setText("0");

		//    if((Diag->name()=="Rect") || (Diag->name()=="PS") || (Diag->name()=="SP") || (Diag->name()=="Curve")) { untested();
		Label4 = new QLabel(tr("y-Axis:"));
		Box2Layout->addWidget(Label4);
		Label4->setEnabled(false);
		yAxisBox = new QComboBox();
		Box2Layout->addWidget(yAxisBox);
		yAxisBox->addItem(NameY);
		yAxisBox->addItem(NameZ);
		yAxisBox->setEnabled(false);
		connect(yAxisBox, SIGNAL(activated(int)), SLOT(slotSetYAxis(int)));
		//    }
#if 0
	}
#endif // init_stuff

	if(Property2) { untested();
		connect(Property2, SIGNAL(textChanged(const QString&)),
				SLOT(slotSetProp2(const QString&)));

		Label1->setEnabled(false);
		PropertyBox->setEnabled(false);
		Label2->setEnabled(false);
		Property2->setEnabled(false);
	}else{ untested();
	}

	QWidget *Box1 = new QWidget();
	Tab1Layout->addWidget(Box1);
	QHBoxLayout *Box1Layout = new QHBoxLayout();
	Box1->setLayout(Box1Layout);
	Box1Layout->setSpacing(5);
	Box1Layout->setContentsMargins(0, 0, 0, 0);

	QGroupBox *DataGroup = new QGroupBox(tr("Dataset"));
	Box1Layout->addWidget(DataGroup);
	QVBoxLayout *DataGroupLayout = new QVBoxLayout();
	DataGroup->setLayout(DataGroupLayout);
	ChooseData = new DataChooser;

	// diag lives in main, for some reason. go up twice.
	{
		Diagram* diag = prechecked_cast<Diagram*>(element(_orig));
		assert(diag);
		auto oo = dynamic_cast<Element const*>(diag->owner());
		assert(oo);
		oo = dynamic_cast<Element const*>(oo->owner());
		assert(oo);
		assert(oo->scope());
		ChooseData->setScope(oo->scope());
		DataGroupLayout->addWidget(ChooseData);
	}

	// todo: replace by QTableWidget
	// see https://gist.github.com/ClemensFMN/8955411

	Name = QString::fromStdString(diag->short_label());
	connect(ChooseData, SIGNAL(activated(int)), SLOT(slotReadVars(int)));
	ChooseVars = new QTableWidget(1, 3);
	ChooseVars->verticalHeader()->setVisible(false);
	ChooseVars->horizontalHeader()->setStretchLastSection(true);
	ChooseVars->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
	// make sure sorting is disabled before inserting items
	ChooseVars->setSortingEnabled(false);
	ChooseVars->horizontalHeader()->setSortIndicatorShown(true);
	ChooseVars->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);

	ChooseVars->setSelectionBehavior(QAbstractItemView::SelectRows);
	//ChooseVars->selectRow(0);
	DataGroupLayout->addWidget(ChooseVars);

	QStringList headers;
	headers << tr("Name") << tr("Type") << tr("Size");
	ChooseVars->setHorizontalHeaderLabels(headers);

	connect(ChooseVars, &QTableWidget::itemDoubleClicked, this, &DiagramDialog::slotTakeVar);

	QGroupBox *GraphGroup = new QGroupBox(tr("Data"));
	Box1Layout->addWidget(GraphGroup);
	QVBoxLayout *GraphGroupLayout = new QVBoxLayout();
	GraphGroup->setLayout(GraphGroupLayout);
	GraphList = new QListWidget();
	GraphGroupLayout->addWidget(GraphList);
	connect(GraphList, SIGNAL(itemClicked(QListWidgetItem*)), SLOT(slotSelectGraph(QListWidgetItem*)));
	connect(GraphList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), SLOT(slotDeleteGraph()));
	QPushButton *NewButt = new QPushButton(tr("New Variable"));
	GraphGroupLayout->addWidget(NewButt);
	connect(NewButt, SIGNAL(clicked()), SLOT(slotNewGraph()));
	QPushButton *DelButt = new QPushButton(tr("Delete Variable"));
	GraphGroupLayout->addWidget(DelButt);
	connect(DelButt, SIGNAL(clicked()), SLOT(slotDeleteGraph()));

	t->addTab(Tab1, tr("Data"));

	{ // makeTab2...........................................................
		int Row = 0;
		if(1) { // } Diag->name().at(0) != 'T') {  // not tabular or timing diagram
			QWidget *Tab2 = new QWidget(t);
		QGridLayout *gp = new QGridLayout();
		Tab2->setLayout(gp);

		gp->addWidget(new QLabel(tr("x-Axis Label:"), Tab2), Row, 0);
		xLabel = new QLineEdit(Tab2);
		xLabel->setValidator(Validator);
		gp->addWidget(xLabel, Row, 1);
		Row++;

		gp->addWidget(new QLabel(NameY+" "+tr("Label:"), Tab2), Row, 0);
		ylLabel = new QLineEdit(Tab2);
		ylLabel->setValidator(Validator);
		gp->addWidget(ylLabel, Row, 1);
		Row++;

		//    if((Diag->name() != "Smith") && (Diag->name() != "Polar")) {
		gp->addWidget(new QLabel(NameZ +" "+tr("Label:"), Tab2), Row, 0);
		yrLabel = new QLineEdit(Tab2);
		yrLabel->setValidator(Validator);
		gp->addWidget(yrLabel, Row, 1);
		Row++;
		//    }

		gp->addWidget(
				new QLabel(tr("<b>Label text</b>: Use LaTeX style for special characters, e.g. \\tau"), Tab2),
				Row, 0, 1, 2);
		Row++;

		// init_style_stuff?
		//   if(Diag->name() != "Rect3D") {
		GridOn = new QCheckBox(tr("show Grid"), Tab2);
		gp->addWidget(GridOn, Row, 0);
		Row++;

		GridLabel1 = new QLabel(tr("Grid Color:"),Tab2);
		gp->addWidget(GridLabel1, Row, 0);
		GridColorButt = new QPushButton("",Tab2);
		connect(GridColorButt, SIGNAL(clicked()), SLOT(slotSetGridColor()));
		gp->addWidget(GridColorButt, Row, 1);
		Row++;
		misc::setPickerColor(GridColorButt, diag->GridPen.color());

		GridLabel2 = new QLabel(tr("Grid Style: "), Tab2);
		gp->addWidget(GridLabel2, Row, 0);
		GridStyleBox = new QComboBox(Tab2);
		GridStyleBox->addItem(tr("solid line"));
		GridStyleBox->addItem(tr("dash line"));
		GridStyleBox->addItem(tr("dot line"));
		GridStyleBox->addItem(tr("dash dot line"));
		GridStyleBox->addItem(tr("dash dot dot line"));
		gp->addWidget(GridStyleBox, Row, 1);
		Row++;
		GridStyleBox->setCurrentIndex(diag->GridPen.style()-1);

		GridOn->setChecked(diag->xAxis.GridOn);
		if(!diag->xAxis.GridOn) slotSetGridBox(0);
		connect(GridOn, SIGNAL(stateChanged(int)), SLOT(slotSetGridBox(int)));
		//  }

		// ...........................................................
		xLabel->setText(diag->xAxis.Label);
		ylLabel->setText(diag->yAxis.Label);
		if(yrLabel) {
			yrLabel->setText(diag->zAxis.Label);
		}else{
		}

		//    if((diag->name().left(4) == "Rect") || (diag->name() == "Curve")) {
		GridLogX = new QCheckBox(tr("logarithmical X Axis Grid"), Tab2);
		gp->addWidget(GridLogX, Row, 0);
		Row++;

		GridLogY = new QCheckBox(tr("logarithmical")+" "+NameY+" "+tr("Grid"), Tab2);
		gp->addWidget(GridLogY, Row, 0);
		Row++;

		GridLogZ = new QCheckBox(tr("logarithmical")+" "+NameZ+" "+tr("Grid"), Tab2);
		gp->addWidget(GridLogZ, Row, 0);
		Row++;

		// ...........................................................
		// transfer the diagram properties to the dialog
		GridLogX->setChecked(diag->xAxis.log);
		GridLogY->setChecked(diag->yAxis.log);
		GridLogZ->setChecked(diag->zAxis.log);

		//    }else{
		//		 // 3d??
		//		 GridLogX = GridLogY = GridLogZ = 0;
		//	 }

		t->addTab(Tab2, tr("Properties"));
		// tab2 end??

		// Tab #3 - Limits ...........................................................
		QWidget *Tab3 = new QWidget();
		QVBoxLayout *Tab3Layout = new QVBoxLayout();

		QGroupBox *axisX = new QGroupBox(tr("x-Axis"));
		QHBoxLayout *axisXLayout = new QHBoxLayout();

		QWidget *VBox1 = new QWidget();
		axisXLayout->addWidget(VBox1);
		QVBoxLayout *VBox1Layout = new QVBoxLayout();
		VBox1Layout->addStretch();
		manualX = new QCheckBox(tr("manual"));//, VBox1);
		VBox1Layout->addWidget(manualX);
		VBox1->setLayout(VBox1Layout);
		connect(manualX, SIGNAL(stateChanged(int)), SLOT(slotManualX(int)));

		QWidget *VBox2 = new QWidget();
		axisXLayout->addWidget(VBox2);
		QVBoxLayout *VBox2Layout = new QVBoxLayout();
		VBox2Layout->addWidget(new QLabel(tr("start")));
		startX = new QLineEdit();
		VBox2Layout->addWidget(startX);
		startX->setValidator(ValDouble);
		VBox2->setLayout(VBox2Layout);

		QWidget *VBox3 = new QWidget();
		axisXLayout->addWidget(VBox3);
		QVBoxLayout *VBox3Layout = new QVBoxLayout();
		VBox3Layout->addWidget(new QLabel(tr("step")));
		stepX = new QLineEdit();//VBox3);
		VBox3Layout->addWidget(stepX);
		stepX->setValidator(ValDouble);
		VBox3->setLayout(VBox3Layout);

		QWidget *VBox4 = new QWidget();
		axisXLayout->addWidget(VBox4);
		QVBoxLayout *VBox4Layout = new QVBoxLayout();
		VBox4Layout->addWidget(new QLabel(tr("stop")));
		stopX = new QLineEdit();
		VBox4Layout->addWidget(stopX);
		stopX->setValidator(ValDouble);
		VBox4->setLayout(VBox4Layout);

		axisX->setLayout(axisXLayout);
		Tab3Layout->addWidget(axisX);
		Tab3Layout->addStretch();

		QGroupBox *axisY = new QGroupBox(NameY);
		QHBoxLayout *axisYLayout = new QHBoxLayout();

		QWidget *VBox5 = new QWidget();
		axisYLayout->addWidget(VBox5);
		QVBoxLayout *VBox5Layout = new QVBoxLayout();
		VBox5Layout->addStretch();
		manualY = new QCheckBox(tr("manual"));
		VBox5Layout->addWidget(manualY);
		connect(manualY, SIGNAL(stateChanged(int)), SLOT(slotManualY(int)));
		VBox5->setLayout(VBox5Layout);

		QWidget *VBox6 = new QWidget();
		axisYLayout->addWidget(VBox6);
		QVBoxLayout *VBox6Layout = new QVBoxLayout();
		VBox6Layout->addWidget(new QLabel(tr("start")));
		startY = new QLineEdit();
		VBox6Layout->addWidget(startY);
		startY->setValidator(ValDouble);
		VBox6->setLayout(VBox6Layout);

		QWidget *VBox7 = new QWidget();
		axisYLayout->addWidget(VBox7);
		QVBoxLayout *VBox7Layout = new QVBoxLayout();
		//    if((diag->name()=="Smith") || (diag->name()=="ySmith") || (diag->name()=="PS")){
		//		 VBox7Layout->addWidget(new QLabel(tr("number")));
		//	 }else{
		VBox7Layout->addWidget(new QLabel(tr("step")));
		//	 }
		stepY = new QLineEdit();
		VBox7Layout->addWidget(stepY);
		stepY->setValidator(ValDouble);
		VBox7->setLayout(VBox7Layout);

		QWidget *VBox8 = new QWidget();
		axisYLayout->addWidget(VBox8);
		QVBoxLayout *VBox8Layout = new QVBoxLayout();
		VBox8Layout->addWidget(new QLabel(tr("stop")));
		stopY = new QLineEdit();
		VBox8Layout->addWidget(stopY);
		stopY->setValidator(ValDouble);
		VBox8->setLayout(VBox8Layout);

		axisY->setLayout(axisYLayout);
		Tab3Layout->addWidget(axisY);
		Tab3Layout->addStretch();

		QGroupBox *axisZ = new QGroupBox(NameZ);
		QHBoxLayout *axisZLayout = new QHBoxLayout();

		QWidget *VBox9 = new QWidget();
		axisZLayout->addWidget(VBox9);
		QVBoxLayout *VBox9Layout = new QVBoxLayout();
		VBox9Layout->addStretch();
		manualZ = new QCheckBox(tr("manual"));
		VBox9Layout->addWidget(manualZ);
		connect(manualZ, SIGNAL(stateChanged(int)), SLOT(slotManualZ(int)));
		VBox9->setLayout(VBox9Layout);

		QWidget *VBox10 = new QWidget();
		axisZLayout->addWidget(VBox10);
		QVBoxLayout *VBox10Layout = new QVBoxLayout();
		VBox10Layout->addWidget(new QLabel(tr("start")));
		startZ = new QLineEdit();
		VBox10Layout->addWidget(startZ);
		startZ->setValidator(ValDouble);
		VBox10->setLayout(VBox10Layout);

		QWidget *VBox11 = new QWidget();
		axisZLayout->addWidget(VBox11);
		QVBoxLayout *VBox11Layout = new QVBoxLayout();
		//    if(diag->name() == "SP"){
		VBox11Layout->addWidget(new QLabel(tr("number")));
		//	 } else {
		//		 VBox11Layout->addWidget(new QLabel(tr("step")));
		//	 }
		stepZ = new QLineEdit();
		VBox11Layout->addWidget(stepZ);
		stepZ->setValidator(ValDouble);
		VBox11->setLayout(VBox11Layout);

		QWidget *VBox12 = new QWidget();
		axisZLayout->addWidget(VBox12);
		QVBoxLayout *VBox12Layout = new QVBoxLayout();
		VBox12Layout->addWidget(new QLabel(tr("stop")));
		stopZ = new QLineEdit();
		VBox12Layout->addWidget(stopZ);
		stopZ->setValidator(ValDouble);
		VBox12->setLayout(VBox12Layout);

		Tab3Layout->setStretchFactor(new QWidget(Tab3),5); // stretchable placeholder

		axisZ->setLayout(axisZLayout);
		Tab3Layout->addWidget(axisZ);

		Tab3->setLayout(Tab3Layout);
		t->addTab(Tab3, tr("Limits"));

		// ...........................................................
		// transfer the diagram properties to the dialog
		if(diag->xAxis.autoScale)  slotManualX(0);
		else  manualX->setChecked(true);
		if(diag->yAxis.autoScale)  slotManualY(0);
		else  manualY->setChecked(true);
		if(diag->zAxis.autoScale)  slotManualZ(0);
		else  manualZ->setChecked(true);

		diag->calcLimits();    // inserts auto-scale values if not manual

		startX->setText(QString::number(diag->xAxis.limit_min));
		stepX->setText(QString::number(diag->xAxis.step));
		stopX->setText(QString::number(diag->xAxis.limit_max));

		startY->setText(QString::number(diag->yAxis.limit_min));
		stepY->setText(QString::number(diag->yAxis.step));
		stopY->setText(QString::number(diag->yAxis.limit_max));

		startZ->setText(QString::number(diag->zAxis.limit_min));
		stepZ->setText(QString::number(diag->zAxis.step));
		stopZ->setText(QString::number(diag->zAxis.limit_max));

#if 0
		if((diag->name() == "Smith") || (diag->name() == "ySmith") ||
				(diag->name() == "Polar")) {
			axisZ->setEnabled(false);
		}else if(diag->name().left(4) != "Rect") {
			// cartesian 2D and 3D
			if((diag->name() != "Curve")) {
				axisX->setEnabled(false);
				startY->setEnabled(false);
				startZ->setEnabled(false);
			}else{
			}
		}
#endif

	}else{
		unreachable();
		// tabular or some Truth? not here.
		stepX = 0;
	}
	} // tab2

	connect(t, SIGNAL(currentChanged(QWidget*)), SLOT(slotChangeTab(QWidget*)));
	// ...........................................................
	QWidget *Butts = new QWidget();
	QHBoxLayout *ButtsLayout = new QHBoxLayout();
	ButtsLayout->setSpacing(5);
	ButtsLayout->setMargin(5);
	Butts->setLayout(ButtsLayout);
	all->addWidget(Butts);

	QPushButton *OkButt = new QPushButton(tr("OK"));
	ButtsLayout->addWidget(OkButt);
	connect(OkButt, SIGNAL(clicked()), SLOT(slotButtOK()));
	QPushButton *ApplyButt = new QPushButton(tr("Apply"));
	ButtsLayout->addWidget(ApplyButt);
	connect(ApplyButt, SIGNAL(clicked()), SLOT(slotButtApply()));
	QPushButton *CancelButt = new QPushButton(tr("Cancel"));
	ButtsLayout->addWidget(CancelButt);
	connect(CancelButt, SIGNAL(clicked()), SLOT(slotButtCancel()));

	OkButt->setDefault(true);


	// ...........................................................
	// put all data files into ComboBox
	QFileInfo Info(defaultDataSet);
	QDir ProjDir(Info.path());
	QStringList Elements = ProjDir.entryList(QStringList("*.dat"), QDir::Files, QDir::Name);
	QStringList::iterator it;
	ChooseData->refresh();

	slotReadVars(0);  // put variables into the ListView

	// ...........................................................
	// put all graphs into the ListBox
	//refreshListBox(diag, GraphList)
	{
		Data* currentGraph = nullptr;
		index_t Row = 0;
		if(diag->scope()){
			for(auto pg : *diag->scope()) { untested();
				if(!pg){
					unreachable();
				}else if(auto dd=dynamic_cast<Data*>(pg)){
					trace1("refreshList", pg->short_label());
					auto qv = QVariant::fromValue<Data*>(dd);
					auto I = new QListWidgetItem(QString_(pg->short_label()));
					I->setData(Qt::UserRole, qv);
					GraphList->insertItem(Row, I);
					if(pg == currentGraph) { untested();
						GraphList->setCurrentRow(Row);   // select current graph
						SelectGraph(currentGraph);
					}
					Row++;
				}
			}
		}else{
		}

		if(ColorButt) { untested();
			if(!currentGraph) { untested();
				QColor selectedColor(DefaultColors[GraphList->count()%NumDefaultColors]);
				misc::setPickerColor(ColorButt, selectedColor);
			}
		}else{ untested();
		}
	}
}

DiagramDialog::~DiagramDialog()
{ untested();
	delete all;   // delete all widgets from heap
	delete ValInteger;
	delete ValDouble;
	delete Validator;
}

/*--------------------------------------------------------------------------*/
// a QTableWidgetItem with a data reference.
class DataRef : public QTableWidgetItem{
	DataRef(DataRef const& x) = delete;
public:
	DataRef(CommonData const* x) :
		QTableWidgetItem(QString_(x->short_label())){ untested();
			CommonData::attach(x, &_data);
		}
	~DataRef(){ untested();
		CommonData::detach(&_data);
	}
	std::string short_label() const{ untested();
		if(_data){ untested();
			return _data->short_label();
		}else{ untested();
			return "null";
		}
	}
private:
	CommonData const* _data{nullptr};
};
// fill variables into table.
// TODO: move to ChooseVars?
void DiagramDialog::slotReadVars(int)
{ untested();
#if 0 // ?
	auto ChooseData = prechecked_cast<QComboBox*>(sender());
	assert(ChooseData);
#endif

	//  QFileInfo Info(defaultDataSet); // TODO
	QVariant cv = ChooseData->currentData();
	trace1("readVars", ChooseData->currentText());
	CommonData const* common = cv.value<CommonData const*>();

	QString Line, tmp, Var;
	int varNumber = 0;

	// make sure sorting is disabled before inserting items
	ChooseVars->setSortingEnabled(false);
	ChooseVars->clearContents();

	if(auto d = dynamic_cast< SimOutputDir const*> (common)){ untested();

		for(auto v : *d){ untested();
			auto vd = dynamic_cast<SimOutputData const*>(v);

			if(!vd){ untested();
				trace1("slotReadVars not data", v->short_label());
				// message(Object::QucsMsgLog, "unknown data");
			}else{ untested();
				auto Var = QString_(v->short_label());
				QTableWidgetItem *vcell = new DataRef(v);
				//new Q3ListViewItem(ChooseVars, Var, "dep", tmp.remove('>'));
				trace3("slotReadVars", varNumber, Var, tmp.remove('>'));
				ChooseVars->setRowCount(varNumber+1);
				vcell->setFlags(vcell->flags() ^ Qt::ItemIsEditable);
				ChooseVars->setItem(varNumber, 0, vcell);

				QTableWidgetItem *cell = nullptr;
				if((vd->numDeps())){ untested();
					tmp = Line.section(' ', 2);
					cell = new QTableWidgetItem("dep");
				}else{ untested();
					tmp = Line.section(' ', 2, 2);
					cell = new QTableWidgetItem("indep");
				}
				cell->setFlags(cell->flags() ^ Qt::ItemIsEditable);
				ChooseVars->setItem(varNumber, 1, cell);

				cell = new QTableWidgetItem("deplist?");
				cell->setFlags(cell->flags() ^ Qt::ItemIsEditable);
				ChooseVars->setItem(varNumber, 2, cell);

				varNumber++;
			}
		}
	}
	// sorting should be enabled only after adding items
	ChooseVars->setSortingEnabled(true);
}
/*--------------------------------------------------------------------------*/

// ------------------------------------------------------------------------
// Add the double-clicked variable to the Diagram. Refresh the list.
//
// What does this mean?
// If the Graph Input is empty, then the variable is
// also inserted as graph.
void DiagramDialog::slotTakeVar(QTableWidgetItem* Item)
{ untested();
	GraphInput->blockSignals(true);
	if(toTake){
		GraphInput->setText("");
	}else{
	}

	GraphInput->cursorPosition();
	//QString s="";
	//QString s1 = Item->text();
	int row = Item->row();
	auto x = ChooseVars->item(row, 0);
	auto xr = prechecked_cast<DataRef*>(x);
	assert(xr);
	auto s1 = QString_(xr->short_label());

	QFileInfo Info(defaultDataSet);
	if(ChooseData->currentText() != Info.completeBaseName()){
		s1 = ChooseData->currentText() + ":" + s1;
	}else{
	}
	GraphInput->setText(s1);

	//if(s.isEmpty()) { untested();
	GraphList->addItem(GraphInput->text());////insertItem(i, GraphInput->text());
	GraphList->setCurrentRow(GraphList->count()-1);

#if 0
	// what?
	Graph *g = new Graph(diag, GraphInput->text());   // create a new graph
#else
	// TODO: ask diagram. use suitable type.
	Data* g = data_dispatcher.clone("diagramvariable");
#endif

	//    if(Diag->name() != "Tab") {
	//      if(Diag->name() != "Truth") {
	//        g->set_param_by_name("color", misc::getWidgetBackgroundColor(ColorButt).toString());
	g->set_param_by_name("thick", Property2->text().toStdString());
	QColor selectedColor(DefaultColors[GraphList->count()%NumDefaultColors]);
	misc::setPickerColor(ColorButt, selectedColor);

#if 0
	// HB hack. does not work.
	if(g->Var.right(3) != ".Vb") {
	}else if(PropertyBox->count() >= GRAPHSTYLE_ARROW){
		PropertyBox->setCurrentIndex(GRAPHSTYLE_ARROW);
	}else{
	}
#endif

	g->set_param_by_name("style", std::to_string(PropertyBox->currentIndex())); // see graphstyle enum.
	//        assert(g->Style!=GRAPHSTYLE_INVALID); // why?
	if(yAxisBox) { untested();
		// TODO axes could be objects instead.
		g->set_param_by_name("yaxisno", std::to_string(yAxisBox->currentIndex()));
		yAxisBox->setEnabled(true);
		Label4->setEnabled(true);
	}
	// TODO axes could be objects instead. needed for 3d plot?
	//        else if(Diag->name() == "Rect3D"){
	//			  g->yAxisNo = 1;
	//		  }
	else{ untested();
	}

	Label3->setEnabled(true);
	ColorButt->setEnabled(true);
	//      } // truth hack
	//    } else
	if(0){
		g->set_param_by_name("precision", Property2->text().toStdString());
		g->set_param_by_name("nummode", std::to_string(PropertyBox->currentIndex()));
	}

	auto d = element(_clone);
	assert(d);

	Diagram* diag = prechecked_cast<Diagram*>(d);
	assert(diag);
	assert(diag->scope());
	diag->scope()->push_back(g);

	//  Graphs.append(g);
	//
	changed = true;
	toTake  = true;
	//}

	GraphInput->blockSignals(false);

	if(Property2) { untested();
		Label1->setEnabled(true);
		PropertyBox->setEnabled(true);
		Label2->setEnabled(true);
		Property2->setEnabled(true);
	}else{
	}
}

// a graph text is clicked in the ListBox.
void DiagramDialog::slotSelectGraph(QListWidgetItem *item)
{ untested();
	if(!item) { untested();
		GraphList->clearSelection();
		return;
	}else{
		trace1("click", item->text());
		QListWidgetItem* i = GraphList->currentItem();
		QVariant v = i->data(Qt::UserRole);
		Data* g = v.value<Data*>();
		assert(g);
		SelectGraph(g);
	}
}

/*!
  Puts the text of the selected graph into the line edit.
  */
void DiagramDialog::SelectGraph(Data *g)
{ untested();
	assert(GraphInput);
	assert(g);
	GraphInput->blockSignals(true);
	GraphInput->setText(QString_(g->short_label()));
	GraphInput->blockSignals(false);

	//  if(Diag->name() != "Tab") {
	//    if(Diag->name() != "Truth") {
	std::string a = g->get_param_by_name("thick");
	Property2->setText(QString_(a));
#if 0 // use params.
	misc::setPickerColor(ColorButt, g->Color);
	PropertyBox->setCurrentIndex(g->Style);
	if(yAxisBox) { untested();
		yAxisBox->setCurrentIndex(g->yAxisNo);
		yAxisBox->setEnabled(true);
		Label4->setEnabled(true);
	}
#endif

	Label3->setEnabled(true);
	ColorButt->setEnabled(true);
	//    }
	//  }
	//  else {
	//    Property2->setText(QString::number(g->Precision));
	//    PropertyBox->setCurrentIndex(g->numMode);
	//  }
	toTake = false;

	if(Property2) { untested();
		Label1->setEnabled(true);
		PropertyBox->setEnabled(true);
		Label2->setEnabled(true);
		Property2->setEnabled(true);
	}
}

void DiagramDialog::slotDeleteGraph()
{
	int i = GraphList->currentRow();

	//  if(Diag->name() != "Phasor" || Var2 != ".a") {
	//  } else {
	//    i = loc;
	//  }

	if(i < 0){ untested();
		return;
	}else{ untested();
	}

	GraphList->takeItem(i);
	//Graphs.remove(i);

	int k=0;
	if (GraphList->count()!=0) { untested();
		if (i>(GraphList->count()-1)) { untested();
			k = GraphList->count()-1;
		} else { untested();
			k=i;
		}
		GraphInput->setText(GraphList->item(k)->text());
	} else { untested();
		GraphInput->setText("");  // erase input line and back to default values
	}

	//  if(Diag->name() != "Tab") {
	//    if(Diag->name() != "Truth") {
	QColor selectedColor(DefaultColors[GraphList->count()%NumDefaultColors]);
	misc::setPickerColor(ColorButt, selectedColor);
	Property2->setText("0");
	if(yAxisBox) { untested();
		yAxisBox->setCurrentIndex(0);
		yAxisBox->setEnabled(false);
		Label4->setEnabled(false);
	}

	Label3->setEnabled(false);
	ColorButt->setEnabled(false);
	//    }
	//  }  else{
	//	  Property2->setText("3");
	//  }
	changed = true;
	toTake  = false;

	if(Property2) { untested();
		PropertyBox->setCurrentIndex(0);

		Label1->setEnabled(false);
		PropertyBox->setEnabled(false);
		Label2->setEnabled(false);
		Property2->setEnabled(false);
	}
}

// --------------------------------------------------------------------------
// why not pick data from listing?!
// add later. maybe.
void DiagramDialog::slotNewGraph()
{ untested();
	incomplete();
#if 0
	assert(Diag);
	if(GraphInput->text().isEmpty()) return;

	GraphList->addItem(GraphInput->text());

	Graph *g = new Graph(Diag, GraphInput->text());
	// FIXME: call  Diag->whateverelse();
	//  if(Diag->name() != "Tab") { // BUG
	//    if(Diag->name() != "Truth") { // BUG
	g->Color = misc::getWidgetBackgroundColor(ColorButt);
	g->Thick = Property2->text().toInt();
	g->Style = toGraphStyle(PropertyBox->currentIndex());
	assert(g->Style!=GRAPHSTYLE_INVALID);
	if(yAxisBox){
		g->yAxisNo = yAxisBox->currentIndex();
	} else {
		//	if(Diag->name() == "Rect3D")  g->yAxisNo = 1;
	}
	//    }
	//  }
	//  else {
	//    g->Precision = Property2->text().toInt();
	//    g->numMode   = PropertyBox->currentIndex();
	//  }
	Graphs.append(g);
	changed = true;
	toTake  = false;
#endif
}

/*!
  Is called if "Ok" button is pressed.
  */
void DiagramDialog::slotButtOK()
{ untested();
	slotButtApply();
	slotButtCancel();
}

// "Apply" button pressed
//  todo: tabdiagram/truthtable override
void DiagramDialog::slotButtApply()
{
#if 1
	incomplete();
	//TODO: execute swap command
#else
	// obsolete
	// TODO: remove. or move.

	{
		if(Diag->xAxis.Label.isEmpty())
			Diag->xAxis.Label = ""; // can be not 0 and empty!
		if(xLabel->text().isEmpty()) xLabel->setText("");
		if(Diag->xAxis.Label != xLabel->text()) { untested();
			Diag->xAxis.Label = xLabel->text();
			changed = true;
		}
		if(Diag->yAxis.Label.isEmpty())
			Diag->yAxis.Label = ""; // can be not 0 and empty!
		if(ylLabel->text().isEmpty()) ylLabel->setText("");
		if(Diag->yAxis.Label != ylLabel->text()) { untested();
			Diag->yAxis.Label = ylLabel->text();
			changed = true;
		}

		if(GridOn) if(Diag->xAxis.GridOn != GridOn->isChecked()) { untested();
			Diag->xAxis.GridOn = GridOn->isChecked();
			Diag->yAxis.GridOn = GridOn->isChecked();
			changed = true;
		}
		if(GridColorButt) { untested();
			QColor gridColBut = misc::getWidgetBackgroundColor(GridColorButt);
			if(Diag->GridPen.color() != gridColBut) { untested();
				Diag->GridPen.setColor(gridColBut);
				changed = true;
			}
		}
		if(GridStyleBox)
			if(Diag->GridPen.style()!=(Qt::PenStyle)(GridStyleBox->currentIndex()+1)) { untested();
				Diag->GridPen.setStyle((Qt::PenStyle)(GridStyleBox->currentIndex()+1));
				changed = true;
			}
		//if((Diag->name() != "Smith") && (Diag->name() != "Polar"))
		{
			if(Diag->zAxis.Label.isEmpty())
				Diag->zAxis.Label = ""; // can be not 0 and empty!
			if(yrLabel->text().isEmpty()) yrLabel->setText("");
			if(Diag->zAxis.Label != yrLabel->text()) { untested();
				Diag->zAxis.Label = yrLabel->text();
				changed = true;
			}
		}

		//    if(Diag->name().left(4) == "Rect") {
		if(Diag->xAxis.log != GridLogX->isChecked()) {
			Diag->xAxis.log = GridLogX->isChecked();
			changed = true;
		}
		if(Diag->yAxis.log != GridLogY->isChecked()) { untested();
			Diag->yAxis.log = GridLogY->isChecked();
			changed = true;
		}
		if(Diag->zAxis.log != GridLogZ->isChecked()) { untested();
			Diag->zAxis.log = GridLogZ->isChecked();
			changed = true;
		}
		//    }

		//    if((Diag->name() == "Smith") || (Diag->name() == "ySmith") ||
		//				  (Diag->name() == "PS"))
		//      if(stopY->text().toDouble() < 1.0)
		//        stopY->setText("1");

		//    if(Diag->name() == "SP")
		//      if(stopZ->text().toDouble() < 1.0)
		//        stopZ->setText("1");

		if(Diag->xAxis.autoScale == manualX->isChecked()) { untested();
			Diag->xAxis.autoScale = !(manualX->isChecked());
			changed = true;
		}

		// Use string compares for all floating point numbers, in
		// order to avoid rounding problems.
		if(QString::number(Diag->xAxis.limit_min) != startX->text()) { untested();
			Diag->xAxis.limit_min = startX->text().toDouble();
			changed = true;
		}
		if(QString::number(Diag->xAxis.step) != stepX->text()) { untested();
			Diag->xAxis.step = stepX->text().toDouble();
			changed = true;
		}
		if(QString::number(Diag->xAxis.limit_max) != stopX->text()) { untested();
			Diag->xAxis.limit_max = stopX->text().toDouble();
			changed = true;
		}
		if(Diag->yAxis.autoScale == manualY->isChecked()) { untested();
			Diag->yAxis.autoScale = !(manualY->isChecked());
			changed = true;
		}
		if(QString::number(Diag->yAxis.limit_min) != startY->text()) { untested();
			Diag->yAxis.limit_min = startY->text().toDouble();
			changed = true;
		}
		if(QString::number(Diag->yAxis.step) != stepY->text()) { untested();
			Diag->yAxis.step = stepY->text().toDouble();
			changed = true;
		}
		if(QString::number(Diag->yAxis.limit_max) != stopY->text()) { untested();
			Diag->yAxis.limit_max = stopY->text().toDouble();
			changed = true;
		}
		if(Diag->zAxis.autoScale == manualZ->isChecked()) { untested();
			Diag->zAxis.autoScale = !(manualZ->isChecked());
			changed = true;
		}
		if(QString::number(Diag->zAxis.limit_min) != startZ->text()) { untested();
			Diag->zAxis.limit_min = startZ->text().toDouble();
			changed = true;
		}
		if(QString::number(Diag->zAxis.step) != stepZ->text()) { untested();
			Diag->zAxis.step = stepZ->text().toDouble();
			changed = true;
		}
		if(QString::number(Diag->zAxis.limit_max) != stopZ->text()) { untested();
			Diag->zAxis.limit_max = stopZ->text().toDouble();
			changed = true;
		}

#if 0 //BUG
		if(hideInvisible)
			if(((Rect3DDiagram*)Diag)->hideLines != hideInvisible->isChecked()) { untested();
				((Rect3DDiagram*)Diag)->hideLines = hideInvisible->isChecked();
				changed = true;
			}

		if(rotationX)
			if(((Rect3DDiagram*)Diag)->rotX != rotationX->text().toInt()) { untested();
				((Rect3DDiagram*)Diag)->rotX = rotationX->text().toInt();
				changed = true;
			}

		if(rotationY)
			if(((Rect3DDiagram*)Diag)->rotY != rotationY->text().toInt()) { untested();
				((Rect3DDiagram*)Diag)->rotY = rotationY->text().toInt();
				changed = true;
			}

		if(rotationZ)
			if(((Rect3DDiagram*)Diag)->rotZ != rotationZ->text().toInt()) { untested();
				((Rect3DDiagram*)Diag)->rotZ = rotationZ->text().toInt();
				changed = true;
			}
#endif

	}   // of "if(Diag->Name != "Tab")"

//	Diag->Graphs.clear();   // delete the graphs
//	Graphs.setAutoDelete(false);
//	for(Graph *pg = Graphs.first(); pg != 0; pg = Graphs.next()){ untested();
//		Diag->Graphs.append(pg);  // transfer the new graphs to diagram
//	}
//	Graphs.clear();
//	Graphs.setAutoDelete(true);

	Diag->loadGraphData(defaultDataSet);
	((SchematicDoc*)parent())->viewport()->repaint();
	// copyDiagramGraphs();

	// what?
	if(changed){ untested();
		transfer = true;
	}else{ untested();
	}
#endif
}


// --------------------------------------------------------------------------
// Is called if "Cancel" button is pressed.
void DiagramDialog::slotButtCancel()
{ untested();
	//  Diag->loadGraphData(defaultDataSet);
	//  ((QucsView*)parent())->viewport()->repaint();
	if(transfer){ untested();
		done(QDialog::Accepted);
	}else{ untested();
		done(QDialog::Rejected);
	}
}

//-----------------------------------------------------------------
// To get really all close events (even <Escape> key).
void DiagramDialog::reject()
{ untested();
	slotButtCancel();
}

// --------------------------------------------------------------------------
void DiagramDialog::slotSetColor()
{ untested();
	QColor c = QColorDialog::getColor(
			misc::getWidgetBackgroundColor(ColorButt),this);
	if(!c.isValid()) return;
	misc::setPickerColor(ColorButt, c);

	int i = GraphList->currentRow();
	if(i < 0) return;   // return, if no item selected

	incomplete();
//	Graph *g = Graphs.at(i);
//	g->Color = c;
//	changed = true;
//	toTake  = false;
}

// --------------------------------------------------------------------------
void DiagramDialog::slotSetGridColor()
{ untested();
	QColor c = QColorDialog::getColor(
			misc::getWidgetBackgroundColor(GridColorButt),this);
	if(!c.isValid()) return;
	misc::setPickerColor(GridColorButt, c);
	changed = true;
}

/*!
  Is set if the graph input line changes.
  */
void DiagramDialog::slotResetToTake(const QString& s)
{ untested();
	int i = GraphList->currentRow();
	if(i < 0) return;   // return, if no item selected

	incomplete();
//	Graph *g = Graphs.at(i);
//	g->Var = s;
//	// \todo GraphList->changeItem(s, i);   // must done after the graph settings !!!
//	changed = true;
//	toTake  = false;
}

// the user changed the graph thickness or the precision.
void DiagramDialog::slotSetProp2(const QString& s)
{
	int i = GraphList->currentRow();
	if(i < 0){
		return;   // return, if no item selected
	}else{

		incomplete();
//		Graph *g = Graphs.at(i);
//		//  if(Diag->name() == "Tab"){
//		//  g->Precision = s.toInt();
//		//  } else {
//		g->Thick = s.toInt();
//		//	}
//		changed = true;
//		toTake  = false;
	}
}

// The user changed the number mode.
void DiagramDialog::slotSetNumMode(int Mode)
{ untested();
	int i = GraphList->currentRow();
	if(i < 0) return;   // return, if no item selected

	incomplete();
//	Graph *g = Graphs.at(i);
//	g->numMode = Mode;
//	changed = true;
//	toTake  = false;
}

/*!
  Is called when the "show grid" checkbox is changed.
  */
void DiagramDialog::slotSetGridBox(int state)
{ untested();
	if(state == 2) { untested();
		GridColorButt->setEnabled(true);
		GridStyleBox->setEnabled(true);
		GridLabel1->setEnabled(true);
		GridLabel2->setEnabled(true);
	}
	else { untested();
		GridColorButt->setEnabled(false);
		GridStyleBox->setEnabled(false);
		GridLabel1->setEnabled(false);
		GridLabel2->setEnabled(false);
	}
}

// The user changed the graph style (combobox).
void DiagramDialog::slotSetGraphStyle(int style)
{ untested();
	int i = GraphList->currentRow();
	if(i < 0){
		// if no item selected
		return;
	}else{
//		Graph *g = Graphs.at(i);
//		g->Style = toGraphStyle(style);
//		assert(g->Style!=GRAPHSTYLE_INVALID);
//		changed = true;
//		toTake  = false;
	}
}

#if 0
/*!
  Makes a copy of all graphs in the diagram.
  */
void DiagramDialog::copyDiagramGraphs()
{ untested();
	foreach(Graph *pg, Diag->Graphs)
		Graphs.append(pg->sameNewOne());
}
#endif

/*!
  Is called if the combobox changes that defines which y axis uses the graph.
  */
void DiagramDialog::slotSetYAxis(int axis)
{ untested();
	int i = GraphList->currentRow();
	if(i < 0) return;   // return, if no item selected

	incomplete();
//	Graph *g = Graphs.at(i);
//	g->yAxisNo = axis;
//	changed = true;
//	toTake  = false;
}

// --------------------------------------------------------------------------
void DiagramDialog::slotManualX(int state)
{
	if(state == 2) {
		// if((Diag->name().left(4) == "Rect") || (Diag->name() == "Curve")){
		startX->setEnabled(true);
		// }else{
		// }
		stopX->setEnabled(true);
		if(GridLogX) if(GridLogX->isChecked())  return;
		stepX->setEnabled(true);
	} else {
		startX->setEnabled(false);
		stepX->setEnabled(false);
		stopX->setEnabled(false);
	}
}

// --------------------------------------------------------------------------
void DiagramDialog::slotManualY(int state)
{
	if(state == 2) {
		//    if((Diag->name().left(4) == "Rect") || (Diag->name() ==
		//    "Curve")||(Diag->name()=="Phasor")  || (Diag->name()=="Waveac")){
		startY->setEnabled(true);
		//  }
		stopY->setEnabled(true);
		if(GridLogY) if(GridLogY->isChecked())  return;
		stepY->setEnabled(true);
	}
	else { untested();
		startY->setEnabled(false);
		stepY->setEnabled(false);
		stopY->setEnabled(false);
	}
}

// --------------------------------------------------------------------------
void DiagramDialog::slotManualZ(int state)
{
	if(state == 2) {
		//    if((Diag->name().left(4) == "Rect") || (Diag->name() ==
		//    "Curve")||(Diag->name()=="Phasor")  || (Diag->name()=="Waveac")){
		startZ->setEnabled(true);
		//}
		stopZ->setEnabled(true);
		if(GridLogZ){
			if(GridLogZ->isChecked()){
				return;
			}
		}
		stepZ->setEnabled(true);
	} else { untested();
		startZ->setEnabled(false);
		stepZ->setEnabled(false);
		stopZ->setEnabled(false);
	}
}
/*--------------------------------------------------------------------------*/
// Is called if the current tab of the QTabWidget changes.
void DiagramDialog::slotChangeTab(QWidget*)
{ untested();
	if(stepX == 0) return;   // BUG

	if(!GridLogX) { untested();
	}else if(GridLogX->isChecked()) {
		stepX->setEnabled(false);
	}else if(manualX->isChecked()) {
		stepX->setEnabled(true);
	}else{
	}

	if(!GridLogY) { untested();
	}else if(GridLogY->isChecked()){
	 	stepY->setEnabled(false);
	}else if(manualY->isChecked()){
	  	stepY->setEnabled(true);
	}else{
	}

	if(!GridLogZ) { untested();
	}else if(GridLogZ->isChecked()){
	 	stepZ->setEnabled(false);
	}else if(manualZ->isChecked()){
		stepZ->setEnabled(true);
	}else{
	}
}
/*--------------------------------------------------------------------------*/
// The slider for rotation angle has changed
void DiagramDialog::slotNewRotX(int Value)
{ untested();
	rotationX->setText(QString::number(Value));
	DiagCross->rotX = float(Value) * pi/180.0;
	DiagCross->update();
}
/*--------------------------------------------------------------------------*/
// The slider for rotation angle has changed
void DiagramDialog::slotNewRotY(int Value)
{ untested();
	rotationY->setText(QString::number(Value));
	DiagCross->rotY = float(Value) * pi/180.0;
	DiagCross->update();
}
/*--------------------------------------------------------------------------*/
// The slider for rotation angle has changed
void DiagramDialog::slotNewRotZ(int Value)
{ untested();
	rotationZ->setText(QString::number(Value));
	DiagCross->rotZ = float(Value) * pi/180.0;
	DiagCross->update();
}

/*--------------------------------------------------------------------------*/
// the number (text) for rotation angle has changed
void DiagramDialog::slotEditRotX(const QString& Text)
{ untested();
	SliderRotX->setValue(Text.toInt());
	DiagCross->rotX = Text.toFloat() * pi/180.0;
	DiagCross->update();
}
/*--------------------------------------------------------------------------*/
// the number (text) for rotation angle has changed
void DiagramDialog::slotEditRotY(const QString& Text)
{ untested();
	SliderRotY->setValue(Text.toInt());
	DiagCross->rotY = Text.toFloat() * pi/180.0;
	DiagCross->update();
}
/*--------------------------------------------------------------------------*/
// The number (text) for rotation angle has changed
void DiagramDialog::slotEditRotZ(const QString& Text)
{ untested();
	SliderRotZ->setValue(Text.toInt());
	DiagCross->rotZ = Text.toFloat() * pi/180.0;
	DiagCross->update();
}
/*--------------------------------------------------------------------------*/
class DialogFactory : public Widget {
	Widget* clone() const override{
		SchematicDialog* wd = new DiagramDialog();
		return wd;
	}
}F;
static Dispatcher<Widget>::INSTALL p1(&qucs::widget_dispatcher, "RectDialog|DiagramDialog", &F);
/*--------------------------------------------------------------------------*/
} // namespace
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
