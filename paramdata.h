#ifndef PARAMDATA_H 
#define PARAMDATA_H 
 
#include <QString>
#include <QMap>
#include <QDebug>
#include <iostream>
#include <QWindow>
#include <QScreen>
class drawView;
class paramData  
{
public:
    paramData();
    ~paramData();
    void init();
    float scaleX,scaleY;
    int drawType;//DRAW_WA,DRAW_VD
    int posX,posY;
    float rms,gain;
    int compareMode;
    int agc;

    void getParams(drawView *v);
    void setParams(drawView *v);
  
};

#endif



