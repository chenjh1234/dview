#ifndef DV_RECENT_FILE_H
#define DV_RECENT_FILE_H

 
#include <QStringList>
#include <QString>
#include <QSettings>
#include <QMenu>
#include <QAction>
#include <QSignalMapper>
#include <QWidget>
#include <QAction>
#include <QObject>
#include "dvdata.h"
#define MAX_RECENT_FILES 10

class recentData  
{

public:
    QString file;
    int type;
};
typedef QList<recentData> recentList;
class dvRecentFile : QObject
{
    Q_OBJECT
public:
    dvRecentFile();
    ~dvRecentFile(); 
    //recentData fData;
    
    int addFile(QString f,int type);
    int saveToSetting();
    int loadFromSetting();
    recentList  getFileList();
    void reFreshMenu(QMenu *menu,QWidget *win,QSignalMapper *map);
    int typeOfFile(QString f);
    void removeActions();


private:
    recentList m_listFile;
    QList<QAction *> m_listAction;
};
#endif
