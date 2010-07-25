/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * GTest.cpp - Created on 2010-07-23                                                         *
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

#include "GTest.h"

#include <QDebug>
#include <QFile>

GTest::GTest(QString filePath)
: filePath(filePath), state(VALID), processLock(),
  gtest(0), outputLock(), standardOutput(), standardError()
{
	getState();
}

GTest::~GTest() {

}

void GTest::produceListing() {
	//We lock so that any attempt to try to produce a listing
	//or run a test will block until we're done with what we're
	//doing here.
	processLock.lock();
	setupExecutable();
	QObject::connect(gtest, SIGNAL(finished(int, QProcess::ExitStatus)),
					 this, SLOT(parseListing(int, QProcess::ExitStatus)));
	gtest->start(filePath, QStringList() << "--gtest_list_tests");
	//unlock the processLock in the parseListing slot
}

void GTest::parseListing(int exitCode, QProcess::ExitStatus exitStatus) {

	processLock.unlock();
	emit listingReady(this);
}

void GTest::runTest() {
	//We lock so that any attempt to try to produce a listing
	//or run a test will block until we're done with what we're
	//doing here.
	processLock.lock();
	setupExecutable();
	QObject::connect(gtest, SIGNAL(finished(int, QProcess::ExitStatus)),
					 this, SLOT(parseTestResults(int, QProcess::ExitStatus)));
	gtest->start(filePath, QStringList() << "--gtest_output=xml");
	//unlock the processLock in the parseTestResults slot
}

void GTest::parseTestResults(int exitCode, QProcess::ExitStatus exitStatus) {

	processLock.unlock();
	emit testResultsReady(this);
}

void GTest::setupExecutable() {
	//Should we check and see if gtest is non-zero?
	//if(gtest != 0) {}
	gtest = new QProcess();
	QObject::connect(gtest, SIGNAL(readyReadStandardOutput()),
					 this, SLOT(standardOutputAvailable()));
	QObject::connect(gtest, SIGNAL(finished(int, QProcess::ExitStatus)),
					 this, SLOT(executableFinished(int, QProcess::ExitStatus)));
}

void GTest::standardOutputAvailable() {
	qDebug() << "std output available...";
	outputLock.lock();
	gtest->setReadChannel(QProcess::StandardOutput);
	readExecutableOutput(standardOutput);
	outputLock.unlock();
	qDebug() << "List finished populating";
}

void GTest::standardErrorAvailable() {
	qDebug() << "std error available...";
	outputLock.lock();
	gtest->setReadChannel(QProcess::StandardError);
	readExecutableOutput(standardError);
	outputLock.unlock();
	qDebug() << "List finished populating";
}

void GTest::readExecutableOutput(QBuffer& standardChannel) {
	//Don't expect it to be that large,
	//but whatever, who doesn't have a MB
	//or two lying around?
	const int BUF_SIZE = 1024;
	char buffer[BUF_SIZE];
	qint64 lineLength;
	while(!gtest->atEnd()) {
		while(gtest->canReadLine()) {
			qDebug() << "reading a line...";
			lineLength = gtest->readLine(buffer, BUF_SIZE);
			if(lineLength > 0 && lineLength <= BUF_SIZE) {
				standardChannel.write(buffer);
				qDebug() << buffer << "added to buffer.";
			}
		}
	}
}

void GTest::executableFinished(int exitCode, QProcess::ExitStatus exitStatus) {
	qDebug() << "exe finished";
	QObject::disconnect(gtest, SIGNAL(readyReadStandardOutput()),
						this, SLOT(standardOutputAvailable()));
	QObject::disconnect(gtest, SIGNAL(finished(int, QProcess::ExitStatus)),
						this, SLOT(executableFinished(int, QProcess::ExitStatus)));

	//We don't do anymore processing or anything in here,
	//just record the exit status/code for reference.
	error = gtest->error();
	this->exitStatus = exitStatus;
	this->exitCode = exitCode;
	gtest->deleteLater();
	gtest = 0;
}

GTest::STATE GTest::getState() {
	state = VALID;
	QFile file(filePath);
	if(!file.exists()) {
		state = FILE_NOT_FOUND;
	}
	else if(!(file.permissions() & QFile::ExeUser)) {
		state = INSUFFICIENT_PRIVILEGES;
	}
	return state;
}


