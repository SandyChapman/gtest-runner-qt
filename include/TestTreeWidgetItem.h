/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * TestTreeWidgetItem.h - Created on 2010-08-07
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

#ifndef TESTTREEWIDGETITEM_H_
#define TESTTREEWIDGETITEM_H_

#include <QObject>
#include <QStringList>
#include <QTreeWidgetItem>

class TestTreeWidget;
class TestTreeWidgetItem;

#include "GTestResults.h"

/*! \brief Extends the QTreeWidgetItem to make a QObject derived version.
 *
 * \todo TODO::Examine alternative solutions to this structure (Custom treeview with hash data struct).
 */
class TestTreeWidgetItem : public QObject, public QTreeWidgetItem  {

Q_OBJECT

public:
	TestTreeWidgetItem(TestTreeWidget* parent, const QStringList& strings, int type = Type);
	TestTreeWidgetItem(TestTreeWidgetItem* parent, const QStringList& strings, int type = Type);
	virtual ~TestTreeWidgetItem();

};

#endif /* TESTTREEWIDGETITEM_H_ */
