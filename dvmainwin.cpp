#include "dvapp.h"
#include "dvmainwin.h"
#include "segydata.h"
#include "grisysdata.h"
#include "segddata.h"

#include <QSettings>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QModelIndex>
#include <QFileDialog>
#include <QSpinBox>
#include <QLabel>
#include <QToolButton>
#include <QLineEdit>
#include <QDockWidget>
#include <QFileInfo>
//rewind;
//header:


dvMainWin::dvMainWin() : QMainWindow()
{
   mdiArea = new QMdiArea;
   mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
   mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
   setCentralWidget(mdiArea);
   connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)),
           this, SLOT(slotActiveWin(QMdiSubWindow*)));

   windowMapper = new QSignalMapper(this);

   lastDir = "./";
   //drawview = createMdiChild();
   //DOC = theApp->m_doc;
   paramData *pm;
   pm = new paramData();
   DOC->setParam(pm); // in construct
   headDlg = new headView(this);
   connect(headDlg, SIGNAL(finished(int)),
           this, SLOT(slotCloseHeader()));


   paramDockWidget = new QDockWidget("ParamView", this);
   addDockWidget(Qt::LeftDockWidgetArea, paramDockWidget);
   paramView = new paramWin(this);
   paramDockWidget->setWidget(paramView);
   paramDockWidget->hide();

   naviDockWidget = new QDockWidget("NaviView", this);
   addDockWidget(Qt::BottomDockWidgetArea, naviDockWidget);
   //naviView = new naviWin(this);
   //naviDockWidget->setWidget(naviView);
   naviDockWidget->hide();


   //buttons:
   // connect(formMod.pushButtonUpdate, SIGNAL(clicked()), this, SLOT(slotModUpdate()));

   createActions();
   createMenus();
   createToolBars();
   createStatusBar();
   createTape();
   connect(windowMapper, SIGNAL(mapped(int)),
           this, SLOT(slotParam(int)));
   connect(windowMapper, SIGNAL(mapped(QWidget*)),
           this, SLOT(setActiveSubWindow(QWidget*)));


   drawview = NULL;
   readSettings();
   qDebug() << "data name = " << DOC->m_dataName;
   qDebug() << "data type = " << DOC->m_dataType;
   if (!DOC->m_dataName.isEmpty())
   {
      //qDebug() << "is in" << DOC->m_dataName.isEmpty() << DOC->m_dataName.length();
      openFile(DOC->m_dataName);

   }
   setTitle();
   return;

};
void dvMainWin::createTape()
{
   QString str;
   openSegdAct = new QAction(QIcon(":/images/opensegd.png"), tr("openSegd"), this);
   //openSegdAct->setShortcuts(QKeySequence::New);
   openSegdAct->setStatusTip(tr("openSegd"));
   connect(openSegdAct, SIGNAL(triggered()), this, SLOT(slotOpenSegd()));

   tapeNextAct = new QAction(QIcon(":/images/tapenext.png"), tr("tapeNext"), this);
   //openSegdAct->setShortcuts(QKeySequence::New);
   tapeNextAct->setStatusTip(tr("tapeNext"));
   connect(tapeNextAct, SIGNAL(triggered()), this, SLOT(slotTapeNext()));

   //compareHAct = compareToolBar->addAction(QIcon(":/images/compareh.png"), "CompareH");
   tapeRewindAct = new QAction(QIcon(":/images/taperewind.png"), tr("tapeRewind"), this);
   //openSegdAct->setShortcuts(QKeySequence::New);
   tapeRewindAct->setStatusTip(tr("tapeRewind"));
   connect(tapeRewindAct, SIGNAL(triggered()), this, SLOT(slotTapeRewind()));


   // tapeNextAct = tapeToolBar->addAction(QIcon(":/images/tapenext.png"), "tapeNext");

   tapeToolBar->addAction(tapeNextAct);
   str = "Skip files";
   tapeToolBar->addWidget(createSpinWidget("SkipFiles", str, PARAM_TAPE_SKIP, 0, 0, 10, 1));
   tapeToolBar->addAction(tapeRewindAct);

   fileToolBar->insertAction(openSegyAct, openSegdAct);
   fileMenu->insertAction(openSegyAct, openSegdAct);

   // opTape(false);
   setConfig();


}
void dvMainWin::opTape(bool b)
{
   bool bb;
   bb = true;
   if (b) bb = false;

   locationToolBar->setEnabled(bb);
   locationMenu->setEnabled(bb);
   openSegyAct->setEnabled(bb);
   tapeToolBar->setEnabled(b);
   openSegdAct->setEnabled(b);
   if (b)
   {
      //locationMenu->hide();
      locationToolBar->hide();
      tapeToolBar->show();
   }
   else
   {
      //locationMenu->show();
      locationToolBar->show();
      tapeToolBar->hide();
   }
}
void dvMainWin::setConfig()
{

   if (CONFIG_GRISYS) openGrisysAct->setEnabled(true);
   else openGrisysAct->setEnabled(false);

   if (CONFIG_SEGY) openSegyAct->setEnabled(true);
   else openSegyAct->setEnabled(false);

   if (CONFIG_SEGY) openSegdAct->setEnabled(true);
   else openSegdAct->setEnabled(false);

#if 0
   if (CONFIG_PICK)
   {
      pickToolBar->setEnabled(true);
      pickToolBar->show();
      pickMenu->setEnabled(true);
      pickMenu->show();
   }
   else
   {
      pickToolBar->setEnabled(false);
      pickToolBar->hide();
      pickMenu->setEnabled(false);
      pickMenu->hide();

   }

   if (CONFIG_COMPARE)
   {
      compareToolBar->show();
      compareToolBar->setEnabled(true);
   }
   else
   {
      compareToolBar->hide();
      compareToolBar->setEnabled(false);
   }
#endif
   //  compareMenu->setEnabled(b);
}
void dvMainWin::slotTapeNext()
{
   qDebug() << "tapeNext slot";
   slotMoveNext();

}
void dvMainWin::slotTapeRewind()
{
   qDebug() << "tapeRewind slot";
   //slotMoveNext();



   // mdiArea->removeSubWindow(drawview);
   DOC->m_data->closeData();
   mdiArea->closeActiveSubWindow();
   //DOC->m_data->closeData();

   QString s;
   s = DOC->m_dataName;
   openFile(s);


}
void dvMainWin::readSettings()
{
   float f;
   QSettings settings(ORG_NAME, APP_NAME);
   QString str = settings.value("dataname").toString();
   int i = settings.value("datatype").toInt();
   DOC->m_dataName = str;
   DOC->m_dataType = i;
   f = settings.value("scaleX").toFloat();
   DOC->getParam()->scaleX = f;
   f = settings.value("scaleY").toFloat();
   DOC->getParam()->scaleY = f;
   f = settings.value("rms").toFloat();
   DOC->getParam()->rms = f;
   f = settings.value("gain").toFloat();
   DOC->getParam()->gain = f;
   i = settings.value("drawType").toInt();
   DOC->getParam()->drawType = i;
   i = settings.value("posX").toInt();
   DOC->getParam()->posX = i;
   i = settings.value("posY").toInt();
   DOC->getParam()->posY = i;
   i = settings.value("compareMode").toInt();
   DOC->getParam()->compareMode = i;
   i = settings.value("agc").toInt();
   DOC->getParam()->agc = i;
   qDebug() << "reading agc = " << i;
   recentFile.loadFromSetting();


}
void dvMainWin::writeSettings()
{
   recentFile.saveToSetting();
   qDebug() << "writeSetting drawview = " << drawview;
   if (drawview == NULL) return;
   setParam();
   QSettings settings(ORG_NAME, APP_NAME);
   //qDebug() << "write setting = " << theApp->m_doc->m_dataName;
   settings.setValue("dataname", DOC->m_dataName);
   settings.setValue("datatype", DOC->m_dataType);
   settings.setValue("scaleX", DOC->getParam()->scaleX);
   settings.setValue("scaleY", DOC->getParam()->scaleY);
   settings.setValue("rms", DOC->getParam()->rms);
   settings.setValue("gain", DOC->getParam()->gain);
   settings.setValue("drawType", DOC->getParam()->drawType);
   settings.setValue("posX", DOC->getParam()->posX);
   settings.setValue("posY", DOC->getParam()->posY);
   settings.setValue("compareMode", DOC->getParam()->compareMode);
   settings.setValue("agc", DOC->getParam()->agc);
   qDebug() << "writeing agc = " << DOC->getParam()->agc;


   if (drawview)
   {
      savePickToMap();
   }

   //savePickToFile();
}
void dvMainWin::createActions()
{
// openfile:
   qDebug() <<  "yes create actions";
   compareAct = new QAction(QIcon(":/images/compare.png"), tr("Compare"), this);
   compareAct->setShortcuts(QKeySequence::New);
   compareAct->setStatusTip(tr("Compare"));
   connect(compareAct, SIGNAL(triggered()), this, SLOT(slotCompare()));


   openSegyAct = new QAction(QIcon(":/images/openfile.png"), tr("OpenSegy"), this);
   openSegyAct->setShortcuts(QKeySequence::Open);
   openSegyAct->setStatusTip(tr("open Segy"));
   connect(openSegyAct, SIGNAL(triggered()), this, SLOT(slotOpenFile()));

   openGrisysAct = new QAction(QIcon(":/images/openfile.png"), tr("OpenGRISYS"), this);
   //compareAct->setShortcuts(QKeySequence::New);
   openGrisysAct->setStatusTip(tr("open GRISYS"));
   connect(openGrisysAct, SIGNAL(triggered()), this, SLOT(slotOpenGrisysFile()));

   openGeoseisAct = new QAction(QIcon(":/images/openfile.png"), tr("OpenGeoseis"), this);
   //compareAct->setShortcuts(QKeySequence::New);
   openGeoseisAct->setStatusTip(tr("open Geoseis"));
   connect(openGeoseisAct, SIGNAL(triggered()), this, SLOT(slotOpenFile()));



   saveFileAct = new QAction(QIcon(":/images/savefile.png"), tr("SaveFile"), this);
   //compareAct->setShortcuts(QKeySequence::Save);
   saveFileAct->setStatusTip(tr("saveFile"));
   connect(saveFileAct, SIGNAL(triggered()), this, SLOT(slotSaveFile()));

   saveFileAsAct = new QAction(QIcon(":/images/savefileas.png"), tr("SnapShot"), this);
   //compareAct->setShortcuts(QKeySequence::New);
   saveFileAct->setStatusTip(tr("savePNGFile"));
   connect(saveFileAsAct, SIGNAL(triggered()), this, SLOT(slotSaveAsFile()));
#if 0
   saveFileAsAct = new QAction(QIcon(":/images/savefileas.png"), tr("SaveFileAs"), this);
   //compareAct->setShortcuts(QKeySequence::New);
   saveFileAct->setStatusTip(tr("saveFile"));
   connect(saveFileAsAct, SIGNAL(triggered()), this, SLOT(slotSaveFileAs()));
#endif
   exitAct = new QAction(tr("E&xit"), this);
   exitAct->setShortcuts(QKeySequence::Quit);
   exitAct->setStatusTip(tr("exit the application"));
   connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

//location:

   moveFirstAct = new QAction(QIcon(":/images/movefirst.png"), tr("MoveFirst"), this);
   //moveFirstAct->setShortcuts(QKeySequence::Quit);
   moveFirstAct->setStatusTip(tr("move First"));
   connect(moveFirstAct, SIGNAL(triggered()), this, SLOT(slotMoveFirst()));

   moveLastAct = new QAction(QIcon(":/images/movelast.png"), tr("MoveLast"), this);
   //moveFirstAct->setShortcuts(QKeySequence::Quit);
   moveLastAct->setStatusTip(tr("move Last"));
   connect(moveLastAct, SIGNAL(triggered()), this, SLOT(slotMoveLast()));

   movePreAct = new QAction(QIcon(":/images/movepre.png"), tr("MovePre"), this);
   moveFirstAct->setShortcuts(QKeySequence::Back);
   movePreAct->setStatusTip(tr("move Pre"));
   connect(movePreAct, SIGNAL(triggered()), this, SLOT(slotMovePre()));

   moveNextAct = new QAction(QIcon(":/images/movenext.png"), tr("MoveNext"), this);
   moveFirstAct->setShortcuts(QKeySequence::Forward);
   moveNextAct->setStatusTip(tr("move Next"));
   connect(moveNextAct, SIGNAL(triggered()), this, SLOT(slotMoveNext()));

   moveGatherAct = new QAction(QIcon(":/images/movegather.png"), tr("MoveGather"), this);
   //moveFirstAct->setShortcuts(QKeySequence::Quit);
   moveGatherAct->setStatusTip(tr("move Gather"));
   connect(moveGatherAct, SIGNAL(triggered()), this, SLOT(slotMoveGather()));
// display paramters:
   superHeaderAct = new QAction(QIcon(":/images/superheader.png"), tr("SuperHeader"), this);
   //moveFirstAct->setShortcuts(QKeySequence::Quit);
   superHeaderAct->setStatusTip(tr("superHeader"));
   connect(superHeaderAct, SIGNAL(triggered()), this, SLOT(slotSuperHeader()));

   headerAct = new QAction(QIcon(":/images/header.png"), tr("Header"), this);
   //moveFirstAct->setShortcuts(QKeySequence::Quit);
   headerAct->setStatusTip(tr("header"));
   connect(headerAct, SIGNAL(triggered()), this, SLOT(slotHeader()));

   viewParameterAct = new QAction(QIcon(":/images/viewparameter.png"), tr("viewParameter"), this);
   //moveFirstAct->setShortcuts(QKeySequence::Quit);
   viewParameterAct->setStatusTip(tr("viewParameter"));
   connect(viewParameterAct, SIGNAL(triggered()), this, SLOT(slotViewParameter()));
//pick:

   pickLayerAct = new QAction(QIcon(":/images/picklayer.png"), tr("PickLayer"), this);
   //moveFirstAct->setShortcuts(QKeySequence::Quit);
   pickLayerAct->setStatusTip(tr("pickLayer"));
   connect(pickLayerAct, SIGNAL(triggered()), this, SLOT(slotPickLayer()));

   pickMuteAct = new QAction(QIcon(":/images/pickmute.png"), tr("PickMute"), this);
   //moveFirstAct->setShortcuts(QKeySequence::Quit);
   pickMuteAct->setStatusTip(tr("pickMute"));
   connect(pickMuteAct, SIGNAL(triggered()), this, SLOT(slotPickMute()));

   pickNoAct = new QAction(QIcon(":/images/pickno.png"), tr("PickNo"), this);
   //moveFirstAct->setShortcuts(QKeySequence::Quit);
   pickNoAct->setStatusTip(tr("pickNo"));
   connect(pickNoAct, SIGNAL(triggered()), this, SLOT(slotPickNo()));

   pickLayerAct->setCheckable(true);
   pickNoAct->setCheckable(true);
   pickMuteAct->setCheckable(true);
   pickGroup = new QActionGroup(this);
   pickGroup->setExclusive(true);
   pickGroup->addAction(pickLayerAct);
   pickGroup->addAction(pickMuteAct);
   pickGroup->addAction(pickNoAct);
   pickMuteAct->setChecked(false);
   pickLayerAct->setChecked(false);
   pickNoAct->setChecked(true);

// windows:
   winCascadeAct = new QAction(tr("Cascade"), this);
   connect(winCascadeAct, SIGNAL(triggered()), windowMapper, SLOT(map()));
   windowMapper->setMapping(winCascadeAct, PARAM_WIN_CASCADE);
   //winCascadeAct->setCheckable(true);

   winTileAct = new QAction(tr("Tile"), this);
   connect(winTileAct, SIGNAL(triggered()), windowMapper, SLOT(map()));
   windowMapper->setMapping(winTileAct, PARAM_WIN_TILE);

   outlookAct = new QAction(QIcon(":/images/outlook.png"), tr("Outlook"), this);
   connect(outlookAct, SIGNAL(triggered()), windowMapper, SLOT(map()));
   windowMapper->setMapping(outlookAct, PARAM_WIN_OUTLOOK);
   outlookAct->setCheckable(true);
   outlookAct->setChecked(true);

//help:
   aboutAct = new QAction(QIcon(":/images/about.png"), tr("&About"), this);
   aboutAct->setStatusTip(tr("show the application's About box"));
   connect(aboutAct, SIGNAL(triggered()), this, SLOT(slotAbout()));

   helpAct = new QAction(QIcon(":/images/help.png"), tr("&Help"), this);
   helpAct->setStatusTip(tr("Help"));
   connect(helpAct, SIGNAL(triggered()), this, SLOT(slotHelp()));

   qDebug() << "create actions ok";

}
void dvMainWin::createMenus()
{
   fileMenu = menuBar()->addMenu(tr("&File"));
   if (CONFIG_SEGY)  fileMenu->addAction(openSegyAct);
   if (CONFIG_GRISYS) fileMenu->addAction(openGrisysAct);
   //fileMenu->addAction(openGeoseisAct);
   fileMenu->addSeparator();
   //fileMenu->addAction(saveFileAct);
   fileMenu->addAction(saveFileAsAct);
   fileMenu->addSeparator();
   fileMenu->addAction(exitAct);

   fileMenu->addSeparator();
   recentMenu = fileMenu->addMenu("Rencent Files");

   locationMenu = menuBar()->addMenu(tr("&Location"));
   locationMenu->addAction(moveFirstAct);
   locationMenu->addAction(moveLastAct);
   locationMenu->addAction(movePreAct);
   locationMenu->addAction(moveNextAct);
   locationMenu->addAction(moveGatherAct);

   viewMenu = menuBar()->addMenu(tr("&View"));
   viewMenu->addAction(superHeaderAct);
   viewMenu->addAction(headerAct);
   viewMenu->addAction(viewParameterAct);
   if (CONFIG_PICK)
   {
      pickMenu = menuBar()->addMenu(tr("&Pick"));
      pickMenu->addAction(pickLayerAct);
      //pickMenu->addAction(pickMuteAct);
      pickMenu->addAction(pickNoAct);
   }

   windowMenu = menuBar()->addMenu(tr("&Window"));
   connect(windowMenu, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));

   helpMenu = menuBar()->addMenu(tr("&Help"));
   helpMenu->addAction(helpAct);
   helpMenu->addAction(aboutAct);
   qDebug() << "create menu";

}
void dvMainWin::updateMenus()
{
   bool hasMdiChild = (activeMdiChild() != 0);
   //compareAct->setEnabled(hasMdiChild);
   saveFileAsAct->setEnabled(hasMdiChild);
   moveFirstAct->setEnabled(hasMdiChild);
   moveLastAct->setEnabled(hasMdiChild);
   movePreAct->setEnabled(hasMdiChild);
   moveNextAct->setEnabled(hasMdiChild);
   moveGatherAct->setEnabled(hasMdiChild);
   pickLayerAct->setEnabled(hasMdiChild);
   pickMuteAct->setEnabled(hasMdiChild);
   winCascadeAct->setEnabled(hasMdiChild);
   winTileAct->setVisible(hasMdiChild);
   dragGroup->setEnabled(hasMdiChild);
   drawGroup->setEnabled(hasMdiChild);
   if (hasMdiChild)
   {
      compareGroup->setEnabled(activeMdiChild()->isCompareView());
   }
#if 0
   bool hasSelection = (activeMdiChild() &&
                        activeMdiChild()->textCursor().hasSelection());
   cutAct->setEnabled(hasSelection);
   copyAct->setEnabled(hasSelection);
#endif
}
void dvMainWin::updateWindowMenu()
{


   windowMenu->clear();
   windowMenu->addAction(winCascadeAct);
   windowMenu->addAction(winTileAct);
   windowMenu->addSeparator();

   QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
   //separatorAct->setVisible(!windows.isEmpty());

   for (int i = 0; i < windows.size(); ++i)
   {
      drawView *child = qobject_cast<drawView *>(windows.at(i)->widget());

      QString text;
#if 0
      if (i < 9)
      {
         text = tr("&%1 %2").arg(i + 1)
         .arg(child->m_dataName);
      }
      else
      {
         text = tr("%1 %2").arg(i + 1)
         .arg(child->m_dataName);
      }
#endif
      text = child->m_dataName;
      qDebug() << "text=" << text;
      QAction *action  = windowMenu->addAction(text);
      action->setCheckable(true);
      action->setChecked(child == activeMdiChild());
      connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
      windowMapper->setMapping(action, windows.at(i));
   }

}
void dvMainWin::createToolBars()
{
   QString str;
//file:
   fileToolBar = addToolBar(tr("File"));

   if (CONFIG_SEGY) fileToolBar->addAction(openSegyAct);
   if (CONFIG_GRISYS) fileToolBar->addAction(openGrisysAct);
   if (CONFIG_COMPARE) fileToolBar->addAction(compareAct);

   //fileToolBar->addAction(openGeoseisAct);
   fileToolBar->addSeparator();
   tapeToolBar = addToolBar(tr("Tape"));
//location:
   locationToolBar = addToolBar(tr("Location"));
   locationToolBar->addAction(moveFirstAct);
   locationToolBar->addAction(moveNextAct);
   locationToolBar->addAction(movePreAct);
   locationToolBar->addAction(moveLastAct);
   locationToolBar->addAction(moveGatherAct);
//view:
   viewToolBar = addToolBar(tr("View"));
   viewToolBar->addAction(superHeaderAct);
   viewToolBar->addAction(headerAct);
   viewToolBar->addAction(viewParameterAct);
//pick

   if (CONFIG_PICK)
   {
      pickToolBar = addToolBar(tr("Pick"));
      pickToolBar->addAction(pickLayerAct);
      //pickToolBar->addAction(pickMuteAct);
      pickToolBar->addAction(pickNoAct);
   }
//help:
   helpToolBar = addToolBar(tr("Help"));;
   helpToolBar->addAction(aboutAct);
   //helpToolBar->addAction(aboutAct);
   qDebug() << "create toolBar";
//draw type:
   drawToolBar = new QToolBar(tr("Draw"));
   addToolBar(Qt::LeftToolBarArea, drawToolBar);

   drawWAAct = drawToolBar->addAction(QIcon(":/images/drawwa.png"), "drawWA");
   connect(drawWAAct, SIGNAL(triggered()), windowMapper, SLOT(map()));
   windowMapper->setMapping(drawWAAct, PARAM_DRAW_WA);

   drawVDAct = drawToolBar->addAction(QIcon(":/images/drawvd.png"), "drawVD");
   connect(drawVDAct, SIGNAL(triggered()), windowMapper, SLOT(map()));
   windowMapper->setMapping(drawVDAct, PARAM_DRAW_VD);

   drawToolBar->addAction(outlookAct);

   drawVDAct->setCheckable(true);
   drawWAAct->setCheckable(true);
   drawGroup = new QActionGroup(this);
   drawGroup->setExclusive(true);
   drawGroup->addAction(drawVDAct);
   drawGroup->addAction(drawWAAct);
   drawVDAct->setChecked(true);


//drag:
   dragToolBar = new QToolBar(tr("Drag"));
   addToolBar(Qt::LeftToolBarArea, dragToolBar);
   dragHandAct = dragToolBar->addAction(QIcon(":/images/draghand.png"), "DragHand");
   connect(dragHandAct, SIGNAL(triggered()), windowMapper, SLOT(map()));
   windowMapper->setMapping(dragHandAct, PARAM_DRAG_HAND);
#if 0 //band
   dragRubberBandAct = dragToolBar->addAction(QIcon(":/images/dragrubber.png"), "DragRubberBand");
   connect(dragRubberBandAct, SIGNAL(triggered()), windowMapper, SLOT(map()));
   windowMapper->setMapping(dragRubberBandAct, PARAM_DRAG_RUBBER_BAND);
#endif
   dragNormalAct = dragToolBar->addAction(QIcon(":/images/dragnormal.png"), "DragNormal");
   connect(dragNormalAct, SIGNAL(triggered()), windowMapper, SLOT(map()));
   windowMapper->setMapping(dragNormalAct, PARAM_DRAG_NORMAL);

   dragHandAct->setCheckable(true);
//   dragRubberBandAct->setCheckable(true);
   dragNormalAct->setCheckable(true);

   dragGroup = new QActionGroup(this);
   dragGroup->setExclusive(true);
   dragGroup->addAction(dragHandAct);
//   dragGroup->addAction(dragRubberBandAct);
   dragGroup->addAction(dragNormalAct);
   dragNormalAct->setChecked(true);

//zoom:
   zoomToolBar = new QToolBar(tr("Zoom"));
   addToolBar(Qt::LeftToolBarArea, zoomToolBar);
   //zoomToolBar = addToolBar(tr("Zoom"));
   QAction *fit = zoomToolBar->addAction(QIcon(":/images/zoomfit.png"), "FitWindow");
   connect(fit, SIGNAL(triggered()), windowMapper, SLOT(map()));
   windowMapper->setMapping(fit, PARAM_ZOOM_FIT);
   QAction *norm = zoomToolBar->addAction(QIcon(":/images/zoomnormal.png"), "Normal");
   connect(norm, SIGNAL(triggered()), windowMapper, SLOT(map()));
   windowMapper->setMapping(norm, PARAM_ZOOM_NORMAL);
   // zoomout in
   QAction *zout = zoomToolBar->addAction(QIcon(":/images/zoomout.png"), "ZoomOut");
   connect(zout, SIGNAL(triggered()), windowMapper, SLOT(map()));
   windowMapper->setMapping(zout, PARAM_ZOOM_OUT);
   QAction *zin = zoomToolBar->addAction(QIcon(":/images/zoomin.png"), "ZoomIN");
   connect(zin, SIGNAL(triggered()), windowMapper, SLOT(map()));
   windowMapper->setMapping(zin, PARAM_ZOOM_IN);
   // zoomin h v
   QAction *zinV = zoomToolBar->addAction(QIcon(":/images/zoominv.png"), "ZoomIN_V");
   connect(zinV, SIGNAL(triggered()), windowMapper, SLOT(map()));
   windowMapper->setMapping(zinV, PARAM_ZOOM_INV);
   QAction *zinH = zoomToolBar->addAction(QIcon(":/images/zoominh.png"), "ZoomIN_H");
   connect(zinH, SIGNAL(triggered()), windowMapper, SLOT(map()));
   windowMapper->setMapping(zinH, PARAM_ZOOM_INH);
   // zoomout h v
   QAction *zoutV = zoomToolBar->addAction(QIcon(":/images/zoomoutv.png"), "ZoomOut_V");
   connect(zoutV, SIGNAL(triggered()), windowMapper, SLOT(map()));
   windowMapper->setMapping(zoutV, PARAM_ZOOM_OUTV);
   QAction *zoutH = zoomToolBar->addAction(QIcon(":/images/zoomouth.png"), "ZoomOut_H");
   connect(zoutH, SIGNAL(triggered()), windowMapper, SLOT(map()));
   windowMapper->setMapping(zoutH, PARAM_ZOOM_OUTH);

   configToolBar = addToolBar(tr("Config"));
   str = "USER Zoom scale: H:TRS/CM,V:CMS/S";
   configToolBar->addWidget(createEdit2Widget("USER:", str, "X", "Y", PARAM_ZOOM_USER, "10", "10", userXEdit, userYEdit));


// compare mode:
   compareToolBar = new QToolBar(tr("Compare"));
   addToolBar(Qt::LeftToolBarArea, compareToolBar);
   //compareToolBar = addToolBar(tr("Compare"));
   compareHAct = compareToolBar->addAction(QIcon(":/images/compareh.png"), "CompareH");
   connect(compareHAct, SIGNAL(triggered()), windowMapper, SLOT(map()));
   windowMapper->setMapping(compareHAct, PARAM_COMPARE_H);

   compareVAct = compareToolBar->addAction(QIcon(":/images/comparev.png"), "CompareV");
   connect(compareVAct, SIGNAL(triggered()), windowMapper, SLOT(map()));
   windowMapper->setMapping(compareVAct, PARAM_COMPARE_V);
   compareNOAct = compareToolBar->addAction(QIcon(":/images/compareno.png"), "CompareNO");
   connect(compareNOAct, SIGNAL(triggered()), windowMapper, SLOT(map()));
   windowMapper->setMapping(compareNOAct, PARAM_COMPARE_NO);
   compareTurnoverAct = compareToolBar->addAction(QIcon(":/images/compareturnover.png"), "CompareTURNOVER");
   connect(compareTurnoverAct, SIGNAL(triggered()), windowMapper, SLOT(map()));
   windowMapper->setMapping(compareTurnoverAct, PARAM_COMPARE_TURNOVER);

   compareTurnoverAct->setCheckable(true);
   compareHAct->setCheckable(true);
   compareVAct->setCheckable(true);
   compareNOAct->setCheckable(true);
   compareGroup = new QActionGroup(this);
   compareGroup->setExclusive(true);
   compareGroup->addAction(compareTurnoverAct);
   compareGroup->addAction(compareHAct);
   compareGroup->addAction(compareVAct);
   compareGroup->addAction(compareNOAct);

   compareVAct->setChecked(true);



//amplitude:
   ampToolBar = addToolBar(tr("Amplitude"));
   //QString str;
   str = "RMS Amplitude lever0-8:5";
   ampToolBar->addWidget(createSpinWidget("RMS", str, PARAM_AMP_RMS, 5, 0, 8, 1));
   str = "GAIN Amplitude lever0-10:0";
   ampToolBar->addWidget(createSpinWidget("GAIN", str, PARAM_AMP_GAIN, 0, 0, 10, 1));
   //ampToolBar->addWidget(createSpinWidget("AGC",PARAM_AMP_AGC,0,0,1000,100));
#if 0 //  AGC TEST
   str = "AGC window(ms) 0:no AGC";
   ampToolBar->addWidget(createEditWidget("AGC", str, PARAM_AMP_AGC, "0", agcEdit));
// testToolBar
   testToolBar = addToolBar(tr("Test"));
   str = "Test Slider:0-10";
   testToolBar->addWidget(createSliderWidget("TEST", str, PARAM_TEST, 0, 0, 10, 1));
#endif

#if 0 //   TEST
   testToolBar = addToolBar(tr("Test"));
   str = "Open all recentfile test";
   QAction *tAct = testToolBar->addAction(QIcon(":/images/open.png"), str);
   connect(tAct, SIGNAL(triggered()), windowMapper, SLOT(map()));
   windowMapper->setMapping(tAct, PARAM_TEST);

#endif
// config the toolbar
   helpToolBar->hide();
   //ampToolBar->hide();

   //dragToolBar->setAllowedAreas(Qt::LeftToolBarArea);
   //drawToolBar->setAllowedAreas(Qt::LeftToolBarArea);
   //zoomToolBar->setAllowedAreas(Qt::LeftToolBarArea);
   //compareToolBar->setAllowedAreas(Qt::RightToolBarArea);
   qDebug() <<  "toolbar created" << dragToolBar->allowedAreas();


}
QWidget* dvMainWin::createEdit2Widget(const QString& text, const QString& tip, const QString& text0, const QString& text1, int ty, QString v, QString v1, QLineEdit *& edit, QLineEdit *& edit1)
{
   QString str;
   str = tip; // "User Scale(H:trs/CM,V:CMs/S)";
   QLineEdit *le = new QLineEdit;
   QLineEdit *le1 = new QLineEdit;
   le->setMaximumWidth(LINEEDIT_WIDTH);
   le1->setMaximumWidth(LINEEDIT_WIDTH);
   le->setText(v);
   le1->setText(v1);
   edit = le;
   edit1 = le1;
   QGridLayout *layout = new QGridLayout;
   QToolButton *tb = new QToolButton();
   tb->setText(text);
   tb->setToolTip(str);
   //tb1->setText(text1);

   layout->addWidget(tb, 0, 0, Qt::AlignCenter);
   //layout->addWidget(new QLabel(text), 0, 0, Qt::AlignCenter);
   layout->addWidget(new QLabel(text0), 0, 1, Qt::AlignCenter);
   layout->addWidget(le, 0, 2, Qt::AlignHCenter);
   layout->addWidget(new QLabel(text1), 0, 3, Qt::AlignCenter);
   layout->addWidget(le1, 0, 4, Qt::AlignHCenter);

   QWidget *widget = new QWidget;
   widget->setLayout(layout);
   //connect(box, SIGNAL(valueChanged(int)),this, SLOT(slotSpinValue(int )));
   //connect(le, SIGNAL(textChanged ( const QString)), windowMapper, SLOT(map()));
   //windowMapper->setMapping(le, ty);
   //connect(tb,  SIGNAL(triggered(QAction * action )), windowMapper, SLOT(map()));
   connect(tb,  SIGNAL(clicked()), windowMapper, SLOT(map()));
   windowMapper->setMapping(tb, ty);
   return widget;
}
QWidget* dvMainWin::createEditWidget(const QString& text, const QString& tip, int ty, QString v, QLineEdit *& edit)
{
   QString str;
   str = tip;
   QLineEdit *le = new QLineEdit;
   le->setMaximumWidth(LINEEDIT_WIDTH);
   le->setText(v);
   edit = le;
   QGridLayout *layout = new QGridLayout;
   QToolButton *tb = new QToolButton();
   tb->setText(text);
   tb->setToolTip(str);

   layout->addWidget(tb, 0, 0, Qt::AlignCenter);
   layout->addWidget(le, 0, 1, Qt::AlignHCenter);
   QWidget *widget = new QWidget;
   widget->setLayout(layout);
   //connect(box, SIGNAL(valueChanged(int)),this, SLOT(slotSpinValue(int )));
   //connect(le, SIGNAL(textChanged ( const QString)), windowMapper, SLOT(map()));
   //windowMapper->setMapping(le, ty);
   //connect(tb,  SIGNAL(triggered(QAction * action )), windowMapper, SLOT(map()));
   connect(tb,  SIGNAL(clicked()), windowMapper, SLOT(map()));
   windowMapper->setMapping(tb, ty);
   return widget;
}
QWidget* dvMainWin::createSpinWidget(const QString& text, const QString& tip, int ty, int v, int min, int max, int step)
{
   QString str;
   str = tip;
   QSpinBox *box = new QSpinBox;
   box->setValue(v);
   box->setMinimum(min);
   box->setMaximum(max);
   box->setSingleStep(step);
   box->setToolTip(str);
   QGridLayout *layout = new QGridLayout;

   layout->addWidget(new QLabel(text), 0, 0, Qt::AlignCenter);
   layout->addWidget(box, 0, 1, Qt::AlignHCenter);
   QWidget *widget = new QWidget;
   widget->setLayout(layout);
   //connect(box, SIGNAL(valueChanged(int)),this, SLOT(slotSpinValue(int )));
   connect(box, SIGNAL(valueChanged(int)), windowMapper, SLOT(map()));
   windowMapper->setMapping(box, ty);

   return widget;
}
QWidget* dvMainWin::createSliderWidget(const QString& text, const QString& tip, int ty, int v, int min, int max, int step)
{
//   qDebug() << "slider";
   QString str;
   str = tip;
   QSlider *box = new QSlider;
   box->setValue(v);
   box->setMinimum(min);
   box->setMaximum(max);
   box->setSingleStep(step);
   box->setToolTip(str);
   box->setOrientation(Qt::Horizontal);
   QGridLayout *layout = new QGridLayout;
//qDebug() << "slider";
   layout->addWidget(new QLabel(text), 0, 0, Qt::AlignCenter);
   layout->addWidget(box, 0, 1, Qt::AlignHCenter);
   QWidget *widget = new QWidget;
   widget->setLayout(layout);
//   qDebug() << "slider";
//connect(box, SIGNAL(valueChanged(int)),this, SLOT(slotSpinValue(int )));
   connect(box, SIGNAL(valueChanged(int)), windowMapper, SLOT(map()));
   windowMapper->setMapping(box, ty);
//qDebug() << "slider" <<widget;
   return widget;
}
void dvMainWin::createStatusBar()
{
   statusBar()->showMessage(tr("Ready"));
   //qDebug() << "create StatusBar";
}
void dvMainWin::slotShowHeader(int i)
{
   QMap<QString, double> map;
   map = DOC->m_data->mapHeader(i);
   headDlg->setData(map);
   headDlg->show();

}
drawView* dvMainWin::createMdiChild()
{
   if (activeMdiChild()) if (drawview != NULL) // get display parameters from current view
         getParam(); // in create MidCjild
   drawView *child = new drawView(this);
   mdiArea->addSubWindow(child);
   // scroll sync:synchronized
   connect(child, SIGNAL(sigMovedTo(int, int)), this, SLOT(slotViewMovedTo(int, int)));
   connect(child, SIGNAL(sigClosed(drawView *)), this, SLOT(slotViewClosed(drawView *)));
   connect(child, SIGNAL(sigShowHeader(int)), this, SLOT(slotShowHeader(int)));

   //setParam();
   child->show();
   mdiArea->tileSubWindows();
   //mdiArea->cascadeSubWindows();
   return child;
}
void dvMainWin::slotViewClosed(drawView *v)
{

   int num, i;
   //drawView *sender = qobject_cast<drawView *>(this->sender());
   //drawView *obj = qobject_cast<drawView *>(v);
   drawView *view;
   drawView *sender = v;
   QList<drawView *> vlist;
   //qDebug() << " destroyed = " << sender << v;
   vlist = getViewList();
   num = vlist.size();
   // if the compare view destroyed ,we ensure the view not compare;
   //qDebug() << "num of Views =" <<num;

   // current view and data set to NULL;
   if (sender == drawview)
   {
      delete drawview;

      drawview = NULL;
      DOC->m_data = NULL;
   }
   //we ensure the view not compare;
   for (i = 0; i < num; i++)
   {
      view = vlist[i];
      //qDebug() << " views = " <<i<<view;
      if (view->isCompareView())
      {
         if (view->m_comView == sender)
         {
            //qDebug() << "NULL the comView = " << view->m_comView;
            view->m_comView = NULL;
         }
      }

   }
   qDebug() << "no view not num =" << num;
   if (num <= 1)
   {
      qDebug() << "no view not";
      setConfig();
   }

}
drawView* dvMainWin::activeMdiChild()
{
   if (QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow()) return qobject_cast<drawView *>(activeSubWindow->widget());
   return NULL;
}
QMdiSubWindow* dvMainWin::findMdiChild(const QString& fileName)
{
   QString file = QFileInfo(fileName).canonicalFilePath();

   foreach(QMdiSubWindow * window, mdiArea->subWindowList())
   {
      drawView *mdiChild = qobject_cast<drawView *>(window->widget());
      if (mdiChild->m_dataName == file) return window;
   }
   return NULL;
}
QList<drawView *> dvMainWin::getViewList()
{
   QList<drawView *> vlist;
   foreach(QMdiSubWindow * window, mdiArea->subWindowList())
   {
      drawView *mdiChild = qobject_cast<drawView *>(window->widget());
      vlist.append(mdiChild);
   }
   return vlist;
}
drawView* dvMainWin::findCompareWin()
{
   foreach(QMdiSubWindow * window, mdiArea->subWindowList())
   {
      drawView *mdiChild = qobject_cast<drawView *>(window->widget());
      if (mdiChild->m_data == NULL) return mdiChild;
   }
   return NULL;

}
void dvMainWin::switchLayoutDirection()
{
   if (layoutDirection() == Qt::LeftToRight) qApp->setLayoutDirection(Qt::RightToLeft);
   else qApp->setLayoutDirection(Qt::LeftToRight);
}

