/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * GTestRunner.h - Created on 2010-07-23                                                     *
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

#ifndef GTESTRUNNER_H
#define GTESTRUNNER_H

#include <QFile>
#include <QList>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QModelIndex>
#include <QStandardItemModel>
#include <QStatusBar>
#include <QString>
#include <QToolBar>
#include <QTreeWidget>

#include "GTest.h"

class GTestRunner : public QMainWindow
{
    Q_OBJECT

public:
    GTestRunner(QWidget *parent = 0, Qt::WFlags flags = 0);
    ~GTestRunner();

public slots:
	void addTests();
	void updateAllListings();
	void runTests();

	void updateListing(GTestExecutable* gtest);

	void fillTestResults(GTestExecutable* gtest);

	void treeItemClicked(QTreeWidgetItem* item, int column);

private:
	QMenuBar *menuBar;
	QMenu fileMenu;
	QMenu helpMenu;
	QStatusBar statusBar;

	QWidget centralWidget;
	QToolBar testTreeTools;
	QTreeWidget testTree;

	QList<GTestExecutable*> gtestList;

	void setup();
	void setupMenus();
	void setupToolBars();
	void setupLayout();

	void invokeListingRetrieval(QString filename);

};

#endif // GTESTRUNNER_H
