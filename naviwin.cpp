#include "naviwin.h"
#include "dvapp.h"
naviWin::naviWin(QWidget *p) : arrayDraw(p)
{
   idxBuf = NULL;
   curBuf = NULL;
   pickBuf = NULL;
}
naviWin::~naviWin()
{
   if (idxBuf != NULL) delete idxBuf;
   if (curBuf != NULL) delete curBuf;
   if (pickBuf != NULL) delete pickBuf;


}
 
 
void naviWin::draw()
{
   //if (WIN == NULL) return; 
   //float *fbuf;
   int num, idx, i, j, num1;
   num =  DOC->m_data->getGathers();
   idx =  DOC->m_data->m_iCurIndex;
   if (num <=0 ) return;
   //qDebug() << "naviWin::draw()  num,idx = " << num << idx <<WIN;
   //qDebug() << "naviWin::draw() WIN->getView() = " << WIN->getView();
   //qDebug() << "naviWin::draw() WIN->getView() = " << DOC->m_data->pickData;
   num1 = DOC->m_data->pickData.size();

   qDebug() << "naviWin::draw()  num1 = " << num<< num1<< curBuf<<pickBuf;
   for (i = 0; i < num; i++)
   {
      curBuf[i] = 0;
      pickBuf[i] = 0;
      for (j = 0; j < num1; j++)
      {
         if (idxBuf[i] == DOC->m_data->pickData.keys()[j])
         {
             pickBuf[i] = idxBuf[i];
             break;
          }
      }
   }
   qDebug() << "idx = " << idx << DOC->m_data->m_iCurGather;
   curBuf[idx] = DOC->m_data->m_iCurGather;

   drawit();
   return;
   // make it displayed;
        if (WIN != NULL)
        {
            if (WIN->naviDockWidget->isVisible())
            {
                WIN->naviDockWidget->hide();
                WIN->naviDockWidget->show();
            }            
        }
        
   
  // setVisible(true);


}
void naviWin::mousePressEvent(QMouseEvent *e)
{
   arrayDraw::mousePressEvent(e);


   int x, y, idx;
   QString str;

   x = e->x();
   y = e->y();

   idx = mapToScene(x, y).x();
   //isam  = mapToScene(x, y).y(); //is the time
   str.sprintf("idx =%d,gather = %d", idx,DOC->m_data->m_listGather[idx]);
   //qDebug() << "idx = " << tr << isam;
   setBottomMessage(str);

}
void naviWin::mouseDoubleClickEvent(QMouseEvent *e)
{
   arrayDraw::mouseDoubleClickEvent(e);
   int x, y, sx;
   QString str;

   x = e->x();
   y = e->y();

   sx = mapToScene(x, y).x();
  // sy  = mapToScene(x, y).y(); //is the time
   //str.sprintf("%d,%d,%d,%d", x, y, sx, sy);
   qDebug() << "double click,sx = " << sx;
   emit sigMove(sx);

   //WIN->moveData(MOVE_INDEX,sx);
   //draw();
}
void naviWin::init()
{

   int num;
   QList<int> glist;


   num = DOC->m_data->getGathers();
   glist = DOC->m_data->getGatherList();
   qDebug() << "initVani: start : " << num;
   if (num <= 0)  return;// for tape
   int i;
   //float *f;
   float min, max;
   idxBuf = new float[num]; //first array
   curBuf = new float[num]; //fsecond array
   pickBuf = new float[num]; //third array
   max = -10;
   min = 10000;

   //qDebug() << "glist.size = " << glist.size();
   for (i = 0; i < num; i++)
   {
      idxBuf[i] = glist[i];
      curBuf[i] = 0;
      pickBuf[i] = 0;
      if (max < idxBuf[i]) max = idxBuf[i];
      if (min > idxBuf[i]) min = idxBuf[i];

   }
   //naviView->curBuf[1] = glist[1];
   //naviView->pickBuf[4] = glist[4];

   setMaxMin(0, min, num - 1, max);

   drawit();
}
void naviWin::drawit()
{
   int num;

   num = DOC->m_data->getGathers();
   QString key;
   key = DOC->m_data->getGather();
   key = NV_IDX;
   setColorBar(0);
   //qDebug() << "initVani: start draw";

   setGrid(5, 10, 5, 10);
   //naviView->setScale(5,5);
   setDrawStyle(2, 0, 1, 1, 7);
   setFitWin();
   setCoordinate(RIGHT_UP); // last set
   startDraw(idxBuf, num, key); //first array
   key = NV_PICK;
   setDrawStyle(2, 0, 1, 1, 2);
   startDraw(pickBuf, num, key); //first array
   key = NV_CUR;
   setDrawStyle(2, 0, 1, 1, 1);
   startDraw(curBuf, num, key); //first array
                                                    //qDebug() << "initVani: end";
}
