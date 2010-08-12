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

#include <QTreeWidget>
#include <QTreeWidgetItem>

#include "GTest.h"
#include "GTestResults.h"
#include "TestTreeWidgetItem.h"

/*! \brief An extension of the QTreeWidget to provide custom functionality.
 *
 * Currently, the only purpose of this subclass is to provide a mechanism of
 * custom defined slots to asynchronously populate test results into the
 * tree.
 */
class TestTreeWidget : public QTreeWidget {

Q_OBJECT

public slots:
	void populateTestResult(QObject* item);

public:
	TestTreeWidget(QWidget* parent = 0);
	virtual ~TestTreeWidget();

};


#endif /* TESTTREEWIDGET_H_ */
