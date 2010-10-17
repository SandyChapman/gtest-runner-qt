/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Created on 2010-09-26 2010
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

#include "TreeModel.h"

/*! \brief Constructor
 *
 */
TreeModel::TreeModel(QObject* parent)
: QAbstractItemModel(parent)
{}

/*! \brief Destructor
 *
 */
TreeModel::~TreeModel() {}


/*! \brief Gives a count of the number of columns contained by the index 'parent'.
 *
 * If the index is invalid the number of columns contained by the rootItem.are
 * returned.
 * \return The number of columns given by the index 'parent' or the number of
 * columns in the rootItem.is 'parent' is invalid.
 */
int TreeModel::columnCount(const QModelIndex& parent) const {
	if (parent.isValid())
		return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
	else
		return rootItem.columnCount();
}

/*! \brief Accesses data contained within the model.
 *
 * The data contained in the index 'index' for the given 'role' are returned
 * within a QVariant. If the index is invalid, or if the item specified by
 * the index does not contain data for the role 'role', an empty QVariant
 * is returned.
 * \return The data specified by the index 'index' and the role 'role'.
 */
QVariant TreeModel::data(const QModelIndex& index, int role) const {
	if (!index.isValid())
		return QVariant();

	switch(role) {
	//list supported data accesses here.
	case Qt::BackgroundColorRole:
	case Qt::CheckStateRole:
	case Qt::DisplayRole: {
		TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
		if(item)
			return item->data(0, role);
		return QVariant();
	}
	default:
		return QVariant();
	}
}

/*! \brief Returns the flags for the item specified by the given index.
 *
 * If the index is invalid, this function returns 0. Otherwise, items within
 * the TreeModel are enabled, selectable, checkable, and if they have children,
 * are tristate checkable.
 * \see TreeModel::setCheckState() for more information on check states.
 */
Qt::ItemFlags TreeModel::flags(const QModelIndex& index) const {
	if (!index.isValid())
		 return 0;

	Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;

	if(static_cast<TreeItem*>(index.internalPointer())->childCount() > 0)
		flags |= Qt::ItemIsTristate;

	return flags;
}

/*! \brief Retrieves the header information for the model.
 *
 * Section and orientation are ignored for this model. We only provide data in
 * one dimension so there are unnecessary. The data for the given role is provided.
 * \return The data for the given role.
 */
QVariant TreeModel::headerData(int /*section*/, Qt::Orientation /*orientation*/, int role) const {
	return rootItem.data(0, role);
}

/*! \brief Retrieves a model index at the specified (row, columns) from the given parent.
 *
 * Retrieves a model index for the model. If parent is not specified, or is a default
 * constructed index, an index to a top level item is returned.
 * \return A model index for the given row and column with the parent 'parent'.
 */
QModelIndex TreeModel::index(int row, int column, const QModelIndex& parent) const {
	if(!hasIndex(row, column, parent))
		return QModelIndex();

	const TreeItem* parentItem;
	TreeItem* childItem;

	if(parent.isValid())
		parentItem = static_cast<TreeItem* >(parent.internalPointer());
	else
		parentItem = &rootItem;

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
QModelIndex TreeModel::parent(const QModelIndex &index) const {
	if (!index.isValid())
		return QModelIndex();

	TreeItem* childItem = static_cast<TreeItem* >(index.internalPointer());
	if(!childItem || childItem == &rootItem)
		return QModelIndex();

	TreeItem* parentItem = childItem->parent();

	if (!parentItem || parentItem == &rootItem)
		return QModelIndex();

	return createIndex(parentItem->row(), 0, parentItem);
}

/*! \brief Retrieves the number of rows provided by the parent 'parent'.
 *
 * \return If the index is valid, the number of children held by 'parent' is returned.
 * Else, the number of top level items is returned.
 */
int TreeModel::rowCount(const QModelIndex& parent) const {
	const TreeItem* parentItem;
	if (parent.column() > 0)
		return 0;

	if (!parent.isValid())
		parentItem = &rootItem;
	else
		parentItem = static_cast<TreeItem* >(parent.internalPointer());

	return parentItem->childCount();
}

/*! \brief Sets data on the model.
 *
 * It is trusted that the user of this function respects the information provided by
 * \see TreeModel::flags(). This function will set the data contained in var
 * on the item specified by index for the given role. If data in the model is changed,
 * the TreeModel::dataChanged() signal is emitted.
 */
bool TreeModel::setData(const QModelIndex& index, const QVariant& var, int role) {
	if(!index.isValid())
		return false;
	TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
	if(!item)
		return false;

	if(!item->setData(var, 0, role))
		return false;
	emit dataChanged(index, index);
	return true;
}


/*! \brief Inserts a single item into the model before row 'row' on the parent given.
 *
 * If the parent, or the item is null, this function does nothing and returns false.
 * Otherwise, the appropriate QAbstractItemModel signals are emitted and the child
 * is inserted into the model.
 * \return true if the item was inserted successfully, false otherwise.
 */
bool TreeModel::insertItem(TreeItem* item, int row, TreeItem* parent) {
	if(!item || !parent)
		return false;
	beginInsertRows(createIndex(parent->row(), 0, parent), row, row+1);
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
bool TreeModel::insertItems(QList<TreeItem* > items, int row, TreeItem* parent) {
	if(items.count() == 0 || parent == 0)
		return false;
	beginInsertRows(createIndex(parent->row(), 0, parent), row, row+items.count());
	parent->insertChildren(row, items);
	endInsertRows();
	return true;
}
