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
#include <QApplication>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    NoteScroll w;
    w.setWindowTitle(QObject::tr("NoteScroL"));
    w.resize(640, 120);
    w.show();

    // read input file, 1 string per line
    QString name = QFileDialog::getOpenFileName(0, QObject::tr("Select Text file"), QString(), QObject::tr("Text Files (*)"));
    if (!QFile::exists(name))
        return 1;
    QFile file(name);
    if (!file.open(QIODevice::ReadOnly))
        return 1;
    QTextStream stream(&file);
    QStringList strings;
    while (!stream.atEnd())
        strings.append(stream.readLine());
    if (strings.isEmpty())
        return 1;
    w.setStrings(strings);
    return a.exec();
}
