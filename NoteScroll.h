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
class TextScene;

class NoteScroll : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QStringList strings READ strings WRITE setStrings)
    public:
        NoteScroll(QWidget *parent = 0);
        ~NoteScroll();

        QStringList strings() const;
        void setStrings(const QStringList & strings);

    private:
        MyGraphicsView * m_view;
        TextScene * m_scene;
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
        void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
        void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
        void mousePressEvent(QGraphicsSceneMouseEvent * event);

    Q_SIGNALS:
        void clicked(int);

    private:
        int m_id;
};

#include <QGraphicsTextItem>
class TextItem : public QGraphicsObject
{
    Q_OBJECT
    public:
        TextItem(const QString & text, QGraphicsScene * scene, QGraphicsItem * parent = 0);

        void setFont(const QFont & font);
        QFont font() const;
        void setColor(const QColor & color);
        QColor color() const;

        void enter(bool upwards);
        void dispose(bool upwards);

        // ::QGraphicsObject
        QRectF boundingRect() const;
        void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);

    private:
        QString m_text;
        QGraphicsScene * m_scene;
        QFont m_font;
        QColor m_color;
};

#include <QGraphicsScene>
class TextScene : public QGraphicsScene
{
    Q_OBJECT
    public:
        TextScene(QObject * parent = 0);

        void resize(const QSize & size);

        QStringList strings() const;
        void setStrings(const QStringList & strings);

        // ::QGraphicsScene
        void drawBackground(QPainter *painter, const QRectF &rect);
        void mousePressEvent(QGraphicsSceneMouseEvent * event);

    private:
        ButtonItem * m_upArrow;
        ButtonItem * m_downArrow;
        TextItem * m_textItem;
        QSize m_size;
        QRectF m_rect;
        QColor m_textColor;
        QStringList m_strings;
        int m_currentStringIdx;;

    private Q_SLOTS:
        void slotButtonPressed(int id);
};

#include <QGraphicsView>
class MyGraphicsView : public QGraphicsView
{
    Q_OBJECT
    public:
        MyGraphicsView(QWidget * parent = 0);

        void setMySene(TextScene * scene);

    protected:
        void resizeEvent(QResizeEvent * event);

    private:
        TextScene * m_TextScene;
};

#endif
