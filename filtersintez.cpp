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
    lblA1 = new QLabel(tr("Затухание фильтра в полосе пропускания, Ap"));
    lblA2 = new QLabel(tr("Минимальное затухание фильтра в полосе задерживания, As"));
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
    edtKv = new QLineEdit("2");
    edtKv->setValidator(val1);

    lblTyp = new QLabel(tr("Рассчитать фильтр:"));
    cbxFilterFunc = new QComboBox;
    QStringList lst2;
    lst2<<tr("Фильтр Баттерворта")
        <<tr("Фильтр Чебышева")
        <<tr("Инверсный фильтр Чебышева")
        <<tr("Эллиптический фильтр");
    cbxFilterFunc->addItems(lst2);
    btnCalcFiltFunc = new QPushButton(tr("Рассчитать функцию фильтра"));
    connect(btnCalcFiltFunc,SIGNAL(clicked()),this,SLOT(slotCalcFilter()));
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
      <<tr("Саллена-Кея")
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
    //left->addWidget(lblRpl2);
    //left->addWidget(edtStopbRpl);
    left->addWidget(lblKv);
    left->addWidget(edtKv);
    left->addWidget(lblTyp);
    left->addWidget(cbxFilterFunc);


    center->addWidget(lblResult);
    center->addWidget(txtResult);

    left->addWidget(lblSch);
    left->addWidget(btnLowPass);
    left->addWidget(btnHighPass);
    left->addWidget(cbxFilterType);
    left->addWidget(btnCalcFiltFunc);
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


void FilterSintez::slotCalcFilter()
{
    switch (cbxFilterFunc->currentIndex()) {
        case 0 : Nfil = calcButterworth();
                 break;
        case 1 : Nfil = calcChebyshev();
                 break;
        case 2 : Nfil = calcInvChebyshev();
                 break;
        case 3 : Nfil = calcElliptic();
                 break;
        default: break;
    }

    //calcButterworth();
}

