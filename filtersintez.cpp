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
    btnChebyshev = new QPushButton(tr("Фильтр Чебышева"));
    btnButterworth = new QPushButton(tr("Фильтр Баттерворта"));
    btnInvChebyshev = new QPushButton(tr("Инверсный фильтр Чебышева"));
    btnElliptic = new QPushButton(tr("Эллиптический фильтр"));

    lblResult = new QLabel(tr("Результаты расчёта: "));
    txtResult = new QTextEdit;

    btnHighPass = new QRadioButton(tr("ФВЧ"));
    btnLowPass = new QRadioButton(tr("ФНЧ"));
    QButtonGroup *grp1 = new QButtonGroup;
    grp1->addButton(btnHighPass);
    grp1->addButton(btnLowPass);

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
    left->addWidget(btnChebyshev);
    left->addWidget(btnButterworth);
    left->addWidget(btnInvChebyshev);
    left->addWidget(btnElliptic);

    center->addWidget(lblResult);
    center->addWidget(txtResult);

    right->addWidget(btnLowPass);
    right->addWidget(btnHighPass);

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
