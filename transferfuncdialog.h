#ifndef TRANSFERFUNCDIALOG_H
#define TRANSFERFUNCDIALOG_H

#include <QWidget>
#include <QtGui>

class TransferFuncDialog : public QDialog
{
    Q_OBJECT

private:
    QLabel *lblA;
    QLabel *lblB;
    QLabel *imgTrfuncEq;
    QTableWidget *tblA;
    QTableWidget *tblB;

    QPushButton *btnAccept;
    QPushButton *btnCancel;

    QHBoxLayout *top;
    QVBoxLayout *top1;
    QVBoxLayout *low1;
    QVBoxLayout *low2;
    QHBoxLayout *low3;

public:
    explicit TransferFuncDialog(QVector<long double> &a, QVector<long double> &b, QWidget *parent = 0);

    void getCoeffs(QVector<long double> &a, QVector<long double> &b);
    
signals:
    
private slots:
    void slotCheckCoeffs();

public slots:
    
};

#endif // TRANSFERFUNCDIALOG_H
