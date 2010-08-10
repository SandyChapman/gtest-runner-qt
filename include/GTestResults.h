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
		//! \TODO TODO::Investigate other run status of a GTest.
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

	void setStatus(QString status);
	void setRunningTime(uint runTime);
	void addFailureMessage(QString failureMsg);

	STATUS getStatus() const;
	QString getName() const;
	uint getRunningTime() const;
	virtual uint getFailureCount() const;
	QString getFailureMessage(uint num) const;
	QStringList getFailureMessages() const;

	virtual GTestResults* getTestResults(QString name);

};

inline void GTestResults::setStatus(QString statusString) {
	if(statusString == "run")
		status = RUN;
}

inline void GTestResults::setRunningTime(uint runTime) {
	time = runTime;
}

inline void GTestResults::addFailureMessage(QString failureMsg) {
	failureMessages.append(failureMsg);
}

inline GTestResults::STATUS GTestResults::getStatus() const { return status; }

inline QString GTestResults::getName() const {
	return name;
}

inline uint GTestResults::getRunningTime() const {
	return time;
}

inline uint GTestResults::getFailureCount() const {
	return failureMessages.size();
}

inline QString GTestResults::getFailureMessage(uint num) const {
	return failureMessages.at(num);
}

inline QStringList GTestResults::getFailureMessages() const {
	return failureMessages;
}

inline GTestResults* GTestResults::getTestResults(QString name) { return (name == this->name ? this : 0); }

#endif /* GTESTRESULTS_H_ */
