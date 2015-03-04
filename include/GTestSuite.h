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

/*! \brief This class logically represents a suite of unit tests.
 *
 * This class attempts to mirror what an actual unit test suite is comprised of.
 * For example, it extends the concept of a unit test in that it has both a name and a result.
 * However, it differs in that a suite holds a collection of individual unit tests.
 * Thus, this structure is reflected in objects of this type are hold GTests
 * and is held by a GTestExecutable object.
 */
class GTestSuite : public GTest {

Q_OBJECT

protected:
	QList<GTest*> runList; /*! The list of GTests that have requested to be run.
							*  This list is maintained so as to increase the efficiency
							*  with which test result objects can be disseminated.
							*/

public slots:
	virtual void receiveRunRequest(QString testName, QString testCase = QString());

public:
    GTestSuite(QObject* parent, QString name = QString());
    GTestSuite(GTestSuite* parent, QString name = QString());
	virtual ~GTestSuite();
	void addTest(GTest* test);
	void removeTest(GTest* test);
	virtual void receiveTestResults(GTestResults* testSuiteResults);

	virtual void run();
};

Q_DECLARE_METATYPE(GTestSuite*);

/*! \brief Adds a test to the suite of tests.
 *
 * This method sets up the signal/slot relation between this GTestSuite
 * and the added GTest. Thus, when a GTest requests to be run through a signal,
 * this GTestSuite can received the signal, add the GTest to its run list, and
 * propagate the signal to the GTestExecutable for running.
 */
inline void GTestSuite::addTest(GTest* test) {
	QObject::connect(test, SIGNAL(requestingRun(QString, QString)),
					 this, SLOT(receiveRunRequest(QString, QString)));
}

/*! \brief Removes a test from the GTestSuite object.
 *
 * This dismantles the signal/slot relation set up in addTest.
 */
inline void GTestSuite::removeTest(GTest* test) {
	QObject::disconnect(test, SIGNAL(requestingRun(QString, QString)),
					 this, SLOT(receiveRunRequest(QString, QString)));
}


#endif /* GTESTFIXTURE_H_ */
