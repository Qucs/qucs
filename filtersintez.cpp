#include "filtersintez.h"
#include "sallenkey.h"
#include "mfbfilter.h"
#include "schcauer.h"
#include <QTextCodec>


FilterSintez::FilterSintez(QWidget *parent)
    : QMainWindow(parent)
{
    Nfil = 4;
    Fc = 1000;
    ftyp = Filter::NoFilter;

    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

    //lblInputData = new QLabel(tr("Входные данные"));
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
    connect(cbxFilterFunc,SIGNAL(currentIndexChanged(int)),this,SLOT(slotSwitchParameters()));

    btnCalcSchematic = new QPushButton(tr("Рассчитать элементы схемы фильтра"));
    connect(btnCalcSchematic,SIGNAL(clicked()),SLOT(slotCalcSchematic()));

    lblResult = new QLabel(tr("Результаты расчёта: "));
    txtResult = new QTextEdit;


    lblSch = new QLabel(tr("Схемная реализация фильтра"));
    lblResp = new QLabel(tr("Filter type:"));
    cbxResponse = new QComboBox;
    QStringList lst3;
    lst3<<tr("LowPass")
        <<tr("High Pass")
        <<tr("Band Pass")
        <<tr("Band Stop");
    cbxResponse->addItems(lst3);
    connect(cbxResponse,SIGNAL(currentIndexChanged(int)),this,SLOT(slotUpdateResponse()));
    connect(cbxResponse,SIGNAL(currentIndexChanged(int)),this,SLOT(slotUpdateSchematic()));

    cbxFilterType = new QComboBox;
    QStringList lst;
    lst<<tr("Cauer section")
      <<tr("С многопетлевой ОС")
      <<tr("Саллена-Ки")
      <<tr("Пассивный");
    cbxFilterType->addItems(lst);
    connect(cbxFilterType,SIGNAL(currentIndexChanged(int)),this,SLOT(slotUpdateSchematic()));
    this->slotSwitchParameters();

    QString s1 = ":/images/AFR.svg";
    QSvgRenderer *ren = new QSvgRenderer(s1);
    QSize sz = ren->defaultSize();
    sz *= 1.1;
    delete ren;
    imgAFR = new QSvgWidget(s1);
    imgAFR->setFixedSize(sz);
    imgAFR->show();

    s1 = ":/images/cauer.svg";
    ren = new QSvgRenderer(s1);
    sz = ren->defaultSize();
    sz *= 0.65;
    delete ren;

    sch_pic = new QSvgWidget(s1);
    sch_pic->setFixedSize(sz);
    sch_pic->show();

    top = new QHBoxLayout;
    left = new QVBoxLayout;
    center = new QVBoxLayout;
    right = new QVBoxLayout;

    //left->addWidget(lblInputData);
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
    QHBoxLayout *l3 = new QHBoxLayout;
    l3->addWidget(lblTyp);
    l3->addWidget(cbxFilterFunc);
    left->addLayout(l3);

    QHBoxLayout *l1 = new QHBoxLayout;
    l1->addWidget(lblResp);
    l1->addWidget(cbxResponse);
    left->addLayout(l1);
    QHBoxLayout *l2 = new QHBoxLayout;
    l2->addWidget(lblSch);
    l2->addWidget(cbxFilterType);
    left->addLayout(l2);
    left->addWidget(btnCalcSchematic);
    left->addStretch();

    right->addWidget(imgAFR);
    right->addWidget(sch_pic);
    right->addStretch();

    top->addLayout(left);
    top->addLayout(center);
    top->addLayout(right);

    top1 = new QVBoxLayout;
    top1->addLayout(top);
    QSplitter *sp1 = new QSplitter;
    top1->addWidget(sp1);
    txtResult->setReadOnly(true);
    top1->addWidget(txtResult);
    txtResult->setMinimumHeight(180);

    zenter = new QWidget;
    this->setCentralWidget(zenter);
    zenter->setLayout(top1);
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
            default: ffunc = Filter::NoFunc;
                     break;
        }



    switch (cbxResponse->currentIndex()) {
    case 0 : ftyp = Filter::LowPass;
        break;
    case 1 : ftyp = Filter::HighPass;
        break;
    case 2 : ftyp = Filter::BandPass;
        break;
    case 3 : ftyp = Filter::BandStop;
        break;
    default: ftyp = Filter::NoFilter;
        break;
    }

    switch (cbxFilterType->currentIndex()) {
    case 0 : {
                QString s;
                if (((ffunc==Filter::InvChebyshev)||(ffunc==Filter::Cauer))) {
                   SchCauer cauer(ffunc,ftyp,par);
                   bool ok = cauer.calcFilter();
                   cauer.createPolesZerosList(lst);
                   cauer.createPartList(lst);
                   if (ok) {
                       cauer.createSchematic(s);
                   }
                   txtResult->setText(lst.join("\n"));
                } else {
                    errorMessage(tr("Unable to use Cauer section for Chebyshev or Butterworth \n"
                                 "frequency response. Try to use another topology."));
                }
             }

             break;
    case 1 : {
                QString s;
                if (!((ffunc==Filter::InvChebyshev)||(ffunc==Filter::Cauer))) {
                    MFBfilter mfb(ffunc,ftyp,par);
                    bool ok = mfb.calcFilter();
                    mfb.createPolesZerosList(lst);
                    mfb.createPartList(lst);
                    txtResult->setText(lst.join("\n"));
                    if (ok) {
                        mfb.createSchematic(s);
                    } else {
                        errorMessage(tr("Unable to implement filter with such parameters and topology \n"
                                        "Change parapeters and/or topology and try again!"));
                    }
                } else {
                    errorMessage(tr("Unable to use MFB filter for Cauer or Inverse Chebyshev \n"
                                 "frequency response. Try to use another topology."));
                }
             }
             break;
    case 2 : {
               QString s;
               SallenKey sk(ffunc,ftyp,par);
               bool ok = sk.calcFilter();
               sk.createPolesZerosList(lst);
               sk.createPartList(lst);
               txtResult->setText(lst.join("\n"));
               if (ok) {
                   sk.createSchematic(s);
               } else {
                   errorMessage(tr("Unable to implement filter with such parameters and topology \n"
                                   "Change parapeters and/or topology and try again!"));
               }
             }
             break;
    case 3 : errorMessage(tr("Function will be implemented in future version"));
             break;
    default: break;
    }
}

