
#ifndef NAVIWIN_H 
#define NAVIWIN_H 
 
#include <QWidget>
#include <QDebug>
#include <QFileDialog>
#include <QMouseEvent>

#include "seisdata.h"
#include "arraydraw.h"
#define NV_IDX "0idx"
#define NV_CUR "2cur"
#define NV_PICK "1pick"

using namespace std;

class naviWin :public  arrayDraw//QWidget
{
    Q_OBJECT

public:
    naviWin(QWidget *p);
    ~naviWin();
    void draw();
    void init();
 
    
private:
    float *idxBuf,*curBuf,*pickBuf;
    void drawit();

protected:
    //gdata *data;
    void mousePressEvent(QMouseEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *e);
public slots:
signals:
    void sigMove(int);

   // virtual bool edit(const QModelIndex &index, EditTrigger trigger, QEvent *event);
};



#endif


