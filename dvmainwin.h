#ifndef DVMAINWIN_H
#define DVMAINWIN_H
#include <QDebug>
#include <QMainWindow>
#include <QSplitter>
#include <QString>
#include <QTabWidget>
#include <QDockWidget>
#include <QTextBrowser>
#include <QProcess>
#include <iostream>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QToolBar>
#include <QStatusBar>
#include <QFile>
#include <QTextStream>
 
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QSignalMapper>
#include <QLineEdit>
#include <QSlider>

#define ZOOM_NORMAL_X 10
#define ZOOM_NORMAL_Y 10
#define ZOOM_IN_STEP 2

#define PARAM_ZOOM_FIT 0
#define PARAM_ZOOM_NORMAL 1
#define PARAM_ZOOM_OUT 2
#define PARAM_ZOOM_IN 3
#define PARAM_ZOOM_USER 4

#define PARAM_ZOOM_OUTH 5
#define PARAM_ZOOM_INH 6
#define PARAM_ZOOM_OUTV 7
#define PARAM_ZOOM_INV 8

#define PARAM_AMP_RMS 10
#define PARAM_AMP_GAIN 11
#define PARAM_AMP_AGC 12

#define PARAM_DRAW_WA 100
#define PARAM_DRAW_VD 101

#define PARAM_WIN_CASCADE 200
#define PARAM_WIN_TILE 201
#define PARAM_TEST 202
#define PARAM_WIN_OUTLOOK 203


#define PARAM_DRAG_HAND 300
#define PARAM_DRAG_RUBBER_BAND 301
#define PARAM_DRAG_NORMAL 302

#define PARAM_COMPARE_H 400
#define PARAM_COMPARE_V 401
#define PARAM_COMPARE_NO 402
#define PARAM_COMPARE_TURNOVER 403

#define PARAM_TAPE_SKIP 501

#define MOVE_FIRST 0
#define MOVE_LAST 1
#define MOVE_PRE 2
#define MOVE_NEXT 3
#define MOVE_GATHER 4
#define MOVE_INDEX 5
#define MOVE_PERCENT 6
// config if support segy
#define CONFIG_SEGY 1
#define CONFIG_SEGD 1
#define CONFIG_GRISYS 0
#define CONFIG_COMPARE 0
#define CONFIG_PICK 0
// 
#define CONFIG_SAME_TYPE 1 // keep open the same type of data,

#define LINEEDIT_WIDTH 32


#include "drawview.h"

#include "headview.h"
#include "paramwin.h"
#include "naviwin.h"
#include "dvrecentfile.h"

//#include "ui_module.h"
//#include "ui_parmvalue.h"
//#include "ui_modparameter.h"
using namespace std;
 
class dvMainWin : public QMainWindow
{
    Q_OBJECT

public:
    dvMainWin();


    void openFile(seisData *newData);
    void openFile(QString file);
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    void setTitle();
    void setTopLabel();
    void setConfig();//disable some functions
    void setConfig(int d);//enable only one type of data
    void setTopLabel(drawView *v);
    /// set parameters to view, the data in paramData object
    void setParam();
    void setParam(drawView *v);
    /// get params from view to paramData object;
    void getParam();
    void getParam(drawView *v);

    void initNavi();
    /// move data to target gather, and sort(if yes ) ->display
    int moveData(int where, int data=0);
    void setSortData(int d);
    int getSortData();
    drawView* getView();

    QDockWidget *naviDockWidget;
    QDockWidget *paramDockWidget;


    QMenu *fileMenu;
    QMenu *recentMenu;
    QMenu *locationMenu;
    QMenu *viewMenu;
    QMenu *pickMenu;
    QMenu *helpMenu;
    QMenu *windowMenu;

    QToolBar *fileToolBar;
    QToolBar *locationToolBar;
    QToolBar *viewToolBar;
    QToolBar *pickToolBar;
    QToolBar *helpToolBar;
    QToolBar *zoomToolBar;
    QToolBar *ampToolBar;
    QToolBar *drawToolBar;
    QToolBar *dragToolBar;
    QToolBar *compareToolBar;
    QToolBar *configToolBar;
    QToolBar *testToolBar;

    QAction *openSegyAct;
    QAction *compareAct;
    QAction *openGrisysAct;
    QAction *openGeoseisAct;
    QAction *saveFileAct;
    QAction *saveFileAsAct;
    QAction *exitAct;

    QAction *moveFirstAct;
    QAction *moveLastAct;
    QAction *movePreAct;
    QAction *moveNextAct;
    QAction *moveGatherAct;

    QAction *superHeaderAct;
    QAction *headerAct;
    QAction *viewParameterAct;

