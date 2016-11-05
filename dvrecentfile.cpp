#include "dvrecentfile.h"
#include "dvapp.h"
#include "dvdata.h"
#include <QDebug>
dvRecentFile::dvRecentFile() : QObject()
{

}
dvRecentFile::~dvRecentFile()
{
    removeActions();
}
int dvRecentFile::addFile(QString f,int t)
{
    recentData fd;
    fd.file = f;
    fd.type = t;
    int ip,size;
    size = m_listFile.size();
// if found remove it first
    for (int i = 0; i < size; ++i)
    { 
        ip = size - 1 - i;
        if (f == m_listFile.at(ip).file )
        {
             m_listFile.removeAt(ip);
        }
    }
// add to the top;
    m_listFile.insert(0, fd);
    qDebug()<< "recentFIle:add =" << f<<t;
// if over load:
    if(m_listFile.size() > MAX_RECENT_FILES)
    {
        m_listFile.removeLast();
    }
    return 0;
}
int dvRecentFile::saveToSetting()
{
    QSettings settings(ORG_NAME, APP_NAME);
    settings.beginWriteArray("recentFiles");
    for (int i = 0; i < m_listFile.size(); ++i) 
    {
        settings.setArrayIndex(i);
        settings.setValue("fileName", m_listFile.at(i).file); 
        settings.setValue("fileType", m_listFile.at(i).type); 
    }
    settings.endArray();
    return 0;
}
int dvRecentFile::loadFromSetting()
{
    QSettings settings(ORG_NAME, APP_NAME);
    int size,ty;
    QString str;
    recentData fd;
    size = settings.beginReadArray("recentFiles");
    if (size > MAX_RECENT_FILES) size = MAX_RECENT_FILES;
     
    for (int i = 0; i < size; ++i)
    {
        settings.setArrayIndex(i);
        str = settings.value("fileName").toString();
        ty = settings.value("fileType").toInt();
        fd.file = str;
        fd.type = ty;
        m_listFile.append(fd);    
    }
    settings.endArray();
    return 0;

}
recentList dvRecentFile::getFileList()
{
    return m_listFile;
}
// it is not used now: doubt it will cause core down;
void dvRecentFile::removeActions()
{
    int i,size;
    QAction *act;
    size = m_listAction.size();

    qDebug() << "dvRecentFile::removeActions start = " <<m_listAction.size() ;
    for (i = 0; i < size; ++i)
    {
        //qDebug() << "dvRecentFile::removeActions = " << i;
        act = m_listAction.at(i);
        //qDebug() << "remove  = act = "<< m_listAction[i];
        //qDebug() << "dvRecentFile::removeActions = ";

        ///qDebug() << "dvRecentFile::removeActions  act = " <<  act;
        act->disconnect();
        //qDebug() << "dvRecentFile::removeActions delete";
        delete act;
    } 
    qDebug() << "dvRecentFile::removeActions clear";
    m_listAction.clear();
}
void dvRecentFile::reFreshMenu(QMenu *menu,QWidget *win,QSignalMapper *winMap)
{
    QString str;
    int i; //ty,;
    QAction *act;
    int size = getFileList().size();

    
    //removeActions();
    menu->clear();// if clear actions attch it will removed.
// we have a problem not solved here:
// why the act enter slotRecentFile size times
    for (i = 0; i < size; ++i)
    {
        str = getFileList().at(i).file;
        //ty =  getFileList().at(i).type;
        
         act = menu->addAction(str);
         //qDebug() << " act = " <<act <<str;
        //m_listAction.append(act);
        //qDebug() << "refreshMenu = act = "<< m_listAction[i];
    //}
        connect(act, SIGNAL(triggered()), winMap, SLOT(map()));
        winMap->setMapping(act, str);
        //qDebug()<< "refreshMenu = "<< win << str <<act;
        connect(winMap, SIGNAL(mapped(QString)),
           win, SLOT(slotRecentFile(QString)));
    }
}
int dvRecentFile::typeOfFile(QString f)
{
    int ret = -1;
    QString str;
    int ty;
    int size = getFileList().size();

    for (int i = 0; i < size; ++i)
    {
        str = getFileList().at(i).file;
        ty =  getFileList().at(i).type;
        if (f == str)
        {
            ret = ty;
            break;
        }
    }
    return ret;
}
