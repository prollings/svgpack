#ifndef ITEMLIST_H
#define ITEMLIST_H

#include <QObject>
#include <QTreeWidget>

class ItemList : public QTreeWidget
{
    Q_OBJECT
public:
    ItemList(QWidget* parent);

    void clearRowStates();

    void setRowFailed(int id);
};

#endif // ITEMLIST_H
