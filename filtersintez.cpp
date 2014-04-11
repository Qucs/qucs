#include "filtersintez.h"
#include <QTextCodec>

FilterSintez::FilterSintez(QWidget *parent)
    : QMainWindow(parent)
{
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

    lblInputData = new QLabel(tr("Входные данные"));
    lblA1 = new QLabel(tr("Затухание фильтра в полосе пропускания, A1"));
    lblA2 = new QLabel(tr("Затухание фильтра в полосе задерживания, A2"));
    lblF1 = new QLabel(tr("Частота среза фильтра в полосе пропускания, F1"));
    lblF2 = new QLabel(tr("Частота фильтра в полосе задерживания, F2"));

    edtA1 = new QLineEdit;
    QDoubleValidator *val1 = new QDoubleValidator(0,100000,3);
    edtA1->setValidator(val1);
    edtA2 = new QLineEdit;
    edtA2->setValidator(val1);
    edtF1 = new QLineEdit;
    edtF1->setValidator(val1);
    edtF2 = new QLineEdit;
    edtF2->setValidator(val1);

    lblTyp = new QLabel(tr("Рассчитать фильтр:"));
    cbxFilterFunc = new QComboBox;
    QStringList lst2;
    lst2<<tr("Фильтр Баттерворта")
        <<tr("Фильтр Чебышева")
        <<tr("Инверсный фильтр Чебышева")
        <<tr("Эллиптический фильтр");
    cbxFilterFunc->addItems(lst2);
    btnCalcFiltFunc = new QPushButton(tr("Рассчитать функцию фильтра"));

    lblResult = new QLabel(tr("Результаты расчёта: "));
    txtResult = new QTextEdit;


    lblSch = new QLabel(tr("Схеманая реализация фильтра"));
    btnHighPass = new QRadioButton(tr("ФВЧ"));
    btnLowPass = new QRadioButton(tr("ФНЧ"));
    QButtonGroup *grp1 = new QButtonGroup;
    grp1->addButton(btnHighPass);
    grp1->addButton(btnLowPass);

    cbxFilterType = new QComboBox;
    QStringList lst;
    lst<<tr("Биквадратный")
      <<tr("С многопетлевой ОС")
      <<tr("Саллена-Кея")
      <<tr("Пассивный");
    cbxFilterType->addItems(lst);

    sch_pic = new QLabel;
    QPixmap pix("dblquad.png");
    sch_pic->resize(pix.size());
    sch_pic->setPixmap(pix);

    top = new QHBoxLayout;
    left = new QVBoxLayout;
    center = new QVBoxLayout;
    right = new QVBoxLayout;

    left->addWidget(lblInputData);
    left->addWidget(lblA1);
    left->addWidget(edtA1);
    left->addWidget(lblA2);
    left->addWidget(edtA2);
    left->addWidget(lblF1);
    left->addWidget(edtF1);
    left->addWidget(lblF2);
    left->addWidget(edtF2);
    left->addWidget(lblTyp);
    left->addWidget(cbxFilterFunc);


    center->addWidget(lblResult);
    center->addWidget(txtResult);

    left->addWidget(lblSch);
    left->addWidget(btnLowPass);
    left->addWidget(btnHighPass);
    left->addWidget(cbxFilterType);
    left->addWidget(btnCalcFiltFunc);

    right->addWidget(sch_pic);

    top->addLayout(left);
    top->addLayout(center);
    top->addLayout(right);

    zenter = new QWidget;
    this->setCentralWidget(zenter);
    zenter->setLayout(top);
}

FilterSintez::~FilterSintez()
{
    
}


void FilterSintez::slotCalcFilter()
{

}

void FilterSintez::calcChebyshev()
{

}

void FilterSintez::calcButterworth()
{

}

void FilterSintez::calcElliptic()
{

}
