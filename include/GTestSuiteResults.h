/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * GTestSuiteResults.h - Created on 2010-07-27
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

#ifndef GTESTSUITERESULTS_H_
#define GTESTSUITERESULTS_H_

#include <QHash>
#include <QString>

#include "GTestResults.h"

/*! \brief This class logically represents the results of running a suite of unit tests.
 *
 * The class is the analogue to a gtest suite's test results. It consists of the name of
 * the test suite (which is used to assign the result to the correct GTestSuite), the running
 * time, the error count, the failure count and run count of the suite's constituents.
 */
class GTestSuiteResults : public GTestResults {

protected:
	QHash<QString, GTestResults*> testResultsHash; //!< A hash to map between a test name and its results.

public:
    GTestSuiteResults();
	virtual ~GTestSuiteResults();

    void addTestResults(GTestResults* testResults);

    GTestResults* getTestResults(QString testName);
};

/*! \brief Adds the test result as a child of this test result.
 *
 * This inserts the test result into the hash for fast retrieval.
 * \param testResults The results of a single unit test.
 */
inline void GTestSuiteResults::addTestResults(GTestResults* testResults) {
    testResultsHash.insert(testResults->get("name"), testResults);
}

/*! \brief Retrieves the test results given by the test named 'testName'.
 *
 * If this test suite results object doesn't have a result for the given test,
 * a null pointer is returned.
 * \param testName The name of the test to retrieve its results.
 * \return The test result object for the test 'testName'.
 */
inline GTestResults* GTestSuiteResults::getTestResults(QString testName) {
	return testResultsHash.value(testName);
}

#endif /* GTESTSUITERESULTS_H_ */
