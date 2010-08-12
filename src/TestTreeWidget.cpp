/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * TestTreeWidget.cpp - Created on 2010-08-07
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

#include "TestTreeWidget.h"
#include "GTestSuite.h"
#include "GTestExecutable.h"

/*! \brief Constructor
 *
 * \param parent The parent QWidget.
 */
TestTreeWidget::TestTreeWidget(QWidget* parent)
: QTreeWidget(parent)
{}

/*! \brief Destructor
 *
 */
TestTreeWidget::~TestTreeWidget()
{}

/*! \brief Populates a test result into the test tree.
 *
 * This function takes a QObject* which should be a TestTreeWidgetItem.
 * If it is called with anything else, the function does nothing. It takes
 * the item and retrieves its corresponding GTest. The GTest should already
 * have its updated result attached to it, thus, it retrieves the result
 * and sets the item to the appropriate pass / fail state.
 * \param item This should be a TestTreeWidgetItem which needs its test result updated.
 */
void TestTreeWidget::populateTestResult(QObject* item) {
	TestTreeWidgetItem* treeItem = static_cast<TestTreeWidgetItem*>(item);
	if(treeItem == 0)
		return; //TODO:: exception management stuff here

	QVariant var = treeItem->data(0,Qt::UserRole);
	GTest* testItem = var.value<GTest*>();
	if(testItem == 0) {
		testItem = var.value<GTestSuite*>();
		if(testItem == 0) {
			testItem = var.value<GTestExecutable*>();
			if(testItem == 0)
				return;
		}
	}

	const GTestResults* testResults = testItem->getTestResults();
	if(testResults == 0)
		return;

	if(testResults->getFailureCount() == 0)
		treeItem->setBackgroundColor(0,Qt::green);
	else
		treeItem->setBackgroundColor(0,Qt::red);
}




