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
#include <QHash>
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

#include "GTestExecutable.h"
#include "TestTreeWidget.h"

/*! \brief The is the main application window class.
 *
 * This class handles all the GUI functionality of the application (this may
 * change as the application grows). It provides the interaction between
 * the test running functionality and the user.
 */
class GTestRunner : public QMainWindow
{
    Q_OBJECT

public:
    GTestRunner(QWidget *parent = 0, Qt::WFlags flags = 0);
    ~GTestRunner();

signals:
	void aboutToRunTests(); //!< Sends a signal to any listeners to prepare for test runs.
	void runningTests();	//!< Sends a signal when the processes have been launched.

public slots:
	void addTests();
	void updateAllListings();
	void runTests();
	void updateListing(GTestExecutable* gtest);
	void setTestResult(GTest* gtest);
	void treeItemClicked(QTreeWidgetItem* item, int column);

private:
	QMenuBar *menuBar;		//!< The main menu bar.
	QMenu fileMenu;			//!< The file menu.
	QMenu helpMenu;			//!< The help menu.
	QStatusBar statusBar;	//!< The status bar.

	QWidget centralWidget;	//!< The <a href="http://doc.qt.nokia.com/4.6/qmainwindow.html#details">central widget</a>.
	QToolBar testTreeTools;	//!< The tool bar for the test tree.
	TestTreeWidget testTree;//!< The test tree.

	QList<GTestExecutable*> gtestList;	//!< A list of all loaded gtest executables. \todo TODO::change list to a hash.

	void setup();
	void setupMenus();
	void setupToolBars();
	void setupLayout();

	void invokeListingRetrieval(GTestExecutable* gtest);

};

#endif // GTESTRUNNER_H
