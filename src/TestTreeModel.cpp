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
#include <QSharedPointer>
#include <QStack>

#include "TestTreeModel.h"
#include "TestTreeItem.h"
#include "GTestExecutable.h"

/*! \brief Constructor
 *
 */
TestTreeModel::TestTreeModel(QObject* parent)
: QAbstractItemModel(parent)
{
	QMap<int, QVariant> data;
	data.insert(0,"Test Name");
	rootItem = new TestTreeItem(data, 0);
}

/*! \brief Destructor
 *
 */
TestTreeModel::~TestTreeModel() {
	delete rootItem;
}

/*! \brief Gives a count of the number of columns contained by the index 'parent'.
 *
 * If the index is invalid the number of columns contained by the rootItem are
 * returned.
 * \return The number of columns given by the index 'parent' or the number of
 * columns in the rootItem is 'parent' is invalid.
 */
int TestTreeModel::columnCount(const QModelIndex& parent) const {
    if (parent.isValid())
        return static_cast<TestTreeItem*>(parent.internalPointer())->columnCount();
    else
        return rootItem->columnCount();
}

/*! \brief Accesses data contained within the model.
 *
 * The data contained in the index 'index' for the given 'role' are returned
 * within a QVariant. If the index is invalid, or if the item specified by
 * the index does not contain data for the role 'role', an empty QVariant
 * is returned.
 * \return The data specified by the index 'index' and the role 'role'.
 */
QVariant TestTreeModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return QVariant();

    switch(role) {
    //list supported data accesses here.
    case Qt::BackgroundColorRole:
    case Qt::CheckStateRole:
    case Qt::DisplayRole: {
        TestTreeItem *item = static_cast<TestTreeItem*>(index.internalPointer());
        if(item)
        	return item->data(role);
        return QVariant();
    }
    default:
        return QVariant();
    }
}

/*! \brief Returns the flags for the item specified by the given index.
 *
 * If the index is invalid, this function returns 0. Otherwise, items within
 * the TestTreeModel are enabled, selectable, checkable, and if they have children,
 * are tristate checkable.
 * \see TestTreeModel::setCheckState() for more information on check states.
 */
Qt::ItemFlags TestTreeModel::flags(const QModelIndex& index) const {
	if (!index.isValid())
		 return 0;

	Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;

	if(static_cast<TestTreeItem*>(index.internalPointer())->childCount() > 0)
		flags |= Qt::ItemIsTristate;

	return flags;
}

/*! \brief Retrieves the header information for the model.
 *
 * Section and orientation are ignored for this model. We only provide data in
 * one dimension so there are unnecessary. The data for the given role is provided.
 * \return The data for the given role.
 */
QVariant TestTreeModel::headerData(int /*section*/, Qt::Orientation /*orientation*/, int role) const {
	if(rootItem)
		return rootItem->data(role);
	return QVariant();
}

/*! \brief Retrieves a model index at the specified (row, columns) from the given parent.
 *
 * Retrieves a model index for the model. If parent is not specified, or is a default
 * constructed index, an index to a top level item is returned.
 * \return A model index for the given row and column with the parent 'parent'.
 */
QModelIndex TestTreeModel::index(int row, int column, const QModelIndex& parent) const {
	if(!hasIndex(row, column, parent))
		return QModelIndex();

	TestTreeItem* parentItem;
	TestTreeItem* childItem;

	if(parent.isValid())
		parentItem = static_cast<TestTreeItem*>(parent.internalPointer());
	else
		parentItem = rootItem;

	childItem = parentItem->child(row);
	if(childItem)
		return createIndex(row, column, childItem);
	return QModelIndex();
}

/*! \brief Retrieves the parent of the index provided.
 *
 * If the index is invalid or is a top level item, an invalid index is returned.
 * \return The parent of the index if it exists. An invalid index otherwise.
 */
QModelIndex TestTreeModel::parent(const QModelIndex &index) const {
	if (!index.isValid())
		return QModelIndex();

	TestTreeItem *childItem = static_cast<TestTreeItem*>(index.internalPointer());
	TestTreeItem *parentItem = childItem->parent();

	if (parentItem == rootItem || !parentItem)
		return QModelIndex();

	return createIndex(parentItem->row(), 0, parentItem);
}

