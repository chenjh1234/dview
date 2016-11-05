#ifndef DVAPP_H
#define DVAPP_H

#include <QApplication>
#include "dvmainwin.h"
#include "dvdata.h"
#include <iostream>
class dvApp;
//class modMain;

#define DOC theApp->m_doc
#define WIN theApp->m_win
#define DRAWVIEW theApp->m_win->drawview;

using namespace std;
extern dvApp *theApp;
 
class dvApp :public QApplication  
{
public:
    dvApp( int & argc, char ** argv );
    ~dvApp();
    dvData *m_doc;
    dvMainWin *m_win;

private:

};
#endif

