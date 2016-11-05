#ifndef GRISYSDATA_H 
#define GRISYSDATA_H 
 
#include <QString>
#include <QMap>
#include <iostream>
#include "seisdata.h"
#include "gridataadi.h"



/**
 * grisys data class for seismic drawSection; 
 * header: ¡±HD1¡° 
 * 
 *  
 */
#define GHEADER_LEN 128
class grisysData : public seisData
{
    Q_OBJECT
public:

    grisysData();
    ~grisysData();
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

    //
    QString getHDStr(int i); 
    griData m_gri;

    int m_iMainHD;// gather =3
signals:
    void sigParamChanged();



};

#endif




