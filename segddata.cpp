#include "segddata.h"

segdData::segdData():seisData() //:seisData()
{
   m_iMainHD = 3;
   displayHeader = NULL;
      ptrace = NULL;
   pheader = NULL;
   setHeadLen(DHEADER_LEN);
   mapHeader();
   //m_bIdxFile = false;
   m_iMyData = 0;
}
segdData::~segdData()
{
   closeData();

}

int segdData::openData(QString name, QString mode)
{
   int i, idd;
   QString str;
   //qDebug() << name << mode;
   if (mode == "r")
   {
      idd = m_segd.InitTape(name);
      qDebug() << "open ret = " << idd;
      if (idd < 0)
      { // cannot open segy file;
         return -1;
      }
      m_iMainHD = 1; // file
#if 0
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
#endif
// set parameters:
      setLTR(m_segd.m_iLTR );// / 1000);
      setSI(m_segd.m_iSI / 1000);
      // setGathers(m_segd.m_iAllGathers);
      // setTraces(m_segd.m_iAllTraces);
      setGathers(1);
      setTraces(1);
      setHeadLen(DHEADER_LEN);
      setCurTrs(1);
      m_iCurIndex = 0;
      m_iCurGather = 1;
// key and display HD
#if 0
      str = m_segy.getKey();
      qDebug()<<"key = "<< str;
      if (str == GATHER_SHOT) setDisplayHD(getHDStr(4)); // trace in shot
      if (str == GATHER_LINE) setDisplayHD(getHDStr(6)); //cmp number
      if (str == GATHER_CMP) setDisplayHD(getHDStr(7)); // trace in cmp offset = HD10
#endif
      //setDisplayHD(getHDStr(10)); //testoffset
      //setGather(str);
      str = GATHER_SHOT;
      setDisplayHD(getHDStr(1));
      setGather(str);

      m_dataName = name;
      m_mapSuperHeader = mapSuperHeader();
// buffer ,header:
// m_iMaxTrsOfGather = m_segd.m_iMaxTrsOfGather;
      m_iMaxTrsOfGather = MAX_TRS_OF_GATHER;
      ptrace = new float[m_iMaxTrsOfGather * getSamples()];
      pheader = new char[m_iMaxTrsOfGather * DHEADER_LEN * sizeof(int)];
// for display header:

      if (displayHeader != NULL) delete[] displayHeader;
           displayHeader = new float[m_iMaxTrsOfGather];
// index gather:
#if 1
      QList<int> glist;
      for (i = 0; i < getGathers(); i++)
      {
         glist.append(1);
      }
      setGatherList(glist);
#endif
      
      setDevType(DEV_TAPE);
// print head message:
      qDebug() << " trace buffer in float = " << m_iMaxTrsOfGather * getSamples();
      qDebug() << " head buffer in char = " << m_iMaxTrsOfGather * DHEADER_LEN * 4;

      qDebug() << "ltr,oltr = " << getLTR() << m_segd.m_iLTR;
      qDebug() << "si,osi = " << getSI() << m_segd.m_iSI;
      qDebug() << "samples = " << getSamples();
      qDebug() << "gathers = " << getGathers();
      qDebug() << "traces = " << getTraces();
      qDebug() << "maxtrsofgather = " << m_iMaxTrsOfGather;


   }
   return 0;
}
int segdData::readGather(int g, float *trace, char *header)
{
   return 0;
   int idx;
   //idx = m_segd.getGatherIndex(g);
   //return readGatherIndex(idx, trace, header);

}
int segdData::readPercent(int g, float *trace, char *header)
{
   return 0;
   //int idx;
   //idx = g*(m_segd.m_iAllGathers-1)/100;
   //return readGatherIndex(idx, trace, header);

}
int segdData::readGatherIndex(int idx, float *trace, char *header)
{
   return 0;
#if 0
   int i, trs;
   if (idx >= getGathers() || idx < 0) return -1;
//location:
   trs = m_segd.m_pIdx[idx].num;
   //qDebug() << "trs = " << trs;
   if (trs <= 0) return -2;
   m_segd.seekGatherIdx(idx);
   //qDebug() << "READ IDX,trs = " << idx << trs;
//read:
   i = m_segd.readTraces(
                         (int *)header,
                         trace,
                         trs);
// set gather parameters:


   setCurTrs(trs);
   m_iCurIndex = idx;
   m_iCurGather = m_segd.getIndexGather(idx);
   setDisplayHeader();
   if (i == 0) return trs;
   else return -1;
#endif
}
int segdData::readPreGather(float *trace, char *header)
{
   return 0;
   if (m_iCurIndex <= 0) return -1;
   m_iCurIndex--;
   return readGatherIndex(m_iCurIndex, trace, header);
}
int segdData::readNextGather(float *trace, char *header)
{
   int  i, idx, num, sz;
   //if (m_iCurIndex >= m_segy.m_iAllGathers - 1) return -1;
   //return readGatherIndex(m_iCurIndex, trace, header);
   float *f;
   f = (float*) header;
   sz = m_iMyData;
   qDebug() << "m_iMyData = " <<sz;
   for (i = 0; i < sz; i++)
   {
      num = m_segd.readShot(trace, (int *)header); // skip sz files
   }
   num = m_segd.readShot(trace, (int *)header);
    qDebug() << "read shot trs = " <<num;
    qDebug() << "read header = " <<  f[0] << f[1] << f[21];
    f = (float*) header;

// set gather parameters:
   setCurTrs(num);
   // m_iCurIndex = idx;
   m_iCurGather = m_segd.m_infoMHD.iShot;
   setDisplayHeader();
   if (num > 0) return num;
   else return -1;
}
int segdData::readFirstGather(float *trace, char *header)
{

   return readNextGather(trace, header);
   //m_iCurIndex = 0;
   //return readGatherIndex(m_iCurIndex, trace, header);
}
int segdData::readLastGather(float *trace, char *header)
{
   return 0;
   //m_iCurIndex = m_segd.m_iAllGathers - 1;
   // return readGatherIndex(m_iCurIndex, trace, header);
}
int segdData::closeData()
{
   m_segd.CloseTape();
      qDebug() << "in closeData ";
   if (displayHeader != NULL) delete displayHeader;
   if (pheader != NULL) delete pheader;
   if (ptrace != NULL) delete ptrace;
   return 0;
}

