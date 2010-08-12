/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * GTestRunner.cpp - Created on 2010-07-23                                                   *
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
#include "TestTreeWidgetItem.h"

#include <QAction>
#include <QDebug>
#include <QFileDialog>
#include <QGroupBox>
#include <QMessageBox>
#include <QSignalMapper>
#include <QTreeWidgetItem>
#include <QTreeWidgetItemIterator>
#include <QVBoxLayout>

/*! \brief Constructor
 *
 * \param parent The parent QWidget object. This should be null, as this is the main window.
 * \param flags The main window flags for the application.
 * <a href="http://doc.qt.nokia.com/4.6/qt.html#WindowType-enum">Qt::WFlags Reference</a>
 */
GTestRunner::GTestRunner(QWidget *parent, Qt::WFlags flags)
 : QMainWindow(parent, flags),
   fileMenu(tr("&File")), helpMenu(tr("&Help")), statusBar(this),
   centralWidget(this), testTreeTools(this), testTree(this),
   gtestList()
{
	resize(500, 800);
	setup();
}

/*! \brief Destructor
 *
 */
GTestRunner::~GTestRunner()
{}

/*! \brief Sets up the application GUI.
 *
 * This function sets up some high level objects and gets them
 * to set up as well.
 */
void GTestRunner::setup() {
	//menus
	menuBar = QMainWindow::menuBar();
	setupMenus();
	setMenuBar(menuBar);

	//toolbar
	setupToolBars();

	setCentralWidget(&centralWidget);

	QObject::connect(&testTree, SIGNAL(itemChanged(QTreeWidgetItem*, int)),
					 this, SLOT(treeItemClicked(QTreeWidgetItem*, int)));

	setupLayout();
}

/*! \brief Sets up the application's menus.
 *
 * Creates menu items and adds actions to them.
 */
void GTestRunner::setupMenus() {
	QAction* newTestSetupAct = new QAction(tr("&New Test Setup..."), this);
	QAction* openTestSetupAct = new QAction(tr("&Open Test Setup..."), this);
	QAction* saveTestSetupAct = new QAction(tr("&Save Test Setup..."), this);
	QAction* importTestExeAct = new QAction(tr("&Import Test Executable"), this);
	QAction* quitAct = new QAction(tr("&Exit"), this);

	QObject::connect(importTestExeAct, SIGNAL(triggered()),
					 this, SLOT(addTests()));

	fileMenu.addAction(newTestSetupAct);
	fileMenu.addAction(openTestSetupAct);
	fileMenu.addAction(saveTestSetupAct);
	fileMenu.addAction(importTestExeAct);
	fileMenu.addAction(quitAct);

	QAction* aboutAct = new QAction(tr("&About GTestRunner..."), this);
	QAction* aboutQtAct = new QAction(tr("About &Qt..."), this);

	helpMenu.addAction(aboutAct);
	helpMenu.addAction(aboutQtAct);

	menuBar->addMenu(&fileMenu);
	menuBar->addMenu(&helpMenu);
}

/*! \brief Sets up the application's toolbars.
 *
 * Creates the toolbars and adds actions to them.
 */
void GTestRunner::setupToolBars() {
	QAction* runTestsAct = new QAction(tr("Run"), this);
	QAction* stopTestsAct = new QAction(tr("Stop"), this);
	QAction* addTestsAct = new QAction(tr("Add"), this);
	QAction* removeTestsAct = new QAction(tr("Remove"), this);

	QObject::connect(runTestsAct, SIGNAL(triggered()),
					 this, SLOT(runTests()));

	QObject::connect(addTestsAct, SIGNAL(triggered()),
					 this, SLOT(addTests()));

	testTreeTools.addAction(runTestsAct);
	testTreeTools.addAction(stopTestsAct);
	testTreeTools.addAction(addTestsAct);
	testTreeTools.addAction(removeTestsAct);
}

/*! \brief Sets up the application's layout.
 *
 * After all the elements are created, they are placed
 * in their appropriate layout.
 */
void GTestRunner::setupLayout() {
	QGroupBox *treeBox = new QGroupBox(tr("Unit Tests Loaded"));
	QVBoxLayout *treeLayout = new QVBoxLayout;
	treeLayout->addWidget(&testTreeTools);
	treeLayout->addWidget(&testTree);
	treeBox->setLayout(treeLayout);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(treeBox);

	centralWidget.setLayout(mainLayout);
}

/*! \brief Slot to prompt a dialog to have the user add unit tests to run.
 *
 * This function prompts a file selection dialog and has the user select
 * the gtest executable they wish to add to the testing setup. It has some
 * basic error handling, however, it could use a bit more robustness.
 * \todo TODO::Investigate security issues with this. Any way to detect gtest before running?
 */
