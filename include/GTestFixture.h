/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * GTestFixture.h - Created on 2010-07-25                                                    *
 *                                                                                           *
 * Copyright (C) 2010 Sandy Chapman                                                          *
 *                                                                                           *
 * This library is free software; you can redistribute it and/or modify it under the         *
 * terms of the GNU Lesser General Public License as published by the Free Software          *
 * Foundation; either version 2.1 of the License, or (at your option) any later version.     *
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; *
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. *
 * See the GNU Lesser General Public License for more details.                               *
 * You should have received a copy of the GNU Lesser General Public License along with this  *
 * library; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, *
 * Boston, MA 02111-1307 USA                                                                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef GTESTFIXTURE_H_
#define GTESTFIXTURE_H_

#include <QList>
#include <QMetaType>
#include <QObject>

#include "GTest.h"

class GTestFixture : public QObject {

Q_OBJECT

private:
	QString name;
	QList<GTest*> testList;
	QList<GTest*> runList;

signals:
	void requestRun();

public slots:
	void receiveRunRequest();

public:
	GTestFixture(QString name);
	void addTest(GTest* test);
	void removeTest(GTest* test);

};

Q_DECLARE_METATYPE(GTestFixture*);

inline void GTestFixture::addTest(GTest* test) {
	testList << test;
	QObject::connect(test, SIGNAL(requestRun()),
					 this, SLOT(receiveRunRequest()));
}

inline void GTestFixture::removeTest(GTest* test) {
	testList.removeOne(test);
	QObject::disconnect(test, SIGNAL(requestRun()),
						this, SLOT(receiveRunRequest()));
}

#endif /* GTESTFIXTURE_H_ */