void dvMainWin::setActiveSubWindow(QWidget *window)
{
   if (!window) return;
   mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
}
void dvMainWin::slotOpenSegd()
{

   QPixmap shotIcon;
   QString dir, str, filter, title, s;
   title = "Open File;";
   qDebug() << "open segd";
   dir = lastDir;
   filter = QString("Tape devices (*);; All files (*.*)");
   //dir = data->m_strDir;

   s = QFileDialog::getOpenFileName(this, title, dir, filter);
   if (s.isEmpty()) return;

   DOC->m_dataType = SEGD_DATA;
   DOC->m_dataName = s;
   openFile(s);
   return;
}
void dvMainWin::slotOpenFile()
{

   QPixmap shotIcon;
   QString dir, str, filter, title, s;
   title = "Open File;";
   dir = lastDir;;
   filter = QString("Segy files (*.sgy);; All files (*.*)");
   //dir = data->m_strDir;

   s = QFileDialog::getOpenFileName(this, title, dir, filter);
   if (s.isEmpty()) return;


   DOC->m_dataType = SEGY_DATA;
   DOC->m_dataName = s;
   openFile(s);
   return;
}
void dvMainWin::slotOpenGrisysFile()
{

   QPixmap shotIcon;
   QString dir, str, filter, title, s;
   title = "Open File;";
   dir = lastDir;
   filter = QString("Grisys files (*.gri);; All files (*.*)");
   //dir = data->m_strDir;

   s = QFileDialog::getOpenFileName(this, title, dir, filter);
   if (s.isEmpty()) return;

   DOC->m_dataType = GRISYS_DATA;
   DOC->m_dataName = s;
   openFile(s);
   return;
}
void dvMainWin::slotRecentFile(QString s)
{
   int ty;
   if (s == DOC->m_failedFile) return; // void last failed rencentfile enter several times
   qDebug() << "slot REcent ======================= " << s;
   if (findMdiChild(s) != NULL) return; // avoid open the same file sever times.

   ty = recentFile.typeOfFile(s);
   qDebug() << "slotRecentFile " << s << ty;
   DOC->m_dataType = ty;
   DOC->m_dataName = s;
   openFile(s);
   qDebug() << "slotRecentFile End ";
}
void dvMainWin::openFile(QString filen)
{
   if (findMdiChild(filen) != NULL) return; // avoid open the same file sever times.
   qDebug() << " openFile =" << filen;
   QFileInfo fi(filen);
   lastDir = fi.path();

   seisData *newData;
   if (DOC->m_dataType == GRISYS_DATA)
   {
      newData = new grisysData();
      newData->setDataType(GRISYS_DATA);

   }

   if (DOC->m_dataType == SEGY_DATA)
   {
      newData = new segyData();
      newData->setDataType(SEGY_DATA);
   }

   if (DOC->m_dataType == SEGD_DATA)
   {
      newData = new segdData();
      newData->setDataType(SEGD_DATA);
   }
//   recentFile.addFile(filen,DOC->m_dataType);
//   recentFile.reFreshMenu(recentMenu,this,windowMapper);
   openFile(newData);
}
void dvMainWin::openFile(seisData *newData)
{
   QString str, fname;
   int idd;
   fname = DOC->m_dataName;

// for common seisData
//
// new data:
//   seisData *newData;
//   newData = new segyData();
   idd = newData->openData(fname, "r");
   if (idd < 0)
   { // cannot open segy file;
      DOC->m_failedFile = fname;
      str = QString("%1,%2").arg(idd).arg(fname);
      QMessageBox::warning(this, "Cannot open file", "Status = " + str);
      return;
   }
   else
   { //open ok;
      qDebug() << "openfile ok";
   }
   qDebug() << "openfile newData=" << newData;
   // move from openFile(QString)
   recentFile.addFile(fname, DOC->m_dataType);
   recentFile.reFreshMenu(recentMenu, this, windowMapper);

   DOC->setData(newData);

   int sam;
   sam = newData->getSamples();


   //newData->setDisplayHD("HD4");


// create draw view:

   drawview = createMdiChild();
   qDebug() << "after create";
   drawview->setData(DOC->m_data);
   drawview->setDataPara(sam, (int)DOC->m_data->getSI(), DOC->m_data->getHeadLen());

   //connect(drawview, SIGNAL(sigMovedBy(int,int)), this, SLOT(slotViewMovedTo(int ,int)));
// draw:
   loadPickFromFile();
   qDebug() << "after loadPick";
   initNavi();

   // here setParam is dangrous:move it to after  slotMoveFirst();
   //setParam();
   qDebug() << "before move first";
   if (DOC->m_data->getDevType() == DEV_TAPE)
       slotMoveNext();
   else
       slotMoveFirst();
   qDebug() << "after move first";
   setParam(); //
   qDebug() << "after set Params";
   qDebug() << " dataType  ============= " << DOC->m_data->getDataType();

   return;
   if (CONFIG_SAME_TYPE)
   {
      if (DOC->m_data->getDataType() == SEGD_DATA)
      {
         opTape(true);
      }
      else opTape(false);
   }

   return;
}
void dvMainWin::initNavi()
{
   if (DOC->m_data->getDevType() == DEV_TAPE) return;
   //int num;
   QList<int> glist;

   naviWin *naviView = new naviWin(this);
   naviDockWidget->setWidget(naviView);
   drawview->nvView = naviView;
   connect(naviView, SIGNAL(sigMove(int)),
           this, SLOT(slotMoveIndex(int)));
   naviView->init();


   //qDebug() << "initVani: start";
#if 0
   num = DOC->m_data->getGathers();
   glist = DOC->m_data->getGatherList();
   int i;
   //float *f;
   float min, max;
   naviView->idxBuf = new float[num]; //first array
   naviView->curBuf = new float[num]; //first array
   naviView->pickBuf = new float[num]; //first array
   max = -10;
   min = 10000;

   //qDebug() << "glist.size = " << glist.size();
   for (i = 0; i < num; i++)
   {
      naviView->idxBuf[i] = glist[i];
      naviView->curBuf[i] = 0;
      naviView->pickBuf[i] = 0;
      if (max < naviView->idxBuf[i]) max = naviView->idxBuf[i];
      if (min > naviView->idxBuf[i]) min = naviView->idxBuf[i];

   }
   //naviView->curBuf[1] = glist[1];
   //naviView->pickBuf[4] = glist[4];

   naviView->setMaxMin(0, min, num - 1, max);
   QString key;
   key = DOC->m_data->getGather();
   key = NV_IDX;
   naviView->setColorBar(0);
   //qDebug() << "initVani: start draw";

   naviView->setGrid(5, 10, 5, 10);
   //naviView->setScale(5,5);
   naviView->setDrawStyle(2, 0, 1, 1, 7);
   naviView->setFitWin();
   naviView->setCoordinate(RIGHT_UP); // last set
   naviView->startDraw(naviView->idxBuf, num, key); //first array
   key = NV_PICK;
   naviView->setDrawStyle(2, 0, 1, 1, 2);
   naviView->startDraw(naviView->pickBuf, num, key); //first array
   key = NV_CUR;
   naviView->setDrawStyle(2, 0, 1, 1, 1);
   naviView->startDraw(naviView->curBuf, num, key); //first array
                                                    //qDebug() << "initVani: end";
#endif
}
void dvMainWin::slotParamChanged(QtProperty *pt)
{
   QString name, str, str1;
   QStringList slist;
   int idx, id;
   name = pt->propertyName();
   if (WIN == NULL) return;

   qDebug() << "changed name = " << name;
// displayHD:
   if (name == "displayHD")
   { //drawview->setPickMode
     //if (drawview->getPickMode() != PICK_MODE_LAYER) return;
      str1 = DOC->m_data->getDisplayHD();
      str = pt->valueText();
      qDebug() << "hd = " << str1 << str;
      if (str1 != str)
      {
         if (DOC->m_data->pickData.size() > 0)
         {
            id = QMessageBox::question(this,  str1 + "-->" + str,
                                       "display Header changed will Mess pick result \n do you really wangt to chang??");
            if (id == QMessageBox::Yes)
            {
               qDebug() << "yes=" << id;
               // we are not clear the contains of the pick data here;

               DOC->m_data->setDisplayHeader(str); //set hd and data
               qDebug() << "yes= after setDisplayHeader";
               qDebug() << "yes= after setDisplayHeader = " << DOC->m_data->getDisplayHeader();
               drawview->setDisplayHeader(DOC->m_data->getDisplayHeader());
               qDebug() << "yes= after setDisplayHeader";
            }
            else
            {
               qDebug() << "NO=" << id;
               // set back;
               idx = DOC->m_data->m_mapHeader.keys().indexOf(str1);
               paramView->setPropertyEV(name, idx);
               paramDockWidget->hide();
               paramDockWidget->show();
               //return;
            }
            setTopLabel();
         }
         else // no pick
         {
            DOC->m_data->setDisplayHeader(str); //set hd and data
            drawview->setDisplayHeader(DOC->m_data->getDisplayHeader());
         }
      }
   }
// timeLine:
   if (name == "timeLine")
   {

      idx = ((QtEnumPropertyManager *)pt->propertyManager())->value(pt);
      qDebug() << "timeLine  = " << idx;
      drawview->setGrid(idx);
   }
// pickTo:
// qDebug() << "pickto1 = " << name;
   if (name == "pickTo")
   {
      idx = ((QtEnumPropertyManager *)pt->propertyManager())->value(pt);
      id =  drawview->getPickTo();
      if (idx != id)
      {
         drawview->setPickTo(idx);
      }
   }
// sort data:
   if (name == "sortData")
   {
      idx = ((QtEnumPropertyManager *)pt->propertyManager())->value(pt);
      id =  getSortData();
      if (idx != id)
      {
         setSortData(idx);
         moveData(MOVE_INDEX, DOC->m_data->m_iCurIndex);
      }
   }
   // Label:
   int x1, x2, y1, y2;
   if (name == "label")
   {
      str = pt->valueText();
      slist = str.split(",");
      if (slist.size() < 4) return;
      x1 = slist[0].toInt();
      x2 = slist[1].toInt();
      y1 = slist[2].toInt();
      y2 = slist[3].toInt();
      if (x1 <= 0 || x2 <= 0 || y1 <= 0 || y2 <= 0) return;
      drawview->setGrid(x1, x2, y1, y2);
   }
// filter:
   if (name == "filter")
   {
      str = pt->valueText();
      slist = str.split(",");
      if (slist.size() < 4) return;
      x1 = slist[0].toInt();
      x2 = slist[1].toInt();
      y1 = slist[2].toInt();
      y2 = slist[3].toInt();
      //if (x1<=0 || x2<=0||y1<=0 ||y2<=0¡¡¡¡) return;
      drawview->setFilter(x1, x2, y1, y2);
      //moveData(MOVE_INDEX, DOC->m_data->m_iCurIndex);
      drawData();
   }
   // AGC
   if (name == "AGC")
   {

      str = pt->valueText();
      x1 = str.toInt();
      x2 = drawview->getAGC();
      qDebug() << "  AGC changed=" << x1 << x2;
      if (x1 != x2)
      {
         drawview->setAGC(x1);
         qDebug() << "set AGC=" << x1;
         ///moveData(MOVE_INDEX, DOC->m_data->m_iCurIndex);// cjh: redrawagc
         drawData();
      }
   }
   drawview->reDraw();
   getParam();
   qDebug() << "slot ParamChanged : after getParam";
// sync to all views:
   int num, j;
   drawView *view;
   viewList = getViewList();
   num = viewList.size();
   for (j = 0; j < num; j++)
   {
      view = viewList[j];
      if (view != drawview)
      {
         setParam(view);
         qDebug() << "slot ParamChanged :views = " << j << num << view << view->getAGC();
         qDebug() << "slot ParamChanged :file = " << view->m_dataName;
         view->startDraw();
         view->reDraw();
      }

   }
   qDebug() << "slot ParamChanged : end";
}
void dvMainWin::setParam()
{

   QString str;
   int idx, v;
   float sx, sy;
   setParam(drawview); // in setParam()

   qDebug() << "after set parm(dataview)";

//display HD
   str = DOC->m_data->getDisplayHD();
   idx = DOC->m_data->m_mapHeader.keys().indexOf(str);
   paramView->setPropertyEV(QString("displayHD"),  idx, DOC->m_data->m_mapHeader.keys());
//agc
   paramView->setPropertyV(QString("AGC"), DOC->getParam()->agc);

//rms
   QSpinBox *b = (QSpinBox *)windowMapper->mapping(PARAM_AMP_RMS);
   v = DOC->getParam()->rms;
   b->setValue(v);
//gain
   b = (QSpinBox *)windowMapper->mapping(PARAM_AMP_GAIN);
   v = DOC->getParam()->gain;
   b->setValue(v);
//drawType:
   v = DOC->getParam()->drawType;
   if (v == DRAW_WA) drawWAAct->setChecked(true);
   if (v == DRAW_VD) drawVDAct->setChecked(true);
//compareMode
   v = DOC->getParam()->compareMode;
   if (v == COMPARE_MODE_H) compareHAct->setChecked(true);
   if (v == COMPARE_MODE_NO) compareNOAct->setChecked(true);
   if (v == COMPARE_MODE_V) compareVAct->setChecked(true);
   if (v == COMPARE_MODE_TURNOVER) compareTurnoverAct->setChecked(true);
// scaleX,Y
   drawview->getUserScale(sx, sy);
   str = QString("%1").arg(sx);
   userXEdit->setText(str);
   str = QString("%1").arg(sy);
   userYEdit->setText(str);

}
void dvMainWin::setParam(drawView *view)
{
   qDebug() << "int setparam,view = " << view;
   if (view == NULL) return;
   qDebug() << "int setparam,view = OK ";
   DOC->getParam()->setParams(view); // in setParams

}
void dvMainWin::getParam()
{
   getParam(drawview); // in setParam()
}
void dvMainWin::getParam(drawView *view)
{
   if (view == NULL) return;
   DOC->getParam()->getParams(view);
}

