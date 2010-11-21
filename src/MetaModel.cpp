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

#include <QItemSelectionModel>
#include <QSharedPointer>
#include <QStack>

#include "MetaModel.h"

/*! \brief Constructor
 *
 */
MetaModel::MetaModel(QObject* parent)
 : TreeModel(parent), itemModel(0)
{}

/*! \brief Destructor
 *
 */
MetaModel::~MetaModel() {

}

/*! \brief Sets the selection model to use for the meta model.
 *
 * \param model The selection model this should use as the metamodel.
 */
void MetaModel::setSelectionModel(QItemSelectionModel *model) {
	if(this->selectionModel)
		this->disconnect(this, SLOT(updateModel(QItemSelection, QItemSelection)));
	this->selectionModel = model;
	if(model == 0)
		return;
	QObject::connect(this->selectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
					 this, SLOT(updateModel(QItemSelection, QItemSelection)));
	//other signals to perhaps connect:
	//void currentChanged (const QModelIndex& current, const QModelIndex& previous)
	//void currentColumnChanged (const QModelIndex& current, const QModelIndex& previous)
	//void currentRowChanged (const QModelIndex& current, const QModelIndex& previous)
}

/*! \brief Sets the item model whose meta data this model shows.
 *
 * This function handles connecting the signal/slot relations to the
 * underlying model.
 * \param model The model to set as the base for the meta model.
 */
void MetaModel::setItemModel(TreeModel* model) {
	if(this->itemModel)
		QObject::disconnect(this->itemModel, SIGNAL(metaDataAboutToBeChanged(const QModelIndex&)),
							this, SLOT(removeItem(const QModelIndex&)));
	this->itemModel = model;
	//Set up the header data meta info.
	QList<QMap<int, QVariant> > data;
	QMap<int, QVariant> datum;
	datum.insert(Qt::DisplayRole, model->headerData(0, Qt::Horizontal, Qt::MetaDataRole));
	data.append(datum);
	this->rootItem.setData(data);

	QObject::connect(model, SIGNAL(metaDataAboutToBeChanged(const QModelIndex&)),
					 this, SLOT(removeItem(const QModelIndex&)));
}

/*! \brief This function updates the model when the current selection has changed.
 *
 * \code
 * Algorithm:
 * for each item in deselected
 *   remove item from model (removes children)
 *   for each child of item
 *      if child was in selectedIndices
 *         re-add child and its children to model
 * for each item in selected
 *   add item to model
 *   for each child index of item, child
 *      add child as child of item in model
 *      remove child from selected
 * update selectedIndices
 * \endcode
 * This function is a slot that gets hooked up to \see QSelectionModel::selectionChanged().
 * \param selected The newly selected elements.
 * \param deselected The newly deselected elements.
 */
void MetaModel::updateModel(QItemSelection selected, QItemSelection deselected) {

	QModelIndexList indices = deselected.indexes();
	QModelIndexList::iterator it = indices.begin();
	while(it != indices.end()) {
		this->removeItem(*it);
		++it;
	}

	indices = selected.indexes();
	it = indices.begin();
	while(it != indices.end()) {
		this->insertItem(*it);
		++it;
	}
	emit layoutChanged();

	this->currentlySelected.merge(selected, QItemSelectionModel::Select);
	this->currentlySelected.merge(deselected, QItemSelectionModel::Deselect);

}

/*! \brief Removes an item from the meta model.
 *
 * It is the job of this function to remove items from the model.
 * This involves removing the item and ensuring that items (such
 * as children of the removed item) that are still selected, still
 * appear in the model.
 * \param index The index of the itemModel to remove from the meta model.
 */
void MetaModel::removeItem(const QModelIndex& index) {
	//Remove the item from our list of selected indices
	if(this->currentlySelected.contains(index))
		this->currentlySelected.merge(QItemSelection (index, index), QItemSelectionModel::Deselect);

	//Remove the item from the model
	MetaItem* metaItem = itemModel->data(index, Qt::MetaDataRole).value<MetaItem*>();
	if(metaItem) {
		this->beginRemoveRows(createIndex(0, 0, &rootItem), metaItem->row(), metaItem->row());
		if(!rootItem.removeChild(metaItem)) {
			//metaItem exists, and it is not a rootItem (pointers are probably different now)
			//For now, we just assume that all indices are children of the rootItem.
			TreeItem* item = rootItem.findChild(QVariant(metaItem->data(0, Qt::DisplayRole)), Qt::DisplayRole);
			if(item) {
				rootItem.removeChild(item);
				//! \todo Use QSharedPointer's for the metaitems.
			}
		}
		this->endRemoveRows();
	}

	//we're going to recursively search the children of *it
	//to see if we should keep them selected
	QStack<QModelIndex> searchStack;
	searchStack.push(index.child(0, 0));
	while(!searchStack.isEmpty()) {
		QModelIndex currentIndex = searchStack.pop();

		//add the valid children to the stack
		QModelIndex child = currentIndex.child(0, 0);
		while(child.isValid()) {
			searchStack.push(child);
			child = child.sibling(child.row()+1, 0);
		}

		//add the sibling to the stack if valid (next in traversal)
		QModelIndex sibling = currentIndex.sibling(currentIndex.row()+1, 0);
		if(sibling.isValid())
			searchStack.push(sibling);

		//re-add to the model if selected
		if(this->currentlySelected.contains(currentIndex))
			this->insertItem(currentIndex);
	}
}

/*! \brief Inserts an element specified by index into the meta model.
 *
 * This function manages the insertion of elements. Its job is to maintain
 * a consistent layout for the displaying of the model. This means that if
 * a parent element is shown, its children must be shown as well. Best way
 * to learn how it works, is to actually run the app and play with the
 * meta model selection.
 * \param index The index of the itemModel whose metadata we want to add.
 */
void MetaModel::insertItem(const QModelIndex& index) {

	//Add the index to our selected list
	if(this->currentlySelected.contains(index))
		this->currentlySelected.merge(QItemSelection (index, index), QItemSelectionModel::Select);

	//first, check if parent is selected, if so, we do nothing.
	QModelIndex parent = index.parent();
	while(parent.isValid()) {
		if(this->currentlySelected.contains(parent))
			return;
		parent = parent.parent();
	}

	//next, insert the item and its children

	//insert the item
	MetaItem* metaItem = itemModel->data(index, Qt::MetaDataRole).value<MetaItem*>();
	if(metaItem) {
		TreeModel::insertItem(metaItem, rootItem.childCount(), &rootItem);

		//we're going to recursively iterate over the children
		//and add them (or move them if they're already there) to the model.
		QStack<QModelIndex> stack;
		MetaItem* currentMetaItem;
		MetaItem* childMetaItem;
		stack.push(index);
		while(!stack.isEmpty()) {
			QModelIndex currentIndex = stack.pop();
			currentMetaItem = itemModel->data(currentIndex, Qt::MetaDataRole).value<MetaItem*>();
			//currentMetaItem->removeAllChildren();

			//add the valid children to the stack
			QModelIndex child = currentIndex.child(0, 0);
			while(child.isValid()) {
				stack.push(child);
				childMetaItem = itemModel->data(child, Qt::MetaDataRole).value<MetaItem*>();
				TreeModel::insertItem(childMetaItem, currentMetaItem->childCount(), currentMetaItem);
				child = child.sibling(child.row()+1, 0);
			}
		}
	}
}










