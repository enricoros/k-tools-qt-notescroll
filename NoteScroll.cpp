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
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsItem>
#include <QSize>
#include <QRect>
#include <QPainter>
#include <QApplication>
#include <QVBoxLayout>

#if QT_VERSION >= 0x040600
#include <QPropertyAnimation>
#endif

ButtonItem::ButtonItem(const QPixmap & pixmap, int id)
  : QGraphicsPixmapItem(pixmap)
  , m_id(id)
{
    setAcceptHoverEvents(true);
    setShapeMode(BoundingRectShape);
    show();
    leave();
}

void ButtonItem::enter()
{
#if QT_VERSION >= 0x040600
    // fade in animation
    QPropertyAnimation * ani = new QPropertyAnimation(this, "opacity");
    ani->setEasingCurve(QEasingCurve::OutCubic);
    ani->setDuration(400);
    ani->setEndValue(1.0);
    ani->start(QPropertyAnimation::DeleteWhenStopped);
#else
    show();
#endif
}

void ButtonItem::leave()
{
#if QT_VERSION >= 0x040600
    // fade in animation
    QPropertyAnimation * ani = new QPropertyAnimation(this, "opacity");
    ani->setEasingCurve(QEasingCurve::OutCubic);
    ani->setDuration(400);
    ani->setEndValue(0.2);
    ani->start(QPropertyAnimation::DeleteWhenStopped);
#else
    hide();
#endif
}

void ButtonItem::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
    enter();
    QGraphicsItem::hoverEnterEvent(event);
}

void ButtonItem::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
    leave();
    QGraphicsItem::hoverLeaveEvent(event);
}

void ButtonItem::mousePressEvent(QGraphicsSceneMouseEvent * /*event*/)
{
    emit clicked(m_id);
}



TextScene::TextScene(QObject * parent)
  : QGraphicsScene(parent)
  , m_currentText(0)
  , m_currentStringIdx(-1)
{
    // plus and minus buttons
    m_upArrow = new ButtonItem(QPixmap(":/button-minus.png")/*.scaled(16, 16, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)*/, 1);
    connect(m_upArrow, SIGNAL(clicked(int)), this, SLOT(slotButtonPressed(int)));
    addItem(m_upArrow);
    m_downArrow = new ButtonItem(QPixmap(":/button-plus.png")/*.scaled(16, 16, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)*/, 2);
    addItem(m_downArrow);
    connect(m_downArrow, SIGNAL(clicked(int)), this, SLOT(slotButtonPressed(int)));
}

void TextScene::resize(const QSize & size)
{
    m_size = size;
    m_rect = QRectF(0, 0, m_size.width(), m_size.height());

    // change my rect
    setSceneRect(m_rect);

    // relayout buttons and center text
    m_upArrow->setPos(m_rect.right() - m_upArrow->boundingRect().width(), m_rect.top());
    m_downArrow->setPos(m_rect.right() - m_downArrow->boundingRect().width(), m_rect.bottom() - m_downArrow->boundingRect().height());
    if (m_currentText)
        m_currentText->setPos(0, 0);
}

QStringList TextScene::strings() const
{
    return m_strings;
}

void TextScene::setStrings(const QStringList & strings)
{    
    m_strings = strings;
    m_currentStringIdx = -1;
    slotButtonPressed(2);
}

void TextScene::drawBackground(QPainter *painter, const QRectF &/*rect*/)
{
    QLinearGradient lg(0, 0, 0, 1);
    lg.setCoordinateMode(QGradient::ObjectBoundingMode);
    lg.setColorAt(0.0, Qt::transparent);
    lg.setColorAt(0.5, QColor(255, 220, 0, 64));
    lg.setColorAt(1.0, Qt::transparent);
    painter->fillRect(sceneRect(), lg);
}

void TextScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * /*event*/)
{
    emit doubleClicked();
}

TextItem::TextItem(const QString & text, QGraphicsScene * scene, QGraphicsItem * parent)
  : QGraphicsObject(parent)
  , m_text(text)
  , m_scene(scene)
{
    m_font.setPixelSize(38);
    scene->addItem(this);
    show();
}

QRectF TextItem::boundingRect() const
{
    return m_scene->sceneRect();
}

void TextItem::paint(QPainter * painter, const QStyleOptionGraphicsItem */*option*/, QWidget * /*widget*/)
{
    painter->setFont(m_font);
    painter->drawText(boundingRect(), Qt::AlignCenter | Qt::TextWordWrap, m_text);
}


void TextScene::slotButtonPressed(int id)
{
    // get string
    QString string;
    if (id == 2 && m_currentStringIdx < (m_strings.size() - 1))
        string = m_strings[++m_currentStringIdx];
    else if (id == 1 && m_currentStringIdx > 0)
        string = m_strings[--m_currentStringIdx];
    if (string.isEmpty())
        return;

    // animate fade in/out
    if (m_currentText) {
        removeItem(m_currentText);
        delete m_currentText;
        m_currentText = 0;
    }
    m_currentText = new TextItem(string, this);

}




MyGraphicsView::MyGraphicsView(QWidget * parent)
    : QGraphicsView(parent)
    , m_TextScene(0)
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

void MyGraphicsView::setMySene(TextScene * scene)
{
    setScene(scene);
    m_TextScene = scene;
}

void MyGraphicsView::resizeEvent(QResizeEvent * event)
{
    if (m_TextScene)
        m_TextScene->resize(contentsRect().size());
    QGraphicsView::resizeEvent(event);
}



NoteScroll::NoteScroll(QWidget *parent)
  : QWidget(parent)
  , m_view(new MyGraphicsView)
  , m_scene(new TextScene)
{
    // customize this
#if 1
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_TranslucentBackground, true);
#else
    QPalette pal;
    pal.setBrush(QPalette::Window, Qt::yellow);
    setPalette(pal);
#endif
    m_oldWindowFlags = windowFlags() | Qt::WindowStaysOnTopHint;
    slotToggleBorder();

    // layout things
    QVBoxLayout * lay = new QVBoxLayout(this);
    lay->setSpacing(0);
    lay->setMargin(0);
    lay->addWidget(m_view);

    // customize scene
    connect(m_scene, SIGNAL(doubleClicked()), this, SLOT(slotToggleBorder()));

    // customize graphicsview
    m_view->setMySene(m_scene);
}

NoteScroll::~NoteScroll()
{
}

QStringList NoteScroll::strings() const
{
    return m_scene->strings();
}

void NoteScroll::setStrings(const QStringList & strings)
{
    m_scene->setStrings(strings);
}

void NoteScroll::slotToggleBorder()
{
    static bool frameLess = false;
    QSize prevSize = size();
    if ((frameLess = !frameLess))
        setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    else
        setWindowFlags(m_oldWindowFlags);
    show();
    resize(prevSize);
}