void dvMainWin::slotCompare()
{
   int sam, num;

   if (DOC->m_data == NULL) return;
// if the compare view exist,if not creat it:
   drawView *view = findCompareWin();
   if (view == NULL) view = createMdiChild();

//get the copare data from current data;
   sam = DOC->m_data->getSamples();
   view->setDataPara(sam, (int)DOC->m_data->getSI(), DOC->m_data->getHeadLen());
//save the data:
   if (view->comBuf != NULL) delete view->comBuf;
   if (view->comHeader != NULL) delete view->comHeader;
   float *bufnew = new float[sam * DOC->m_data->m_iMaxTrsOfGather];
   int *hdnew = new int[DOC->m_data->getHeadLen() * DOC->m_data->m_iMaxTrsOfGather];
   view->comBuf = bufnew;
   view->comHeader = hdnew;
   num = DOC->m_data->getCurTrs();

   memcpy(view->comBuf, DOC->m_data->ptrace, sam * sizeof(float) * num);
   memcpy(view->comHeader, DOC->m_data->pheader, DOC->m_data->getHeadLen() * sizeof(int) * num);


//draw: the compare view

   // qDebug() << "copy compare data buf = " <<  view->comBuf;

   view->setMaxMin(0, 0, num, DOC->m_data->getLTR()); // make it careless of SI
   view->startDraw((int *)view->comHeader, view->comBuf, num);
   view->setDisplayHeader(DOC->m_data->getDisplayHeader());
   //view->startDraw((int *)DOC->m_data->pheader, DOC->m_data->ptrace, num);
   view->m_dataName = QString("Compare ") + DOC->m_data->m_dataName;
   setTopLabel(view);
   view->setTitle();
   //setActiveSubWindow(view);
   setParam(view); //in slotCompare()
                   //qDebug() << "this is Compare";
}
void dvMainWin::slotSaveFile()
{


}
void dvMainWin::slotSaveAsFile()
{
   QString strt, strd, strf, str;
   QPixmap originalPixmap;

   originalPixmap = QPixmap(); // clear image for low memory situations
                               // on embedded devices.
   originalPixmap = QPixmap::grabWindow(drawview->viewport()->winId());

   QString format = "png";
   QString initialPath = QDir::currentPath() + tr("/untitled.") + format;

   QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                                                   initialPath,
                                                   tr("%1 Files (*.%2);;All Files (*)")
                                                   .arg(format.toUpper())
                                                   .arg(format));
   if (!fileName.isEmpty()) originalPixmap.save(fileName, format.toUtf8().data());

}
void dvMainWin::slotHelp()
{
   QString cmd, str;

   cmd = "showpdf ";
   //cmd = cmd+ getDocDir()+ SLASH + "dataview.pdf";
   //cmd = "ls";
   qDebug() << "cmd=" << cmd;
   hrun.start(cmd);
}
void dvMainWin::slotAbout()
{
   QString title, str, str1;
   title = ORG_NAME;
   str1 = __DATE__;
   str = QString("App name : ") + QString(APP_NAME) + "\n" +
      QString("App version : ") + QString(APP_VERSION) + "\n" +
      QString("App Date : ") + str1;
   QMessageBox::about(this, title, str);
}