void FilterSintez::slotCalcSchematic()
{

    slotCalcFilter();

    Filter::FType ftyp;
    if (btnHighPass->isChecked()) {
        ftyp = Filter::HighPass;
    } else {
        ftyp = Filter::LowPass;
    }

    QStringList lst;

    float G = edtKv->text().toFloat();
    float Kv = pow(10,G/20.0);

    switch (cbxFilterType->currentIndex()) {
    case 0 : if (btnHighPass->isChecked()) calcDblQuadHPF();
             else calcDblQuadLPF();
             break;
    case 1 : {
                MFBfilter mfb(Poles,ftyp,Fc,Kv);
                mfb.calcFilter();
                mfb.createPartList(lst);
                txtResult->setText(lst.join("\n"));
             }
             break;
    case 2 : {
               QString s;
               SallenKey sk(Poles,ftyp,Fc,Kv);
               sk.calcFilter();
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

int FilterSintez::calcChebyshev()
{
    float alpha = edtPassbRpl->text().toFloat();
    float A2 = edtA2->text().toFloat();
    float F1 = edtF1->text().toFloat();
    float F2 = edtF2->text().toFloat();
    Fc = F1;

    float eps=sqrt(pow(10,0.1*alpha)-1);

    float N1 = acosh(sqrt((pow(10,0.1*A2)-1)/(eps*eps)))/acosh(F2/F1);
    int N = ceil(N1);

    QStringList lst;
    lst<<tr(" 1. Порядок фильтра Чебышева");
    lst<<QString::number(N);
    lst<<tr(" 2. Неравномерность пропускания");

    lst<<QString::number(eps);

    float a = sinh((asinh(1/eps))/N);
    float b = cosh((asinh(1/eps))/N);

    Poles.clear();

    lst<<""<<tr(" 2. Полюса  Sk=SIN+j*COS");
    for (int k=1;k<=N;k++) {
            float re = -1*a*sin(M_PI*(2*k-1)/(2*N));
            float im = b*cos(M_PI*(2*k-1)/(2*N));
            std::complex<float> pol(re,im);
            Poles.append(pol);
            lst<<QString::number(re) + " + j*" + QString::number(im);
    }


    txtResult->setText(lst.join("\n"));
    return N;
}

int FilterSintez::calcButterworth()
{
    //float R1,l,w,q,Wc,C5,K7;

    float A1 = edtA1->text().toFloat();
    float A2 = edtA2->text().toFloat();
    float F1 = edtF1->text().toFloat();
    float F2 = edtF2->text().toFloat();
    Fc = F1;

    float K1 = pow(10,(0.1*A1));
    float K2 = pow(10,(0.1*A2));
    float C1=(K1-1)/(K2-1);
    float J2=log10(C1)/(2*log10(F1/F2));
    int N2 = round(J2+1);

    QStringList lst;
    lst<<tr(" 1. Порядок фильтра Баттерворта");
    lst<<QString::number(N2);
    int N3= N2/2;
    lst<<tr(" Число звеньев 2-го порядка");
    lst<<QString::number(N3);

    //float S2[50],O2[50];
    lst<<"";
    lst<<tr(" 2. Полюса Sk=SIN+j*COS");

    coeffB.clear();
    coeffC.clear();
    Poles.clear();

    for (int k=1;k<=N2;k++) {
        float re =-1*sin(M_PI*(2*k-1)/(2*N2));
        float im =cos(M_PI*(2*k-1)/(2*N2));
        std::complex<float> pol(re,im);
        Poles.append(pol);
        lst<<QString::number(re) + " + j*" + QString::number(im);
    }

    lst<<"";

    txtResult->setText(lst.join("\n"));

    return N2;
}

int FilterSintez::calcInvChebyshev()
{
    float A1 = edtA1->text().toFloat();
    float A2 = edtA2->text().toFloat();
    float F1 = edtF1->text().toFloat();
    float F2 = edtF2->text().toFloat();
    Fc = F1;

    float W5=F2/F1;
    float K4=pow(10,(0.1*A1));
    float C5=pow((K4-1),0.5);

    float J5 = log(C5+pow((C5*C5-1),0.5))/log(W5+pow((W5*W5-1),0.5));
    int N5 = round(J5+1);

    QStringList lst;

    lst<<tr(" 1. Порядок  инверсного фильтра Чебышева")<<QString::number(N5);

    float E0=pow((K4-1),0.5);
    lst<<tr("  2. Неравномерность пропускания")<<QString::number(E0);

    float E8=1/E0;
    float X=E8+pow((E8*E8+1),0.5);
    float V4=(1/(J5+1))*log(X);
    float X1=log(E8+pow((E8*E8+1),0.5));
    float V5=(1/(J5+1))*X1;
    float G1=0.5*(exp(V5)-exp(-V5));
    float G2=0.5*(exp(V4)+exp(-V4));

    float R0[50],S4[50],O4[50];

    lst<<tr(" 3. Полюса")<<"Sk=SIN+j*COS";

    for (int R=1;R<N5;R++) {
      R0[R]=M_PI*(2*R-1)/(2*N5);
      S4[R]=-1*G1*sin(R0[R]);
      O4[R]=G2*cos(R0[R]);
      lst<<QString::number(S4[R]) + " + j*" + QString::number(O4[R]);
    }

    txtResult->setText(lst.join("\n"));

    return N5;
}

int FilterSintez::calcElliptic()
{
    float A1 = edtA1->text().toFloat();
    float A2 = edtA2->text().toFloat();
    float F1 = edtF1->text().toFloat();
    float F2 = edtF2->text().toFloat();
    Fc = F1;

    float W5=F2/F1;
    float K9=F1/F2;
    float K4=pow(10,(0.1*A1));
    float K1=pow((1-K9*K9),0.5);

    float J0=A2+12.16-20*log10(pow((K4-1),0.5));
    float J9=6.08+20*log10((1+pow(K1,0.5))/(1-pow(K1,0.5)));
    float J7=J0/J9;
    int N7=round(J7+1);

    QStringList lst;
    lst<<tr(" 1. Порядок  эллиптического фильтра")<<QString::number(N7);
    float E0=pow((K4-1),0.5);
    lst<<tr(" 2. Неравномерность пропускания")<<QString::number(E0);

    float E8=1/E0;
    float X=E8+pow((E8*E8+1),0.5);
    float V4=(1/(J7+1))*log(X);
    float X1=log(E8+pow((E8*E8+1),0.5));
    float V5=(1/(J7+1))*X1;
    float G1=0.5*(exp(V5)-exp(-V5));
    float G2=0.5*(exp(V4)+exp(-V4));
    float R0[50],S4[50],O4[50];

    lst<<tr(" 3. Полюса")<<"Sk=SIN+j*COS";

    for (int R=1;R<N7;R++)
            {
            R0[R]=M_PI*(2*R-1)/(2*N7);
            S4[R]=-1*G1*sin(R0[R]);
            O4[R]=G2*cos(R0[R]);
            lst<<QString::number(S4[R]) + " + j*" + QString::number(O4[R]);
    }

    txtResult->setText(lst.join("\n"));

    return N7;
}

void FilterSintez::calcDblQuadHPF()
{

}

void FilterSintez::calcDblQuadLPF()
{

}

void FilterSintez::calcMultiloopHPF()
{

}

void FilterSintez::calcMultiloopLPF()
{

}



void FilterSintez::calcPassive()
{

}
