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

#include <QList>
#include <QString>

#include "Defines.h"
#include "GTestCollectionResults.h"
#include "GTestSuiteResults.h"

class GTestExecutableResults : public GTestCollectionResults {

private:
	QList<GTestSuiteResults*> suiteResultsList;

public:
	GTestExecutableResults(QString name);
	void addTestSuiteResults(GTestSuiteResults* testSuiteResults);

};

inline void GTestExecutableResults::addTestSuiteResults(GTestSuiteResults* testSuiteResults) {
	suiteResultsList.append(testSuiteResults);
}

#endif /* GTESTEXECUTABLERESULTS_H_ */