void dvMainWin::slotMoveIndex(int idx)
{
   moveData(MOVE_INDEX, idx);

}
int dvMainWin::moveData(int gowhere, int data)
{
   int num; //, gather;
   QString str;
   //gather = 0;
   num = data;

   if (DOC->m_data == NULL) return -1;
   //qDebug() << "save0";
   savePickToMap();
   //savePickToFile();
   qDebug() << "save1 where,data, devType = " << gowhere << data << DOC->m_data->getDevType();

   if (DOC->m_data->getDevType() == DEV_TAPE) switch (gowhere)
      {

      case MOVE_NEXT:
         num = DOC->m_data->readNextGather(DOC->m_data->ptrace, DOC->m_data->pheader);
         break;
      case MOVE_LAST:
      case MOVE_PRE:
      case MOVE_FIRST:
      case MOVE_GATHER:
      case MOVE_INDEX:
      str = "Get gather error!! ";
      statusBar()->showMessage(str);
         break;
      }
   else switch (gowhere)
      {
      case MOVE_FIRST:
         num = DOC->m_data->readFirstGather(DOC->m_data->ptrace, DOC->m_data->pheader);
         break;
      case MOVE_LAST:
         num = DOC->m_data->readLastGather(DOC->m_data->ptrace, DOC->m_data->pheader);
         break;
      case MOVE_PRE:
         num = DOC->m_data->readPreGather(DOC->m_data->ptrace, DOC->m_data->pheader);
         break;
      case MOVE_NEXT:
         num = DOC->m_data->readNextGather(DOC->m_data->ptrace, DOC->m_data->pheader);
         break;
      case MOVE_GATHER:
         num = DOC->m_data->readGather(data, DOC->m_data->ptrace, DOC->m_data->pheader);
         break;
      case MOVE_INDEX:
         num = DOC->m_data->readGatherIndex(data, DOC->m_data->ptrace, DOC->m_data->pheader);
         break;
      }

   qDebug() << "after read num = " << num ;
   if (num <= 0)
   {
      str = "Read gather error,ret = " + QString("%1").arg(num);
      statusBar()->showMessage(str);
      return -1;
   }
   //DOC->m_data->mapHeader();//in segy
   // DOC->m_data->setCurTrs(num);// in segy
   //DOC->m_data->setDisplayHeader();//in segy

   DOC->m_data->sortData(DOC->m_iSortData);
   qDebug() << "before drawData";
   drawData();
   //qDebug() << "after drawData";


   return 0;
}
void dvMainWin::drawData()
{
   qDebug() << "drawData: start";
   if (DOC->m_data->getDevType() == DEV_DISK) 
   {
       if ( drawview->nvView == NULL)
       {
           initNavi();
       }
       drawview->nvView->draw();
   }
   qDebug() << "drawData:   naviDraw";
   drawData(drawview);

   qDebug() << "drawData: after dataDraw";
   loadPickFromMap();
   qDebug() << "drawData: after load";

}
void dvMainWin::drawData(drawView *view)
{
   // draw data:
   int num;
   if (view == NULL) return;
   if (DOC->m_data == NULL) return;

   num = DOC->m_data->getCurTrs();
   //drawview->setMaxMin(0, 0, num, DOC->m_data->getSamples());
   qDebug() << "drawData(drawView *view) = " << view;
   qDebug() << "drawData(drawView *view) trs = " << num << DOC->m_data->getLTR();
   view->setMaxMin(0, 0, num, DOC->m_data->getLTR()); // make it careless of SI
   float *trace;
   int *header;
   trace =  DOC->m_data->ptrace;
   header = (int *)DOC->m_data->pheader;

   qDebug() << "drawData(drawView *view) startDraw " << header << trace << num;
   view->startDraw(header, trace, num);
   qDebug() << "drawData(drawView *view) setDisplayHeader ";
   view->setDisplayHeader(DOC->m_data->getDisplayHeader());
   qDebug() << "drawData(drawView *view) toplabel ";
   //view->startDraw((int *)DOC->m_data->pheader, DOC->m_data->ptrace, num);
   setTopLabel();
   qDebug() << "drawData(drawView *view) end ";

}
void dvMainWin::setView(drawView *v)
{
   if(drawview->nvView == NULL) return;
   drawview = v;
   naviDockWidget->setWidget(drawview->nvView);
}
#if 0
void dvMainWin::reDraw() // removed
{
   drawview->reDraw();
}
#endif
void dvMainWin::setTopLabel()
{
   setTopLabel(drawview);
}
void dvMainWin::setTopLabel(drawView *view)
{
   QString str;
   if (DOC->m_data == NULL) return;

   qDebug() << "setTopLabel : shot = " << DOC->m_data->m_iCurGather;
   str = DOC->m_data->getGather() + "=" + QString("%L1").arg(DOC->m_data->m_iCurGather);
   str = str + ":" + DOC->m_data->getDisplayHD();
   view->setTopMessage(str);
}
void dvMainWin::slotMoveFirst()
{
   moveData(MOVE_FIRST);
}
void dvMainWin::slotMoveLast()
{
   moveData(MOVE_LAST);
}
void dvMainWin::slotMovePre()
{
   moveData(MOVE_PRE);

}
void dvMainWin::slotMoveNext()
{
   moveData(MOVE_NEXT);
}
void dvMainWin::slotMoveGather()
{
   if (DOC->m_data->getDevType() == DEV_TAPE) return;
   //moveData(MOVE_GATHER);
   bool b; //paramDockWidget
   b = naviDockWidget->isVisible();
   if (b) naviDockWidget->hide();
   else naviDockWidget->show();

}
void dvMainWin::slotMovePercent()
{
   moveData(MOVE_PERCENT);
}
void dvMainWin::slotSuperHeader()
{

   QString str;
   str = QString("superHeader");
   drawview->setShowHeader(false);
   headDlg->setWindowTitle(str);
   headDlg->setData(DOC->m_data->m_mapSuperHeader);
   headDlg->show();
}
void dvMainWin::slotHeader()
{
   QString str;
   str = QString("Header");
   //drawview->setShowHeader(true);
   activeMdiChild()->setShowHeader(true);
   //qDebug() << activeMdiChild() << drawview;
   headDlg->setWindowTitle(str);
   headDlg->show();
}
void dvMainWin::slotCloseHeader()
{
   //qDebug() << "close header";
   //drawview->setShowHeader(false);
   activeMdiChild()->setShowHeader(false);
   //qDebug() << activeMdiChild() << drawview;
}
void dvMainWin::slotViewParameter()
{
   bool b;
   b = paramDockWidget->isVisible();
   if (b) paramDockWidget->hide();
   else paramDockWidget->show();

   //pickMuteAct->setCheckable(true);
}
void dvMainWin::slotPickLayer()
{

   pickLayerAct->setChecked(true);
   drawview->setPickMode(PICK_MODE_LAYER);

}
void dvMainWin::slotPickMute()
{

   pickMuteAct->setChecked(true);
   drawview->setPickMode(PICK_MODE_MUTE);

}
void dvMainWin::slotPickNo()
{

   pickNoAct->setChecked(true);
   drawview->setPickMode(PICK_MODE_NO);;

}
void dvMainWin::slotViewMovedTo(int dx, int dy)
{

   drawView *sender = qobject_cast<drawView *>(this->sender());
   //qDebug()<<"sender = " << sender;

   foreach(QMdiSubWindow * window, mdiArea->subWindowList())
   {
      drawView *mdiChild = qobject_cast<drawView *>(window->widget());
      if (mdiChild != sender)
      {
         mdiChild->slotMovedTo(dx, dy);
         //qDebug()<<"receiver = " << mdiChild;
      }
   }
}
void dvMainWin::slotActiveWin(QMdiSubWindow *win)
{
   updateMenus();
   qDebug() << "active Win = " << win;
   if (win == NULL)
   { // no active widow now;
      drawview = NULL;
      DOC->setData(NULL);
      return;
   }

   drawView *mdiChild = qobject_cast<drawView *>(win->widget()); // win is not mdiChild
   if (!mdiChild->data->m_bStart) return;  // first create data not set yet                                                            //qDebug() << "actived win = " << mdiChild->m_dataName;
                                           //set view ,data;
  qDebug() << "active View &data = " << mdiChild << mdiChild->getData() ;
   if (mdiChild->getData() != NULL) // not for compare win:
   {
      // set curren data and view, headdlg:
      setView(mdiChild);
      DOC->setData(mdiChild->getData()); // current data
                                         //headDlg->setRows(DOC->m_data->mapHeader(1));//current data to setup dlg
      setTitle();

   }
   else
   {
      // prepare for compare: get the currnt view' drawing image:
      if (drawview != NULL)
      {
         //qDebug() << " get compare img" << drawview << ((sectPaint *)drawview->getDrawing())->img;
         //mdiChild->m_comDrawing = (sectPaint* )drawview->getDrawing();
         //mdiChild->data->m_bStart = true;
         mdiChild->m_comView = drawview; // point to current view;
                                         //mdiChild->m_paint->imgCom = mdiChild->m_comDrawing->img;
                                         // point to compareing image.
         mdiChild->m_paint->imgCom = ((sectPaint *)drawview->getDrawing())->img;

      }
   }
   qDebug() << "slotActiveWin  end  " ;
}
void dvMainWin::slotTest()
{

#if 0
   for (int i = 0; i < recentFile.getFileList().size(); i++)
   {
      slotRecentFile(recentFile.getFileList().at(i).file);
   }
#endif
}
void dvMainWin::slotParam(int i)
{
   QString x, y;
   bool b;
   int num, j;
   float xx, yy, v, sx, sy;
   drawView *view;
   viewList = getViewList();
   num = viewList.size();
   if (i == PARAM_TEST)
   {
      slotTest();
      return;
   }
   for (j = 0; j < num; j++)
   {
      view = viewList[j];
      if (view == NULL) continue;
//Amplitude:
//-=================================================
//qDebug() << "param = " << i;
// tape begin
      if (i == PARAM_TAPE_SKIP)
      {
         QSpinBox *b = (QSpinBox *)windowMapper->mapping(PARAM_TAPE_SKIP);
         //qDebug() << " rmx text = " << b->value();
         v = b->value();
         qDebug() << "tape skip = " << v;
         //DOC->m_iTapeSkip = v;
         DOC->m_data->m_iMyData = v;
      }
// end of tape:
      if (i == PARAM_AMP_RMS)
      {
         QSpinBox *b = (QSpinBox *)windowMapper->mapping(PARAM_AMP_RMS);
         //qDebug() << " rmx text = " << b->value();
         v = b->value();
         view->setRMS(10. - v);
      }
      if (i == PARAM_AMP_GAIN)
      {
         QSpinBox *b = (QSpinBox *)windowMapper->mapping(PARAM_AMP_GAIN);
         //qDebug() << " gain text = " << b->value();
         v = b->value();
         view->setGain(v);
      }
      //if (i == PARAM_AMP_AGC)
      //{
      //qDebug() << "agc idx = " << i << agcEdit;
      //QLineEdit *b = agcEdit;
      //qDebug() << " gagc text = " << b->text();
      //}
      //=========================================
// test:
      if (i == PARAM_TEST)
      {
         QSlider *b = (QSlider *)windowMapper->mapping(PARAM_TEST);
         //qDebug() << " gain text = " << b->value();
         v = b->value();
         qDebug() << "test = " << v;
         //view->setGain(v);
      }
//Zoom:
//-=================================================
      if (i == PARAM_ZOOM_USER)
      {
         x = userXEdit->text();
         y = userYEdit->text();
         xx = x.toFloat();
         yy = y.toFloat();

         view->setUserScale(xx, yy);
         view->getScale(sx, sy);
         //qDebug() << "USER float x,y =" << xx << yy  << sx << sy;
         view->getUserScale(sx, sy);
         //qDebug() << "get USER scale x,y =" << sx << sy;

      }
      //----------------------------------
      if (i == PARAM_ZOOM_FIT)
      {
         //qDebug() << "yes autoZ";
         view->setFitWin();
      }
      //------------------------------------
      if (i == PARAM_ZOOM_NORMAL)
      {
         view->setUserScale(ZOOM_NORMAL_X, ZOOM_NORMAL_Y);
      }
      //------------------------------------
      if (i == PARAM_ZOOM_IN)
      {
         if (j == 0) //avoid multiple view,continue zoom in.
            view->getScale(sx, sy);
         //qDebug() << "get cal = "<< xx <<yy <<view;
         xx = sx * ZOOM_IN_STEP;
         yy = sy * ZOOM_IN_STEP;
         view->setScale(xx, yy);
         //qDebug() <<"after set";
      }
      //------------------------------------
      if (i == PARAM_ZOOM_INH)
      {
         if (j == 0) //avoid multiple view,continue zoom in.
            view->getScale(sx, sy);
         //qDebug() << "get cal = "<< xx <<yy <<view;
         xx = sx * ZOOM_IN_STEP;
         yy = sy; //*ZOOM_IN_STEP;
         view->setScale(xx, yy);
         //qDebug() <<"after set";
      }
      //------------------------------------
      if (i == PARAM_ZOOM_INV)
      {
         if (j == 0) //avoid multiple view,continue zoom in.
            view->getScale(sx, sy);
         //qDebug() << "get cal = "<< xx <<yy <<view;
         xx = sx; //*ZOOM_IN_STEP;
         yy = sy * ZOOM_IN_STEP;
         view->setScale(xx, yy);
         //qDebug() <<"after set";
      }
      //------------------------------------
      if (i == PARAM_ZOOM_OUT)
      {
         if (j == 0) //avoid multiple view,continue zoom in.
         {
            view->getScale(sx, sy);
            //qDebug() << "get cal0 = "<< xx <<yy <<view;
         }
         // qDebug() << "get cal = "<< xx <<yy <<view;
         xx = sx / ZOOM_IN_STEP;
         yy = sy / ZOOM_IN_STEP;
         view->setScale(xx, yy);
      }
      //------------------------------------
      if (i == PARAM_ZOOM_OUTH)
      {
         if (j == 0) //avoid multiple view,continue zoom in.
         {
            view->getScale(sx, sy);
            //qDebug() << "get cal0 = "<< xx <<yy <<view;
         }
         // qDebug() << "get cal = "<< xx <<yy <<view;
         xx = sx / ZOOM_IN_STEP;
         yy = sy; /// ZOOM_IN_STEP;
         view->setScale(xx, yy);
      }
      //------------------------------------
      if (i == PARAM_ZOOM_OUTV)
      {
         if (j == 0) //avoid multiple view,continue zoom in.
         {
            view->getScale(sx, sy);
            //qDebug() << "get cal0 = "<< xx <<yy <<view;
         }
         // qDebug() << "get cal = "<< xx <<yy <<view;
         xx = sx; // / ZOOM_IN_STEP;
         yy = sy / ZOOM_IN_STEP;
         view->setScale(xx, yy);
      }
//Draw type:
//-=================================================
//------------------------------------
      if (i == PARAM_DRAW_WA)
      {
         view->setDrawType(DRAW_WA);
         drawWAAct->setChecked(true);
         //view->slotMovedBy(0, -50);
      }
      if (i == PARAM_DRAW_VD)
      {
         view->setDrawType(DRAW_VD);
         drawVDAct->setChecked(true);
      }
//outlook -=================================================
//
      if (i == PARAM_WIN_OUTLOOK)
      {

         b = outlookAct->isChecked();
         if (b)
         {
            view->setOutlook(true);
         }
         else view->setOutlook(false);

      }
//Drag mode:
//-=================================================
//
      if (i == PARAM_DRAG_HAND)
      {
         view->setDragMode(QGraphicsView::ScrollHandDrag); //RubberBandDrag
         dragHandAct->setChecked(true);
      }
      if (i == PARAM_DRAG_RUBBER_BAND)
      {
         view->setDragMode(QGraphicsView::RubberBandDrag); //RubberBandDrag
         dragRubberBandAct->setChecked(true);
      }
      if (i == PARAM_DRAG_NORMAL)
      {
         view->setDragMode(QGraphicsView::NoDrag); //RubberBandDrag
         dragNormalAct->setChecked(true);
      }
//Compare:
//-=================================================

      if (i == PARAM_COMPARE_H)
      {
         view->setCompareMode(COMPARE_MODE_H);
         compareHAct->setChecked(true);
      }
      if (i == PARAM_COMPARE_V)
      {
         view->setCompareMode(COMPARE_MODE_V);
         compareVAct->setChecked(true);
      }
      if (i == PARAM_COMPARE_NO)
      {
         view->setCompareMode(COMPARE_MODE_NO);
         compareNOAct->setChecked(true);
      }
      if (i == PARAM_COMPARE_TURNOVER)
      {
         view->setCompareMode(COMPARE_MODE_TURNOVER);
         compareTurnoverAct->setChecked(true);
      }
   }

   //drawview->reDraw();
   getParam(); // in slotParam(int);
   slotSetParamTool();
}
void dvMainWin::slotSetParamTool()
{

   QString str;
   float xx, yy;
// scale:
   str = QString("%1");
   drawview->getUserScale(xx, yy);
   userXEdit->setText(str.arg(xx, 5, 'g', 2));
   userYEdit->setText(str.arg(yy, 5, 'g', 2));

}
void dvMainWin::setTitle()
{

   QString str;
   str = "";
   str = str + APP_NAME + "-" + APP_VERSION + "-" + __DATE__ + "-";
   str = str + DOC->m_dataName;
   setWindowTitle(str);

}
void dvMainWin::closeEvent(QCloseEvent *event)
{
   /*
       mdiArea->closeAllSubWindows();
    if (mdiArea->currentSubWindow()) {
        event->ignore();
    } else {
        writeSettings();
        event->accept();
    }
    */
   writeSettings();
   //qDebug() << "1111111";
}


