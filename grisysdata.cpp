#include "grisysdata.h"
#include "QTapeIO.h"
grisysData::grisysData():seisData()
{
   m_iMainHD = 3;
   displayHeader = NULL;
   setHeadLen(GHEADER_LEN);
   mapHeader();
   //m_bIdxFile = false;

}
grisysData::~grisysData()
{
   closeData();

}

int grisysData::openData(QString name, QString mode)
{
   int i, idd;
   QString str;
   //qDebug() << name << mode;
   if (mode == "r")
   {
      idd = m_gri.testGather(name);
         if (idd > 0)
         {
            m_iMainHD = idd;
            qDebug() << "test Gather = " << idd <<m_gri.getKey();
         }

      idd = m_gri.open_read(name);
      qDebug() << "open gri ret = " << idd;

//idx file m_iMainHD=3
      if (idd >=5)
      {
         m_gri.close_read();
// which gather: return head

         qDebug() << "create idx file";
         i = m_gri.createIdxFile(name, m_iMainHD);
         if (i  != 0) return -2; //can not create idx file
         i = m_gri.open_read(name);
         if (i != 0) return -1; //cannot open gri file
      }
      else if (idd !=0)
      {
         return -1;// open err
      }

// set parameters:
      setLTR(m_gri.getLTR());//ms
      setSI(m_gri.getSI());//ms
      setGathers(m_gri.getAllGroups());
      setTraces(m_gri.getAllTraces());
      //setHeadLen(GHEADER_LEN);
// key and display HD
      str = m_gri.getKey();
      if (str == GATHER_SHOT) setDisplayHD(getHDStr(17));// trace in shot
      if (str == GATHER_LINE) setDisplayHD(getHDStr(4));//cmp number
      if (str == GATHER_CMP) setDisplayHD(getHDStr(20));// trace in cmp offset = HD10
      setDisplayHD(getHDStr(10)); //testoffset segy
      setGather(str);
      m_dataName = name;
      m_mapSuperHeader = mapSuperHeader();
// buffer ,header:
      m_iMaxTrsOfGather = m_gri.getMaxTrsPerGroup();
      ptrace = new float[m_iMaxTrsOfGather * getSamples()];
      pheader = new char[m_iMaxTrsOfGather * GHEADER_LEN * 4];
// for display header:

      if (displayHeader != NULL) delete[] displayHeader;
           displayHeader = new float[m_iMaxTrsOfGather];
// list:
      QList<int> glist;
      for (i = 0; i < getGathers(); i++)
      {
         glist.append(m_gri.listGroup[i].grp);
      }
      setGatherList(glist);
      setDevType(DEV_DISK);
// print head message:
      qDebug() << " trace buffer in float = " << m_iMaxTrsOfGather * getSamples();
      qDebug() << " head buffer in char = " << m_iMaxTrsOfGather * GHEADER_LEN * 4;

      qDebug() << "ltr,oltr = " << getLTR() << m_gri.getLTR();
      qDebug() << "si,osi = " << getSI() << m_gri.getSI();
      qDebug() << "samples = " << getSamples();
      qDebug() << "gathers = " << getGathers();
      qDebug() << "traces = " << getTraces();
      qDebug() << "maxtrsofgather = " << m_iMaxTrsOfGather;


   }
   return 0;
}
int grisysData::readGather(int g, float *trace, char *header)
{
   int idx;
   idx = m_gri.indexOfGroup(g);
   if (idx < 0) return idx;
   return readGatherIndex(idx, trace, header);

}
int grisysData::readPercent(int g, float *trace, char *header)
{
   int idx;
   idx = g*(m_gri.getAllGroups()-1)/100;
   return readGatherIndex(idx, trace, header);

}
int grisysData::readGatherIndex(int idx, float *trace, char *header)
{
   int i, trs;
   if (idx >= getGathers() || idx < 0) return -1;
//location:
   trs = m_gri.listGroup[idx].num;
   //qDebug() << "trs = " << trs;
   if (trs <= 0) return -2;
   m_gri.point_idx_group(idx);
   //qDebug() << "READ IDX,trs = " << idx << trs;
//read:
   i = m_gri.readTraces(
      (int *)header,
      trace,
      trs);
// set gather parameters:


   setCurTrs(trs);
   m_iCurIndex = idx;
   m_iCurGather = m_gri.listGroup[idx].grp;
   setDisplayHeader();
   if (i == 0) return trs;
   else return -1;
}
int grisysData::readPreGather(float *trace, char *header)
{
   if (m_iCurIndex <= 0) return -1;
   m_iCurIndex--;
   return readGatherIndex(m_iCurIndex, trace, header);
}
int grisysData::readNextGather(float *trace, char *header)
{
   if (m_iCurIndex >= m_gri.getAllGroups() - 1) return -1;
   m_iCurIndex++;
   return readGatherIndex(m_iCurIndex, trace, header);
}
int grisysData::readFirstGather(float *trace, char *header)
{
   m_iCurIndex = 0;
   return readGatherIndex(m_iCurIndex, trace, header);
}
int grisysData::readLastGather(float *trace, char *header)
{
   m_iCurIndex = m_gri.getAllGroups() - 1;
   return readGatherIndex(m_iCurIndex, trace, header);
}
int grisysData::closeData()
{
   m_gri.close_read();
   //   qDebug() << "delete segy";
   if (displayHeader != NULL) delete displayHeader;
   if (pheader != NULL) delete pheader;
   if (ptrace != NULL) delete ptrace;
   return 0;
}

