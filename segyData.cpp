#include "segydata.h"

segyData::segyData() : seisData()
{
   m_iMainHD = 3;
   displayHeader = NULL;
   ptrace = NULL;
   pheader = NULL;
   setHeadLen(HEADER_LEN);
   mapHeader();
   connect(&m_segy, SIGNAL(sigThreadDown()),
           this, SLOT(slotUpdateParam()));

}
segyData::~segyData()
{
   closeData();

}
#if 0
int segyData::openData(QString name, QString mode)
{
   int i, idd;
   QString str;
   //qDebug() << name << mode;
   if (mode == "r")
   {
      idd = m_segy.testGather(name);
      if (idd > 0)
      {
         m_iMainHD = idd;
         qDebug() << "test Gather = " << idd <<m_segy.getKey();
      }

      idd = m_segy.openRead(name);
      qDebug() << "open ret = " << idd;
      if (idd < 0)
      { // cannot open segy file;
         return -1;
      }
//idx file m_iMainHD=3
      if (!m_segy.m_bIdxFile)
      {
         m_segy.closeRead();
// which gather: return head

         qDebug() << "create idx file";
         i = m_segy.createIdxFile(name, m_iMainHD);
         if (i  < 0) return -2; //can not create idx file
         i = m_segy.openRead(name);
         if (i < 0) return -1; //cannot open segy file
      }
// set parameters:
      setLTR(m_segy.m_iLTR / 1000);
      setSI(m_segy.m_iSI / 1000);
      setGathers(m_segy.m_iAllGathers);
      setTraces(m_segy.m_iAllTraces);
      //setHeadLen(HEADER_LEN);
// key and display HD
      str = m_segy.getKey();
      qDebug()<<"key = "<< str;
      if (str == GATHER_SHOT) setDisplayHD(getHDStr(4)); // trace in shot
      if (str == GATHER_LINE) setDisplayHD(getHDStr(6)); //cmp number
      if (str == GATHER_CMP) setDisplayHD(getHDStr(7)); // trace in cmp offset = HD10
      setDisplayHD(getHDStr(10)); //testoffset
      setGather(str);
      m_dataName = name;
      m_mapSuperHeader = mapSuperHeader();
// buffer ,header:
      m_iMaxTrsOfGather = m_segy.m_iMaxTrsOfGather;
      ptrace = new float[m_iMaxTrsOfGather * getSamples()];
      pheader = new char[m_iMaxTrsOfGather * HEADER_LEN * sizeof(int)];
// for display header:

      if (displayHeader != NULL) delete[] displayHeader;
      displayHeader = new float[m_iMaxTrsOfGather];
// index gather:
      QList<int> glist;
      for (i = 0; i < getGathers(); i++)
      {
         glist.append(m_segy.getIndexGather(i));
      }
      setGatherList(glist);
      setDevType(DEV_DISK);
// print head message:
      qDebug() << " trace buffer in float = " << m_iMaxTrsOfGather * getSamples();
      qDebug() << " head buffer in char = " << m_iMaxTrsOfGather * HEADER_LEN * 4;

      qDebug() << "ltr,oltr = " << getLTR() << m_segy.m_iLTR;
      qDebug() << "si,osi = " << getSI() << m_segy.m_iSI;
      qDebug() << "samples = " << getSamples();
      qDebug() << "gathers = " << getGathers();
      qDebug() << "traces = " << getTraces();
      qDebug() << "maxtrsofgather = " << m_iMaxTrsOfGather;


   }
   return 0;
}
#endif
#if 1
int segyData::openData(QString name, QString mode)
{
   int i, idd;
   QString str;
   qDebug() << "opendata " << name << mode;
   if (mode == "r")
   {
      idd = m_segy.openReadFile(name);
      qDebug() << "open ret = " << idd;
      if (idd < 0)
      { // cannot open segy file;
         return -1;
      }
      m_dataName = name;
      setParam();
   }
   return 0;
}
#endif
void segyData::slotUpdateParam()
{ // after create idx thread and getIdx info down:
   qDebug() << "segyData: in slot update param";
   setParam();
   setDevType(DEV_DISK);
   emit sigParamChanged();
}
void segyData::setParam()
{
// set parameters:
   QString str;
   int i;
   setLTR(m_segy.m_iLTR / 1000);
   setSI(m_segy.m_iSI / 1000);

   qDebug() << "getSamples() = "<<getSamples() << getSI() << getLTR()   <<m_segy.m_iSI <<m_segy.m_iLTR << m_iMaxTrsOfGather;

   if ( m_segy.m_iAllGathers == 0) m_segy.m_iAllGathers =1;
   if ( m_segy.m_iAllTraces == 0)  m_segy.m_iAllTraces =1;
 
   setGathers(m_segy.m_iAllGathers);
   setTraces(m_segy.m_iAllTraces);
   setCurTrs(1);
   m_iCurIndex = 0;
   m_iCurGather = 1;
   //setHeadLen(HEADER_LEN);
// key and display HD
   str = m_segy.getKey();
   qDebug() << "key = " << str;
   if (str == GATHER_SHOT) setDisplayHD(getHDStr(4)); // trace in shot
   if (str == GATHER_LINE) setDisplayHD(getHDStr(6)); //cmp number
   if (str == GATHER_CMP) setDisplayHD(getHDStr(7)); // trace in cmp offset = HD10
   setDisplayHD(getHDStr(10)); //testoffset
   setGather(str);

   m_mapSuperHeader = mapSuperHeader();
// buffer ,header:
   m_iMaxTrsOfGather = m_segy.m_iMaxTrsOfGather;
   qDebug() << "m_iMaxTrsOfGather = " <<m_iMaxTrsOfGather;
// if idx got:
   setDevType(DEV_TAPE);
   if (m_segy.m_bIdxFile) 
       setDevType(DEV_DISK);
   qDebug() << "m_iMaxTrsOfGather = " <<m_iMaxTrsOfGather;
//
   if (m_iMaxTrsOfGather <= 0)
   {
       m_iMaxTrsOfGather = MAX_GATHER_TRS;
       setDevType(DEV_TAPE);
   }
       
   qDebug() << "ptrace = "<<ptrace ;

      if (ptrace != NULL) delete[] ptrace;
      qDebug() << "pheader = " << pheader;
      if (pheader != NULL) delete[] pheader;
qDebug() << "getSamples() = "<<getSamples() << getSI() << getLTR()   <<m_segy.m_iSI <<m_segy.m_iLTR << m_iMaxTrsOfGather;
      ptrace = new float[m_iMaxTrsOfGather * getSamples()];
      pheader = new char[m_iMaxTrsOfGather * HEADER_LEN * sizeof(int)];
      
// for display header:

   if (displayHeader != NULL) delete[] displayHeader;
   displayHeader = new float[m_iMaxTrsOfGather];
   qDebug() << "getGathers(); " <<getGathers();
// index gather:
   QList<int> glist;
   if (getDevType() == DEV_TAPE)
   {
       glist.append(1);

   }
   else
   for (i = 0; i < getGathers(); i++)
   {
      glist.append(m_segy.getIndexGather(i));
   }
   setGatherList(glist);

// print head message:
   qDebug() << " trace buffer in float = " << m_iMaxTrsOfGather * getSamples();
   qDebug() << " head buffer in char = " << m_iMaxTrsOfGather * HEADER_LEN * 4;

   qDebug() << "ltr,oltr = " << getLTR() << m_segy.m_iLTR;
   qDebug() << "si,osi = " << getSI() << m_segy.m_iSI;
   qDebug() << "samples = " << getSamples();
   qDebug() << "gathers = " << getGathers();
   qDebug() << "traces = " << getTraces();
   qDebug() << "maxtrsofgather = " << m_iMaxTrsOfGather;
}
int segyData::readGather(int g, float *trace, char *header)
{
   int idx;
   idx = m_segy.getGatherIndex(g);
   return readGatherIndex(idx, trace, header);

}
int segyData::readPercent(int g, float *trace, char *header)
{
   int idx;
   idx = g * (m_segy.m_iAllGathers - 1) / 100;
   return readGatherIndex(idx, trace, header);

}
int segyData::readGatherIndex(int idx, float *trace, char *header)
{
   if (DEV_TAPE == getDevType() ) return -1;

   int i, trs;
   if (idx >= getGathers() || idx < 0) return -1;
//location:
   trs = m_segy.m_pIdx[idx].num;
   qDebug() << "segyData::readGatherIndex :trs = " << trs;
   if (trs <= 0) return -2;
   m_segy.seekGatherIdx(idx);
   //qDebug() << "READ IDX,trs = " << idx << trs;
//read:
   i = m_segy.readTraces(
      (int *)header,
      trace,
      trs);
// set gather parameters:

   qDebug() << "segyData::readGatherIndex :setCurTrs = " << trs << i;
   setCurTrs(trs);
   m_iCurIndex = idx;
   m_iCurGather = m_segy.getIndexGather(idx);
   setDisplayHeader();
   if (i == 0) return trs;
   else return -1;
}
int segyData::readPreGather(float *trace, char *header)
{
   if (m_iCurIndex <= 0) return -1;
   m_iCurIndex--;
   return readGatherIndex(m_iCurIndex, trace, header);
}
int segyData::readNextGather(float *trace, char *header)
{
    int i,trs;
   if (getDevType() == DEV_TAPE)
   {
      i = m_segy.readGather(
         (int *)header,
         trace);
// set gather parameters:
      trs = i;
      qDebug() << "segyData::readShot = " << trs;
      setCurTrs(trs);
      m_iCurIndex = 0;
      m_iCurGather = m_segy.m_iCurGather;
      setDisplayHeader();
      if (i> 0) return trs;
      else return -1;
   }
   if (m_iCurIndex >= m_segy.m_iAllGathers - 1) return -1;
   m_iCurIndex++;
   return readGatherIndex(m_iCurIndex, trace, header);
}
int segyData::readFirstGather(float *trace, char *header)
{
   m_iCurIndex = 0;
   return readGatherIndex(m_iCurIndex, trace, header);
}
int segyData::readLastGather(float *trace, char *header)
{
   m_iCurIndex = m_segy.m_iAllGathers - 1;
   return readGatherIndex(m_iCurIndex, trace, header);
}
int segyData::closeData()
{
   m_segy.closeRead();
   //   qDebug() << "delete segy";
   if (displayHeader != NULL) delete[] displayHeader;
   if (pheader != NULL) delete[] pheader;
   if (ptrace != NULL) delete[] ptrace;
   return 0;
}

