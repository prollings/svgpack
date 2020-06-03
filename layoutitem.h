#ifndef LAYOUTITEM_H
#define LAYOUTITEM_H

#include <QGraphicsItemGroup>
#include <QGraphicsSvgItem>
#include <QGraphicsRectItem>


class LayoutItem : public QGraphicsItemGroup
{
public:
    enum { Type = UserType + 1 };

    LayoutItem(QGraphicsItem* parent = nullptr);

    ~LayoutItem() = default;

    void setBb(QGraphicsRectItem* bb);

    void setSvg(QGraphicsSvgItem* svg);

    QGraphicsRectItem* bb();

    QGraphicsSvgItem* svg();

    int type() const override;
private:
    QGraphicsRectItem* m_bb;
    QGraphicsSvgItem* m_svg;
};

#endif // LAYOUTITEM_H
