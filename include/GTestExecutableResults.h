/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * GTestExecutableResults.h - Created on 2010-07-26
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

#ifndef GTESTEXECUTABLERESULTS_H_
#define GTESTEXECUTABLERESULTS_H_

#include <QString>

#include "Defines.h"

class GTestExecutableResults {

private:
	QString exeName;
	uint numTests;
	uint numFailures;
	uint numErrors;
	uint totalTime;

public:
	GTestExecutableResults(QString name);
	void setTestRunCount(uint count);
	void setTestFailureCount(uint count);
	void setTestErrorCount(uint count);
	void setTestTotalTime(uint count);
	uint getTestRunCount();
	uint getTestFailureCount();
	uint getTestErrorCount();
	uint getTestTotalTime();

};

inline void GTestExecutableResults::setTestRunCount(uint count) {
	numTests = count;
}

inline void GTestExecutableResults::setTestFailureCount(uint count) {
	numFailures = count;
}

inline void GTestExecutableResults::setTestErrorCount(uint count) {
	numErrors = count;
}

inline void GTestExecutableResults::setTestTotalTime(uint count) {
	totalTime = count;
}


#endif /* GTESTEXECUTABLERESULTS_H_ */
