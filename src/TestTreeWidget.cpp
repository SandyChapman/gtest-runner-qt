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

#include <QDebug>
#include <QIcon>
#include <QMessageBox>
#include <QVariant>

#include "TestTreeWidget.h"
#include "TestTreeWidgetItem.h"
#include "GTest.h"
#include "GTestSuite.h"
#include "GTestExecutable.h"
#include "GTestResults.h"

/*! \brief Constructor
 *
 * \param parent The parent QWidget.
 */
TestTreeWidget::TestTreeWidget(QWidget* parent)
: QTreeWidget(parent), signalMap(this)
{
	QObject::connect(&signalMap, SIGNAL(mapped(QObject*)),
					 this, SLOT(populateTestResult(QObject*)));
	this->setColumnCount(2);
	this->setHeaderLabels(QStringList()<<tr("Tests")<<tr(" "));
}

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

	if(testResults->getFailureCount() == 0) {
		treeItem->setBackgroundColor(0,QColor(0xAB,0xFF,0xBB,0xFF));
	}
	else {
		treeItem->setBackgroundColor(0,QColor(0xFF,0x88,0x88,0xFF));
	}
}

/* \brief Creates a tree item and sets up some slot/signals.
 *
 * This function is a help to the updateListing method which enables
 * simplified creation of new tree items. It is templated as I didn't
 * feel like writing three copies of the same code (thus making the
 * function useless). It simply sets up some common states among tree
 * items and some signal/slots between the tree item and its corresponding
 * test item.
 */
template <class T, class U>
TestTreeWidgetItem* TestTreeWidget::createNewTreeItem(T* parent, U* test) {
	TestTreeWidgetItem* newTreeItem = new TestTreeWidgetItem(parent, QStringList() << test->objectName());
	newTreeItem->setObjectName(test->objectName());
	newTreeItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
	newTreeItem->setCheckState(0, Qt::Checked);
	QVariant var;
	var.setValue<U*>(test);

	newTreeItem->setData(0,Qt::UserRole,var);
	signalMap.setMapping(test, newTreeItem);	/* E.g. if gtest sends a signal through
												 * the mapper, it will include exeTreeItem
												 * as an argument.
												 */
	//Connect some signals and slots
	QObject::connect(test, SIGNAL(destroyed()),
					 newTreeItem, SLOT(deleteLater()));
	QObject::connect(test, SIGNAL(testResultsReady()),
					 &signalMap, SLOT(map()));
	return newTreeItem;
}

/*! \brief Updates the tree with a listing provided by 'gtest'.
 *
 * This function takes the 'gtest' and retrieves its listing. It then
 * takes the listing and populates the main test tree with the tests
 * it provides.
 * \todo TODO::Update a listing that already exists.
 * \todo TODO::Refactor this method. It's a bit too long.
 */
void TestTreeWidget::updateListing(GTestExecutable* gtest) {
	const int exitCode = gtest->getExitCode();
	QString exePath = gtest->getExecutablePath();
	if(exitCode != 0) {
		QMessageBox::critical(this, "Error Retrieving Test Listing",
					QString("Exit code").append(exitCode).append("was returned from the Google Test executable at").append(exePath).append(". Are you sure this is a valid Google Test unit test executable?"));
		//! \todo Perform switch statement of process error.
		return;
	}

	TestTreeWidgetItem* suiteTreeItem = 0;
	TestTreeWidgetItem* testTreeItem = 0;
	qDebug() << "attempting to find child with name"<<gtest->objectName();
	TestTreeWidgetItem* exeTreeItem = this->findChild<TestTreeWidgetItem*>(gtest->objectName());

	if(!exeTreeItem) {
		//If we haven't added the test yet, make one now.
		qDebug() << "creating new test exe tree item.";
		exeTreeItem = createNewTreeItem<TestTreeWidget,GTestExecutable>(this,gtest);
		qDebug() << "item created with name"<<exeTreeItem->objectName();
		exeTreeItem->setFlags(exeTreeItem->flags() | Qt::ItemIsTristate);
		QObject::connect(this, SIGNAL(runningTests()),
						 gtest, SLOT(runTest()));
		QObject::connect(this, SIGNAL(resettingRunStates()),
						 gtest, SLOT(resetRunState()));
	}

	//Get which of the tests are new. This should be all of them the first
	//time through for this test.
	QSet<QString> newTests = gtest->getListing() - gtest->getOldListing();

	//Iterate through all the suites.
	QList<GTestSuite*> suiteList = gtest->findChildren<GTestSuite*>();
	QList<GTestSuite*>::iterator suiteIter = suiteList.begin();
	while(suiteIter != suiteList.end()) {
		if(!newTests.contains((*suiteIter)->objectName())) {
			suiteIter++;
			continue; //already have this added
		}

		//Create a new GTestSuite tree item.
		suiteTreeItem = createNewTreeItem<TestTreeWidgetItem,GTestSuite>(exeTreeItem, *suiteIter);
		suiteTreeItem->setFlags(suiteTreeItem->flags() | Qt::ItemIsTristate);

		//Iterate through all the tests of this suite.
		QList<GTest*> testList = (*suiteIter)->findChildren<GTest*>();
		QList<GTest*>::iterator testIter = testList.begin();
		while(testIter != testList.end()) {
			if(!newTests.contains((*suiteIter)->objectName().append('.').append((*testIter)->objectName()))) {
				testIter++;
				continue; //already have this added
			}

			//Create a new GTest tree item.
			testTreeItem = createNewTreeItem<TestTreeWidgetItem, GTest>(suiteTreeItem, *testIter);
			++testIter;
		}
		++suiteIter;
	}
}

/*! \brief Updates all the listings for every GTestExecutable.
 *
 * This function iterates through the collection of GTestExecutables
 * and invokes their listing retrieval mechanism.
 */
void TestTreeWidget::updateAllListings() {
	QHash<QString, QSharedPointer<GTestExecutable> >::iterator it = gtestHash.begin();
	while(it != gtestHash.end()) {
		(*it)->produceListing();
		++it;
	}
}

/*! \brief Inserts a new executable into the tree.
 *
 * This inserts a new GTestExecutable into the tree and calls the method
 * to begin retrieving the test listing.
 * \see TestTreeWidget::updateListing()
 */
void TestTreeWidget::insertExecutable(QSharedPointer<GTestExecutable> executable) {
	QObject::connect(executable.data(), SIGNAL(listingReady(GTestExecutable*)),
					 this, SLOT(updateListing(GTestExecutable*)));
	//We insert it so that it doesn't auto-delete from the shared ptr.
	//Will probably be useful later on when we want to save settings.
	gtestHash.insert(executable->objectName(), executable);
	//We've got test, so let's have it send up a listing.
	executable->produceListing();
}





