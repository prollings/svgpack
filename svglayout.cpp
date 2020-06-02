#include "svglayout.h"

#include <QWidget>
#include <QGraphicsSvgItem>
#include <QDebug>
#include <QTimer>
#include <QSvgGenerator>

SvgLayout::SvgLayout(QWidget* parent) :
    QGraphicsView(parent),
    sizeRect(nullptr)
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
    auto item = new QGraphicsSvgItem(path);
    item->setFlags(QGraphicsItem::ItemClipsToShape);
    item->setCacheMode(QGraphicsItem::NoCache);
    item->setZValue(0);
    item->setPos(0, 0);
    item->setData(0, "svg");
    s->addItem(item);

    return true;
}

void SvgLayout::setSize(size_t width, size_t height)
{
    this->height = height;
    this->width = width;
    sizeRect->setRect(0, 0, width, height);
}

void SvgLayout::setDrawBoundingBoxes(bool draw)
{
    auto s = scene();
    if (draw) {
        QPen pen(Qt::green, 1, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin);
        for (auto item : s->items()) {
            if (item->data(0) == "svg") {
                auto bb = new QGraphicsRectItem(item->boundingRect());
                bb->setPen(pen);
                bb->setData(0, "bb");
                scene()->addItem(bb);
            }
        }
    } else {
        for (auto item : s->items()) {
            if (item->data(0) == "bb") {
                s->removeItem(item);
                delete item;
            }
        }
    }
}

void SvgLayout::layoutItems()
{
    QVector<QGraphicsItem*> items {};
    for (auto item : scene()->items()) {
        if (item->data(0) == "svg") {
            items.push_back(item);
        }
    }
    // sort by height
    auto predicate = [](QGraphicsItem* a, QGraphicsItem* b) {
        return a->sceneBoundingRect().height() > b->sceneBoundingRect().height();
    };
    std::sort(items.begin(), items.end(), predicate);
    // create single, all encompassing, space
    QVector<QRect> spaces {
        QRect(0, 0, this->width, this->height)
    };
    auto ep_eq = [](auto a, auto b) {
        return std::abs(a - b) < 0.1;
    };
    // look through items for biggest item to fit into smallest space
    for (int iii = 0; iii < items.length(); ++iii) {
        auto item = items[iii];
        auto bb = item->boundingRect();
        auto bbw = bb.width();
        auto bbh = bb.height();
        for (int jjj = spaces.length() - 1; jjj >= 0; --jjj) {
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
            break;
        }
    }
}

void SvgLayout::exportSvg(QString path)
{
    QSvgGenerator gen;
    gen.setFileName(path);
    gen.setSize(QSize(width, height));
    gen.setViewBox(QRect(0, 0, width, height));

    QPainter painter;
    painter.begin(&gen);
    scene()->render(&painter);
    painter.end();
}
