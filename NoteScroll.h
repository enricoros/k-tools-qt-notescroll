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

#ifndef NOTESCROLL_H
#define NOTESCROLL_H

#include <QtGui/QWidget>
class MyGraphicsView;
class MyScene;

class NoteScroll : public QWidget
{
    Q_OBJECT
    public:
        NoteScroll(QWidget *parent = 0);
        ~NoteScroll();

    private:
        MyGraphicsView * m_view;
        MyScene * m_scene;
        Qt::WindowFlags m_oldWindowFlags;

    private Q_SLOTS:
        void slotToggleBorder();
};

#include <QGraphicsPixmapItem>
class ButtonItem : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)
    public:
        ButtonItem(const QPixmap & pixmap, int id);
        void enter();
        void leave();
        void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
        void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
        void mousePressEvent(QGraphicsSceneMouseEvent * event);

    Q_SIGNALS:
        void clicked(int);

    private:
        int m_id;
};

#include <QGraphicsScene>
class MyScene : public QGraphicsScene
{
    Q_OBJECT
    public:
        MyScene(QObject * parent = 0);

        void resize(const QSize & size);

        // ::QGraphicsScene
        void drawBackground(QPainter *painter, const QRectF &rect);
        void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event);

    Q_SIGNALS:
        void doubleClicked();

    private:
        ButtonItem * m_upArrow;
        ButtonItem * m_downArrow;
        QSize m_size;
        QRectF m_rect;

    private Q_SLOTS:
        void slotButtonPressed(int id);
};

#include <QGraphicsView>
class MyGraphicsView : public QGraphicsView
{
    Q_OBJECT
    public:
        MyGraphicsView(QWidget * parent = 0);

        void setMySene(MyScene * scene);

    protected:
        void resizeEvent(QResizeEvent * event);

    private:
        MyScene * m_myScene;
};

#endif