QMap<QString, double> segyData::mapHeader(int idx)
{
   QMap<QString, double> maph;
   QString str;
   unsigned char *head;
   int i, num, trs;
   trs = getCurTrs();
   if (idx < 0 || idx >= trs)
   {
      maph.clear();
      return maph;
   }

   head = (unsigned char *)pheader;
   num = HEADER_LEN;

   for (i = 0; i < num; i++)
   {
      str = getHDStr(i + 1);
      maph[str] = m_segy.getInt(head +  idx * HEADER_LEN * sizeof(int) + i * sizeof(int));
#if 0
      //j = m_segy.getInt(head +  64 * 4*2 + i* sizeof(int));
      if (i < 5)
      {
         qDebug() << i + 1 << str <<  m_segy.getInt(head + i * 4);
      }
#endif
   }

   return maph;
}
QMap<QString, double> segyData::mapHeader()
{
   QMap<QString, double> maph;
   QString str;
   int i, num;

   num = HEADER_LEN;

   for (i = 0; i < num; i++)
   {
      str = getHDStr(i + 1);
      maph[str] = 0;
   }
   m_mapHeader = maph;
   return maph;
}
/**
 *
 *
 * @param i
 *
 * @return QString
 */
QString segyData::getHDStr(int i)
{
   QString str;
   str = QString("HD%1").arg(i, 3, 10, QLatin1Char('0'));
   return str;
}
QMap<QString, double> segyData::mapSuperHeader()
{
   QMap<QString, double> map;
   map["MainHead"] = m_iMainHD;
   map["Trace Length"] = getLTR();
   map["Sample Rate"] = getSI();
   map["Gathers"] = getGathers();
   map["Traces"] = getTraces();
   map["MaxTr/Gather"] = getTraces();
   map["Headers"] = getHeadLen();


   return map;
}
void segyData::setDisplayHeader()
{
   setDisplayHeader(displayHD);
}
void segyData::setDisplayHeader(QString hd) //hdFrom1
{
   int i, num, trs;
   num = HEADER_LEN;
   trs = getCurTrs();
   displayHD = hd;
   QMap<QString, double> maph;
   if (trs <= 0)
   {
      qDebug() << "segyData::setDisplayHeader trs = " << trs;
      return;
   }
   // move to open file:
   //if (displayHeader != NULL) delete[] displayHeader;
   //displayHeader = new float[trs];

//   float f;
   for (i = 0; i < trs; i++)
   {
      maph = mapHeader(i);
      displayHeader[i] = maph[hd];
   }
   qDebug() << "segyData::setDisplayHeader  end";
}
void segyData::sortData(int idd)
{
   //qDebug() << "sortd = "<<idd;
   int i, num, trs, ihd, id;
   num = HEADER_LEN * sizeof(int);
   trs = getCurTrs();
   QMap<QString, double> maph;
   QMap<int, int> mapsort;
   float *sortTrace;
   char *sortHeader;

   if (idd == SORT_NO) return;
   //qDebug() << "num = "<<num;
   sortTrace = new float[trs * getSamples()];
   sortHeader = new char[trs * num];


   for (i = 0; i < trs; i++)
   {
      maph = mapHeader(i);
      ihd = maph[getDisplayHD()];
      mapsort[ihd] = i;
   }
   QList<int> glist;
   glist = mapsort.keys();
   for (i = 0; i < glist.size(); i++)
   {
      if (idd == SORT_AS) id = mapsort[glist[i]];
      if (idd == SORT_DES) id = mapsort[glist[trs - 1 - i]];
      memcpy(sortTrace + i * getSamples(), ptrace + id * getSamples(), getSamples() * sizeof(float));
      memcpy(sortHeader + i * num, pheader + id * num, num);
   }
   memcpy(ptrace, sortTrace, trs * getSamples() * sizeof(float));
   memcpy(pheader, sortHeader, trs * num);
   delete[] sortTrace;
   delete[] sortHeader;
}