QMap<QString, double> segdData::mapHeader(int idx)
{
   QMap<QString, double> maph;
   QString str;
   unsigned char *head;

   float *f;
   
   int i, num, trs;
   trs = getCurTrs();
   if (idx < 0 || idx >= trs)
   {
      maph.clear();
      return maph;
   }

   head = (unsigned char *)pheader;
    
   num = DHEADER_LEN;

   for (i = 0; i < num; i++)
   {
      str = getHDStr(i);
      //maph[str] = m_segd.getFloat(head +  idx * num * sizeof(float) + i * sizeof(float));
      f = (float*)(head +  idx * num * sizeof(float) + i * sizeof(float));
      maph[str] = *f;
      //if (i == 0 || i == 1)  
      //    qDebug() << "idx ,str ,v = " << idx << str << maph[str]<<f[0] << f[1]; 
   }
   

   return maph;
}
QMap<QString, double> segdData::mapHeader()
{
   QMap<QString, double> maph;
   QString str;
   int i, num;

   num = DHEADER_LEN;

   for (i = 0; i < num; i++)
   {
      str = getHDStr(i);
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
QString segdData::getHDStr(int i)
{
   QString str;
   switch (i)
   {
   case 0:
      str = "Shot"; break;
   case 1:
      str = "Trace"; break;
   case 2:
      str = "Channel"; break;
   case 3:
      str = "Type"; break;
   case 4:
      str = "CMP"; break;
   case 5:
      str = "LTR"; break;
   case 6:
      str = "Offset"; break;
   case 7:
      str = "SI"; break;
   case 8:
      str = "Valid"; break;
   case 9:
      str = "ShotLine"; break;
   case 10:
      str = "ShotPoint"; break;
   case 11:
      str = "ShotIndex"; break;
   case 12:
      str = "ReceiverLine"; break;
   case 13:
      str = "ReceiverPoint"; break;
   case 14:
      str = "ReceiverIndex"; break;
   case 15:
      str = "TrsAll"; break;
   case 16:
      str = "TrsSeismic"; break;
   case 17:
      str = "TrsAux"; break;
   case 18:
      str = "NULL"; break;
   case 19:
      str = "NULL"; break;
   case 20:
      str = "NULL"; break;
   default:
      str = "NULL";

   }
   return str;
}
QMap<QString, double> segdData::mapSuperHeader()
{

   QMap<QString, double> map;
   map["Trace Length"] = getLTR();
   map["Sample Rate"] = getSI();
   map["Headers"] = getHeadLen();
   map["ManufacturerID"] = m_segd.m_infoGHD.iManufacturerID;
   map["FormatCode"] = m_segd.m_infoGHD.iFormatCode;
   map["AddGHD"] = m_segd.m_infoGHD.iAddGHD;
   map["BytesPerScan"] = m_segd.m_infoGHD.iBytesPerScan;
   map["Extended"] = m_segd.m_infoGHD.iExtended;
   map["External"] = m_segd.m_infoGHD.iExternal;
   map["Version"] = m_segd.m_infoGHD.version;
   map["Trailer"] = m_segd.m_infoGHD.iTrailer;
   map["ScanTypes"] = m_segd.m_infoGHD.iScanTypes;
   map["MaxChSets"] = m_segd.m_infoGHD.iMaxChSets;
   map["Skew"] = m_segd.m_infoGHD.iSkew;
   return map;
}
void segdData::setDisplayHeader()
{
   setDisplayHeader(displayHD);
}
void segdData::setDisplayHeader(QString hd) //hdFrom1
{
   int i,num,trs;
   num = DHEADER_LEN;
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
   qDebug() << "setDisplayHeader hd = " << hd ;
   for (i = 0; i < trs; i++)
   {
      maph = mapHeader(i);
      displayHeader[i] = maph[hd];
   }
   qDebug() << "segdData::setDisplayHeader  end";
}
void segdData::sortData(int idd)
{
#if 0
   //qDebug() << "sortd = "<<idd;
   int i,num,trs,ihd,j,id;
   num = HEADER_LEN*sizeof(int);
   trs = getCurTrs();
   QMap<QString, double> maph;
   QMap<int,int> mapsort;
   float *sortTrace;
   char * sortHeader;

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
   for (i = 0; i< glist.size();i++)
   {
      if (idd == SORT_AS)
      id = mapsort[glist[i]];
      if (idd == SORT_DES)
      id = mapsort[glist[trs-1-i]];
      memcpy(sortTrace+i*getSamples() ,ptrace + id*getSamples(),getSamples()*sizeof(float));
      memcpy(sortHeader+i*num ,pheader + id*num,num);
   }
   memcpy(ptrace,sortTrace,trs*getSamples()*sizeof(float));
   memcpy(pheader,sortHeader,trs*num);
   delete sortTrace;
   delete sortHeader;

#endif
}

