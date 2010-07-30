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

GTestParser::GTestParser(QIODevice* inStream)
: xmlSource(inStream)
{}

GTestExecutableResults* GTestParser::parse() {
	xmlSource->open(QIODevice::ReadOnly);
	xmlSource->seek(0);
	QXmlStreamReader xmlStream(xmlSource);
	QXmlStreamAttributes attributes;
	GTestExecutableResults* testExeResults;
	GTestSuiteResults* testSuiteResults;
	GTestResults* testResults;
	while(!xmlStream.atEnd()) {
		qDebug() << xmlStream.text();
		while(!xmlStream.readNextStartElement() && !xmlStream.hasError() && !xmlStream.atEnd()) {
			qDebug() << xmlStream.text();
		}
		qDebug() << xmlStream.text();
		attributes = xmlStream.attributes();
		if(xmlStream.name() == "testcase") {
			testResults = new GTestResults(attributes.value("name").toString());
			testResults->setRunningTime(attributes.value("time").toString().toUInt());
			testResults->setStatus(attributes.value("status").toString());
			while((xmlStream.readNext(), xmlStream.name()) == "failure") {
				attributes = xmlStream.attributes();
				testResults->addFailureMessage(attributes.value("message").toString());
			}
			testSuiteResults->addTestResults(testResults);
		}
		else if(xmlStream.name() == "testsuite") {
			testSuiteResults = new GTestSuiteResults(attributes.value("name").toString());
			testSuiteResults->setTestRunCount(attributes.value("tests").toString().toUInt());
			testSuiteResults->setTestFailureCount(attributes.value("failures").toString().toUInt());
			testSuiteResults->setTestErrorCount(attributes.value("errors").toString().toUInt());
			testSuiteResults->setTestTotalTime(attributes.value("time").toString().toUInt());
			testExeResults->addTestSuiteResults(testSuiteResults);
		}
		else if(xmlStream.name() == "testsuites") {
			testExeResults = new GTestExecutableResults(attributes.value("name").toString());
			testExeResults->setTestRunCount(attributes.value("tests").toString().toUInt());
			testExeResults->setTestFailureCount(attributes.value("failures").toString().toUInt());
			testExeResults->setTestErrorCount(attributes.value("errors").toString().toUInt());
			testExeResults->setTestTotalTime(attributes.value("time").toString().toUInt());
		}
		if(xmlStream.hasError()) {
			qDebug() << xmlStream.errorString();
		}
	}
	return testExeResults;
}
