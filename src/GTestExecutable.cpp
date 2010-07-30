/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * GTestExecutable.cpp - Created on 2010-07-23                                                         *
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

#include "GTestExecutable.h"
#include "GTestParser.h"

#include <QDebug>
#include <QFile>
#include <QMessageBox>

GTestExecutable::GTestExecutable(QObject* parent, QString filePath)
: QObject(parent), filePath(filePath), state(VALID), processLock(),
  gtest(0), outputLock(), standardOutput(), standardError(), runOnSignal(false)
{
	getState();
}

GTestExecutable::GTestExecutable(const GTestExecutable& other)
: QObject(other.parent()),filePath(other.filePath), state(other.state),
  processLock(), gtest(0), outputLock(), standardOutput(),
  standardError()
{
	getState();
}

GTestExecutable::~GTestExecutable() {

}

void GTestExecutable::produceListing() {
	//We lock so that any attempt to try to produce a listing
	//or run a test will block until we're done with what we're
	//doing here.
	processLock.lock();
	gtest = new QProcess();
	QObject::connect(gtest, SIGNAL(readyReadStandardOutput()),
					 this, SLOT(standardOutputAvailable()));
	QObject::connect(gtest, SIGNAL(finished(int, QProcess::ExitStatus)),
					 this, SLOT(executableFinished(int, QProcess::ExitStatus)));
	QObject::connect(gtest, SIGNAL(finished(int, QProcess::ExitStatus)),
					 this, SLOT(parseListing(int, QProcess::ExitStatus)));
	gtest->start(filePath, QStringList() << "--gtest_list_tests");
	//unlock the processLock in the parseListing slot
}

void GTestExecutable::parseListing(int /*exitCode*/, QProcess::ExitStatus exitStatus) {
	if(exitStatus != QProcess::NormalExit) {
		QMessageBox::warning((QWidget*)this->parent(),"Error Retrieving Listing",
				"The Google test executable exited abnormally.");
		processLock.unlock();
	}
	standardOutput.open(QBuffer::ReadOnly);
	QByteArray temp;
	while(standardOutput.canReadLine()) {
		temp = standardOutput.readLine();
		temp.resize(temp.size()-1);
		qDebug() << temp;
		listing << temp;
	}
	standardOutput.close();
	processLock.unlock();
	emit listingReady(this);
}

void GTestExecutable::runTest() {
	//We lock so that any attempt to try to produce a listing
	//or run a test will block until we're done with what we're
	//doing here.
	processLock.lock();
	gtest = new QProcess();
	QObject::connect(gtest, SIGNAL(readyReadStandardOutput()),
					 this, SLOT(standardOutputAvailable()));
	QObject::connect(gtest, SIGNAL(finished(int, QProcess::ExitStatus)),
					 this, SLOT(parseTestResults(int, QProcess::ExitStatus)));
	gtest->start(filePath, QStringList() << "--gtest_output=xml:./test_detail_1337.xml");
	//unlock the processLock in the parseTestResults slot
}

void GTestExecutable::parseTestResults(int exitCode, QProcess::ExitStatus exitStatus) {
	QObject::disconnect(gtest, SIGNAL(finished(int, QProcess::ExitStatus)),
						this, SLOT(parseTestResults(int, QProcess::ExitStatus)));
	processLock.unlock();
	if(exitStatus != QProcess::NormalExit)
		return;
	QFile xmlFile("./test_detail_1337.xml");
	GTestParser parser(&xmlFile);
	testResults = parser.parse();
	QList<GTestSuite*>::iterator it = runList.begin();
	GTestSuiteResults* testSuiteResults;
	while(it != runList.end()) {
		testSuiteResults = testResults->getTestSuiteResults((*it)->getName());
		(*it)->receiveTestResults(testSuiteResults);
		++it;
	}
	runList.clear();
	executableFinished(exitCode, exitStatus);
	emit testResultsReady(this);
}

void GTestExecutable::standardOutputAvailable() {
	qDebug() << "std output available...";
	outputLock.lock();
	gtest->setReadChannel(QProcess::StandardOutput);
	readExecutableOutput(standardOutput);
	outputLock.unlock();
	qDebug() << "List finished populating";
}

void GTestExecutable::standardErrorAvailable() {
	qDebug() << "std error available...";
	outputLock.lock();
	gtest->setReadChannel(QProcess::StandardError);
	readExecutableOutput(standardError);
	outputLock.unlock();
	qDebug() << "List finished populating";
}

void GTestExecutable::readExecutableOutput(QBuffer& standardChannel) {
	//Don't expect it to be that large,
	//but whatever, who doesn't have a MB
	//or two lying around?
	const int BUF_SIZE = 1024;
	char buffer[BUF_SIZE];
	qint64 lineLength;
	standardChannel.open(QBuffer::WriteOnly);
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
	standardChannel.close();
}

void GTestExecutable::executableFinished(int exitCode, QProcess::ExitStatus exitStatus) {
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

GTestExecutable::STATE GTestExecutable::getState() {
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

void GTestExecutable::receiveRunRequest(QString caseName, QString testName) {
	runList.append((GTestSuite*)QObject::sender());
	testsToRun << caseName.append(".").append(testName);
}

