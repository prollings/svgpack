#include "svglayout.h"

#include <QWidget>
#include <QGraphicsSvgItem>
#include <QDebug>
#include <QTimer>
#include <QSvgGenerator>
#include <QFileInfo>
#include "layoutitem.h"

enum ItemType {
    CONTAINER,
    BB,
    SVG,
};

enum ContainerData {
    TYPE_IDX,
    ID_IDX,
};

SvgLayout::SvgLayout(QWidget* parent) :
    QGraphicsView(parent),
    sizeRect(nullptr),
    next_id(0)
{
    // init things once event timer starts
    QTimer::singleShot(0, this, [this](){
        QPen pen(Qt::red, 1, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin);
        this->sizeRect = this->scene()->addRect(0, 0, 0, 0, pen);
        emit this->ready();
    });
}


bool SvgLayout::addItem(QString path)
{
    auto s = scene();

    auto id = next_id++;

    auto container = new LayoutItem();
    container->setData(TYPE_IDX, ItemType::CONTAINER);
    container->setData(ID_IDX, id);
    container->setPath(path);

    auto item = new QGraphicsSvgItem(path);
    item->setData(TYPE_IDX, ItemType::SVG);
    item->setFlags(QGraphicsItem::ItemClipsToShape);
    item->setCacheMode(QGraphicsItem::NoCache);
    item->setZValue(0);
    item->setPos(0, 0);
    container->setSvg(item);

    auto bb = new QGraphicsRectItem();
    bb->setData(TYPE_IDX, ItemType::BB);
    bb->setRect(item->boundingRect());
    bb->setPen(QColor(0, 255, 0, 255));
    bb->hide();
    container->setBb(bb);

    s->addItem(container);
    auto name = QFileInfo(path).fileName().remove(".svg", Qt::CaseInsensitive);
    emit itemAdded(name, id);

    return true;
}

void SvgLayout::removeItem(int id)
{
    for (auto item : scene()->items()) {
        if (item->data(ID_IDX) == id) {
            scene()->removeItem(item);
            delete item;
            break;
        }
    }
}

void SvgLayout::duplicateItem(int id)
{
    for (auto item : scene()->items())
    {
        if (item->data(ID_IDX) == id)
        {
            auto old_item = qgraphicsitem_cast<LayoutItem*>(item);
            addItem(old_item->path());
        }
    }
}

void SvgLayout::setSize(size_t width, size_t height)
{
    this->height = height;
    this->width = width;
    sizeRect->setRect(0, 0, width, height);
}

void SvgLayout::setDrawBoundingBoxes(bool draw)
{
    for (auto item : scene()->items()) {
        if (item->data(0) == ItemType::BB) {
            item->setVisible(draw);
        }
    }
}

void SvgLayout::setPadding(double padding)
{
    for (auto item : scene()->items()) {
        if (item->data(TYPE_IDX) == ItemType::CONTAINER) {
            auto group = qgraphicsitem_cast<LayoutItem*>(item);
            auto svg = group->svg();
            auto bb = group->bb();
            group->setPos(0, 0);
            svg->setPos(0, 0);
            bb->setPos(0, 0);
            bb->setRect(svg->boundingRect());
            svg->setPos(padding, padding);
            bb->setRect(bb->rect().adjusted(0, 0, padding * 2, padding * 2));
        }
    }
}

void SvgLayout::layoutItems()
{
    QVector<QGraphicsItem*> items {};
    for (auto item : scene()->items()) {
        if (item->data(TYPE_IDX) == ItemType::CONTAINER) {
            items.push_back(item);
        }
    }
    // sort by height
    auto predicate = [](QGraphicsItem* a, QGraphicsItem* b) {
        return a->childrenBoundingRect().height() > b->childrenBoundingRect().height();
    };
    std::sort(items.begin(), items.end(), predicate);
    // create single, all encompassing, space
    QVector<QRect> spaces {
        QRect(0, 0, this->width, this->height)
    };
    auto ep_eq = [](auto a, auto b) {
        return std::abs(b - a) < 0.1;
    };
    // look through items for biggest item to fit into smallest space
    for (int iii = 0; iii < items.length(); ++iii)
    {
        auto item = items[iii];
        auto bb = item->childrenBoundingRect();
        auto bbw = bb.width();
        auto bbh = bb.height();
        bool fits = false;
        for (int jjj = spaces.length() - 1; jjj >= 0; --jjj)
        {
            auto& space = spaces[jjj];
            auto sw = space.width();
            auto sh = space.height();
            if (bbw > sw || bbh > sh) {
                continue;
            }
            // give space coords to item and then adjust space accordingly
            item->setPos(space.x(), space.y());
            if (ep_eq(bbw, sw) && ep_eq(bbh, sh)) {
                spaces.remove(jjj);
            } else if (ep_eq(bbh, sh)) {
                space.adjust(bbw, 0, 0, 0);
            } else if (ep_eq(bbw, sw)) {
                space.adjust(0, bbh, 0 ,0);
            } else {
                spaces.push_back(QRect(space.x() + bbw, space.y(), sw - bbw, bbh));
                spaces[jjj].adjust(0, bbh, 0, 0); // above may invalidate space ref
            }
            fits = true;
            break;
        }
        if (fits == false) {
            emit itemDoesNotFit(item->data(ID_IDX).toInt());
        }
        item->setVisible(fits);
    }
}

void SvgLayout::exportSvg(QString path)
{
    QSvgGenerator gen;
    gen.setFileName(path);
    gen.setSize(QSize(width, height));
    gen.setViewBox(QRect(0, 0, width, height));

    sizeRect->hide();
    QPainter painter;
    painter.begin(&gen);
    scene()->render(&painter);
    painter.end();
    sizeRect->show();
}
