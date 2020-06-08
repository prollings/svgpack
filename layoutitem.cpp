#include "layoutitem.h"

LayoutItem::LayoutItem(QGraphicsItem* parent) :
    QGraphicsItemGroup(parent)
{

}

void LayoutItem::setBb(QGraphicsRectItem *bb)
{
    this->m_bb = bb;
    this->m_bb->setParentItem(this);
}

void LayoutItem::setSvg(QGraphicsSvgItem *svg)
{
    this->m_svg = svg;
    this->m_svg->setParentItem(this);
}

void LayoutItem::setPath(QString path)
{
    this->m_path = path;
}

int LayoutItem::type() const
{
    return Type;
}

QGraphicsRectItem* LayoutItem::bb()
{
    return this->m_bb;
}

QGraphicsSvgItem* LayoutItem::svg()
{
    return this->m_svg;
}

QString LayoutItem::path()
{
    return this->m_path;
}
