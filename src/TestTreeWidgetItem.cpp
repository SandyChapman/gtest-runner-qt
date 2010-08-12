/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * TestTreeWidgetItem.cpp - Created on 2010-08-07
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

#include "TestTreeWidgetItem.h"

/*! \brief Constructor
 *
 * <a href="http://doc.qt.nokia.com/4.6/qtreewidgetitem.html#type">See QTreeWidgetItem</a>
 * \param parent, The tree widget to which the object belongs.
 * \param strings Initial strings to be inserted as columns of this element.
 * \param type A type used to identify what kind of data is attached to this item.
 */
TestTreeWidgetItem::TestTreeWidgetItem(QTreeWidget* parent, const QStringList& strings, int type)
: QObject(), QTreeWidgetItem(parent, strings, type)
{}

/*! \brief Constructor
 *
 * <a href="http://doc.qt.nokia.com/4.6/qtreewidgetitem.html#type">See QTreeWidgetItem</a>
 * \param parent, The tree widget item to which the object belongs.
 * \param strings Initial strings to be inserted as columns of this element.
 * \param type A type used to identify what kind of data is attached to this item.
 */
TestTreeWidgetItem::TestTreeWidgetItem(QTreeWidgetItem* parent, const QStringList& strings, int type)
: QObject(), QTreeWidgetItem(parent, strings, type)
{}

/*! \brief Destructor
 *
 */
TestTreeWidgetItem::~TestTreeWidgetItem()
{}