void GTestRunner::addTests() {
	bool addResolved = false; //flag to see if we've got a good test.
	QString filepath;
	GTestExecutable* newTest = new GTestExecutable(this);
	while(!addResolved) {
		filepath = QFileDialog::getOpenFileName(this, tr("Select Google Test Executable"));
		qDebug() << "File path received:" << filepath;
		if(filepath.isEmpty()) //Empty path means user clicked cancel.
			return;

		//Non-empty path, so let's check out whether we can use it or not.
		newTest->setExecutablePath(filepath);
		GTestExecutable::STATE state = newTest->getState();
		switch(state) {
			case GTestExecutable::FILE_NOT_FOUND: {
				QMessageBox::StandardButton btnPressed = QMessageBox::warning(this,
						tr("File Not Found"),
						tr("It appears the filename entered does not exist. Please select a valid Google test executable."),
						QMessageBox::Retry | QMessageBox::Abort);
				switch(btnPressed) {
					case QMessageBox::Retry:
						continue; //go back to top of loop.
					case QMessageBox::Abort: default:
						return;   //exit attempt to add a test executable
					}
			break;
			}
			case GTestExecutable::INSUFFICIENT_PRIVILEGES: {
				QMessageBox::StandardButton btnPressed = QMessageBox::warning(this,
						tr("Insufficient Permissions"),
						tr("It appears that you do not have sufficient privileges to execute this file. \
						   GTestRunner can attempt to reset the permissions to enable executing this test file. \
						   \nWould you like GTestRunner to attempt this?"),
						QMessageBox::Yes | QMessageBox::No);
				switch(btnPressed) {
					case QMessageBox::Yes:
						continue;
					case QMessageBox::No: default: {
						QMessageBox::StandardButton btnPressed = QMessageBox::question(this,
							tr("Try Again?"),
							tr("Would you like to try again and select another file?"),
							QMessageBox::Yes | QMessageBox::No);
						switch(btnPressed) {
							case QMessageBox::Yes:
								continue;
							case QMessageBox::No: default:
								return;
						}
					break;
					}
				}
				break;
			}
			case GTestExecutable::VALID: {
				addResolved = true;
				break;
			}
		}
	}

	//We've got a good test, so let's have it send up a listing.
	invokeListingRetrieval(newTest);
}

/*! \brief Sets up and requests a listing from a GTestExecutable.
 *
 * This creates a signal / slot mapping so that the runner is informed
 * when the listing is ready.
 */
void GTestRunner::invokeListingRetrieval(GTestExecutable* gtest) {
	//Have the executable inform us when the listing is ready.
	QObject::connect(gtest, SIGNAL(listingReady(GTestExecutable*)),
					 this, SLOT(updateListing(GTestExecutable*)));
	gtest->produceListing();
}

/*! \brief Updates the tree with a listing provided by 'gtest'.
 *
 * This function takes the 'gtest' and retrieves its listing. It then
 * takes the listing and populates the main test tree with the tests
 * it provides.
 * \todo TODO::Update a listing that already exists.
 * \todo TODO::Refactor this method. It's a bit too long.
 */
