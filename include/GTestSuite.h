/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * GTestSuite.h - Created on 2010-07-25                                                    *
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
#include "GTestSuiteResults.h"

class GTestSuite : public GTest {

Q_OBJECT

protected:
	QList<GTest*> runList;

public slots:
	virtual void receiveRunRequest(QString testName, QString testCase = QString());

public:
	GTestSuite(QObject* parent = 0, QString name = QString());
	GTestSuite(const GTestSuite& other);
	virtual ~GTestSuite();
	void addTest(GTest* test);
	void removeTest(GTest* test);
	virtual void receiveTestResults(GTestResults* testSuiteResults);

	virtual void run();
};

Q_DECLARE_METATYPE(GTestSuite*);

inline void GTestSuite::addTest(GTest* test) {
	QObject::connect(test, SIGNAL(requestingRun(QString)),
					 this, SLOT(receiveRunRequest(QString)));
}

inline void GTestSuite::removeTest(GTest* test) {
	QObject::disconnect(test, SIGNAL(requestingRun(QString)),
					 this, SLOT(receiveRunRequest(QString)));
}


#endif /* GTESTFIXTURE_H_ */
