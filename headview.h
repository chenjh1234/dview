
#ifndef HEADVIEW_H 
#define HEADVIEW_H 
 
#include <QDialog>
#include <QString>
#include <QDebug>
#include <QMouseEvent>
#include <QMap>
#include <QStandardItemModel>
#include "ui_headdisplay.h"
 
class headView :public QDialog
{
    Q_OBJECT

public:
    headView(QWidget *p);
    ~headView();
    void init();
    void setRows(int );
    void setRows(QMap<QString,double>);
  
protected:
private:
    
     
    Ui::Dialog ui;
    QStandardItemModel *model;
 
    int m_col;
public   slots:
    void setData(QMap <QString,double> map);

};
#endif
