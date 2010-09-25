/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * TestTreeItem.h - Created on 2010-08-28
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

#ifndef TESTTREEITEM_H_
#define TESTTREEITEM_H_

#include <QList>
#include <QMap>
#include <QQueue>
#include <QVariant>

/*! \brief A constituent item of the TestTreeModel.
 *
 * TestTreeItems make up the body of the TestTreeModel. They are what hold the data
 * and the relations between them. Together, the items form a tree which reflects
 * the relations between an executable to a test suite to a test case.
 * \todo Should I just make this a full blown private subclass of QList?
 */
class TestTreeItem {

public:
	TestTreeItem(QMap<int, QVariant>& data, TestTreeItem* parent = 0);
	~TestTreeItem();

	void appendChild(TestTreeItem *child);
	TestTreeItem* child(int row) const;
	int childCount() const;
	QList<TestTreeItem*> children() const;
	int column() const;
	int columnCount() const;
	QVariant data(int role) const;
	QMap<int, QVariant> data() const;
	TestTreeItem* findChild(const QVariant& value, int role) const;
	bool insertChild(int row, TestTreeItem* child);
	bool insertChildren(int row, QList<TestTreeItem*> children);
	TestTreeItem* parent();
	bool removeChild(TestTreeItem* child);
	void removeChildAt(int index);
	int row() const;
	bool setData(const QVariant& value, int role);
	bool setData(const QMap<int, QVariant>& data);

private:
	QList<TestTreeItem*> childItems; //!< The children of the TestTreeItem
	QMap<int, QVariant> itemData;    //!< The data for this item.
	TestTreeItem* parentItem;        //!< The parent of this item.
};

/*! \brief Removes the child specified at the index given.
 *
 * If the index given is larger than the number of children, or less
 * than 0, this function does nothing.
 * \param index The index of the child to remove.
 */
inline void TestTreeItem::removeChildAt(int index) { childItems.removeAt(index); }

/*! \brief Removes the child given from this tree item.
 *
 * \param child The child to remove.
 * \return true if the child is removed, false otherwise.
 */
inline bool TestTreeItem::removeChild(TestTreeItem* child) { return childItems.removeOne(child); }

/*! \brief Retrieves a pointer to the child in row 'row'.
 *
 * If the row is invalid (less than 0 or greater or equal to the number
 * of children, a null pointer is returned.
 * \param row The index of the child.
 * \return null if row is invalid, a pointer to the child in row 'row otherwise.
 */
inline TestTreeItem* TestTreeItem::child(int row) const { return childItems.value(row); }

/*! \brief Retrieves a list of all the children held by this item.
 *
 * \return A list of the children held by this item.
 */
inline QList<TestTreeItem*> TestTreeItem::children() const { return childItems; }

/*! \brief Retrieves the number of children held by this object.
 *
 * \return The number of children.
 */
inline int TestTreeItem::childCount() const { return childItems.count(); }

/*! \brief Retrieves the number of columns this index has.
 *
 * Currently, this is fixed at one, but this may be changed in the future.
 * \return The number of columns (one).
 */
inline int TestTreeItem::columnCount() const { return 1; }

/*! \brief Inserts a child in this item before the item indexed at row.
 *
 * The row parameter will become the index of this child when inserted.
 * If the index is greater than the number of children, or less than 0,
 * this function does nothing and returns false.
 * \param row The row to insert this item before.
 * \param child The child to insert into this item.
 * \return true if the child was successfully inserted, false otherwise.
 */
inline bool TestTreeItem::insertChild(int row, TestTreeItem* child) {
	if(row < 0 || row > childItems.size())
		return false;
	childItems.insert(row, child);
	return true;
}

/*! \brief Inserts the list of children given at the index 'row'.
 *
 * The row parameter will become the index of the first item in
 * the list. If the row parameter is greater than the number of children
 * or less than 0, this function does nothing and returns false.
 * \param row The row to insert this item before.
 * \param children The list of children to insert into this item.
 * \return true if the children were successfully inserted, false otherwise.
 */
inline bool TestTreeItem::insertChildren(int row, QList<TestTreeItem*> children) {
	if(row < 0 || row > childItems.size())
		return false;
	TestTreeItem* child;
	foreach(child, children)
		childItems.insert(row++, child);
	return true;
}

/*! \brief Retrieves the column this item is in.
 *
 * Currently, we only support a single column, so this is always 0.
 * \return The column this item is in (zero).
 */
inline int TestTreeItem::column() const { return 0; }

#endif /* TESTTREEITEM_H_ */
