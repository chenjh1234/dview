#include "headview.h"

headView::headView(QWidget *p) : QDialog(p)
{
    init();
}
headView::~headView()
{

}
void headView::init()
{
    ui.setupUi(this);
    m_col = 2;

    QString str;
    QStringList liststr;
    liststr << "header" << "value" ;
    model = new QStandardItemModel(2,m_col);
    model->setHorizontalHeaderLabels(liststr);
    ui.tableView->setModel(model);

}
void headView::setRows(int num)
{
    int rows;
    rows = model->rowCount();
    model->removeRows(0, rows);
    model->insertRows(0, num);
}
void headView::setRows(QMap<QString, double> map)
{
    int num;
    num = map.size();
    //qDebug() << "set rows = " << num;
    setRows(num);
}
void headView::setData(QMap <QString,double> map)
{
    QString key,value,str;
    int row,column,num,rows;
    rows = model->rowCount();
    num = map.size();
    if (rows != num) setRows(map);
 
    row = 0;
    QMap<QString, double>::const_iterator i = map.constBegin();
    while (i != map.constEnd()) 
    {
        //cout << i.key() << ": " << i.value() << endl;
        key = i.key();
        value = QString("%1").arg(i.value());
        for (column = 0; column < m_col; ++column)
        {
            QModelIndex index = model->index(row, column, QModelIndex());
            if (column == 0) str = key;
            if (column == 1) str = value; 
            model->setData(index, QVariant(str));
        }          
        ++i;
        row++;
    }
    //qDebug() << "headers = " << row;

#if 0
    for (int row = 0; row < MAX_DEFINES; ++row) 
    {
        for (int column = 0; column < 1; ++column) 
        {           
             QModelIndex index = model->index(row, column, QModelIndex());
             model->setData(index, QVariant(str));
        }
    }
#endif
}
 
