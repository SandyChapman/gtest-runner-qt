/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * TestTreeModel.cpp - Created on 2010-08-28
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

#include <QApplication>
#include <QDebug>
#include <QMap>
#include <QMessageBox>
#include <QModelIndexList>
#include <QSharedPointer>
#include <QStack>

#include "TestTreeModel.h"
#include "TreeItem.h"
#include "GTestExecutable.h"

/*! \brief Constructor
 *
 */
TestTreeModel::TestTreeModel(QObject* parent, QPlainTextEdit *result)
: TreeModel(parent)
{
	QList<QMap<int, QVariant> > data;
	QMap<int, QVariant> datum;
	datum.insert(Qt::DisplayRole, "Test Name");
	data.append(datum);
	rootItem.setData(data);

    m_result = result;
}

/*! \brief Destructor
 *
 */
TestTreeModel::~TestTreeModel() {}

/*! \brief This function adds a data source for this model.
 *
 * The data source specified must be a string containing the path to a
 * google test executable.
 * \return An error code whether the addition of the data source was successful.
 * \todo Set this function to take a QFile instead of a string.
 */
TestTreeModel::ERROR TestTreeModel::addDataSource(const QString filepath, const QString outputDir) {
	QSharedPointer<GTestExecutable> newTest(new GTestExecutable(this));
	newTest->setExecutablePath(filepath);
    newTest->setResultPath(outputDir);
	switch(newTest->getState()) {
	case GTestExecutable::VALID:
        QObject::connect(newTest.data(), SIGNAL(listingReady(GTestExecutable*)), this, SLOT(updateListing(GTestExecutable*)));
        QObject::connect(newTest.data(), SIGNAL(BeginTest(GTest*)), this, SLOT(BeginTest(GTest*)));
        QObject::connect(newTest.data(), SIGNAL(EndTest(GTest*, bool)), this, SLOT(EndTest(GTest*, bool)));
        QObject::connect(this, SIGNAL(aboutToRunTests()), newTest.data(), SLOT(resetRunState()));
		//We insert it so that it doesn't auto-delete from the shared ptr.
		//Will probably be useful later on when we want to save settings.
		testExeHash.insert(newTest->objectName(), newTest);
		//We've got test, so let's have it send up a listing.
		newTest->produceListing();
		return NO_ERROR;
	case GTestExecutable::FILE_NOT_FOUND:
		return FILE_NOT_FOUND;
	case GTestExecutable::INSUFFICIENT_PRIVILEGES:
		return INSUFFICIENT_PRIVILEGES;
	default:
		return UNKNOWN;
	}
}

/*! \brief Creates a tree item and sets up some slot/signals.
 *
 * This function is a help to the updateListing method which enables
 * simplified creation of new tree items. It is templated as I didn't
 * feel like writing three copies of the same code (thus making the
 * function useless). It simply sets up some common states among tree
 * items and some signal/slots between the tree item and its corresponding
 * test item.
 */
