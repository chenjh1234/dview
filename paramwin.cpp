#include "paramwin.h"
#include "dvapp.h"

#include <QMessageBox>
#include <QtGlobal>
#include <QList>

paramWin::paramWin(QWidget *p):QtTreePropertyBrowser(p)//QWidget(p)
{
    init();
}
//=======================
paramWin::~paramWin()
{

}

//=======================
void paramWin::init()
{  
 

    m_mngE = new QtEnumPropertyManager();
    m_mng = new QtVariantPropertyManager();
    
    m_fct = new QtVariantEditorFactory();
    m_fctE = new QtEnumEditorFactory();
    m_fctPop = new QtPopupEditorFactory();

    setFactoryForManager(m_mng, m_fct); 
    setFactoryForManager(m_mngE, m_fctE);

    createProperty();
}
int paramWin::createProperty()
{
    QtProperty *pt0;
    pt0 =NULL;
    QString str;
    QStringList slist;
    QVariant vv;
    int idx;
// displayHD:
    str = "displayHD";
    slist << "aa" << "bb" << "cc";
    idx = 0;
    pt0 = m_mngE->addProperty(str); 
    m_mngE->setEnumNames(pt0, slist);
    m_mngE->setValue(pt0,idx);
    pt0->setStatusTip(str);
    addProperty(pt0);
// timeLine:
    str = "timeLine";
    slist.clear();
    slist << "no" << "yes" ;
    idx = 0;
    pt0 = m_mngE->addProperty(str); 
    m_mngE->setEnumNames(pt0, slist);
    m_mngE->setValue(pt0,idx);
    str = "timeLine display or not";
    pt0->setStatusTip(str);
    addProperty(pt0);
// pick to peak:
    str = "pickTo";
    slist.clear();
    slist << "No" << "peak" << "valley"<<"cross";
    idx = 0;
    pt0 = m_mngE->addProperty(str); 
    m_mngE->setEnumNames(pt0, slist);
    m_mngE->setValue(pt0,idx);
    str = "pick time adjust to :pick¡¢peak¡¢lley¡¢cross";
    pt0->setStatusTip(str);
    addProperty(pt0);
// sortData:
    str = "sortData";
    slist.clear();
    slist << "No" << "sortAs" << "sortDes";
    idx = 0;
    pt0 = m_mngE->addProperty(str); 
    m_mngE->setEnumNames(pt0, slist);
    m_mngE->setValue(pt0,idx);
    str = "sort data according to displayHD";
    pt0->setStatusTip(str);
    addProperty(pt0);
// label:
    str = "label";
    pt0 = m_mng->addProperty(QVariant::String, str); 
    vv = "5,10,50,100";
    m_mng->setValue(pt0,vv.toString());
    str = "Label gridLine x1,x2,y1,y2";
    pt0->setStatusTip(str);
    addProperty(pt0);
// AGC:
    str = "AGC";
    pt0 = m_mng->addProperty(QVariant::String, str); 
    vv = "0";
    m_mng->setValue(pt0,vv.toString());
    str = "AGC window in ms,0: No AGCapplyed";
    pt0->setStatusTip(str);
    addProperty(pt0);
// filter:
    str = "filter";
    pt0 = m_mng->addProperty(QVariant::String, str); 
    vv = "0,0,0,0";
    m_mng->setValue(pt0,vv.toString());
    str = "Filter: f1,f2,f3,f4.f1,f2:high pass,f3,f4:low pass,0:no filter applyed";
    pt0->setStatusTip(str);
    addProperty(pt0);
 
    
    connect(m_mng,SIGNAL(propertyChanged( QtProperty *)),
                         parent(),SLOT(slotParamChanged(QtProperty *)));
    
    connect(m_mngE,SIGNAL(propertyChanged( QtProperty *)),
                         parent(),SLOT(slotParamChanged(QtProperty *)));
 return 0;

#if 0
// option:
    slist << "aa" << "bb" << "cc";
    str = "Option";
    idx = 0;
    pt0 = m_mngE->addProperty(str); 
    m_mngE->setEnumNames(pt0, slist);
    m_mngE->setValue(pt0,idx);
    pt0->setEnabled(true);
    pt0->setStatusTip(str.toUtf8().data());
    addProperty(pt0);
     

// text dialog
    str = "dlg";
    pathId = m_mng->pathTypeId();
    pt0 = m_mng->addProperty(pathId, str);
    connect(m_mng, SIGNAL(openPathTriggered(QtVariantProperty * , QtVariantPropertyManager * )), 
            this, SLOT(slotOpenFile(QtVariantProperty* , QtVariantPropertyManager * )));
    pt0->setStatusTip(str.toUtf8().data());
    m_mng->setValue(pt0, vv.toString());
    pt0->setEnabled(true);
    addProperty(pt0);

// single param:
    str = "teststr";
    double v;
    v = 100;
    pt0 = m_mng->addProperty(QVariant::Double, str); 
    m_mng->setValue(pt0, v);
    pt0->setEnabled(true);
    pt0->setStatusTip(str.toUtf8().data());
    addProperty(pt0);
   
    connect(m_mng,SIGNAL(propertyChanged( QtProperty *)),
                         this,SLOT(slotModify(QtProperty *)));
    connect(m_mngE,SIGNAL(propertyChanged( QtProperty *)),
                         this,SLOT(slotModify(QtProperty *))); 
#endif     
//getPropertyV("teststr");

}
void paramWin::setPropertyV(QString name, QVariant v)
{
    QtProperty *pt;
    pt = getProperty(name);
    if(pt != NULL)
    {
        ((QtVariantPropertyManager *)pt->propertyManager())->setValue(pt, v);
    }

}
void paramWin::setPropertyEV(QString name, int v,QStringList slist)
{
    QtProperty *pt;
    pt = getProperty(name);
    if(pt != NULL)
    {
        ((QtEnumPropertyManager *)pt->propertyManager())->setEnumNames(pt, slist);
        ((QtEnumPropertyManager *)pt->propertyManager())->setValue(pt, v);
    }

}
void paramWin::setPropertyEV(QString name, int v)
{
    QtProperty *pt;
    pt = getProperty(name);
    if(pt != NULL)
    {
        //((QtEnumPropertyManager *)pt->propertyManager())->setEnumNames(pt, slist);
        ((QtEnumPropertyManager *)pt->propertyManager())->setValue(pt, v);
    }

}
QVariant paramWin::getPropertyV(QString name)
{
    QtProperty *pt;
    pt = getProperty(name);
    if(pt !=NULL)
    {
        //qDebug() << "custom= " << pt->customValue();
        //qDebug() << "text= " << pt->valueText();
        return pt->valueText();
    }
    else return "";


}
QtProperty * paramWin::getProperty(QString name)
{
    QList<QtProperty *> plist;
    QtProperty *pt;
    plist = properties();
    for(int i = 0; i < plist.size();i++)
    {
        pt = plist[i];
        if(pt->propertyName() == name)
        {
            return pt;
        }
    }
    return NULL;
}


 
void paramWin::slotOpenFile(QtVariantProperty *property, QtVariantPropertyManager *thisManager)
{
    QString fileName,path;
    //bool bsave;
    path = "";
    fileName = QFileDialog::getOpenFileName(this, 
             tr("Open File"), path, tr("data Files (*.*)"));

    property->setValue(fileName); 
}

void paramWin::slotModify(QtProperty *pro  )
{
    qDebug() << "modifyed enter " ;
    //if(theApp->m_doc->m_loadFile) return;

    //theApp->m_win->setWindowModified(true);//modify
    qDebug() << "modifyed = " << pro->propertyName() ;
}



