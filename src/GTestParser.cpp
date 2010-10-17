/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * GTestParser.cpp - Created on 2010-07-19                                                    *
 *                                                                                           *
 * Copyright (C) 2010 Sandy Chapman                                                          *
 *                                                                                           *
 * This library is free software; you can redistribute it and/or modify it under the         *
 * terms of the GNU Lesser General Public License as published by the Free Software          *
 * Foundation; either version 2.1 of the License, or (at your option) any later version.     *
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; *
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. *
 * See the GNU Lesser General Public License for more details.                               *
 * You should have received a copy of the GNU Lesser General Public License along with this  *
 * library; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, *
 * Boston, MA 02111-1307 USA                                                                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <QDebug>
#include <QXmlStreamAttributes>
#include <QXmlStreamReader>


#include "GTestParser.h"

/*! \brief Constructor
 * \param inStream The data stream the represents the gtest .xml file source.
 */
GTestParser::GTestParser(QIODevice* inStream)
: xmlSource(inStream)
{}

/*! \brief Parses the .xml file given to the constructor.
 *
 * This function takes the .xml source file and parses it. It creates a single
 * GTestExecutableResults object which acts as the root node of a tree of
 * GTestResults object.
 * \dot
	  digraph "Test Results Structure" {
		node [shape=record, fontname=Helvetica, fontsize=10];
		exe [label="GTestExecutableResults" URL="\ref GTestExecutableResults"];
		suite1 [label="GTestSuiteResults" URL="\ref GTestSuiteResults"];
		suite2 [label="GTestSuiteResults" URL="\ref GTestSuiteResults"];
		suite3 [label="GTestSuiteResults" URL="\ref GTestSuiteResults"];
		unit1 [label="GTestResults" URL="\ref GTestResults"];
		unit2 [label="GTestResults" URL="\ref GTestResults"];
		unit3 [label="GTestResults" URL="\ref GTestResults"];
		unit4 [label="GTestResults" URL="\ref GTestResults"];
		unit5 [label="GTestResults" URL="\ref GTestResults"];
		exe -> suite1 [arrowhead="open", style="solid"];
		exe -> suite2 [arrowhead="open", style="solid"];
		exe -> suite3 [arrowhead="open", style="solid"];
		suite1 -> unit1 [arrowhead="open", style="solid"];
		suite2 -> unit2 [arrowhead="open", style="solid"];
		suite3 -> unit3 [arrowhead="open", style="solid"];
		suite3 -> unit4 [arrowhead="open", style="solid"];
		suite3 -> unit5 [arrowhead="open", style="solid"];
	  }
   \enddot
 * \return The tree of unit test results.
 */
GTestExecutableResults* GTestParser::parse() {
	xmlSource->open(QIODevice::ReadOnly);
	xmlSource->seek(0);
	QXmlStreamReader xmlStream(xmlSource);
	QXmlStreamAttributes attributes;
	GTestExecutableResults* testExeResults;
	GTestSuiteResults* testSuiteResults;
	GTestResults* testResults;
	//! \bug \todo Fix the bug in here causing "premature end-of-file" error.
	while(!xmlStream.atEnd()) {
		while(!xmlStream.readNextStartElement() && !xmlStream.hasError() && !xmlStream.atEnd()) { qDebug() << xmlStream.name(); }
		attributes = xmlStream.attributes();
		if(xmlStream.name() == "testcase") {
			testResults = new GTestResults(attributes.value("name").toString());
			testResults->setRunningTime(attributes.value("time").toString().toUInt());
			testResults->setStatus(attributes.value("status").toString());
			xmlStream.readNext();
			qDebug() << xmlStream.name();
			while(xmlStream.name() != "testcase") { //no closing </testcase> yet
				if(xmlStream.isCDATA())
					testResults->addFailureMessage(xmlStream.text().toString());
				xmlStream.readNext();
				qDebug() << xmlStream.name();
			}
			testSuiteResults->addTestResults(testResults);
		}
		else if(xmlStream.name() == "testsuite") {
			testSuiteResults = new GTestSuiteResults(attributes.value("name").toString());
			testSuiteResults->setRunCount(attributes.value("tests").toString().toUInt());
			testSuiteResults->setFailureCount(attributes.value("failures").toString().toUInt());
			testSuiteResults->setErrorCount(attributes.value("errors").toString().toUInt());
			testSuiteResults->setRunningTime(attributes.value("time").toString().toUInt());
			testExeResults->addTestResults(testSuiteResults);
		}
		else if(xmlStream.name() == "testsuites") {
			testExeResults = new GTestExecutableResults(attributes.value("name").toString());
			testExeResults->setRunCount(attributes.value("tests").toString().toUInt());
			testExeResults->setFailureCount(attributes.value("failures").toString().toUInt());
			testExeResults->setErrorCount(attributes.value("errors").toString().toUInt());
			testExeResults->setRunningTime(attributes.value("time").toString().toUInt());
		}
		if(xmlStream.hasError()) {
			qDebug() << xmlStream.errorString();
		}
	}
	return testExeResults;
}