template <class T, class U>
TreeItem* TestTreeModel::createNewTreeItem(T parent, U* test) {
	QVariant var;
	QList<QMap<int, QVariant> > data;
	QMap<int, QVariant> datum;
	var.setValue<QString>(test->objectName());
	datum.insert(Qt::DisplayRole, var);
	var.setValue<GTest*>(test);
	datum.insert(Qt::UserRole, var);
    var.setValue(static_cast<int>(Qt::Checked));
	datum.insert(Qt::CheckStateRole, var);
	data.append(datum);

	TreeItem* newTreeItem(new TreeItem(data, parent));
	itemTestHash.insert(test, newTreeItem);
	QObject::connect(test, SIGNAL(testResultsReady()),
					 this, SLOT(populateTestResult()));
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
void TestTreeModel::updateListing(GTestExecutable* gtest) {
	const int exitCode = gtest->getExitCode();
	QString exePath = gtest->getExecutablePath();
	if(exitCode != 0) {
		QMessageBox::critical(0, "Error Retrieving Test Listing",
					QString("Exit code").append(exitCode).append("was returned from the Google Test executable at").append(exePath).append(". Are you sure this is a valid Google Test unit test executable?"));
		//! \todo Perform switch statement of process error.
		return;
	}

	TreeItem* suiteTreeItem;
	TreeItem* treeItem;
	TreeItem* exeTreeItem = rootItem.findChild(gtest->objectName(), Qt::DisplayRole);

	if(!exeTreeItem) {
		//If we haven't added the test yet, make one now.
		exeTreeItem = createNewTreeItem<TreeItem*,GTestExecutable>(&rootItem, gtest);
		this->insertItem(exeTreeItem, rootItem.childCount(), &rootItem);

		QObject::connect(this, SIGNAL(runningTests()),
						 gtest, SLOT(runTest()));

	}
	//Get which of the tests are new. This should be all of them the first
	//time through for this test.
	QSet<QString> newTests = gtest->getListing() - gtest->getOldListing();

	//! \todo Remove old tests that are no longer present.

	//Iterate through all the suites.
	QList<GTestSuite*> suiteList = gtest->findChildren<GTestSuite*>();
	QList<GTestSuite*>::iterator suiteIter = suiteList.begin();
	while(suiteIter != suiteList.end()) {
		if(!newTests.contains((*suiteIter)->objectName())) {
			suiteIter++;
			continue; //already have this added
		}

		//Create a new GTestSuite tree item.
		suiteTreeItem = createNewTreeItem<TreeItem*,GTestSuite>(exeTreeItem, *suiteIter);
		this->insertItem(suiteTreeItem, exeTreeItem->childCount(), exeTreeItem);

		//Iterate through all the tests of this suite.
		QList<GTest*> testList = (*suiteIter)->findChildren<GTest*>();
		QList<GTest*>::iterator testIter = testList.begin();
		while(testIter != testList.end()) {
			if(!newTests.contains((*suiteIter)->objectName().append('.').append((*testIter)->objectName()))) {
				testIter++;
				continue; //already have this added
			}

			//Create a new GTest tree item.
			treeItem = createNewTreeItem<TreeItem*, GTest>(suiteTreeItem, *testIter);
			this->insertItem(treeItem, suiteTreeItem->childCount(), suiteTreeItem);
			++testIter;
		}
		++suiteIter;
	}
	emit layoutChanged();
}

void TestTreeModel::BeginTest(GTest* test){
    TreeItem* treeItem = itemTestHash.value(test);
    if(treeItem == 0)
        return;
    QModelIndex index = createIndex(treeItem->row(), treeItem->column(), treeItem);
    selectionModel->setCurrentIndex(index, QItemSelectionModel::ClearAndSelect);

}

void TestTreeModel::EndTest(GTest * test, bool success){
    TreeItem* treeItem = itemTestHash.value(test);
    if(treeItem == 0)
        return;
    QModelIndex index = createIndex(treeItem->row(), treeItem->column(), treeItem);
    if(success)
        setData(index, QVariant(QBrush(QColor(0xAB,0xFF,0xBB,0xFF))), Qt::BackgroundRole);
    else
        setData(index, QVariant(QBrush(QColor(0xFF,0x88,0x88,0xFF))), Qt::BackgroundRole);
}

/*! \brief Populates a test result into the test tree.
 *
 * This function takes a QObject* which should be a TestTreeWidgetItem.
 * If it is called with anything else, the function does nothing. It takes
 * the item and retrieves its corresponding GTest. The GTest should already
 * have its updated result attached to it, thus, it retrieves the result
 * and sets the item to the appropriate pass / fail state.
 * \param item This should be a TestTreeWidgetItem which needs its test result updated.
 */
void TestTreeModel::populateTestResult() {
	GTest* test = static_cast<GTest*>(sender());
	if(test == 0)
		return; //! \todo exception management stuff here
	TreeItem* treeItem = itemTestHash.value(test);
	if(treeItem == 0)
		return; //! \todo exception management stuff here

	QVariant var = treeItem->data(0, Qt::UserRole);
	GTest* testItem = var.value<GTest*>();
	if(testItem == 0) {
		testItem = var.value<GTestSuite*>();
		if(testItem == 0) {
			testItem = var.value<GTestExecutable*>();
			if(testItem == 0)
				return; //! \todo exception management stuff here
		}
	}

	const GTestResults* testResults = testItem->getTestResults();
	if(testResults == 0)
		return;

	QModelIndex index = createIndex(treeItem->row(), treeItem->column(), treeItem);
	if(testResults->getFailureCount() == 0)
		setData(index, QVariant(QBrush(QColor(0xAB,0xFF,0xBB,0xFF))), Qt::BackgroundRole);
	else
		setData(index, QVariant(QBrush(QColor(0xFF,0x88,0x88,0xFF))), Qt::BackgroundRole);

    if(m_processCount.deref())
        emit allTestsCompleted();
}

/*! \brief Clear background of the Test Tree before running it.
 *
 */
void TestTreeModel::ClearTestTreeBackground(TreeItem * treeItem){
    QModelIndex index = createIndex(treeItem->row(), treeItem->column(), treeItem);
    setData(index, QVariant(QBrush()), Qt::BackgroundRole); // this will restore the default value

    TreeItem * testitem;
    foreach(testitem, treeItem->children()){
        ClearTestTreeBackground(testitem);
    }
}


/*! \brief Runs all tests that are checked.
 *
 * This is the only way tests results are populated into the model.
 * This is the method the model uses to notify the GTestExecutables to run
 * their corresponding executable on the file system and to populate its results
 * into the data model.
 */
void TestTreeModel::runTests() {
	emit aboutToRunTests();

    ClearTestTreeBackground(&rootItem);

	QStack<TreeItem* > stack;
	QList<TreeItem* > children;
	TreeItem* item;
	stack.push(&rootItem);

	while(!stack.isEmpty()) {
		item = stack.pop();
		GTest* test = item->data(0, Qt::UserRole).value<GTest*>();
        if(test && (item->data(0, Qt::CheckStateRole).value<int>() == Qt::Checked)){
            m_processCount.ref();
			test->run();
            }
		else {
			children = item->children();
			QList<TreeItem* >::iterator it = children.begin();
			while(it != children.end()) {
				stack.push(*it);
				++it;
			}
		}
	}

	emit runningTests();
}

/*! \brief Updates all the listings for every GTestExecutable.
 *
 * This function iterates through the collection of GTestExecutables
 * and invokes their listing retrieval mechanism.
 */
void TestTreeModel::updateAllListings() {
	QHash<QString, QSharedPointer<GTestExecutable> >::iterator it = testExeHash.begin();
	while(it != testExeHash.end()) {
		(*it)->produceListing();
		++it;
	}
}

/*! \brief Retrieves the data specified by the given index and role.
 *
 * This function only gives metadata specific data, specified by the
 * Qt::MetaDataRole. Other data is requested from the parent class.
 * \see TreeModel::data()
 * \param index A model index to this tree model.
 * \param role The role to retrieve data from.
 * \return A QVariant of the data.
 */
QVariant TestTreeModel::data(const QModelIndex& index, int role) const {
	TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
	if(item)
		return item->data(0, role);
	return QVariant();
}

/*! \brief Sets the data of the given index and role to the variant var.
 *
 * This function only handles the Qt::CheckStateRole role to manage appropriate
 * checking of check boxes (e.g. when a parent item is checked, so are its children.
 * \see TestTreeModel::setCheckState() for more details. All other data is passed
 * to the parent class to handle.
 * \param index The index to set data on.
 * \param var The value to set the index and role to.
 * \param role The role to set the value to.
 * \return true if successfully set, false otherwise.
 */
bool TestTreeModel::setData(const QModelIndex& index, const QVariant& var, int role) {
	if(role != Qt::CheckStateRole)
		return TreeModel::setData(index, var, role);

	if(!index.isValid())
		return false;
	TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
	if(!item)
		return false;

	return this->setCheckState(item, static_cast<Qt::CheckState>(var.value<int>()));
}

/*! \brief Sets the checkstate on the given item.
 *
 * This function is recursive. The recursion is only allowed to propagate
 * away from the original item that has been checked. This function maintains
 * the consistency of the check states of items contained in the model. The rules
 * are described as follows:
 * \li An item that is a leaf (i.e. has no children) cannot be partially checked.
 * \li An item must be set to partially checked if it contains both checked and unchecked
 * 	   or partially checked child items.
 * \li If a partially checked item is changed to checked, all its children will be checked.
 * \li If a checked item is set to unchecked, all its children will be unchecked.
 * \return true of the checking was successful, false otherwise.
 */
bool TestTreeModel::setCheckState(TreeItem* item, Qt::CheckState newState, int recursionDirection) {
	if(!item)
		return false;
	bool retval;
	if(newState == Qt::Checked || newState == Qt::Unchecked) {
		if(recursionDirection & TO_CHILDREN)
			for(int i=0,j=item->childCount();i<j;i++)
				setCheckState(item->child(i), newState, TO_CHILDREN);

		retval = item->setData(QVariant(static_cast<int>(newState)), 0, Qt::CheckStateRole);
		if(recursionDirection & TO_PARENT) {
			setCheckState(item->parent(), Qt::PartiallyChecked, TO_PARENT);
		}
	}
	else /*newState == Qt::PartiallyChecked*/ {
		bool checked = false;
		bool unchecked = false;
		Qt::CheckState childState;
		for(int i=0,j=item->childCount();i<j&&(!checked||!unchecked);i++) {
			childState = static_cast<Qt::CheckState>(item->child(i)->data(0, Qt::CheckStateRole).value<int>());
			if(childState == Qt::Checked)
				checked = true;
			else if(childState == Qt::Unchecked)
				unchecked = true;
		}
		if(recursionDirection == (TO_PARENT | TO_CHILDREN)) {
			if(checked && !unchecked)
				newState = Qt::Unchecked;
			else if(!checked && unchecked)
				newState = Qt::Checked;
			for(int i=0,j=item->childCount();i<j;i++)
				setCheckState(item->child(i), newState, TO_CHILDREN);
			retval = item->setData(QVariant(static_cast<int>(newState)), 0, Qt::CheckStateRole);
		}
		else {
			if(checked && !unchecked)
				newState = Qt::Checked; //only checked children
			else if(!checked && unchecked)
				newState = Qt::Unchecked; //only unchecked children
			retval = item->setData(QVariant(static_cast<int>(newState)), 0, Qt::CheckStateRole);
		}
		if(recursionDirection & TO_PARENT) {
			setCheckState(item->parent(), Qt::PartiallyChecked, TO_PARENT);
		}
	}
	QModelIndex index(createIndex(item->row(),0,item));
	emit dataChanged(index, index);
	return retval;
}

/*! \brief Populate the result pane.
 *
 */
void  TestTreeModel::printResult ( const QModelIndex & selected, const QModelIndex & deselected ){
    Q_UNUSED(deselected);
    QString Result;

    TreeItem *treeItem = static_cast<TreeItem*>(selected.internalPointer());
    if(treeItem != 0){
        QVariant var = treeItem->data(0, Qt::UserRole);
        GTest* testItem = var.value<GTest*>();
        if(testItem != 0){
            const GTestResults* testResults = testItem->getTestResults();
            if(testResults != 0){
                QStringList messageList = testResults->getFailureMessages();
                int ii = 0;
                while(messageList.count()>ii){
                    Result.append(messageList.at(ii++));
                }
            }
        }
    }
    m_result->setPlainText(Result);
}

void TestTreeModel::removeSelectedTests() {
	QModelIndexList selectedIndexes = this->selectionModel->selectedIndexes();
	QModelIndexList::iterator it = selectedIndexes.begin();
	for(; it != selectedIndexes.end(); ++it) {
		TreeItem* item = static_cast<TreeItem*>(it->internalPointer());
		if(!item)
			continue;
		GTest* gtest = item->data(0, Qt::UserRole).value<GTest*>();
		GTestExecutable* gtestExe = dynamic_cast<GTestExecutable*>(gtest);
		if(!gtestExe) {
			QWidget* parent = 0;
			if(QObject::parent()->isWidgetType())
				parent = static_cast<QWidget*>(QObject::parent());
			QMessageBox::information(parent, "Info", "Currently, only top level tests can be removed.", QMessageBox::Ok);
			continue;
		}
		this->beginRemoveRows(this->createIndex(0, 0, &rootItem), item->row(), item->row());
		item->parent()->removeChild(item);
		this->testExeHash.remove(gtest->objectName());
		this->endRemoveRows();
	}
}
