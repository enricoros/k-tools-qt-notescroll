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
#include <QColorDialog>
#include <QFontDialog>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsItem>
#include <QPainter>
#include <QRect>
#include <QSize>
#include <QVBoxLayout>

#if QT_VERSION >= 0x040600
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#endif


/** Button Item **/

ButtonItem::ButtonItem(const QPixmap & pixmap, int id)
  : QGraphicsPixmapItem(pixmap)
  , m_id(id)
{
    // customize the pixmap item
    setAcceptHoverEvents(true);
    setShapeMode(BoundingRectShape);
    show();
    hoverLeaveEvent(0);
}

void ButtonItem::hoverEnterEvent(QGraphicsSceneHoverEvent * /*event*/)
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

void ButtonItem::hoverLeaveEvent(QGraphicsSceneHoverEvent * /*event*/)
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

void ButtonItem::mousePressEvent(QGraphicsSceneMouseEvent * /*event*/)
{
    emit clicked(m_id);
}


/** TextItem **/

TextItem::TextItem(const QString & text, QGraphicsScene * scene, QGraphicsItem * parent)
  : QGraphicsObject(parent)
  , m_text(text)
  , m_scene(scene)
  , m_color(Qt::black)
{
}

void TextItem::setFont(const QFont & font)
{
    m_font = font;
    update();
}

QFont TextItem::font() const
{
    return m_font;
}

void TextItem::setColor(const QColor & color)
{
    m_color = color;
    update();
}

QColor TextItem::color() const
{
    return m_color;
}

void TextItem::enter(bool upwards)
{
    show();
#if QT_VERSION >= 0x040600
    QParallelAnimationGroup * pAnim = new QParallelAnimationGroup(this);
    // opacity animation
    QPropertyAnimation * oAni = new QPropertyAnimation(this, "opacity");
    oAni->setEasingCurve(QEasingCurve::OutCubic);
    oAni->setDuration(800);
    oAni->setStartValue(0.0);
    oAni->setEndValue(1.0);
    pAnim->addAnimation(oAni);
    // position animation
    QPropertyAnimation * yAni = new QPropertyAnimation(this, "y");
    yAni->setEasingCurve(QEasingCurve::OutQuad);
    yAni->setDuration(800);
    yAni->setStartValue(upwards ? boundingRect().height() : -boundingRect().height());
    yAni->setEndValue(0.0);
    pAnim->addAnimation(yAni);
    pAnim->start(QPropertyAnimation::DeleteWhenStopped);
#endif
}

void TextItem::dispose(bool upwards)
{
#if QT_VERSION >= 0x040600
    QParallelAnimationGroup * pAnim = new QParallelAnimationGroup(this);
    connect(pAnim, SIGNAL(finished()), this, SLOT(deleteLater()));
    // opacity animation
    QPropertyAnimation * oAni = new QPropertyAnimation(this, "opacity");
    oAni->setEasingCurve(QEasingCurve::OutCubic);
    oAni->setDuration(800);
    oAni->setEndValue(0.0);
    pAnim->addAnimation(oAni);
    // position animation
    QPropertyAnimation * yAni = new QPropertyAnimation(this, "y");
    yAni->setEasingCurve(QEasingCurve::OutQuad);
    yAni->setDuration(800);
    yAni->setEndValue(upwards ? -boundingRect().height() / 2 : boundingRect().height() / 2);
    pAnim->addAnimation(yAni);
    pAnim->start(QPropertyAnimation::DeleteWhenStopped);
#else
    deleteLater();
#endif
}

QRectF TextItem::boundingRect() const
{
    return m_scene->sceneRect();
}

void TextItem::paint(QPainter * painter, const QStyleOptionGraphicsItem */*option*/, QWidget * /*widget*/)
{
    painter->setFont(m_font);
    painter->setPen(QPen(m_color));
    painter->drawText(boundingRect(), Qt::AlignCenter /*| Qt::TextWrapAnywhere*/ | Qt::TextWordWrap, m_text);
}


/** TextScene **/

TextScene::TextScene(QObject * parent)
  : QGraphicsScene(parent)
  , m_textItem(0)
  , m_textColor(Qt::black)
  , m_currentStringIdx(-1)
{
    // plus and minus buttons
    m_upArrow = new ButtonItem(QPixmap(":/button-minus.png")/*.scaled(16, 16, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)*/, 1);
    connect(m_upArrow, SIGNAL(clicked(int)), this, SLOT(slotButtonPressed(int)));
    addItem(m_upArrow);
    m_downArrow = new ButtonItem(QPixmap(":/button-plus.png")/*.scaled(16, 16, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)*/, 2);
    addItem(m_downArrow);
    connect(m_downArrow, SIGNAL(clicked(int)), this, SLOT(slotButtonPressed(int)));

    // change default font
    QFont font;
    font.setPixelSize(36);
    setFont(font);
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
    lg.setColorAt(0.5, QColor(255, 255, 255, 128));
    lg.setColorAt(1.0, Qt::transparent);
    painter->fillRect(sceneRect(), lg);
}

void TextScene::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    QGraphicsScene::mousePressEvent(event);
    if (event->isAccepted())
        return;

    // handle scene clicks
    switch (event->button()) {
        case Qt::RightButton: {
            bool ok = false;
            QFont newFont = QFontDialog::getFont(&ok, font(), 0, tr("Change Text Font"));
            if (!ok)
                break;
            setFont(newFont);
            if (m_textItem)
                m_textItem->setFont(newFont);
            } break;

        case Qt::MidButton: {
            QColor newColor = QColorDialog::getColor(m_textColor, 0, tr("Change Text Color"));
            if (!newColor.isValid())
                break;
            m_textColor = newColor;
            if (m_textItem)
                m_textItem->setColor(m_textColor);
            } break;

        default:
            break;
    }
}

void TextScene::slotButtonPressed(int id)
{
    // get string
    QString string;
    bool upwards;
    if (id == 2 && m_currentStringIdx < (m_strings.size() - 1)) {
        string = m_strings[++m_currentStringIdx];
        upwards = true;
    } else if (id == 1 && m_currentStringIdx > 0) {
        string = m_strings[--m_currentStringIdx];
        upwards = false;
    }
    if (string.isEmpty())
        return;

    // animate fade in/out
    if (m_textItem)
        m_textItem->dispose(upwards);
    m_textItem = new TextItem(string, this);
    m_textItem->setColor(m_textColor);
    m_textItem->setFont(font());
    m_textItem->enter(upwards);
    addItem(m_textItem);
}


/** MyGraphicsView **/

MyGraphicsView::MyGraphicsView(QWidget * parent)
    : QGraphicsView(parent)
    , m_TextScene(0)
{
    // customize widget
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
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
