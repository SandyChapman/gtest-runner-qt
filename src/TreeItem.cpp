/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * TreeItem.cpp - Created on 2010-08-29
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

#include "TreeItem.h"

/*! \brief Constructor
 *
 */
TreeItem::TreeItem()
 : childItems(), itemData(), parentItem(0)
{}

/*! \brief Constructor
 *
 * \param data The list of data to assign to this item. The QMap should map
 *             between a Qt::ItemDataRole and the QVariant with the data.
 * \param parent The parent of this item.
 */
TreeItem::TreeItem(QList<QMap<int, QVariant> >& data, TreeItem* parent)
: childItems(), itemData(data), parentItem(parent)
{}

/*! \brief Destructor
 *
 */
TreeItem::~TreeItem()
{}

/*! \brief Appends the child to the end of this item.
 *
 * \param child A pointer to the child to append.
 */
void TreeItem::appendChild(TreeItem* child) {
	childItems.append(child);
}

/*! \brief Retrieves the data item specified by the role (a Qt::ItemDataRole).
 *
 * \param role A value from Qt::ItemDataRole specifying a model role.
 * \return A variant holding the data.
 */
QVariant TreeItem::data(int column, int role) const {
	if(column >= itemData.length())
		return QVariant();
	return itemData.at(column).value(role);
}

/*! \brief Retrieves a pointer to the parent of this item.
 *
 * \return A pointer to the parent of this item.
 */
TreeItem* TreeItem::parent() {
	return parentItem;
}

/*! \brief Retrieves the row that this item is in.
 *
 * The row is relative to the parent. The row this item is in,
 * is the index of the item in the children of its parent.
 * \return The row this item is in, or if it has no parent (the rootItem. 0.
 */
int TreeItem::row() const {
	if(parentItem) {
		TreeItem* item(const_cast<TreeItem*>(this));
		int row = parentItem->childItems.indexOf(item);
		return row;
	}
	return 0;
}

/*! \brief Sets the data for the given role to the variant 'value'.
 *
 * \param value The value to set for the given role.
 * \param role The role specified from Qt::ItemDataRole.
 * \return true if the data was set, false otherwise.
 */
bool TreeItem::setData(const QVariant& value, int col, int role) {
	while(col >= itemData.length())
		itemData.append(QMap<int, QVariant>());
	itemData[col].insert(role, value);
	return true;
}

/*! \brief Sets the data for the given role to the variant 'value'.
 *
 * \param value The value to set for the given role.
 * \param role The role specified from Qt::ItemDataRole.
 * \return true if the data was set, false otherwise.
 */
bool TreeItem::setData(const QList<QMap<int, QVariant> >& data) {
	itemData = data;
	return true;
}

/*! \brief Finds the child who has the value 'value' contained in the role 'role'.
 *
 * If no child item is found who has the value given for the role given, then null
 * is returned.
 * \param value The value to match.
 * \param role The role to match (one of Qt::ItemDataRole).
 * \return The child the holds the value for the given role, or null if not found.
 */
TreeItem* TreeItem::findChild(const QVariant& value, int role) const {
	QList<TreeItem* >::const_iterator it = childItems.begin();
	while(it != childItems.end()) {
		if((*it)->data(0, role) == value)
			return *it;
		++it;
	}
	return 0;
}