void GTestRunner::updateListing(GTestExecutable* gtest) {
	qDebug() << "Updating listing";
	const int exitCode = gtest->getExitCode();
	qDebug() << "got exit code:" << exitCode;
	QString exePath = gtest->getExecutablePath();
	qDebug() << "got exe path:" << exePath;
	if(exitCode != 0) {
		QMessageBox::critical(this, "Error Retrieving Test Listing",
					QString("Exit code").append(exitCode).append("was returned from the Google Test executable at").append(exePath).append(". Are you sure this is a valid Google Test unit test executable?"));
		//TODO: perform switch statement of process error.
		return;
	}
	QStringList testList = gtest->getListing();
	qDebug() << "Retrieved listing. Size ="<<testList.size();
	QStringList::iterator it = testList.begin();


	TestTreeWidgetItem* testContainer = new TestTreeWidgetItem(&testTree,QStringList() << exePath);
	testContainer->setFlags(Qt::ItemIsTristate | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
	testContainer->setCheckState(0, Qt::Checked);
	QVariant var;
	var.setValue<GTestExecutable*>(gtest);
	testContainer->setData(0,Qt::UserRole,var);
	QObject::connect(this, SIGNAL(aboutToRunTests()),
					 gtest, SLOT(resetRunState()));
	QObject::connect(this, SIGNAL(runningTests()),
					 gtest, SLOT(runTest()));
	QSignalMapper* signalMap = new QSignalMapper(&testTree);
	signalMap->setMapping(gtest, testContainer);
	QObject::connect(gtest, SIGNAL(testResultsReady()),
					 signalMap, SLOT(map()));

	TestTreeWidgetItem* topLevelItem = 0;
	TestTreeWidgetItem* newItem = 0;
	GTestSuite* suite = 0;
	GTest* test = 0;
	while(it != testList.end()) {
		qDebug() << *it;
		if(it->endsWith(".")) {
			//drop the '.' and make it a data item
			QString suiteName = it->left(it->size()-1);
			topLevelItem = new TestTreeWidgetItem(testContainer, QStringList()<<suiteName);
			topLevelItem->setFlags(Qt::ItemIsTristate | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
			topLevelItem->setCheckState(0, Qt::Checked);
			suite = new GTestSuite(gtest, suiteName);
			var.setValue<GTestSuite*>(suite);
			topLevelItem->setData(0,Qt::UserRole,var);
			gtest->addTest(suite);

			signalMap->setMapping(suite, topLevelItem);
			QObject::connect(suite, SIGNAL(testResultsReady()),
							 signalMap, SLOT(map()));
		}
		else {
			//drop the spaces and make it a data item
			QString testName = it->right(it->size()-2);
			newItem = new TestTreeWidgetItem(topLevelItem, QStringList()<<testName);
			newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
			newItem->setCheckState(0,Qt::Checked);
			test = new GTest(suite, testName);
			var.setValue<GTest*>(test);
			newItem->setData(0,Qt::UserRole,var);
			suite->addTest(test);

			signalMap->setMapping(test, newItem);
			QObject::connect(test, SIGNAL(testResultsReady()),
							 signalMap, SLOT(map()));
		}
		++it;
	}
	QObject::connect(signalMap, SIGNAL(mapped(QObject*)),
					 &testTree, SLOT(populateTestResult(QObject*)));
}

/*! \brief Updates all the listings for every GTestExecutable.
 *
 * \todo TODO::Launch this function from a 'refresh' button.
 * \todo TODO::Fill out this method.
 */
void GTestRunner::updateAllListings() {

}

/*! \brief Runs all tests that are checked.
 *
 */
void GTestRunner::runTests() {
	emit aboutToRunTests();
	QTreeWidgetItemIterator it(&testTree, QTreeWidgetItemIterator::NoChildren);
	while(*it) {
		if((*it)->checkState(0) != Qt::Checked)
			continue;
		(*it)->data(0,Qt::UserRole).value<GTest*>()->run();
		it++;
	}
	for(int i=0, j=testTree.topLevelItemCount(); i<j; i++) {
		QTreeWidgetItem* topLevelItem = testTree.topLevelItem(i);
		if(topLevelItem->checkState(0) == Qt::Unchecked)
			continue;
		GTestExecutable* gtest = topLevelItem->data(0,Qt::UserRole).value<GTestExecutable*>();
		if(gtest != 0 && gtest->getState() == GTestExecutable::VALID) {
			//QObject::connect(gtest, SIGNAL(testResultsReady(GTest*)),
			//				 this, SLOT(fillTestResults(GTestExecutable*)));
			gtest->setRunFlag(true);
		}
		else
			QMessageBox::warning(this,"Invalid Google Test",
					"An error has occurred when attempting to run a Google Test.");
	}
	emit runningTests();
}

/*! \brief Sets the test result of the given gtest.
 *
 * \todo TODO::Check to see if this function is needed/used.
 */
void GTestRunner::setTestResult(GTest* /*gtest*/)
{}

/*! \brief Slot to handle maintaining valid checkbox states.
 *
 * This function handles correcting the check states. If a parent is
 * part checked and all its children are part checked, we transition
 * to fully checked (previous state must have been unchecked and a click
 * must have been made on the parent). If any of the children are not
 * part checked, we know that we did not in fact click on the parent
 * (and thus we can immediately return), so a part checked parent is fine.
 * \param item The item in the tree that was clicked.
 * \param column The column of the 'item' that was clicked.
 */
void GTestRunner::treeItemClicked(QTreeWidgetItem* item, int /*column*/) {
	if(item->childCount() == 0 || item->parent() == 0)
		return;
	if(item->checkState(0) ==Qt::PartiallyChecked) {
		for(int i=0,j=item->childCount();i<j;i++)
			if(item->child(i)->checkState(0) & (Qt::Unchecked | Qt::Checked))
				return;
		item->setCheckState(0,Qt::Checked);
	}
}


