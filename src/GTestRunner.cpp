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
#include "TestTreeModel.h"

#include <QAction>
#include <QDebug>
#include <QFileDialog>
#include <QGroupBox>
#include <QMessageBox>
#include <QSharedPointer>
#include <QSignalMapper>
#include <QTreeView>
#include <QTreeWidgetItem>
#include <QTreeWidgetItemIterator>
#include <QVBoxLayout>

/*! \brief Constructor
 *
 * \param parent The parent QWidget object. This should be null, as this is the main window.
 * \param flags The main window flags for the application.
 * <a href="http://doc.qt.nokia.com/4.6/qt.html#WindowType-enum">Qt::WFlags Reference</a>
 */
GTestRunner::GTestRunner(QWidget *parent, Qt::WindowFlags flags)
 : QMainWindow(parent, flags)
{
	setupUi(this);
	setup();
}

/*! \brief Destructor
 *
 */
GTestRunner::~GTestRunner()
{
}

/*! \brief Sets up the application GUI.
 *
 * This function sets up some high level objects and gets them
 * to set up as well.
 */
void GTestRunner::setup() {
    testModel = new TestTreeModel(this, result);
	testTree->setModel(testModel);
	testTree->setSelectionBehavior(QAbstractItemView::SelectRows);
	testTree->setSelectionMode(QAbstractItemView::ExtendedSelection);

    QItemSelectionModel *ism = testTree->selectionModel();
    testModel->setSelectionModel(ism);
    QObject::connect(ism, SIGNAL(currentChanged(const QModelIndex& , const QModelIndex&)),
                     testModel, SLOT(printResult(const QModelIndex& , const QModelIndex&)));

	QObject::connect(this->aboutQtAction, SIGNAL(triggered()),
					 qApp, SLOT(aboutQt()));

	QObject::connect(this->importTestAction, SIGNAL(triggered()),
					 this, SLOT(addTests()));

    QObject::connect(this->runTestsAction, SIGNAL(triggered()),
                     testModel, SLOT(runTests()));

    QObject::connect(this->runTestsAction, SIGNAL(triggered()),
                     this, SLOT(DisableRunAction()));

    QObject::connect(this->stopTestsAction, SIGNAL(triggered()),
                     testModel, SLOT(AbortCurrentTests()));

    QObject::connect(testModel, SIGNAL(allTestsCompleted()),
                     this, SLOT(EnableRunAction()));

    QObject::connect(this->refreshAction, SIGNAL(triggered()),
					 testModel, SLOT(updateAllListings()));

	QObject::connect(this->removeTestsAction, SIGNAL(triggered()),
					 testModel, SLOT(removeSelectedTests()));

	QObject::connect(this->exitAction, SIGNAL(triggered()),
					 qApp, SLOT(quit()));
}

/*! \brief Add executable from the command line
 *
 */
void GTestRunner::AddExecutable(QString filepath){
    testModel->addDataSource(filepath, m_resultspath);
}


/*! \brief Disable run button while tests are running.
 *
 */
void GTestRunner::DisableRunAction() {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    this->runTestsAction->setDisabled(true);
}

/*! \brief Enable run button when tests are finished.
 *
 */
void GTestRunner::EnableRunAction() {
    QApplication::restoreOverrideCursor();
    this->runTestsAction->setDisabled(false);
    statusBar()->showMessage("Done.");
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
	while(!addResolved) {
		filepath = QFileDialog::getOpenFileName(this, tr("Select Google Test Executable"));
		qDebug() << "File path received:" << filepath;
		if(filepath.isEmpty()) //Empty path means user clicked cancel.
			return;

		//Non-empty path, so let's check out whether we can use it or not.
        switch(testModel->addDataSource(filepath, m_resultspath)) {
			case TestTreeModel::FILE_NOT_FOUND: {
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
			case TestTreeModel::INSUFFICIENT_PRIVILEGES: {
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
			case TestTreeModel::UNKNOWN: {
				QMessageBox::warning(this,
					tr("Unknown Error"),
					tr("An unknown error has occured."),
					QMessageBox::Ok);
				break;
			}
			case TestTreeModel::NO_ERROR: {
				return;
//				testTree->insertExecutable(newTest);
//				addResolved = true;
//				break;
			}
		}
	}
}

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


