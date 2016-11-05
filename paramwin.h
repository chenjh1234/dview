
#ifndef PARAMWIN_H 
#define PARAMWIN_H 
 
#include <QWidget>
#include <QDebug>
#include <QFileDialog>
#include "qttreepropertybrowser.h"
#include "qtvariantproperty.h"
#include "qteditorfactory.h"
#include "seisdata.h"

using namespace std;

class paramWin :public QtTreePropertyBrowser//QWidget
{
    Q_OBJECT

public:
    paramWin(QWidget *p);
    ~paramWin();

    int createProperty();
    void setPropertyV(QString name,QVariant v);
    void setPropertyEV(QString name,int v,QStringList slist);
    void setPropertyEV(QString name,int v);
    QVariant getPropertyV(QString name);
    QtProperty * getProperty(QString name);

    QtVariantPropertyManager * m_mng;   
    QtEnumPropertyManager *m_mngE;

    
    

private:
    void init();

    QtVariantEditorFactory * m_fct;   
    QtEnumEditorFactory *m_fctE;
    QtPopupEditorFactory *m_fctPop;
 
protected:
    //gdata *data;
public slots:
   // void slotParam(QString s);
   // void slotShowParam(QString s);
   // 
   void slotOpenFile(QtVariantProperty *property, QtVariantPropertyManager *thisManager);
   void slotModify(QtProperty *  );

   // virtual bool edit(const QModelIndex &index, EditTrigger trigger, QEvent *event);
};



#endif

