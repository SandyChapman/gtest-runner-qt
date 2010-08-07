/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * GTest.h - Created on 2010-07-25
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

#ifndef GTEST_H_
#define GTEST_H_

#include <QMetaType>
#include <QObject>
#include <QString>

#include "GTestResults.h"

class GTest : public QObject {

Q_OBJECT

protected:
	QString name;
	GTestResults* testResults;

signals:
	void requestingRun(QString testName, QString caseName = QString());
	void testResultsReady();

public:
	GTest(QObject* parent = 0, QString name = QString());
	GTest(const GTest& other);
	virtual ~GTest();
	QString getName() const;
	const GTestResults* getTestResults() const;

	virtual void receiveTestResults(GTestResults* testResults);
	virtual void run();

};

Q_DECLARE_METATYPE(GTest*);

inline void GTest::run() { emit requestingRun(name); }

inline QString GTest::getName() const { return name; }

inline const GTestResults* GTest::getTestResults() const { return testResults; }

#endif /* GTEST_H_ */
