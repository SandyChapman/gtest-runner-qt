/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Created on 2010-09-25 2010
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

#ifndef TESTRESULTSMODEL_H
#define TESTRESULTSMODEL_H

#include <QItemSelection>

#include "MetaItem.h"
#include "TreeModel.h"

class QItemSelectionModel;

class MetaModel : public TreeModel {

	Q_OBJECT;

private:
	QItemSelection currentlySelected;    //!< The currently selected model indices.
	QAbstractItemModel* itemModel;       //!< The item model whose meta data we show

public slots:
	void updateModel(QItemSelection selected, QItemSelection deselected);
	void removeItem(const QModelIndex& index);
	void insertItem(const QModelIndex& index);

public:

	MetaModel(QObject* parent = 0);
	virtual ~MetaModel();

	virtual void setSelectionModel(QItemSelectionModel* model);
	void setItemModel(TreeModel* model);
};

//! We fugde some Qt namespace stuff to make it look like
//! meta data is a Qt defined role. Not sure if this is a
//! good practice, but whatever.
namespace Qt {
	const int MetaDataRole = Qt::UserRole + 100;
}

#endif // TESTRESULTSMODEL_H