void dvMainWin::savePickToMap()
{
   int i, num;
   i = DOC->m_data->m_iCurGather;
   num = drawview->data->m_pickData->pickPoints.size();
   if (num <= 0)
   {
      // qDebug() <<  "111";
      DOC->m_data->pickData.remove(i);
      // qDebug() <<  "333";
      return;
   }
   seisData::PICKPOINTS pts;
   pts = drawview->data->m_pickData->pickPoints;

   DOC->m_data->pickData[i] = pts;
   //DOC->pickHD = DOC->m_data->m_diskplayHD;

   //qDebug() << "savePickToMap =" << pts.size()<< i;

   savePickToFile();

}
void dvMainWin::savePickToFile()
{

   QString fname, str, line;
   QMap<int, float> picks;
   fname = DOC->m_dataName + ".pick";
   //qDebug() << "size = " << drawview->pickData.size();
   if (DOC->m_data->pickData.size() <= 0) return;

   QFile file(fname);
   if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) return;
   QTextStream out(&file);

   str = QString("%1 %2 %3")
      .arg("@" + QString("%1").arg(DOC->m_data->getGather()), 10)
      .arg("@" + QString("%1").arg(DOC->m_data->getDisplayHD()), 10)
      .arg("time", 10);
   //qDebug() <<  str;
   out << str << endl;
   int i, num, j, tnum, idx, idx1;
   float t;
   QList<int> glist, tlist;
   num = DOC->m_data->pickData.size();
   //qDebug() << "save pick file =" << num;
   glist = DOC->m_data->pickData.keys();
   for (i = 0; i < num; i++)
   {
      idx = glist[i];
      picks = DOC->m_data->pickData[idx];
      tlist = picks.keys();
      tnum = tlist.size();
      //qDebug() << "tnum =" << tnum << picks.size();
      for (j = 0; j < tnum; j++)
      {
         idx1 = tlist[j];
         t = picks[idx1];
         str = QString("%1 %2 %3").arg(idx, 10).arg(idx1, 10).arg(t, 10);
         //qDebug() <<  str;
         out << str << endl;
      }

   }

   file.close();


}
int  dvMainWin::loadPickFromMap()
{
   int i, num, ret;
   QPolygonF pl;
   QList<int> klist;
   seisData::PICKPOINTS pts;
   i = DOC->m_data->m_iCurGather;
// clear:
   ((sectDraw *)drawview)->m_paint->clearPickPoints();
   //qDebug() << "points = " <<  drawview->data->m_pickData->pickPoints.size();

   //qDebug() << "num of pick data =" <<drawview->pickData.size() ;
   if (DOC->m_data->pickData.contains(i))
   { // is a picked gather:
      pts = DOC->m_data->pickData[i];
      num = pts.size();
      klist = pts.keys();
      //qDebug() << "loadPickToMap =" << num << i;
      for (i = 0; i < num; i++)
      {
         ((sectDraw *)drawview)->m_paint->addPickPointHD(klist[i], pts[klist[i]]);
      }
      ret = num;
   }
   else
   { // get pick time from other gathers:
      pl = getPickIntpTimes();
      drawview->data->m_pickData->m_intpPolyTimes = pl;
      ret = 0;
   }

   drawview->setPickMode(drawview->getPickMode());
   return ret;

}

