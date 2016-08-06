#include "ui.h"

ui::ui()
{
      GNUplot_path = QCoreApplication::applicationDirPath() + "/GRABIM.dat";
      setFixedSize(400, 300);
      centralWidget =  new QWidget();
      QHBoxLayout *Impedancelayout = new QHBoxLayout();
      QHBoxLayout * ButtonsLayout = new QHBoxLayout();
      QGridLayout * TopoLayout = new QGridLayout();

      //Create source load impedance buttons
      SourceFileButton = new QPushButton("Source impedance");
      LoadFileButton = new QPushButton("Load impedance");
      connect(SourceFileButton, SIGNAL(clicked()), this, SLOT(SourceImpedance_clicked()));
      connect(LoadFileButton, SIGNAL(clicked()), this, SLOT(LoadImpedance_clicked()));


      QVBoxLayout * SourceLayout = new QVBoxLayout();
      QVBoxLayout * LoadLayout = new QVBoxLayout();

      QHBoxLayout * ConstantZSLayout = new QHBoxLayout();
      QHBoxLayout * ConstantZLLayout = new QHBoxLayout();

      FixedZSLineedit = new QLineEdit("50");
      FixedZLLineedit = new QLineEdit("50");
      ZSOhmLabel = new QLabel("Ohm");
      ZLOhmLabel = new QLabel("Ohm");

      ZSOhmLabel->setVisible(false);
      ZLOhmLabel->setVisible(false);

      FixedZSLineedit->setVisible(false);
      SourceFileButton->setVisible(true);

      FixedZLLineedit->setVisible(false);
      LoadFileButton->setVisible(true);

      ConstantZSLayout->addWidget(FixedZSLineedit);
      ConstantZSLayout->addWidget(ZSOhmLabel);

      ConstantZLLayout->addWidget(FixedZLLineedit);
      ConstantZLLayout->addWidget(ZLOhmLabel);

      SourceLayout->addWidget(SourceFileButton);
      SourceLayout->addLayout(ConstantZSLayout);

      LoadLayout->addWidget(LoadFileButton);
      LoadLayout->addLayout(ConstantZLLayout);

      //Checkboxes for selecting a constant impedance vs freq profile
      FixedZSCheckbox = new QCheckBox("Constant ZS");
      FixedZLCheckbox = new QCheckBox("Constant ZL");
      connect(FixedZSCheckbox, SIGNAL(clicked()), this, SLOT(FixedZSCheckbox_clicked()));
      connect(FixedZLCheckbox, SIGNAL(clicked()), this, SLOT(FixedZLCheckbox_clicked()));

      SourceLayout->addWidget(FixedZSCheckbox);
      LoadLayout->addWidget(FixedZLCheckbox);

      // Image
       QSize sz;
       QString s1 = ":/bitmaps/MatchingNetwork.svg";
       imgWidget = new QSvgWidget(s1);
       sz = imgWidget->size();
       imgWidget->setFixedSize(.2*sz);


      Impedancelayout->addLayout(SourceLayout);
      Impedancelayout->addWidget(imgWidget);
      Impedancelayout->addLayout(LoadLayout);


       // Matching band
       QGroupBox *FreqgroupBox = new QGroupBox(tr("Matching band"));
       minFLabel = new QLabel("Min:");
       minFEdit = new QLineEdit("1");
       minFUnitsCombo =  new QComboBox();
       maxFLabel = new QLabel("Max:");
       maxFEdit = new QLineEdit("2");
       maxFUnitsCombo =  new QComboBox();


       // Fill combos
       minFUnitsCombo->addItem("kHz");
       minFUnitsCombo->addItem("MHz");
       minFUnitsCombo->addItem("GHz");
       minFUnitsCombo->setCurrentIndex(2);
       minFUnitsCombo->setMinimumContentsLength(5);

       maxFUnitsCombo->addItem("kHz");
       maxFUnitsCombo->addItem("MHz");
       maxFUnitsCombo->addItem("GHz");
       maxFUnitsCombo->setCurrentIndex(2);
       maxFUnitsCombo->setMinimumContentsLength(5);

       QHBoxLayout *vbox = new QHBoxLayout;
       vbox->addWidget(minFLabel);
       vbox->addWidget(minFEdit);
       vbox->addWidget(minFUnitsCombo);

       vbox->addWidget(maxFLabel);
       vbox->addWidget(maxFEdit);
       vbox->addWidget(maxFUnitsCombo);
       FreqgroupBox->setLayout(vbox);

       //Topology selection
       SearchModeLabel = new QLabel("Search Mode");

       SearchModeCombo = new QComboBox();
       SearchModeCombo->insertItem(0,"Precomputed set");
       SearchModeCombo->insertItem(1,"Script");
       SearchModeCombo->insertItem(2,"LC order 4");
       SearchModeCombo->insertItem(3,"LC + TL order 6");
       SearchModeCombo->insertItem(4,"LC + TL + Stubs order 6");

       TopoLayout->addWidget(SearchModeLabel, 0, 0);
       TopoLayout->addWidget(SearchModeCombo, 0, 1);

        // GNUPLOT
       QGridLayout *GNUplotLayout = new QGridLayout;
       GNUplotButton = new QPushButton("Browse");
       connect(GNUplotButton, SIGNAL(clicked()), this, SLOT(GNUplotOutput_clicked()));

       GNUplotLayout->addWidget(new QLabel("Save GNUplot data:"), 0, 0);
       GNUplotLayout->addWidget(GNUplotButton, 0, 1);



       QHBoxLayout * TopoScriptLayout =  new QHBoxLayout();
       TopoScriptButton =  new QPushButton("Browse");
       TopoScriptLabel = new QLabel("Topology script:");
       TopoScriptLayout->addWidget(TopoScriptLabel);
       TopoScriptLayout->addWidget(TopoScriptButton);
       connect(TopoScriptButton, SIGNAL(clicked()), this, SLOT(TopoScriptButton_clicked()));
       TopoScriptLabel->setVisible(false);
       TopoScriptButton->setVisible(false);

       TopoScript_path = "predefined_topologies";

      //Create go/cancel buttons
      RunButton = new QPushButton("Go");
      CancelButton = new QPushButton("Cancel");
      ButtonsLayout->addWidget(RunButton);
      ButtonsLayout->addWidget(CancelButton);

      connect(RunButton, SIGNAL(clicked()), this, SLOT(go_clicked()));
      connect(CancelButton, SIGNAL(clicked()), this, SLOT(cancel_clicked()));


      //Create main layout and add individual layouts
      QVBoxLayout * mainLayout = new QVBoxLayout();
      mainLayout->addLayout(Impedancelayout);
      mainLayout->addWidget(FreqgroupBox);
      mainLayout->addLayout(TopoLayout);
      mainLayout->addLayout(GNUplotLayout);
      mainLayout->addLayout(TopoScriptLayout);
      mainLayout->addLayout(ButtonsLayout);

      centralWidget->setLayout(mainLayout);
      setCentralWidget(centralWidget);
      setWindowTitle("Qucs wideband matching tool " PACKAGE_VERSION);

      statusBar()->showMessage(tr("Ready"));
}


