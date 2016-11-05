#ifndef SEGYDATA_H 
#define SEGYDATA_H 
 
#include <QString>
#include <QMap>
#include <iostream>
#include "seisdata.h"
#include "QSegyADI.h"


/**
 * segd read class for seismic drawSection; 
 * header: ¡±HD1¡° 
 * 
 *  
 */
#define HEADER_LEN 60
class segyData : public seisData 
{
    Q_OBJECT

public:

    segyData();
    ~segyData();
    void init();
    int openData(QString name,QString mode);
    int readGather(int g,float *trace,char *header);
    int readGatherIndex(int idx,float *trace,char *header);
    int readPercent(int per,float *trace,char *header);
    int readPreGather(float *trace,char *header);
    int readNextGather(float *trace,char *header);
    int readFirstGather(float *trace,char *header);
    int readLastGather(float *trace,char *header);
    int closeData();
    void sortData(int idd);
    QMap<QString,double> mapHeader(int i);
    QMap<QString,double> mapHeader();
    QMap<QString,double> mapSuperHeader();
    void setDisplayHeader(QString hd);
    void setDisplayHeader();
    void setParam();
    signals:
    void sigParamChanged();
public slots:
    void slotUpdateParam();
public:
    QString getHDStr(int i); 
    QSegyADI m_segy;

    int m_iMainHD;// gather =3



};

#endif




