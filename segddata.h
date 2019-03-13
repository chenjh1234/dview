#ifndef SEGDDATA_H 
#define SEGDDATA_H 
 
#include <QString>
#include <QMap>
#include <iostream>
#include "seisdata.h"
#include "QSegdRead.h"
//#define MAX_TRS_OF_GATHER 20000

/**
 * segd read class for seismic drawSection; 
 * header: ¡±HD1¡° 
 * 
 *  
 */
#define DHEADER_LEN 20
class segdData : public seisData
{
    Q_OBJECT
public:

    segdData();
    ~segdData();
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
    QSegdRead m_segd;

    int m_iMainHD;// gather =3
signals:
    void sigParamChanged();



};

#endif




