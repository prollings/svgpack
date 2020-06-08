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

    void removeItem(int id);

    void setSize(size_t width, size_t height);

    void setDrawBoundingBoxes(bool draw);

    void setPadding(double padding);

    void layoutItems();

    void exportSvg(QString path);

private:
    size_t width;
    size_t height;
    bool drawBoundingBoxes;
    QGraphicsRectItem* sizeRect;
    QVector<QGraphicsRectItem*> itemBoxes;
    int next_id;

signals:
    void ready();
    void itemAdded(QString name, int id);
};

#endif // SVGLAYOUT_H
