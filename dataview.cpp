#include <iostream>
#include <QDebug>
#include <QtGlobal>
#include <Qt>
#include <QApplication>// if we add it qDebug() cannot work
#include <QDebug>
#include "dvapp.h"
#include "dvdata.h"
#include "dvmainwin.h"
#include "QSegyADI.h"
#include "grisysdata.h"
using namespace std;
 
void testgri()
{
    QString gfile;
    grisysData gd;
    gfile = "d:\\2016\\segb\\9003069.001gri";
    gd.openData(gfile,"r");
}
void testopen()
{
    QSegyADI sgy;
    QString gfile;
    int i;
    gfile = "s175.sgy";
    i = sgy.openReadFile(gfile);
   // qDebug() << "end0 = " << sgy.m_bIdxFile <<i ;
    sgy.th.wait();
    //sleep(2);
   // qDebug() << "end1 = " << sgy.m_bIdxFile;
}

int main(int argc, char *argv[])
{

#if 1
    //qInstallMessageHandler((QtMessageHandler)myMessageOutput);
    dvApp app(argc, argv);
    app.setOrganizationName(ORG_NAME);
    app.setApplicationName(APP_NAME);

    //testopen();

    dvData doc;
    app.m_doc = &doc;
    qDebug() << "doc = " << DOC->m_data << DOC->getData();
    qDebug() << "argc= " << argc;
    if (argc >=2) 
    {
        qDebug() << "arg1 = " << argv[1];
        doc.m_strArg1 = argv[1];
    }
    dvMainWin mainWin;
    app.m_win = &mainWin;
    //qDebug() << "win = " << app.m_win;
    mainWin.show();
    return app.exec();
   #endif 
 
} 
