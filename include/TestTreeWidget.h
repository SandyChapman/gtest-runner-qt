/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * TestTreeWidget.h - Created on 2010-08-07
 *
 * Copyright (C) 2010 Sandy Chapman
 *
 * This library is free software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free Software
 * Foundation; either version 2.1 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 * You should have received a copy of the GNU Lesser General Public License along with this
 * library; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307 USA
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef TESTTREEWIDGET_H_
#define TESTTREEWIDGET_H_

#include <QHash>
#include <QList>
#include <QObject>
#include <QSharedPointer>
#include <QSignalMapper>
#include <QString>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QWidget>

#include "GTestExecutable.h"

class GTest;
class TestTreeWidgetItem;

/*! \brief An extension of the QTreeWidget to provide custom functionality.
 *
 * Currently, the only purpose of this subclass is to provide a mechanism of
 * custom defined slots to asynchronously populate test results into the
 * tree.
 */
class TestTreeWidget : public QTreeWidget {

Q_OBJECT

private:
	QHash<QString, QSharedPointer<GTestExecutable> > gtestHash;	//!< A hash of all loaded gtest executables.
	/*!< \todo Investigate whether the signal mapper is needed.
	 * A (gtest signal-> tree item signal-> tree widget slot) might work fine.
	 */
	QSignalMapper signalMap;	//!< Maps signals from GTests to TreeWidgets.

	template <class T, class U>
	TestTreeWidgetItem* createNewTreeItem(T* parent, U* test);

signals:
	void resettingRunStates();
	void runningTests();

public slots:
	void populateTestResult(QObject* item);
	void updateListing(GTestExecutable* gtest);
	void updateAllListings();

public:
	TestTreeWidget(QWidget* parent = 0);
	virtual ~TestTreeWidget();

	void insertExecutable(QSharedPointer<GTestExecutable> executable);
	QList<QSharedPointer<GTestExecutable> > getExecutableList();

};

inline QList<QSharedPointer<GTestExecutable> > TestTreeWidget::getExecutableList() { return gtestHash.values(); }

#endif /* TESTTREEWIDGET_H_ */
