#include "itemlist.h"

ItemList::ItemList(QWidget* parent)
    : QTreeWidget(parent)
{

}

void ItemList::clearRowStates()
{
    for (QTreeWidgetItemIterator it(this); *it; ++it) {
        (*it)->setForeground(0, Qt::black);
    }
}

void ItemList::setRowFailed(int id)
{
    for (QTreeWidgetItemIterator it(this); *it; ++it) {
        if ((*it)->text(0) == QString::number(id)) {
            (*it)->setForeground(0, Qt::red);
            break;
        }
    }
}
