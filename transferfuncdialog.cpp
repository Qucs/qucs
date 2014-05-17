#include "transferfuncdialog.h"

TransferFuncDialog::TransferFuncDialog(QWidget *parent) :
    QDialog(parent)
{
    lblB = new QLabel(tr("Numenator b[i]="));
    lblA = new QLabel(tr("Denomenator a[i]="));

    QStringList indexes;
    for (int i=0;i<50;i++) {
        indexes<<QString::number(i);
    }

    tblA = new QTableWidget;
    QStringList head1;
    head1<<tr("a[i]");
    tblA->setColumnCount(head1.count());
    tblA->setRowCount(50);
    tblA->setHorizontalHeaderLabels(head1);
    tblA->setVerticalHeaderLabels(indexes);

    tblB = new QTableWidget;
    QStringList head2;
    head2<<tr("b[i]");
    tblB->setColumnCount(head2.count());
    tblB->setRowCount(50);
    tblB->setHorizontalHeaderLabels(head2);
    tblB->setVerticalHeaderLabels(indexes);

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

void TransferFuncDialog::getCoeffs(QVector<long double> &a, QVector<long double> &b)
{
    a.clear();
    b.clear();

    for (int i=0;i< tblA->rowCount();i++) {
        QTableWidgetItem *itm = tblA->item(i,0);
        if (itm!=0) {
            QString str = itm->text();
            if (str.isEmpty()) break;
            bool ok;
            long double n = (long double) str.toDouble(&ok);
            //qDebug()<<n;
            if (ok) a.append(n);
        }
    }



    for (int i=0;i< tblB->rowCount();i++) {
        QTableWidgetItem *itm = tblB->item(i,0);
        if (itm!=0) {
            QString str = itm->text();
            if (str.isEmpty()) break;
            bool ok;
            long double n = (long double) str.toDouble(&ok);
            //qDebug()<<n;
            if (ok) b.append(n);
        }
    }
}
