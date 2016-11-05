
#include "drawview.h"
#include "dvapp.h"

drawView::drawView(QWidget *p):sectDraw(p)
{
    init();
}
drawView::~drawView()
{
    qDebug() << "close draw0";
    if(comBuf != NULL)
        delete comBuf;
    if(comHeader != NULL)
        delete comHeader;
    if(displayHeader != NULL)
        delete displayHeader;
    if(nvView != NULL)
        delete nvView;
    if(m_data != NULL)
    {
        delete m_data;
        //qDebug() << "delete down in deconstruct of view";
    }
    qDebug() << "close draw1";
}

void drawView::init()
{
    m_data = NULL;
    comBuf = NULL;
    comHeader = NULL;
    displayHeader = NULL;
    nvView = NULL;
 //pick:
  
}
void drawView::setData(seisData *d)
{
    if(d != NULL)
        m_dataName = d->m_dataName;
    m_data = d;
    setTitle();
}
seisData * drawView::getData()
{
    return m_data;
}

bool drawView::isCompareView()
{
    return (m_data == NULL);
}
void drawView::setTitle()
{
     
    QString str;
#if 0 // m_dataName in compare view alredy set;
    if(isCompareView())
    {
        str = "Compare " +DOC->m_dataName;
    }
    else str = m_dataName;
#endif
    str = m_dataName;
    setWindowTitle(str);
    //qDebug() << "title = " << str;
}
 
void drawView::closeEvent(QCloseEvent *event)
{
    //if(getData() != NULL)
    //{
        //delete m_data;
    //}
    emit sigClosed(this);
}
void drawView::mousePressEvent(QMouseEvent *e)
{
   sectDraw::mousePressEvent(e);
   int x, y, tr;
   QString str;
   x = e->x();
   y = e->y();

   tr = mapToScene(x, y).x();
   showHeader(tr);
    
}
void drawView::showHeader(int idx)
{
    if(m_data == NULL) return; 
    QMap<QString, double> map;
    map = m_data->mapHeader(idx);
    //qDebug() << "header 3,4=" << map["HD2"] << map["HD3"] << map["HD4"]<< map["HD5"];
}
