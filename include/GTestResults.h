/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * GTestResults.h - Created on 2010-07-27
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

#ifndef GTESTRESULTS_H_
#define GTESTRESULTS_H_

#include <QSharedPointer>
#include <QStringList>

/* \brief This class logically represents the results of running a unit test.
 *
 * The class is the analogue to a gtest unit test result. It consists of the name of
 * the test (which is used to assign the result to the correct GTest), the running
 * time of the test, any failure messages that were produced and a summary result
 * status (pass/fail). In the subclasses, these variables also apply, but differ
 * in that a GTestSuiteResults comprises the results of all tests within the suite.
 */
class GTestResults {

public:
	//! The run status of the GTest.
	enum STATUS {
		RUN,		//!< The test was run.
		//! \todo Investigate other run status of a GTest.
		UNDEFINED	//!< Currently not sure what the other possibilities are.
	};

	//! The outcome of the running of the test.
	enum OUTCOME {
		PASSED,		//The test passed.
		FAILED		//The test has at least one failure.
	};

protected:
	QString name;	//!< The name of the GTest that corresponds to this result.
	uint time;		//!< The time it took to run this test (or suite of tests).
	QStringList failureMessages;	//!< The list of failure messages --unparsed.
	STATUS status;	//!< The run status of the unit test \see GTestResults::STATUS

public:
	GTestResults(QString name);
	virtual ~GTestResults();

	//TODO::Check whether these should be private and have GTestExecutable as a friend.
	void setStatus(QString status);
	void setRunningTime(uint runTime);
	void addFailureMessage(QString failureMsg);

	STATUS getStatus() const;
	QString getName() const;
	uint getRunningTime() const;
	virtual uint getFailureCount() const;
	QString getFailureMessage(uint index) const;
	QStringList getFailureMessages() const;

	virtual GTestResults* getTestResults(QString name);

};

/*! \brief Sets the run status of the gtest.
 *
 * This function takes a QString from the 'run' attribute in the
 * .xml file. It parses this and sets it to a enum value for later
 * retrieval.
 * \param statusString The status as retrieved from the gtest .xml file.
 */
inline void GTestResults::setStatus(QString statusString) {
	if(statusString == "run")
		status = RUN;
}

/*! \brief Sets the elapsed running time of the gtest.
 *
 * This function takes  the 'time' attribute in the .xml file.
 * It parses this and sets it to a enum value for later retrieval.
 * \param runTime The running time as retrieved from the gtest .xml file.
 */
inline void GTestResults::setRunningTime(uint runTime) {
	time = runTime;
}

/*! \brief Adds a failure message to the list of failure messages.
 *
 * This appends a failure message to the end of the list of failure messages.
 * \param failureMsg The failure message as retrieved from the gtest executable.
 */
inline void GTestResults::addFailureMessage(QString failureMsg) {
	failureMessages.append(failureMsg);
}

/*! \brief Retrieves the run status of the unit test.
 *
 * This function gives the run status for the unit test result that
 * this object represents.
 * \return The run status of the unit test.
 */
inline GTestResults::STATUS GTestResults::getStatus() const { return status; }

/*! \brief Retrieves the name of the unit test this result belongs to.
 *
 * \return The name of the unit test this result belongs to.
 */
inline QString GTestResults::getName() const {
	return name;
}

/*! \brief Retrieves the time elapsed from this unit test.
 *
 * For single tests, this is the time it took to run the unit test.
 * In subclasses, this is used to get the total running time of an
 * aggregate of tests.
 * \return The total elapses time for this test.
 */
inline uint GTestResults::getRunningTime() const {
	return time;
}

/*! \brief Retrieves the total number of failure messages for this test.
 *
 * \note There can be multiple failures per test.
 * \return The number of failures.
 */
inline uint GTestResults::getFailureCount() const {
	return failureMessages.size();
}

/*! \brief Retrieves the failure message indexed by 'index'.
 *
 * This retrieves the index'th failure string from the list of
 * failure strings.
 * \note If you wish to iterate through all failure messages, it
 * 		 is recommended that you retrieve a copy of the entire list
 * 		 of messages \see GTestResults::getFailureMessage().
 * \return The failure message at index 'index.
 */
inline QString GTestResults::getFailureMessage(uint index) const {
	return failureMessages.at(index);
}

/*! \brief Retrieves the entire list of failure messages.
 *
 * If you wish to iterate through all messages, this function is the
 * prefered method to call.
 * \return A copy of the entire list of failure messages.
 */
inline QStringList GTestResults::getFailureMessages() const {
	return failureMessages;
}

/*! \brief Retrieves the test result object with name 'name'.
 *
 * This function is primarily for subclasses which may hold references
 * to other test results.
 * \return A pointer to a GTestResult.
 */
inline GTestResults* GTestResults::getTestResults(QString name) { return (name == this->name ? this : 0); }

#endif /* GTESTRESULTS_H_ */