void ui::go_clicked()
{

    //Before starting the matching engine, we must ensure that the impedance data is already loaded
    if (SourceFile.isEmpty() && (FixedZSLineedit->text().isEmpty()))
    {
        QMessageBox::warning(0, QObject::tr("Error"),
                             QObject::tr("Please select a Touchstone file for the source"));
        return;
    }

    if (LoadFile.isEmpty()&& (FixedZLLineedit->text().isEmpty()))
    {
        QMessageBox::warning(0, QObject::tr("Error"),
                             QObject::tr("Please select a Touchstone file for the load"));
        return;
    }

    //Check filename extension
    int formatSource=-1, formatLoad=-1;;
    if (SourceFile.contains(".s1p")) formatSource = 0;
    if (LoadFile.contains(".s1p")) formatLoad = 0;


    if ((formatSource != 0) && (!FixedZSCheckbox->isChecked()))
    {
        QMessageBox::warning(0, QObject::tr("Error"),
                             QObject::tr("Unknown source impedace"));
        return;
    }

    if ((formatLoad != 0)  && (!FixedZLCheckbox->isChecked()))
    {
        QMessageBox::warning(0, QObject::tr("Error"),
                             QObject::tr("Unknown load impedance"));
        return;
    }

    //Impedance data paths were already specified, let's proceed to bring the S-par data into memory
    IO * inout_operations = new IO();

    if (!FixedZSCheckbox->isChecked())//Read source impedance from file
    {
        inout_operations->loadS1Pdata(SourceFile.toStdString(), SOURCE);//s1p
    }
    else//Set constant source impedance
    {
       complex<double> zs_temp;
       QByteArray ba = FixedZSLineedit->text().toLatin1();
       char * text = ba.data();
       zs_temp = getComplexImpedanceFromText(text);

       if (real(zs_temp) == -1)//Check if the input value is correct
       {
           QMessageBox::warning(0, QObject::tr("Error"),
                                QObject::tr("The input given for the source impedance is not valid"));
           return;
       }
       inout_operations->set_constant_ZS_vs_freq(zs_temp);
    }

    if (!FixedZLCheckbox->isChecked())
    {
       inout_operations->loadS1Pdata(LoadFile.toStdString(), LOAD);//s1p
    }
    else
    {
        complex<double> zl_temp;
        QByteArray ba = FixedZLLineedit->text().toLatin1();
        char * text = ba.data();
        zl_temp = getComplexImpedanceFromText(text);

        if (zl_temp.real() == -1)//Check if the input value is correct
        {
            QMessageBox::warning(0, QObject::tr("Error"),
                                 QObject::tr("The input given for the load impedance is not valid"));
            return;
        }
        inout_operations->set_constant_ZL_vs_freq(zl_temp);

    }

    //Check frequency specifications
    double fmatching_min = minFEdit->text().toDouble();
    double fmatching_max = maxFEdit->text().toDouble();

    if ((fmatching_min == -1) || (fmatching_max == -1))
    {
        QMessageBox::warning(0, QObject::tr("Error"),
                             QObject::tr("Incorrect frequency settings"));
        return;
    }
    else//Everything correct... lets set frequency
    {
        //Scale frequency according to the combobox units
        fmatching_min *= getFreqScale(minFUnitsCombo->currentIndex());
        fmatching_max *= getFreqScale(maxFUnitsCombo->currentIndex());
        inout_operations->set_matching_band(fmatching_min, fmatching_max);
        if (fmatching_min < min(inout_operations->getFrequency())-1e6)//The lower freq is not present at s1p/s2p
        {
            QMessageBox::warning(0, QObject::tr("Error"),
                                 QObject::tr("One of the impedance data files does not contain the specified lower frequency"));
            return;
        }
        if (fmatching_max > max(inout_operations->getFrequency())+1e6)//The maximum freq is not present at s1p/s2p
        {
            QMessageBox::warning(0, QObject::tr("Error"),
                                 QObject::tr("One of the impedance data files does not contain the specified upper frequency"));
            return;
        }
        //Check if the specified frequencies lie with the s1p/s2p data
        inout_operations->ResampleImpedances();//Force data update
    }

    GRABIM * MatchingObject = new GRABIM();
    // Impedance and frequency settings
    MatchingObject->SetSourceImpedance(inout_operations->getSourceImpedance());
    MatchingObject->SetLoadImpedance(inout_operations->getLoadImpedance());
    MatchingObject->SetFrequency(inout_operations->getFrequency());
    MatchingObject->setTopoScript(TopoScript_path.toStdString());

    MatchingObject->setSearchMode(SearchModeCombo->currentIndex());


    GRABIM_Result R = MatchingObject->RunGRABIM();//Runs GRABIM. Well, this is not exactly the algorithm

    // detailed at [1] but from my point of view is functional and easier to code...
    //Notes:
    // 1) The candidate vector is not in log units. I do not see a good reason for doing so. Maybe I am missing something important
    // 2) Frequency is not in rad/s.
    // 3) The objective function is the magnitude of S11 expressed in dB. log(x) functions usually have strong
    // gradients so it seem to suggest that this is good for derivative free opt algorithms
    // 4) This code takes advantage from NLopt derivative-free local optimisers. NLopt is easy to link and it works
    // fine. Despite the fact that the Nelder-Mead algorithm does not guarantee convergence (among other problems), it leads to achieve a good local (probably, global) optimum. NM is known to fail when the dimmension of the problem is above 14-15. However, as far as matching networks are concerned, the lower order, the simpler the tuning. In practice it is not usual to find matching networks with more than 6 elements

    (FixedZSCheckbox->isChecked()) ? R.source_path = "" : R.source_path = SourceFile.toStdString();
    (FixedZLCheckbox->isChecked()) ? R.load_path = "": R.load_path = LoadFile.toStdString();
    R.QucsVersion = PACKAGE_VERSION;

//Final mesage

    QMessageBox::information(0, QObject::tr("Finished"),
                         QObject::tr("GRABIM has successfully finished. \nA schematic has been copied to the clipboard so you can paste it into Qucs\nAlternatively, you can check the performance of the network using GNUplot"));

    inout_operations->exportGNUplot(R, GNUplot_path.toStdString());
    inout_operations->ExportQucsSchematic(R, "");
    delete MatchingObject;
    delete inout_operations;
}

