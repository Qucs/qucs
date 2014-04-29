#include "filtersintez.h"
#include "sallenkey.h"
#include "mfbfilter.h"
#include <QTextCodec>


FilterSintez::FilterSintez(QWidget *parent)
    : QMainWindow(parent)
{
    Nfil = 4;
    Fc = 1000;

    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

    lblInputData = new QLabel(tr("Входные данные"));
    lblA1 = new QLabel(tr("Затухание в полосе пропускания, Ap"));
    lblA2 = new QLabel(tr("Мин. затухание в полосе задерживания, As"));
    lblF1 = new QLabel(tr("Частота среза фильтра, Fc (Гц)"));
    lblF2 = new QLabel(tr("Начало полосы задерживания, Fs (Гц)"));
    lblRpl1 = new QLabel(tr("Амплитуда пульсаций в полосе пропускания Rp(дБ)"));
    //lblRpl2 = new QLabel(tr("Амплитуда пульсаций в полосе задерживания (дБ)"));
    lblKv = new QLabel(tr("Усиление фильтра, Kv (дБ)"));


    edtA1 = new QLineEdit("3");
    QDoubleValidator *val1 = new QDoubleValidator(0,100000,3);
    edtA1->setValidator(val1);
    edtA2 = new QLineEdit("20");
    edtA2->setValidator(val1);
    edtF1 = new QLineEdit("1000");
    edtF1->setValidator(val1);
    edtF2 = new QLineEdit("1200");
    edtF2->setValidator(val1);
    edtPassbRpl = new QLineEdit("3");
    edtPassbRpl->setValidator(val1);
    //edtStopbRpl = new QLineEdit("3");
    //edtStopbRpl->setValidator(val1);
    edtKv = new QLineEdit("0");
    edtKv->setValidator(val1);

    lblTyp = new QLabel(tr("Рассчитать фильтр:"));
    cbxFilterFunc = new QComboBox;
    QStringList lst2;
    lst2<<tr("Фильтр Баттерворта")
        <<tr("Фильтр Чебышева")
        <<tr("Инверсный фильтр Чебышева")
        <<tr("Эллиптический фильтр");
    cbxFilterFunc->addItems(lst2);
    btnCalcSchematic = new QPushButton(tr("Рассчитать элементы схемы фильтра"));
    connect(btnCalcSchematic,SIGNAL(clicked()),SLOT(slotCalcSchematic()));

    lblResult = new QLabel(tr("Результаты расчёта: "));
    txtResult = new QTextEdit;


    lblSch = new QLabel(tr("Схемная реализация фильтра"));
    btnHighPass = new QRadioButton(tr("ФВЧ"));
    btnLowPass = new QRadioButton(tr("ФНЧ"));
    QButtonGroup *grp1 = new QButtonGroup;
    grp1->addButton(btnHighPass);
    grp1->addButton(btnLowPass);
    btnLowPass->setChecked(true);
    connect(grp1,SIGNAL(buttonClicked(int)),this,SLOT(slotUpdateSchematic()));

    cbxFilterType = new QComboBox;
    QStringList lst;
    lst<<tr("Биквадратный")
      <<tr("С многопетлевой ОС")
      <<tr("Саллена-Ки")
      <<tr("Пассивный");
    cbxFilterType->addItems(lst);
    connect(cbxFilterType,SIGNAL(currentIndexChanged(int)),this,SLOT(slotUpdateSchematic()));

    imgAFR = new QSvgWidget("AFR.svg");
    imgAFR->show();
    sch_pic = new QLabel;
    QPixmap pix("Images/dblquad.png");
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
    left->addWidget(lblRpl1);
    left->addWidget(edtPassbRpl);
    left->addWidget(lblKv);
    left->addWidget(edtKv);
    left->addWidget(lblTyp);
    left->addWidget(cbxFilterFunc);


    center->addWidget(lblResult);
    center->addWidget(txtResult);
    txtResult->setMinimumWidth(400);

    left->addWidget(lblSch);
    left->addWidget(btnLowPass);
    left->addWidget(btnHighPass);
    left->addWidget(cbxFilterType);
    left->addWidget(btnCalcSchematic);

    right->addWidget(imgAFR);
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


void FilterSintez::slotCalcSchematic()
{

    FilterParam par;
    par.Ap = edtA1->text().toFloat();
    par.As = edtA2->text().toFloat();
    par.Rp = edtPassbRpl->text().toFloat();
    par.Fc = edtF1->text().toFloat();
    par.Fs = edtF2->text().toFloat();
    float G = edtKv->text().toFloat();
    par.Kv = pow(10,G/20.0);

    QStringList lst;
    Filter::FilterFunc ffunc;

    switch (cbxFilterFunc->currentIndex()) {
            case 0 : ffunc = Filter::Butterworth;
                     break;
            case 1 : ffunc = Filter::Chebyshev;
                     break;
            case 2 : ffunc = Filter::InvChebyshev;
                     break;
            case 3 : ffunc = Filter::Cauer;
                     break;
            default: break;
        }

    Filter::FType ftyp;
    if (btnHighPass->isChecked()) {
        ftyp = Filter::HighPass;
    } else {
        ftyp = Filter::LowPass;
    }



    switch (cbxFilterType->currentIndex()) {
    case 0 : if (btnHighPass->isChecked()) calcDblQuadHPF();
             else calcDblQuadLPF();
             break;
    case 1 : {
                QString s;
                MFBfilter mfb(ffunc,ftyp,par);
                mfb.calcFilter();
                mfb.createPolesZerosList(lst);
                mfb.createPartList(lst);
                mfb.createSchematic(s);
                txtResult->setText(lst.join("\n"));
             }
             break;
    case 2 : {
               QString s;
               SallenKey sk(ffunc,ftyp,par);
               sk.calcFilter();
               sk.createPolesZerosList(lst);
               sk.createPartList(lst);
               sk.createSchematic(s);
               txtResult->setText(lst.join("\n"));
             }
             break;
    case 3 : calcPassive();
             break;
    default: break;
    }
}

void FilterSintez::slotUpdateSchematic()
{
    QString s;
    switch (cbxFilterType->currentIndex()) {
    case 0 : s = "Images/dblquad.png";
             break;
    case 1 : s= "Images/multiloop.png";
             break;
    case 2 : if (btnHighPass->isChecked()) s = "Images/high-pass1.png";
             else s = "Images/low-pass1.png";
             break;
    case 3 : s = "Images/passive.png";
             break;
    default: break;
    }

    QPixmap pix(s);
    sch_pic->resize(pix.size());
    sch_pic->setPixmap(pix);
}


void FilterSintez::calcDblQuadHPF()
{

}

void FilterSintez::calcDblQuadLPF()
{

}

void FilterSintez::calcPassive()
{

}