QMap<QString, double> grisysData::mapHeader(int idx)
{
   QMap<QString, double> maph;
   QString str;
   unsigned char *head,*h;
   int i,num,trs,*ip;
   trs = getCurTrs();
   if (idx < 0 || idx >= trs)
   {
      maph.clear();
      return maph;
   }

   head = (unsigned char *)pheader;
   num = GHEADER_LEN;

   for (i = 0; i < num; i++)
   {
      str = getHDStr(i+1);
      //maph[str]= m_gri.getInt(head +  idx * num * sizeof(int)+ i* sizeof(int));
      h = head +  idx * num * sizeof(int) + i * sizeof(int);
      ip = (int *)h;
      maph[str]= (double)*ip;

#if 0
      //j = m_gri.getInt(head +  64 * 4*2 + i* sizeof(int));
      if (i < 5)
      {
         qDebug() << i + 1 << str <<  m_gri.getInt(head + i * 4);
      }
#endif
   }

   return maph;
}
QMap<QString, double> grisysData::mapHeader()
{
   QMap<QString, double> maph;
   QString str;
   int i,num;

   num = GHEADER_LEN;

   for (i = 0; i < num; i++)
   {
      str = getHDStr(i+1);
      maph[str]= 0;
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
QString grisysData::getHDStr(int i)
{
   QString str;
   str = QString("HD%1").arg(i,3,10,QLatin1Char('0'));
   return str;
}
QMap<QString, double> grisysData::mapSuperHeader()
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
void grisysData::setDisplayHeader()
{
    setDisplayHeader(displayHD);
}
void grisysData::setDisplayHeader(QString hd)//hdFrom1
{
   int i,trs;
   //num = GHEADER_LEN;
   trs = getCurTrs();
   displayHD = hd;
   QMap<QString, double> maph;
   //if (displayHeader != NULL) delete []displayHeader;

   //displayHeader = new float[trs];


   for (i = 0; i < trs; i++)
   {
      maph = mapHeader(i);
      displayHeader[i] = maph[hd];
   }
   qDebug() << "grisysData::setDisplayHeader  end";
}
void grisysData::sortData(int idd)
{
     //qDebug() << "sortd = "<<idd;
   int i,num,trs,ihd, id;
   num = GHEADER_LEN*sizeof(int);
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
    delete []sortTrace;
    delete []sortHeader;
}
