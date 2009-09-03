/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2009-2009 by Enrico Ros <enrico.ros@gmail.com>          *
 *   Started on 3 Sep 2009 by root.                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "NoteScroll.h"
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QSize>
#include <QRect>
#include <QPainter>
#include <QApplication>
#include <QVBoxLayout>

class MyScene : public QGraphicsScene {
    public:
        MyScene(QObject * parent = 0)
          : QGraphicsScene(parent)
        {
            m_upArrow = new QGraphicsPixmapItem(QPixmap("/button-plus.png"), this);
            m_downArrow = new QGraphicsPixmapItem(QPixmap("/button-minus.png"), this);
        }

        void resize(const QSize & size)
        {
            // relayout contents
            m_size = size;
            m_rect = QRectF(0, 0, m_size.width(), m_size.height());
            m_upArrow->setPos(m_rect.right() - m_upArrow->boundingRect().width(), m_rect.top());
            m_downArrow->setPos(m_rect.right() - m_downArrow->boundingRect().width(), m_rect.bottom() - m_downArrow->boundingRect().height());

            // change my rect
            setSceneRect(m_rect);
        }

        void drawBackground(QPainter * painter, const QRectF & rect)
        {
            painter->fillRect(rect, Qt::red);
        }

    private:
        QGraphicsItem * m_upArrow;
        QGraphicsItem * m_downArrow;
        QSize m_size;
        QRectF m_rect;
};

class MyGraphicsView : public QGraphicsView {
    public:
        MyGraphicsView(QWidget * parent = 0)
            : QGraphicsView(parent)
            , m_myScene(0)
        {
            // customize widget
            setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            setInteractive(true);
            setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing /*| QPainter::SmoothPixmapTransform */);
            setDragMode(QGraphicsView::RubberBandDrag);
            setAcceptDrops(true);
            setFrameStyle(QFrame::NoFrame);

            // don't autofill the view with the Base brush
            QPalette pal;
            pal.setBrush(QPalette::Base, Qt::NoBrush);
            setPalette(pal);
        }

        void setMySene(MyScene * scene)
        {
            setScene(scene);
            m_myScene = scene;
        }

    protected:
        void resizeEvent(QResizeEvent * event)
        {
            if (m_myScene)
                m_myScene->resize(contentsRect().size());
            QGraphicsView::resizeEvent(event);
        }

    private:
        MyScene * m_myScene;
};

NoteScroll::NoteScroll(QWidget *parent)
  : QWidget(parent)
  , m_view(new MyGraphicsView)
  , m_scene(new MyScene)
{
    // customize this
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_TranslucentBackground, true);

    // layout things
    QVBoxLayout * lay = new QVBoxLayout(this);
    lay->setSpacing(0);
    lay->setMargin(0);
    lay->addWidget(m_view);

    // customize graphicsview
    m_view->setScene(m_scene);
}

NoteScroll::~NoteScroll()
{

}

void NoteScroll::mouseDoubleClickEvent(QMouseEvent * /*event*/)
{
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    qWarning("DK");
}
