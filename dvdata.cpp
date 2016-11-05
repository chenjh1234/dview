#include <dvdata.h>

dvData::dvData()
{
    init(); 

}
//-----------------
dvData::~dvData()
{

}
//----------------------------------
void dvData::init()
{
    m_dataName = "";
    m_dataType = 0;
    m_data = NULL;
    m_param = NULL;
    m_iSortData = SORT_NO;
    m_iTapeSkip = 0;
  
}
 
void dvData::setData(seisData *d)
{
    //qDebug() << "set data data_name0 =" ;
    //qDebug() << "set data data_name0 =" <<d;
    m_data = d;
    
    //qDebug() << "set data data_name1 =" << d->m_dataName;
    if(d != NULL)
    {
        m_dataName = d->m_dataName;
        m_dataType = d->getDataType();
    }
}
seisData* dvData::getData()
{
    return m_data;
}
void dvData::setParam(paramData *d)
{
    m_param = d;
  
}
paramData* dvData::getParam()
{
    return m_param;
}
