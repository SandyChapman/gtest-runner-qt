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
	uint errorCount;   //!< The number of errors produced by the suite's tests.
	uint failureCount; //!< The number of failures in the suite's tests.
	uint runCount;     //!< The number of tests that were run.

public:
	GTestSuiteResults(QString name);
	virtual ~GTestSuiteResults();

	void addTestResults(GTestResults* testResults);
	void setErrorCount(uint errorCount);
	void setFailureCount(uint failureCount);
	void setRunCount(uint runCount);

	GTestResults* getTestResults(QString testName);
	uint getErrorCount() const;
	virtual uint getFailureCount() const;
	uint getRunCount() const;
};

/*! \brief Adds the test result as a child of this test result.
 *
 * This inserts the test result into the hash for fast retrieval.
 * \param testResults The results of a single unit test.
 */
inline void GTestSuiteResults::addTestResults(GTestResults* testResults) {
	testResultsHash.insert(testResults->getName(), testResults);
}

/*! \brief Sets the number of errors this suite had when run.
 *
 * \param errorCount The number of test errors.
 */
inline void GTestSuiteResults::setErrorCount(uint errorCount) { this->errorCount = errorCount; }

/*! \brief Sets the number of failures this suite had when run.
 *
 * \param failureCount The number of test failures.
 */
inline void GTestSuiteResults::setFailureCount(uint failureCount) { this->failureCount = failureCount; }

/*! \brief Sets the number of unit tests that were run from this suite.
 *
 * \param runCount The number of tests run.
 */
inline void GTestSuiteResults::setRunCount(uint runCount) { this->runCount = runCount; }

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

/*! \brief Retrieves the number of errors that occurred in running this suite.
 *
 * \return The number of errors.
 */
inline uint GTestSuiteResults::getErrorCount() const { return errorCount; }

/*! \brief Retrieves the number of failures that occurred in running this suite.
 *
 * \return The number of failed tests.
 */
inline uint GTestSuiteResults::getFailureCount() const { return failureCount; }

/*! \brief Retrieves the number of tests run by running this suite.
 *
 * \return The number of tests run.
 */
inline uint GTestSuiteResults::getRunCount() const { return runCount; }

#endif /* GTESTSUITERESULTS_H_ */
