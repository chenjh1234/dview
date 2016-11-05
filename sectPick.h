#ifndef SECTPICK_H
#define SECTPICK_H
#include "drawing.h"
#include "sectdata.h"

#include "intpsub.h"
#include "colorbar.h"
/// paint the seismic data traces
/** discarded
   implement the paint number for sectDraw;
  */
class sectPick : public drawing
{
    //Q_OBJECT
public:
    sectPick();
    ~sectPick();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                 QWidget *widget);
    void setData(drawData *d);
    int plotPick(int iw, int ih, int sttr, int trs, int ssam, int sams);
    sectData *data;
    colorBar *cb;// we have cb both in draw and paint

private:

    /// interpolation class
    intpsub intp;
    /** set the image color table
    find the index of the color table ;do not reDraw
    */
    int setImageColor();
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

};

#endif // SECTPAINT_H

