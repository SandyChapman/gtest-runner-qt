/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * GTest.h - Created on 2010-07-25
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

#ifndef GTEST_H_
#define GTEST_H_

#include <QMetaType>
#include <QObject>
#include <QString>

#include "GTestResults.h"

/*! \brief This class logically represents a single unit test.
 *
 * This class attempts to mirror what an actual unit test is comprised of.
 * For example, it contains a name and a result and the varying methods
 * to access and modify these elements. However, like a real unit test,
 * it is usually contained within a test suite which then requires an
 * executable to run. Thus, this structure is reflected in objects of this
 * type are held by a GTestSuite object which are then held by a
 * GTestExecutable object.
 */
class GTest : public QObject {

Q_OBJECT

protected:
	QString name;	/**< Represents the name of the test.
					 *   In the case of the GTestExecutable class, this holds
					 *   the executable's file path.
					 */

	GTestResults* testResults; /**< Pointer to a test result object
								*   This is null until the test is actuallyu run.
								*/

signals:
	void requestingRun(QString testName, QString caseName = QString()); //!< Sends a request to the parent GTest
	void testResultsReady(); //!< Sends a notification that the test results have been received and are ready to be read.

public:
	GTest(QObject* parent = 0, QString name = QString());
	virtual ~GTest();
	QString getName() const;
	const GTestResults* getTestResults() const;

	virtual void receiveTestResults(GTestResults* testResults);
	virtual void run();

};

Q_DECLARE_METATYPE(GTest*);

/*! \brief Called to run the unit test
 *
 * Currently this function simply emits a signal which is slotted to the
 * owning GTestSuite, which then emits another signal with its name,
 * so as to have the entirety of the test case and test name needed for
 * running.
 * TODO::We're planning on allowing a user to right click on a test and select
 * 	     run. In which case, this will not simply add the test to the queue,
 *       but may need some mechanism to run immediately. This is still in planning.
 */
inline void GTest::run() { emit requestingRun(name); }

/*! \brief Retrieve the name of the test.
 *
 * \return For a GTest, this returns the test name.
 * \return For a GTestSuite, it returns the test case name.
 * \return For a GTestExecutable, it returns the file's path.
 */
inline QString GTest::getName() const { return name; }

/*! \brief Retrieve the results of the test.
 *
 * The pointer will be null if the test hasn't been run yet.
 * \return the pointer to the test results object
 */
inline const GTestResults* GTest::getTestResults() const { return testResults; }

#endif /* GTEST_H_ */
