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

#include "GTestCollectionResults.h"
#include "GTestResults.h"

class GTestSuiteResults : public GTestCollectionResults {

private:
	QHash<QString, GTestResults*> testResultsHash;

public:
	GTestSuiteResults(QString name);
	void addTestResults(GTestResults* testResults);
	GTestResults* getTestResults(QString testName);

};

inline void GTestSuiteResults::addTestResults(GTestResults* testResults) {
	testResultsHash.insert(testResults->getName(), testResults);
}

inline GTestResults* GTestSuiteResults::getTestResults(QString testName) {
	return testResultsHash.value(testName);
}

#endif /* GTESTSUITERESULTS_H_ */
