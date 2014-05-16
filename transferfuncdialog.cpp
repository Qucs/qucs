#include "transferfuncdialog.h"

TransferFuncDialog::TransferFuncDialog(QWidget *parent) :
    QDialog(parent)
{
    lblB = new QLabel(tr("Numenator b[i]="));
    lblA = new QLabel(tr("Denomenator a[i]="));

    tblA = new QTableWidget;
    QStringList head1;
    head1<<tr("a[i]");
    tblA->setColumnCount(head1.count());
    tblA->setRowCount(50);
    tblA->setHorizontalHeaderLabels(head1);
    tblB = new QTableWidget;
    QStringList head2;
    head2<<tr("b[i]");
    tblB->setColumnCount(head2.count());
    tblB->setRowCount(50);
    tblB->setHorizontalHeaderLabels(head2);

    btnAccept = new QPushButton(tr("Accept"));
    connect(btnAccept,SIGNAL(clicked()),this,SLOT(accept()));
    btnCancel = new QPushButton(tr("Cancel"));
    connect(btnCancel,SIGNAL(clicked()),this,SLOT(reject()));

    low1 = new QVBoxLayout;
    low1->addWidget(lblB);
    low1->addWidget(tblB);

    low2 = new QVBoxLayout;
    low2->addWidget(lblA);
    low2->addWidget(tblA);

    low3 = new QHBoxLayout;
    low3->addWidget(btnAccept);
    low3->addWidget(btnCancel);

    top = new QHBoxLayout;
    top->addLayout(low1);
    top->addLayout(low2);

    top1 = new QVBoxLayout;
    top1->addLayout(top);
    top1->addLayout(low3);


    this->setLayout(top1);

}

void TransferFuncDialog::getCoeffs(QVector<float> &a, QVector<float> &b)
{
    a.clear();
    b.clear();

    for (int i=0;i<tblA->rowCount();i++) {
        QString str = tblA->item(i,0)->text();
        if (str.isEmpty()) break;
        a.append(str.toFloat());
    }

    for (int i=0;i<tblB->rowCount();i++) {
        QString str = tblB->item(i,0)->text();
        if (str.isEmpty()) break;
        b.append(str.toFloat());
    }
}
