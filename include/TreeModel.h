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

#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QAbstractItemModel>

#include "TreeItem.h"

class QItemSelectionModel;

class TreeModel : public QAbstractItemModel {

	Q_OBJECT;


protected:
	TreeItem rootItem; //!< The root of the data model. It's data is shown in the header.
	QItemSelectionModel* selectionModel; //!< A model to identify selected items in this model.

public:
	TreeModel(QObject* parent = 0);
	virtual ~TreeModel();

	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	virtual Qt::ItemFlags flags(const QModelIndex& index) const;
	virtual QVariant headerData(int secton, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
	virtual bool insertItem(TreeItem* item, int row, TreeItem* parent);
	virtual bool insertItems(const QList<TreeItem* > items, int row, TreeItem* parent);
	virtual QModelIndex parent(const QModelIndex& index) const;
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
	virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
	virtual void setSelectionModel(QItemSelectionModel* model);

};

#endif // TREEMODEL_H