// Exits app
void ui::cancel_clicked()
{
   this->close();
}

// Opens a file dialog to select the s1p file which contains source impedance data
void ui::SourceImpedance_clicked()
{
    SourceFile = QFileDialog::getOpenFileName(this,
        tr("Open S-parameter file"), QCoreApplication::applicationDirPath());
}

// Opens a file dialog to select the s1p file which contains load impedance data
void ui::LoadImpedance_clicked()
{
    LoadFile = QFileDialog::getOpenFileName(this,
        tr("Open S-parameter file"), QCoreApplication::applicationDirPath());
}

// Apart from generating a Qucs schematic, the S parameters of the network are stored in GNUplot format
// so the user can check the performance of the network without running Qucs
void ui::GNUplotOutput_clicked()
{
    GNUplot_path = QFileDialog::getSaveFileName(this,
        tr("Save GNUplot script"), QCoreApplication::applicationDirPath());
}

// The user can feed the engine with custom topologies. In this sense, this function opens a file dialog for setting the path to
// a list of user-defined topologies
void ui::TopoScriptButton_clicked()
{
    TopoScript_path = QFileDialog::getOpenFileName(this,
        tr("Open script"), QCoreApplication::applicationDirPath());
}

// Shows hides button/lineedit for source impedance selection
void ui::FixedZSCheckbox_clicked()
{
    if (FixedZSCheckbox->isChecked())
    {
        FixedZSLineedit->setVisible(true);
        ZSOhmLabel->setVisible(true);
        SourceFileButton->setVisible(false);
    }
    else
    {
        FixedZSLineedit->setVisible(false);
        ZSOhmLabel->setVisible(false);
        SourceFileButton->setVisible(true);
    }
}