void FilterSintez::slotUpdateResponse()
{
    QString s = ":/images/AFR.svg";

    switch (cbxResponse->currentIndex()) {
        case 0 :
            s = ":/images/AFR.svg";
            ftyp = Filter::LowPass;
            break;
        case 1 : s = ":/images/high-pass.svg";
            ftyp = Filter::HighPass;
            break;
        case 2 : ftyp = Filter::BandPass;
            break;
        case 3 : ftyp = Filter::BandStop;
            break;
        default: ftyp = Filter::NoFilter;
            break;
        }

    QSvgRenderer *ren = new QSvgRenderer(s);
    QSize sz = ren->defaultSize();
    sz *= 1.1;
    delete ren;

    imgAFR->load(s);
    imgAFR->setFixedSize(sz);
}

void FilterSintez::slotUpdateSchematic()
{
    slotUpdateResponse();
    QString s;
    switch (cbxFilterType->currentIndex()) {
    case 0 : s = ":images/cauer.svg";
             break;
    case 1 : if (ftyp==Filter::HighPass) {
            s = ":/images/mfb-highpass.svg";
        } else if (ftyp==Filter::LowPass) {
            s = ":/images/mfb-lowpass.svg";
        }
             break;
    case 2 : if (ftyp==Filter::HighPass) {
            s = ":/images/sk-highpass.svg";
        } else if (ftyp==Filter::LowPass) {
           s = ":/images/sk-lowpass.svg";
        }
        break;
    case 3 : s = ":/images/mfb-lowpass.svg";
        break;
    default:
        break;
    }

    QSvgRenderer *ren = new QSvgRenderer(s);
    QSize sz = ren->defaultSize();
    sz *= 0.65;
    delete ren;

    sch_pic->load(s);
    sch_pic->setFixedSize(sz);
}

void FilterSintez::slotSwitchParameters()
{
    if (cbxFilterFunc->currentIndex()==0) {
        edtA1->setEnabled(true);
        edtPassbRpl->setEnabled(false);
    } else {
        edtA1->setEnabled(false);
        edtPassbRpl->setEnabled(true);
    }

    if ((cbxFilterFunc->currentIndex()==3)||(cbxFilterFunc->currentIndex()==2)) {
        cbxFilterType->setCurrentIndex(0);
        cbxFilterType->setDisabled(true);
    } else {
        cbxFilterType->setDisabled(false);
    }
}

void FilterSintez::errorMessage(QString str)
{
    QMessageBox* msg =  new QMessageBox(QMessageBox::Critical,tr("Active filter design"),
                                        str,
                                        QMessageBox::Ok);
    msg->exec();
    delete msg;
}
