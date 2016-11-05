#ifndef SEISDATA_H 
#define SEISDATA_H 
 
#include <QString>
#include <QObject>
#include <QMap>
#include <QDebug>
#include <iostream>
// data type
#define SEGY_DATA 0
#define GRISYS_DATA 1
#define SEGD_DATA 2
// dev Type: defined in QTapeIO
// 
// gather
#define GATHER_LINE "line"
#define GATHER_SHOT "shot"
#define GATHER_CMP "cmp"
// sort
#define SORT_NO 0
#define SORT_AS 1
#define SORT_DES 2

#define MAX_GATHER_TRS 5000;
 
class seisData : public QObject
{
    Q_OBJECT
public:
    seisData():QObject(){};
//    ~seisData(){};
    virtual int openData(QString name,QString mode)=0;
    virtual int readPreGather(float *trace,char *header)=0;
    virtual int readNextGather(float *trace,char *header)=0;
    virtual int readGatherIndex(int index,float *trace,char *header)=0;
    virtual int readGather(int g,float *trace,char *header)=0;//return number of gather
    virtual int readPercent(int p,float *trace,char *header)=0;
    virtual int readFirstGather(float *trace,char *header)=0;//return number of gather
    virtual int readLastGather(float *trace,char *header)=0;//return number of gather
    virtual int closeData()=0;
    virtual QMap<QString,double> mapHeader(int i) = 0;
    virtual QMap<QString,double> mapHeader() = 0;
    virtual QMap<QString,double> mapSuperHeader() = 0;
    virtual void setDisplayHeader(QString hd)=0;
    virtual void setDisplayHeader()=0;
    virtual void sortData(int ){};  
    virtual void setParam(){};
    public slots:
    virtual void slotUpdateParam(){};
public:
    float* getDisplayHeader()
    {
        return displayHeader;
    };
    QString  getDisplayHD()
    {
        return displayHD;
    };
    void setDisplayHD(QString hd)
    {
        //qDebug() << "2222";
        displayHD = hd;
        //qDebug() << "222222299";
    };


// set in open:
    void setSI(float sii) {si = sii;};
    void setSI(int sii){si = sii;}; 
    float getSI() { return si;};

    void setLTR(float ltrr) { ltr = ltrr;};
    void setLTR(int sii){ltr = sii;}; 
    float getLTR() { return ltr;};

    int getSamples() { return (int)ltr/si;};

    void setGather(QString g) { m_gather = g;};//which gather in string
    QString getGather() { return m_gather;};

    void setGathers(int g){m_gathers = g;}; // allgathers
    int getGathers(){return m_gathers;};

    void setTraces(int g){m_traces = g;}; //alltraces;
    int getTraces(){return m_traces;};

    void setHeadLen(int g){m_headLen = g;};  
    int getHeadLen(){return m_headLen;};// length of int;
// set in read gather:

    void setCurTrs(int g){m_iCurTrs = g;}; //  trs in cur gather 
    int getCurTrs(){return m_iCurTrs;};

    void setGatherList(QList<int> g){m_listGather = g;}; //  trs in cur gather 
    QList<int> getGatherList(){return m_listGather;};
//data type
    void setDataType(int id){m_dataType = id;};
    int getDataType(){return m_dataType;};
// devTape:
    void setDevType(int id){m_devType = id;};
    int getDevType(){return m_devType;};

signals:
    virtual void sigParamChanged();
public:


// data type
    int m_dataType;//0;
    int m_devType;//0;
// data and header
    float *ptrace;
    char *pheader;
//parameters :set in open
    float ltr,si;//s,ms,number;
    int headBytes,traceBytes;   
    int m_gathers,m_traces;   
    int m_iMaxTrsOfGather;
    int m_headLen;//in word



    float *displayHeader;//buffer: we it contain headers to display
    QString m_dataName;
    QString m_gather;//line,cmp,shot
    QString displayHD;// header in top label,set i
// current:
    int m_iCurIndex,m_iCurGather;
    int m_iCurTrs;// trs in gather ,last read;
    int m_iMyData;// ¡¡segd: read skip files
    //
    QMap<QString, double> m_mapSuperHeader;
    QMap<QString, double> m_mapHeader;
    QList<int> m_listGather;
// pick 
// 
/// get the pick time from other knee gather 
    QMap<int, float> pickIntpTimes;

/** all pick gather of the data store here
 *  int :gather
 *  PICKPOINTS: pick points in the gather
 */
    typedef  QMap<int, float> PICKPOINTS;
    QMap<int, PICKPOINTS > pickData;
    QString pickHD;
     
};

#endif



