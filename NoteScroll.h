/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2009-2009 by Enrico Ros <enrico.ros@gmail.com>        *
 *   Started on 3 Sep 2009 by root.
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

class NoteScroll : public QWidget
{
    Q_OBJECT

public:
    NoteScroll(QWidget *parent = 0);
    ~NoteScroll();
};

#endif // NOTESCROLL_H