QPolygonF dvMainWin:: getPickIntpTimes()
{
   int i, num, id0, id1, idx, trs;
   QMap<int, float> ret;
   QPolygonF plg;
   QList<int> glist;
   seisData::PICKPOINTS pts1, pts2, pts;
   num = DOC->m_data->pickData.size();
   idx = DOC->m_data->m_iCurGather;
   //trs = DOC->m_data->m_iCurTrs;
   trs = drawview->data->m_MaxDisplayHeader;
   //qDebug() << "in intpTime fmax = " << trs;
   if (num <= 0) return plg;
   if (DOC->m_data->pickData.contains(idx)) return plg;
// find nearby picked gathers

   glist = DOC->m_data->pickData.keys();
   id0 = -1;
   id1 = -1;
   for (i = 0; i < num; i++)
   {
      if (glist[i] >= idx)
      {
         id1 = glist[i];
         break;
      }
      else
      {
         id0 = glist[i];
      }
   }
   //qDebug() << "in intpTime id0 id1 " << id0<<id1;
   if (id0 == -1 && id1 == -1) return plg;
// left:
   if (id0 == -1 && id1  >= 0)
   {
      ret = DOC->m_data->pickData[id1]; // left
      plg = ((sectDraw *)drawview)->m_paint->getIntpPickTime(ret, trs + 1);
   }
// right
   if (id0 >= 0 && id1  == -1) //right
   {
      //qDebug() << "in intpTime right " << id0<<id1;
      ret = DOC->m_data->pickData[id0]; // left[id0];//right
      plg = ((sectDraw *)drawview)->m_paint->getIntpPickTime(ret, trs + 1);
      //qDebug() << "in intpTime right1 " << id0<<id1;
   }
//middle:
   if (id0 >= 0 && id1  >= 0) //middel
   {

      plg = intpMap(DOC->m_data->pickData[id0], DOC->m_data->pickData[id1], id0, id1, idx, trs + 1); // left[id0];//right
   }
// get return;

   return plg;

}
QPolygonF dvMainWin::intpMap(QMap<int, float> map1, QMap<int, float> map2, int idx0, int idx1, int idx, int trs)
{
   float x[2], y[2], *r;
   int i;
   QPolygonF plg1, plg2, plg;
   plg1 = ((sectDraw *)drawview)->m_paint->getIntpPickTime(map1, trs);
   plg2 = ((sectDraw *)drawview)->m_paint->getIntpPickTime(map2, trs);

   x[0] = idx0;
   x[1] = idx1;
   r = new float[idx + 1];

   for (i = 0; i < trs; i++)
   {
      y[0] = plg1[i].y();
      y[1] = plg2[i].y();
//linearInterpolation( int numSamplesIn, float const* xin, float const* yin,
//                 int numSamplesOut, float sampleIntOut, float* yout )

      drawview->data->linearInterpolation(2, x, y, idx + 1, 1, r);
      plg << QPointF(i, r[idx]);

   }
   delete[] r;
   return plg;


}

