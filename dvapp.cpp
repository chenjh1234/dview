#include "dvapp.h"
dvApp *theApp;

dvApp::dvApp( int & argc, char ** argv ):QApplication(argc,argv)
{
    theApp = this;
    m_win = NULL;
    m_doc = NULL;

}
dvApp::~dvApp()
{
}
