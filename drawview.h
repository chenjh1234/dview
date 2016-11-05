#ifndef DRAWVIEW_H
#define DRAWVIEW_H

#include "sectdraw.h"
#include "seisdata.h"
#include "naviwin.h"
 
class drawView :public  sectDraw
{
    Q_OBJECT

public:
    drawView(QWidget *p=0);
    ~drawView();
    void init();
    QString m_dataName;
    QString diplayHD;
    float *comBuf;// data for compare;
    float *displayHeader;// displayHeader;
    int *comHeader;
    //sectPickk *m_pick;
    bool isCompareView();

    /// the data object ,we create it whenopen data file
    seisData *m_data;
    naviWin *nvView;

    seisData *getData();
    void setData(seisData *);
    //void reDraw();
    void setTitle();
    void showHeader(int idx);


signals:
    void sigClosed(drawView *v);

protected:

    
    void  closeEvent(QCloseEvent *event);
    void mousePressEvent ( QMouseEvent * e );
};
#endif