void dvMainWin::loadPickFromFile()
{

   QString fname, str, line;
   QStringList  slist;
   QMap<int, float> picks;
   int idx, idx1, lidx;
   float t;
   lidx = -1;
   idx = -1;

   fname = DOC->m_dataName + ".pick";
   //qDebug() << "load pick file n=" << fname;
   QFile file(fname);
   if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;
   QTextStream tin(&file);
   // header:
   str = tin.readLine();
   //qDebug() << "load pick file ist line=" << str;

   while (!tin.atEnd())
   {
      line  = tin.readLine();
      //qDebug() << "line = " << line;

      slist = line.split(" ", QString::SkipEmptyParts);
      //qDebug() << "line split = " << slist[0]<< slist[1]<<slist[2];
      idx = slist[0].toInt();
      idx1 = slist[1].toInt();
      t = slist[2].toFloat();
      //qDebug() << "line split1 = " << idx<< idx1<<t<<lidx;
      if (idx != lidx && lidx != -1)
      {
         DOC->m_data->pickData[lidx] = picks;
         //qDebug() << "read pick =" << lidx << picks;
         picks.clear();
      }
      picks[idx1] = t;
      lidx = idx;
   }
   if (idx == -1) return;

// last one:
   picks[idx1] = t;
   DOC->m_data->pickData[lidx] = picks;
   //qDebug() << "read pick =" << lidx << picks<<drawview->pickData.size();
   file.close();


}
void dvMainWin::setSortData(int d)
{
   DOC->m_iSortData = d;
}
int dvMainWin::getSortData()
{
   return DOC->m_iSortData;
}
drawView* dvMainWin::getView()
{
   return drawview;
}

