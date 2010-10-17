/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * GTestResults.cpp - Created on 2010-07-27
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

#include "GTestResults.h"

/*! \brief Constructor
 *
 * \param name The name of the unit test to which this object belongs.
 */
GTestResults::GTestResults(QString name)
: name(name), time(-1), failureMessages(), status(UNDEFINED)
{}

/*! \brief Destructor
 *
 */
GTestResults::~GTestResults()
{}

/*! \brief This function creates a meta item that can be given to
 * a TreeModel to store as metadata.
 *
 * It is up to the TreeModel to decide how to handle the item. It's
 * given as a sharedpointer as the meta item can be held by two different
 * models, and responsibility is shared between them. The item could
 * exist solely in the metamodel (as an out-dated selection), or in
 * the datamodel as a piece of unread metadata, or in both.
 * \return A MetaItem that holds the information contained in this test result.
 */
MetaItem* GTestResults::createMetaItem() const {
	//Set up the data object to insert intsudoo our metaitem.
	QList<QMap<int, QVariant> > metaData; //for multiple columns
	QMap<int, QVariant> metaDatum;        //for a single column
	//insert the name into the first column as the display role.
	//For now, that's the only piece of data we give for the top level.
	metaDatum.insert(Qt::DisplayRole, QVariant(QString(this->getName())));
	metaData.append(metaDatum);
	MetaItem* item(new MetaItem(metaData));

	//We're going to insert child rows that hold the failure messages
	//so we prepare those structures as well.
	MetaItem* child;
	QList<QMap<int, QVariant> > childData;
	QMap<int, QVariant> childDatum;

	QList<QString> messages = this->getFailureMessages();
	if(!messages.isEmpty()) {
		//if there are messages, iterate over the messages and insert them as child rows.
		QList<QString>::iterator it = messages.begin();
		while(it != messages.end()) {
			childDatum.insert(Qt::DisplayRole, QVariant(*it));
			childData.append(childDatum);
			child = new MetaItem(childData, item);
			item->appendChild(child);
			childDatum.clear();
			childData.clear();
			++it;
		}
	}
//	else {
//		//There are no messages, so insert a row saying the tests have passed.
//		childDatum.insert(Qt::DisplayRole, QVariant(QString("All tests passed!")));
//		childData.append(childDatum);
//		child = new MetaItem(childData, item);
//		item->appendChild(child);
//	}
	return item;
}