/*! \brief Retrieves the number of rows provided by the parent 'parent'.
 *
 * \return If the index is valid, the number of children held by 'parent' is returned.
 * Else, the number of top level items is returned.
 */
int TestTreeModel::rowCount(const QModelIndex& parent) const {
    TestTreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TestTreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

/*! \brief Sets data on the model.
 *
 * It is trusted that the user of this function respects the information provided by
 * \see TestTreeModel::flags(). This function will set the data contained in var
 * on the item specified by index for the given role. If data in the model is changed,
 * the TestTreeModel::dataChanged() signal is emitted.
 */
bool TestTreeModel::setData(const QModelIndex& index, const QVariant& var, int role) {
	if(!index.isValid())
		return false;
	TestTreeItem* item = static_cast<TestTreeItem*>(index.internalPointer());
	if(!item)
		return false;

	bool dataModified;
	if(role == Qt::CheckStateRole)
		dataModified = this->setCheckState(item, static_cast<Qt::CheckState>(var.value<int>()));
	else {
		dataModified = item->setData(var, role);
		if(dataModified)
			emit dataChanged(index, index);
	}
	return dataModified;
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
bool TestTreeModel::setCheckState(TestTreeItem* item, Qt::CheckState newState, int recursionDirection) {
	if(!item)
		return false;
	bool retval;
	if(newState == Qt::Checked || newState == Qt::Unchecked) {
		if(recursionDirection & TO_CHILDREN)
			for(int i=0,j=item->childCount();i<j;i++)
				setCheckState(item->child(i), newState, TO_CHILDREN);

		retval = item->setData(QVariant(static_cast<int>(newState)), Qt::CheckStateRole);
		if(recursionDirection & TO_PARENT) {
			setCheckState(item->parent(), Qt::PartiallyChecked, TO_PARENT);
		}
	}
	else /*newState == Qt::PartiallyChecked*/ {
		bool checked = false;
		bool unchecked = false;
		Qt::CheckState childState;
		for(int i=0,j=item->childCount();i<j&&(!checked||!unchecked);i++) {
			childState = static_cast<Qt::CheckState>(item->child(i)->data(Qt::CheckStateRole).value<int>());
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
			retval = item->setData(QVariant(static_cast<int>(newState)), Qt::CheckStateRole);
		}
		else {
			if(checked && !unchecked)
				newState = Qt::Checked; //only checked children
			else if(!checked && unchecked)
				newState = Qt::Unchecked; //only unchecked children
			retval = item->setData(QVariant(static_cast<int>(newState)), Qt::CheckStateRole);
		}
		if(recursionDirection & TO_PARENT) {
			setCheckState(item->parent(), Qt::PartiallyChecked, TO_PARENT);
		}
	}
	QModelIndex index(createIndex(item->row(),0,item));
	emit dataChanged(index, index);
	return retval;
}

/*! \brief This function adds a data source for this model.
 *
 * The data source specified must be a string containing the path to a
 * google test executable.
 * \return An error code whether the addition of the data source was successful.
 * \todo Set this function to take a QFile instead of a string.
 */
TestTreeModel::ERROR TestTreeModel::addDataSource(const QString filepath) {
	QSharedPointer<GTestExecutable> newTest = QSharedPointer<GTestExecutable>(new GTestExecutable(this));
	newTest->setExecutablePath(filepath);
	switch(newTest->getState()) {
	case GTestExecutable::VALID:
		QObject::connect(newTest.data(), SIGNAL(listingReady(GTestExecutable*)),
						 this, SLOT(updateListing(GTestExecutable*)));
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
TestTreeItem* TestTreeModel::createNewTreeItem(T* parent, U* test) {
	QVariant var;
	QMap<int, QVariant> data;
	var.setValue<QString>(test->objectName());
	data.insert(Qt::DisplayRole, var);
	var.setValue<GTest*>(test);
	data.insert(Qt::UserRole, var);
	var.setValue(static_cast<int>(Qt::Unchecked));
	data.insert(Qt::CheckStateRole, var);
	TestTreeItem* newTreeItem = new TestTreeItem(data, parent);

	itemTestHash.insert(test, newTreeItem);

	QObject::connect(test, SIGNAL(destroyed()),
					 this, SLOT(removeSenderItem()));
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

	TestTreeItem* suiteTreeItem = 0;
	TestTreeItem* testTreeItem = 0;
	TestTreeItem* exeTreeItem = rootItem->findChild(gtest->objectName(), Qt::DisplayRole);

	if(!exeTreeItem) {
		//If we haven't added the test yet, make one now.
		exeTreeItem = createNewTreeItem<TestTreeItem,GTestExecutable>(rootItem,gtest);
		insertItem(exeTreeItem, rootItem->childCount(), rootItem);

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
		suiteTreeItem = createNewTreeItem<TestTreeItem,GTestSuite>(exeTreeItem, *suiteIter);
		insertItem(suiteTreeItem, exeTreeItem->childCount(), exeTreeItem);

		//Iterate through all the tests of this suite.
		QList<GTest*> testList = (*suiteIter)->findChildren<GTest*>();
		QList<GTest*>::iterator testIter = testList.begin();
		while(testIter != testList.end()) {
			if(!newTests.contains((*suiteIter)->objectName().append('.').append((*testIter)->objectName()))) {
				testIter++;
				continue; //already have this added
			}

			//Create a new GTest tree item.
			testTreeItem = createNewTreeItem<TestTreeItem, GTest>(suiteTreeItem, *testIter);
			insertItem(testTreeItem, suiteTreeItem->childCount(), suiteTreeItem);
			++testIter;
		}
		++suiteIter;
	}
	emit layoutChanged();
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
	TestTreeItem* treeItem = itemTestHash.value(test);
	if(treeItem == 0)
		return; //! \todo exception management stuff here

	QVariant var = treeItem->data(Qt::UserRole);
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
}

/*! \brief A slot that removes the signal's sender from the data model.
 *
 * The sender must be a GTest or this function does nothing.
 */
//! \todo Investigate whether we can give a persistent index to a GTest.
void TestTreeModel::removeSenderItem() {
	GTest* test = static_cast<GTest*>(sender());
	if(!test)
		return; //! \todo exception management stuff here
	TestTreeItem* item = itemTestHash.value(test);
	item->parent()->removeChild(item);
	itemTestHash.remove(test);
	delete item; //! \todo use QSharedPointers for items
}

/*! \brief Inserts a single item into the model before row 'row' on the parent given.
 *
 * If the parent, or the item is null, this function does nothing and returns false.
 * Otherwise, the appropriate QAbstractItemModel signals are emitted and the child
 * is inserted into the model.
 * \return true if the item was inserted successfully, false otherwise.
 */
bool TestTreeModel::insertItem(TestTreeItem* item, int row, TestTreeItem* parent) {
	if(item == 0 || parent == 0)
		return false;
	beginInsertRows(createIndex(parent->row(), 0, parent), row, row);
	parent->insertChild(row, item);
	endInsertRows();
	return true;
}

/*! \brief Inserts a list of items into the model before row 'row' on the parent given.
 *
 * If the parent, or the item is null, this function does nothing and returns false.
 * Otherwise, the appropriate QAbstractItemModel signals are emitted and the children
 * are inserted into the model.
 * \param items A list of items to be inserted.
 * \param row The position to insert the items.
 * \param parent The parent to add the items to as child rows.
 * \return true if the items were inserted successfully, false otherwise.
 */
bool TestTreeModel::insertItems(QList<TestTreeItem*> items, int row, TestTreeItem* parent) {
	if(items.count() == 0 || parent == 0)
		return false;
	beginInsertRows(createIndex(parent->row(), 0, parent), row, row+items.count());
	parent->insertChildren(row, items);
	endInsertRows();
	return true;
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

	QStack<TestTreeItem*> stack;
	QList<TestTreeItem*> children;
	TestTreeItem* item;
	stack.push(rootItem);

	while(!stack.isEmpty()) {
		item = stack.pop();
		GTest* test = item->data(Qt::UserRole).value<GTest*>();
		if(test && (item->data(Qt::CheckStateRole).value<int>() == Qt::Checked))
			test->run();
		else {
			children = item->children();
			QList<TestTreeItem*>::iterator it = children.begin();
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



