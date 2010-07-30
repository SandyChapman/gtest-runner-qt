/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * GTestCollectionResults.h - Created on 2010-07-27
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

#ifndef GTESTCOLLECTIONRESULTS_H_
#define GTESTCOLLECTIONRESULTS_H_

#include <QString>

class GTestCollectionResults {

protected:
	QString exeName;
	uint numTests;
	uint numFailures;
	uint numErrors;
	uint totalTime;

public:
	GTestCollectionResults(QString name);
	void setTestRunCount(uint count);
	void setTestFailureCount(uint count);
	void setTestErrorCount(uint count);
	void setTestTotalTime(uint count);
	uint getTestRunCount() const;
	uint getTestFailureCount() const;
	uint getTestErrorCount() const;
	uint getTestTotalTime() const;
	QString getTestName() const;

};

inline void GTestCollectionResults::setTestRunCount(uint count) { numTests = count; }

inline void GTestCollectionResults::setTestFailureCount(uint count) { numFailures = count; }

inline void GTestCollectionResults::setTestErrorCount(uint count) { numErrors = count; }

inline void GTestCollectionResults::setTestTotalTime(uint count) { totalTime = count; }

inline uint GTestCollectionResults::getTestRunCount() const { return numTests; }

inline uint GTestCollectionResults::getTestFailureCount() const { return numFailures; }

inline uint GTestCollectionResults::getTestErrorCount() const { return numErrors; }

inline uint GTestCollectionResults::getTestTotalTime() const { return totalTime; }

inline QString GTestCollectionResults::getTestName() const { return exeName; }


#endif /* GTESTCOLLECTIONRESULTS_H_ */