    QActionGroup *pickGroup;
    QAction *pickLayerAct;
    QAction *pickMuteAct;
    QAction *pickNoAct;

    QAction *winCascadeAct;
    QAction *winTileAct;
    QAction *outlookAct;

    //QAction *winTileAct;
    QActionGroup *drawGroup,*compareGroup,*dragGroup;
    QAction *drawWAAct,*drawVDAct;
    QAction *compareHAct,*compareVAct,*compareNOAct,*compareTurnoverAct;
    QAction *dragNormalAct,*dragHandAct,*dragRubberBandAct;



    QAction *aboutAct;
    QAction *helpAct;
    headView *headDlg;
    paramWin *paramView;
    //naviWin *nvView;//move to drawview
    //int m_value;// no use now
    dvRecentFile recentFile;

    QLineEdit *agcEdit,*userXEdit,*userYEdit;

 
protected:
    void closeEvent(QCloseEvent *event);
    
    
     

   // QPlainTextEdit *textEdit;
    //QTextBrowser *textView;
    //QSplitter *split;
    drawView *drawview;
    QTabWidget *tabWidget;
    QDockWidget *modDockWidget;
    QWidget *modWidget;

    //QString curFile,m_strFilename;

    QProcess hrun;
    QString lastDir;

// tape action:
         QToolBar *tapeToolBar;
         QAction *openSegdAct;
         QAction *tapeNextAct;
         QAction *tapeRewindAct;
         void createTape();// toolbar:
         void opTape(bool b);

private slots:
         void slotTapeNext();
         void slotTapeRewind();
         void slotOpenSegd();


    void slotActiveWin(QMdiSubWindow*n);
 
    void slotOpenFile();
    void slotOpenGrisysFile();
    void slotSaveFile();
    void slotSaveAsFile();
/**
     * create a compare win,the data is the current data,and 
     * gather. 
     * if a compare win already exist,it will renew the compre 
     * win data ,to the current active data(gather); 
     * 

     * @param  
*/
    void slotCompare();

    void slotMoveFirst();
    void slotMoveLast();
    void slotMovePre();
    void slotMoveNext();
    void slotMoveGather();
    void slotMovePercent();
    void slotMoveIndex(int);

    void slotSuperHeader();
    void slotHeader();
    void slotViewParameter();

    void slotPickLayer();
    void slotPickMute();
    void slotPickNo();

    void slotTest();

    void slotHelp();
    void slotAbout();
    void slotViewMovedTo(int ,int);
    void slotViewClosed(drawView *v);
    void slotShowHeader(int);
    void slotCloseHeader();

    void slotSetParamTool();
    void slotParam( int i);
    void slotParamChanged(QtProperty *pt);
    void updateMenus();
    void updateWindowMenu();
    void slotRecentFile(QString s);
    // slotSpinValue( int i);
    
    void switchLayoutDirection();
    void setActiveSubWindow(QWidget *window);
    drawView *createMdiChild();
    




signals:
    void sigModUpdate();
  
private:
        //QWidget *topw,*btw,*midw;
        //QVBoxLayout *vLayout;
    
    void drawData();
    void drawData(drawView *v);// draw the gather
    //void reDraw();
    void setView(drawView *v);// change active view
    QWidget * createSliderWidget(const QString &text,const QString &tip,int ty,int v,int min,int max,int step);
    QWidget * createSpinWidget(const QString &text,const QString &tip,int ty,int v,int min,int max,int step);
    QWidget *createEditWidget(const QString &text,const QString &tip,int ty,QString v,QLineEdit* &edit);
    QWidget *createEdit2Widget(const QString &text,const QString &tip,const QString &text0,const QString &text1,int ty,QString v,QString v1,QLineEdit *&edit,QLineEdit *&edit1);

    drawView *activeMdiChild();
    QMdiSubWindow *findMdiChild(const QString &fileName);
    drawView *findCompareWin();
    QMdiArea *mdiArea;
    QSignalMapper *windowMapper;
   // Ui::FormParam formParam;
    //Ui::FormMod formMod;
    //Ui::FormValue formValue;
    QList<drawView *> getViewList();
    QList<drawView *> viewList;
// pick interface:
    void savePickToMap();//change gather
    void savePickToFile();//chang gather in map
    int loadPickFromMap();// start new gather
    void loadPickFromFile();// start Program
    /** 
     * if this gather is not pick gather ,we can get pick time from 
     * intplation of nearby knee pick gathers . 
     * get pick times from knee pick gather* pickIntpTimes
     */
    QPolygonF getPickIntpTimes();
    QPolygonF intpMap(QMap<int,float>,QMap<int,float>,int idx0,int idx1,int idx,int trs);
 
};
  
#endif


