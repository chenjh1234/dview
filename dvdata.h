#ifndef DVDATA_H 
#define DVDATA_H 
 
#include <QString>
#include <QMap>
#include <QDebug>
#include <iostream>
#include <QModelIndex>
#include <QWindow>
#include <QScreen>
#define ORG_NAME "DATATOOL"
#define APP_NAME "dataView"
/// 1.00: 2015.7:
/// 1.01: 2016.3: add GRISYS data,recentfle,move pick data to seisData,so we can support more files pick parallel
///               update naviWin, to support more files.
/// 1.02: 2016.10 add segd format 
/// 1.03: config the segd,segy,grisys,pick compare,
/// 1.04: not complete:  make tape segy:
/// 1.05: add cmd line parameter
#define APP_VERSION "1.03"
#define APP_DATE "2016.10"

#include "seisdata.h"
#include "paramdata.h"
//#define PICK_NO 0
//#define PICK_LAYER 1
//#define PICK_MUTE 2


class dvData  
{
public:
    dvData();
    ~dvData();
    void init();
 //current active data,and view

    seisData* getData();
    void setData(seisData *d);
// all the view share the same display param Data created by mainWin;
    paramData* getParam();
    void setParam(paramData *d);

// current dataname and type
    QString m_dataName;
    QString m_failedFile;// last renent file failed to open
    QString m_strArg1;// cmdline parameter
    int m_dataType;

    seisData *m_data;
    paramData *m_param;

    int m_iSortData;
    int m_iTapeSkip;
#if 0 // move it to drawView;

/// get the pick time from other knee gather 
    QMap<int, float> pickIntpTimes;

/** all pick gather of the data store here
 *  int :gather
 *  PICKPOINTS: pick points in the gather
 */
    typedef  QMap<int, float> PICKPOINTS;
    QMap<int, PICKPOINTS > pickData;
    QString pickHD;
#endif 
    float linear(float x1, float y1, float x2, float y2, float x)
    {
        float y;
        if( x2 == x1 ) return 0;
        y =  (x - x1) * (y2 - y1) / (x2 - x1) + y1;
        return y;
    };
 
};

#endif


