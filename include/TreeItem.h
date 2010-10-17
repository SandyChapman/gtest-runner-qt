/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * TreeItem.h - Created on 2010-08-28
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

#ifndef TREEITEM_H_
#define TREEITEM_H_

#include <QList>
#include <QMap>
#include <QQueue>
#include <QSharedPointer>
#include <QVariant>

/*! \brief A constituent item of the TreeModel.
 *
 * TreeItems make up the body of the TreeModel. They are what hold the data
 * and the relations between them. Together, the items form a tree which reflects
 * the relations between an executable to a  suite to a  case.
 * \todo Should I just make this a full blown private subclass of QList?
 */
class TreeItem {

public:
	TreeItem();
	TreeItem(QList<QMap<int, QVariant> >& data, TreeItem* parent = 0);
	virtual ~TreeItem();

	void appendChild(TreeItem* child);
	TreeItem* child(int row) const;
	int childCount() const;
	QList<TreeItem* > children() const;
	int column() const;
	int columnCount() const;
	QVariant data(int column, int role) const;
	QList<QMap<int, QVariant> > data() const;
	TreeItem* findChild(const QVariant& value, int role) const;
	bool insertChild(int row, TreeItem* child);
	bool insertChildren(int row, QList<TreeItem* > children);
	TreeItem* parent();
	void removeAllChildren();
	bool removeChild(TreeItem* child);
	void removeChildAt(int index);
	int row() const;
	bool setData(const QVariant& value, int column, int role);
	bool setData(const QList<QMap<int, QVariant> >& data);


private:
	QList<TreeItem* > childItems; //!< The children of the TreeItem
	//!< The data for this item, first indexed by column, then by role.
	QList<QMap<int, QVariant> > itemData;
	TreeItem* parentItem;        //!< The parent of this item.

	void setParent(TreeItem* parent);
};

Q_DECLARE_METATYPE(TreeItem*);

/*! \brief Removes all children from this tree item.
 *
 * This removes all the children from the tree item. The caller should
 * be careful as this does not delete the children. Thus, if the only
 * reference to these children are held in this item, then they should
 * be sure to call delete on the children.
 */
inline void TreeItem::removeAllChildren() { childItems.clear(); }

/*! \brief Removes the child specified at the index given.
 *
 * If the index given is larger than the number of children, or less
 * than 0, this function does nothing.
 * \param index The index of the child to remove.
 */
inline void TreeItem::removeChildAt(int index) { childItems.removeAt(index); }

/*! \brief Removes the child given from this tree item.
 *
 * \param child The child to remove.
 * \return true if the child is removed, false otherwise.
 */
inline bool TreeItem::removeChild(TreeItem* child) { return childItems.removeOne(child); }

/*! \brief Retrieves a pointer to the child in row 'row'.
 *
 * If the row is invalid (less than 0 or greater or equal to the number
 * of children, a null pointer is returned.
 * \param row The index of the child.
 * \return null if row is invalid, a pointer to the child in row 'row otherwise.
 */
inline TreeItem* TreeItem::child(int row) const { return childItems.value(row); }

/*! \brief Retrieves a list of all the children held by this item.
 *
 * \return A list of the children held by this item.
 */
inline QList<TreeItem* > TreeItem::children() const { return childItems; }

/*! \brief Retrieves the number of children held by this object.
 *
 * \return The number of children.
 */
inline int TreeItem::childCount() const { return childItems.count(); }

/*! \brief Retrieves the number of columns this index has.
 *
 * Currently, this is fixed at one, but this may be changed in the future.
 * \return The number of columns (one).
 */
inline int TreeItem::columnCount() const { return 1; }

/*! \brief Inserts a child in this item before the item indexed at row.
 *
 * The row parameter will become the index of this child when inserted.
 * If the index is greater than the number of children, or less than 0,
 * this function does nothing and returns false.
 * \param row The row to insert this item before.
 * \param child The child to insert into this item.
 * \return true if the child was successfully inserted, false otherwise.
 */
inline bool TreeItem::insertChild(int row, TreeItem* child) {
	if(row < 0 || row > childItems.size() || !child)
		return false;
	if(child->parent())
		child->parent()->removeChild(child);
	child->setParent(this);
	if(!childItems.contains(child))
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
inline bool TreeItem::insertChildren(int row, QList<TreeItem* > children) {
	if(row < 0 || row > childItems.size())
		return false;
	TreeItem* child;
	foreach(child, children)
		childItems.insert(row++, child);
	return true;
}

/*! \brief Retrieves the column this item is in.
 *
 * Currently, we only support a single column, so this is always 0.
 * \return The column this item is in (zero).
 */
inline int TreeItem::column() const { return 0; }

/*! \brief Sets the parent of the current tree item.
 *
 * This function should only be used by the call insertChild
 * so as to maintain consistency in parent-child relations.
 * \param parent The new parent item.
 */
inline void TreeItem::setParent(TreeItem *parent) { this->parentItem = parent; }

#endif /* TREEITEM_H_ */