// Shows hides button/lineedit for load impedance selection
void ui::FixedZLCheckbox_clicked()
{
    if (FixedZLCheckbox->isChecked())
    {
        FixedZLLineedit->setVisible(true);
        ZLOhmLabel->setVisible(true);
        LoadFileButton->setVisible(false);
    }
    else
    {
        FixedZLLineedit->setVisible(false);
        ZLOhmLabel->setVisible(false);
        LoadFileButton->setVisible(true);
    }
}

// The user can also specify a constant complex impedance vs frequency. This function parses user's input
// and return the cx_double value
complex<double> ui::getComplexImpedanceFromText(char *arg)
{
    string arg_str = arg;
    int index = arg_str.find_first_of("j");
    int sign = 1;
    double zreal, zimag;
    if (index != -1)//Then it is a complex impedance
    {
        zreal = atof(arg_str.substr(0, index-1).c_str());
        if (!arg_str.substr(index-1, 1).compare("-")) sign = -1;
        zimag = sign*atof(arg_str.substr(index+1).c_str());
        return complex<double>(zreal, zimag);
    }
    else//Real impedance
    {
        zreal = atof(arg);
        if (zreal > 0)return complex<double>(zreal, 0);
        else//Wrong input...
            return complex<double>(-1, -1);
    }
}

// Gets frequency scale from the combobox index
double ui::getFreqScale(int index)
{
   switch (index)
   {
      case 0: return 1e3;break;
      case 1: return 1e6;break;
      case 2: return 1e9;break;

   }
   return 1e6;
}

// Returns the path to the user-defined topologies list
QString ui::getTopoScriptPath()
{
    return TopoScript_path;
}

// Search mode changed
void ui::TopoCombo_clicked(int index)
{
   if (index==0)
   {
     TopoScriptLabel->setVisible(true);
     TopoScriptButton->setVisible(true);
   }
   else
   {
     TopoScriptLabel->setVisible(false);
     TopoScriptButton->setVisible(false);
   }
}
