#ifndef SVGLAYOUT_H
#define SVGLAYOUT_H

#include <QGraphicsView>
#include <QVariant>

struct TreeNode {
    QGraphicsItem* item;
    // right and down can be lengths or TreeNode pointers
    QVariant right;
    QVariant down;
};

class SvgLayout : public QGraphicsView
{
    Q_OBJECT
public:
    SvgLayout(QWidget* parent);

    bool addItem(QString path);

    void setSize(size_t width, size_t height);

    void setDrawBoundingBoxes(bool draw);

    void layoutItems(qreal marginWidth = 0, qreal marginHeight = 0);

    void exportSvg(QString path);

private:
    size_t width;
    size_t height;
    bool drawBoundingBoxes;
    QGraphicsRectItem* sizeRect;
    QVector<QGraphicsRectItem*> itemBoxes;

signals:
    void ready();
};

#endif // SVGLAYOUT_H
