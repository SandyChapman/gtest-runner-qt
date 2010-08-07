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

class GTestSuiteResults : public GTestResults {

protected:
	QHash<QString, GTestResults*> testResultsHash;
	uint errorCount;
	uint failureCount;
	uint runCount;

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

inline void GTestSuiteResults::addTestResults(GTestResults* testResults) {
	testResultsHash.insert(testResults->getName(), testResults);
}

inline void GTestSuiteResults::setErrorCount(uint errorCount) { this->errorCount = errorCount; }

inline void GTestSuiteResults::setFailureCount(uint failureCount) { this->failureCount = failureCount; }

inline void GTestSuiteResults::setRunCount(uint runCount) { this->runCount = runCount; }

inline GTestResults* GTestSuiteResults::getTestResults(QString testName) {
	return testResultsHash.value(testName);
}

inline uint GTestSuiteResults::getErrorCount() const { return errorCount; }

inline uint GTestSuiteResults::getFailureCount() const { return failureCount; }

inline uint GTestSuiteResults::getRunCount() const { return runCount; }

#endif /* GTESTSUITERESULTS_H_ */
