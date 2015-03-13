/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * main.cpp - Created on 2010-07-23                                                          *
 *                                                                                           *
 * Copyright (C) 2010 Sandy Chapman                                                          *
 *                                                                                           *
 * This library is free software; you can redistribute it and/or modify it under the         *
 * terms of the GNU Lesser General Public License as published by the Free Software          *
 * Foundation; either version 2.1 of the License, or (at your option) any later version.     *
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; *
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. *
 * See the GNU Lesser General Public License for more details.                               *
 * You should have received a copy of the GNU Lesser General Public License along with this  *
 * library; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, *
 * Boston, MA 02111-1307 USA                                                                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "GTestRunner.h"

#include <QtGui>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    GTestRunner w;

    QCommandLineParser parser;
    QCommandLineOption testExecutablePath(QStringList() << "f" << "file", "Specify a googletest executable filepath.", "executable");
    parser.addOption(testExecutablePath);
    QCommandLineOption testOutputPath(QStringList() << "o" << "output-directory", "Specify a directory to store the result xml.", "outputDir");
    parser.addOption(testOutputPath);

    parser.process(a);

    // Set directory before the executable because the output dir will be propagated
    QString targetDir = parser.value(testOutputPath);
    if(QFile::exists(targetDir)){
        QDir lDir(targetDir);
        if(lDir.exists()){  // check that it is a directory.
            w.AddResultsPath(targetDir);
        }
    }

    // If a filename was passed on the command line, add the tests immediately.
    QString targetFile = parser.value(testExecutablePath);
    if(QFile::exists(targetFile)){
        w.AddExecutable(targetFile);
    }

    w.show();
    return a.exec();
}
