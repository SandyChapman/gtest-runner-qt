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

#include <QXmlStreamAttributes>
#include <QXmlStreamReader>


#include "GTestParser.h"

GTestParser::GTestParser(QIODevice* inStream)
: xmlSource(inStream)
{}

void GTestParser::parse() {
	QXmlStreamReader xmlStream(xmlSource);
	QXmlStreamAttributes attributes;
	GTestExecutableResults *testResults;
	while(!xmlStream.atEnd()) {
		while(!xmlStream.readNextStartElement());
		attributes = xmlStream.attributes();
		if(xmlStream.name() == "testcase")
			;
		else if(xmlStream.name() == "testsuite") {

		}
		else if(xmlStream.name() == "testsuites") {
			testResults = new GTestExecutableResults(attributes.value("name").toString());
			testResults->setTestRunCount(attributes.value("tests").toString().toUInt());
			testResults->setTestFailureCount(attributes.value("failures").toString().toUInt());
			testResults->setTestErrorCount(attributes.value("errors").toString().toUInt());
			testResults->setTestTotalTime(attributes.value("time").toString().toUInt());
		}
	}
}
