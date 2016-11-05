#include "paramdata.h"
#include "drawview.h"
  
paramData::paramData()
{
    init(); 

}
//-----------------
paramData::~paramData()
{

}
//----------------------------------
void paramData::init()
{
    gain = 0;
    rms = 5;
    scaleX = 3.77953;
    scaleY = 0.377953;
    posX = 0;
    posY = 0;
    drawType = DRAW_VD;
    compareMode = COMPARE_MODE_V;
    agc = 0;


}
void paramData::setParams(drawView *v)
{
    qDebug() << " setparam in view";
    
    v->setGain(gain);
    v->setRMS(10 - rms);
    v->setScale(scaleX,scaleY);
    v->setPos(posX,posY);
    v->setDrawType(drawType);
    v->setCompareMode(compareMode);
    v->setAGC(agc);
    qDebug() << "paramData::setParams in set agc=" << agc << gain << rms << scaleX<< scaleY;
    qDebug() << "paramData::setParams in set pos=" << posX << posY << drawType << compareMode;
    v->show();
    //v->reDraw();
    

}

void paramData::getParams(drawView *v)
{
    gain = v->getGain();
    rms = 10 - v->getRMS();
    v->getScale(scaleX,scaleY);
    v->getPos(posX,posY);
    drawType = v->getDrawType();
    compareMode = v->getCompareMode();
    agc = v->getAGC();
    //qDebug() << "in get x,y=" << scaleX << scaleY;
}



