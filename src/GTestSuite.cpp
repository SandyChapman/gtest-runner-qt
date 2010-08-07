/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * GTestSuite.cpp - Created on 2010-07-25
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

#include "GTestSuite.h"
#include "GTestResults.h"

GTestSuite::GTestSuite(QObject* parent, QString name)
: GTest(parent, name), runList()
{}

GTestSuite::GTestSuite(const GTestSuite& other)
: GTest(other.parent(), other.name), runList()
{}

GTestSuite::~GTestSuite() {

}

void GTestSuite::receiveRunRequest(QString testName, QString testCase) {
	GTest* test = static_cast<GTest*>(QObject::sender());
	if(!runList.contains(test))
		runList.append(test);
	if(testCase.isEmpty())
		emit requestingRun(name, testName);
}

void GTestSuite::receiveTestResults(GTestResults* testSuiteResults) {
	this->testResults = testSuiteResults;
	QList<GTest*>::iterator it = runList.begin();
	GTestResults* testResults;
	while(it != runList.end()) {
		testResults = testSuiteResults->getTestResults((*it)->getName());
		(*it)->receiveTestResults(testResults);
		++it;
	}
	runList.clear();
	emit testResultsReady();
}

void GTestSuite::run() {
	//TODO::Run all tests in suite
}

