#include "sectpick.h"
#include "sectdraw.h"
#include <QDebug>

sectPick::sectPick() : drawing()
{

}
sectPick::~sectPick()
{

}


void sectPick::paint(QPainter *p, const QStyleOptionGraphicsItem *op,
                      QWidget *w1)
{
   int ibit;
   int iw, ih, str, trs, ssam, sam, x0, y0;

   if (!data->m_bStart) return;
   //n = data->n;

   if (data->m_coordinate == RIGHT_UP) ibit = -1;
   else ibit = 1;

   iw = boundingRect().width() - 1;
   ih = boundingRect().height() - ibit;
// fixed scale:

   QRectF f;
   QPointF pf;

   f =   map2Scene(QRectF(0, 0, iw, ih));

   str = f.x();
   if (str < 0) str = 0;

   trs = f.width();
   if (trs > data->trs) // image is not full in horizon
   {
      trs = data->trs;
      //pf = scene2Item(trs, 0);// if trs ,the buf will exceed.
      pf = scene2Item(trs - 1, 0); // trs -1,ensure the buf is not exceed.
      iw = pf.x();
   }

   ssam = f.y(); //time:
   if (ssam < 0) ssam = 0;

   sam = f.height(); //time
                     //qDebug() << "ssam = " << ssam <<sam;
   if (sam > data->sam * data->si) // image is not full in vertical
   {
      sam = data->sam * data->si; //time
      pf = scene2Item(0, sam); //time
      ih = pf.y(); // *data->si;
   }

   /**
   core down if drawing area is nagative:
    */
   if (iw <= 0 || ih <= 0) return;

   plotPick(iw, ih, str, trs, ssam, sam);

}

void sectPick::mousePressEvent(QGraphicsSceneMouseEvent *e)
{
   //QGraphicsItem::mousePressEvent(e);
   int x,y;
//   x = e->pos().x();
 //  y = e->pos().y();
}

int  sectPick::setImageColor()
{
   int i;
   int len = BAR_COLORS;
   int idx, id;
   idx = 0;
   idx = cb->m_iIdx;
   //qDebug() << "idx = " << idx;
   //idx >0 all the time

   for (i = 0; i < len; i++)
   {
      id = i + idx;
      if (id >= len) id = id - len;

      //img->setColor(i,cb->m_color[idx].rgb());// gb->r
      //img->setColor(i, cb->m_color[id].rgb()); // gb->r

   }
   return 0;
}

int sectPick::plotPick(int iw, int ih, int sttr, int trs, int ssam, int sams)
{
   //infact :trs sam is not effective;

   int i, j;
//   unsigned char *imgdata;
   float *buf, fmax, fmin, startT, startS;
   int stsam, sam;
   QPointF pf;
// stsam,sam:
   stsam = ssam / data->si;
   sam = sams / data->si;
   if (trs <= 0) trs = 1;
//startT,startS :fraction of the data:
   pf = scene2Item(sttr + 1, stsam + 1); //start from the positon dot the trace1
   startT  = pf.x(); // adjest to the dot that not exactly on the  trace
   pf = map2Scene(0, 0); //time
   startS = (pf.y() / data->si - stsam);
   //startT = pf.x() - sttr; //VD: use this ,is exactly the dot
//buf:
   buf = data->buf;
   fmax = data->fmax;
   fmin = data->fmin;
   int bytes;
   //bytes = img->bytesPerLine();
   //imgdata = img->bits();

// step,hspd:
   float step;
   // for display trs ,give trs+1 space of traces interval dots
   if (trs > 0)
      // step = (float)iw /((float)(trs-1));
      //step = (float)iw / ((float)(trs));
       step = ((drawFrame * )view())->getScaleX();
   else step = iw;

// gain:
   float g, xx; //(x:0-10)
   xx = data->m_fGain;
   if (xx < 0) xx = 0;
   g = pow((qreal)2, (qreal)xx / GAIN_MAX);
// draw loop :

   float *x, *y;
   int *iy;

   unsigned char pix1;
   pix1 = 0x00;
   //plt.pltinit(iw, ih, bytes, imgdata, pix1);

   y = new float[ih + 10];
   iy = new int[ih + 10];

   float ii;
   i = 0;
   //for (i = 0,ii = startT; ii < iw ; ii += step, i++)
   for (i = 0,ii = startT; ii < iw && (sttr + i) <= data->trs -1; ii += step, i++)
   {
      x = buf + (sttr + i) * data->sam + stsam;
      intp.intpy((float *)x, (float *)y, sam, ih, startS); // interplate in vertical

// get the image data in vertical:
      for (j = 0; j < ih; j++)
      {
         iy[j] = (y[j] - fmin) * g / (fmax - fmin) * step;
      }
      //plt.pltwiggle(int(ii), 0, ih, iy);
      //plt.pltpa(int(ii), 0, ih, iy);
   }

   delete[] iy;
   delete[] y;

   return 0;
}

void sectPick::setData(drawData *d)
{
   data = (sectData *)d;
   drawing::data = d;
}

