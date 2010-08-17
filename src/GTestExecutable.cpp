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

/*! \brief Constructor
 *
 * \param parent The parent QObject.
 * \param filePath The path to the executable file.
 * 		  This is validated by a getState() call.
 */
GTestExecutable::GTestExecutable(QObject* parent, QString filePath)
: GTestSuite(parent, filePath), state(VALID), processLock(),
  outputLock(), gtest(0), standardOutput(), standardError(),
  listingSet(), oldListingSet(), testsToRun(), runOnSignal(false)
{
	getState();
}

/*! \brief Destructor
 *
 * Deletes the process handle to the executable file.
 */
GTestExecutable::~GTestExecutable() {
	if(gtest)
		delete gtest;
}

/*! \brief Produces a listing of the unit tests available from this executable.
 *
 * The function produces a listing of all unit tests provided by the executable
 * located at the filePath 'name'. Since QProcess allows us to execute the
 * executable in another thread, we have an asynchronous return the this object.
 * This allows the runner to continue calling produceListing() on other
 * GTestExecutable objects while the QProcess retrieves the result.
 * A caller can determine when the listing is ready by connecting a slot to
 * the listingReady() slot.
 */
void GTestExecutable::produceListing() {
	//We lock so that any attempt to try to produce a listing
	//or run a test will block until we're done with what we're
	//doing here.
	if(getState() != VALID)
		return; //! \todo throw error here?
	//! \todo Check if the gtest file has changed, and preempt here if unchanged.
	processLock.lock();
	gtest = new QProcess();
	QObject::connect(gtest, SIGNAL(readyReadStandardOutput()),
					 this, SLOT(standardOutputAvailable()));
	QObject::connect(gtest, SIGNAL(finished(int, QProcess::ExitStatus)),
					 this, SLOT(executableFinished(int, QProcess::ExitStatus)));
	QObject::connect(gtest, SIGNAL(finished(int, QProcess::ExitStatus)),
					 this, SLOT(parseListing(int, QProcess::ExitStatus)));
	gtest->start(objectName(), QStringList() << "--gtest_list_tests");
	//unlock the processLock in the parseListing slot
}

/*! \brief Parses the listing once the QProcess has exited.
 *
 * This function is called after the QProcess emits its finished() signal.
 * It first checks the exit status and if everything has gone smoothly, it
 * processes the stdout buffer that was filled by the gtest executable.
 * It breaks the buffer into lines which is then understood by the GTestRunner.
 *
 * \todo TODO::Have this function create the GTest / GTestSuite tree instead of the test runner
 */
void GTestExecutable::parseListing(int /*exitCode*/, QProcess::ExitStatus exitStatus) {
	//Check status
	if(exitStatus != QProcess::NormalExit) {
		QMessageBox::warning((QWidget*)this->parent(),"Error Retrieving Listing",
				"The Google test executable exited abnormally.");
		processLock.unlock();
		return;
	}
	//Status is good, set up some vars and let's start parsing
	oldListingSet = listingSet; //copy old set, to find elements to remove
	QString name;
	QString line;
	GTestSuite* testSuite;
	GTest* test;
	standardOutput.open(QBuffer::ReadOnly);
	while(standardOutput.canReadLine()) {
		line = standardOutput.readLine();
		line.resize(line.size()-1);	//remove \n
		if(line.endsWith('.')) {	//this means its a test suite name
			name = line.left(line.size()-1);	//get the name without the '.'
			testSuite = findChild<GTestSuite*>(name);
			if(!testSuite)	//if it doesn't already exist, make one
				testSuite = new GTestSuite(this, name);
			listingSet << name;	//add it to our new listing set
		}
		else {
			//We should always run into 'testsuitename.'
			//before we hit a unit test name.
			Q_ASSERT(testSuite != 0);
			name = line.right(line.size()-2); //test name is prepended with 2 spaces
			test = testSuite->findChild<GTest*>(name);
			if(!test)	//if it doesn't already exist, make one
				test = new GTest(testSuite, name);
			//add it to our new listingSet in form of 'testsuitename.testname"
			listingSet << testSuite->objectName().append('.').append(name);
		}
	}
	//diff our old set with our new set to find removed tests.
	QSet<QString> diffedListing = oldListingSet - listingSet;
	QStringList deletedSuites;
	QSet<QString>::iterator it = diffedListing.begin();
	while(it != diffedListing.end()) {
		int indexOfDot = it->indexOf('.');
		if(indexOfDot == -1) { //not in form of testsuitename.testname
			//must be an entire suite to be removed.
			testSuite = findChild<GTestSuite*>(*it);
			Q_ASSERT(testSuite != 0);
			deletedSuites << testSuite->objectName();
			testSuite->deleteLater(); //deletes children and cleans up signals/slots
			continue;
		}
		//its of the form "testsuitename.testname"
		//check if we've deleted the suite already
		name = it->left(indexOfDot);
		if(deletedSuites.contains(name))
			continue; // already handled the deletion
		testSuite = findChild<GTestSuite*>(name);
		Q_ASSERT(testSuite != 0);
		test = findChild<GTest*>(it->right(it->length()-indexOfDot));
		Q_ASSERT(test != 0);
		test->deleteLater();
		++it;
	}

	standardOutput.close();
	processLock.unlock();
	//! \todo Only emit listingReady if it differs from the last listing.
	emit listingReady(this);
}

/*! \brief Launches a QProcess to run the gtest executable
 *
 * This creates a process and runs it based on the 'name' instance
 * variable which represents the filepath of the executable.
 *
 * \todo TODO::Check the state before running the test.
 */
void GTestExecutable::runTest() {
	//Check our state, and whether we're listening to run signals.
	if(this->state != VALID || !this->runOnSignal)
		return;
	//We lock so that any attempt to try to produce a listing
	//or run a test will block until we're done with what we're
	//doing here.
	processLock.lock();
	gtest = new QProcess();
	QObject::connect(gtest, SIGNAL(readyReadStandardOutput()),
					 this, SLOT(standardOutputAvailable()));
	QObject::connect(gtest, SIGNAL(finished(int, QProcess::ExitStatus)),
					 this, SLOT(parseTestResults(int, QProcess::ExitStatus)));
	gtest->start(objectName(), QStringList() << "--gtest_output=xml:./test_detail_1337.xml");
	//unlock the processLock in the parseTestResults slot
}

/*! \brief Parses the test results .xml file produced by the QProcess.
 *
 * This function opens the .xml file produced by the gtest process and parses
 * its contents. It uses the GTestParser
 */
void GTestExecutable::parseTestResults(int exitCode, QProcess::ExitStatus exitStatus) {
	QObject::disconnect(gtest, SIGNAL(finished(int, QProcess::ExitStatus)),
						this, SLOT(parseTestResults(int, QProcess::ExitStatus)));
	processLock.unlock();
	if(exitStatus != QProcess::NormalExit)
		return;
	QFile xmlFile("./test_detail_1337.xml");
	GTestParser parser(&xmlFile);
	GTestExecutableResults* testResults = parser.parse();
	this->testResults = testResults;
	QList<GTest*>::iterator it = this->runList.begin();
	GTestResults* testSuiteResults;
	while(it != this->runList.end()) {
		testSuiteResults = testResults->getTestResults((*it)->objectName());
		(*it)->receiveTestResults(testSuiteResults);
		++it;
	}
	runList.clear();
	executableFinished(exitCode, exitStatus);
	emit testResultsReady();
}

/*! \brief Slot that is called when stdout data is available from the process.
 *
 * This function sets up the executable reader function using a lock so that
 * data isn't attempted to be read from both standard output and standard
 * error simultaneously (which currently isn't supported by the QProcess
 * class).
 * \see GTestExecutable::readExecutableOutput()
 */
void GTestExecutable::standardOutputAvailable() {
	outputLock.lock();
	gtest->setReadChannel(QProcess::StandardOutput);
	readExecutableOutput(standardOutput);
	outputLock.unlock();
}

/*! \brief Slot that is called when stderr data is available from the process.
 *
 * This function sets up the executable reader function using a lock so that
 * data isn't attempted to be read from both standard output and standard
 * error simultaneously (which currently isn't supported by the QProcess
 * class).
 * \see GTestExecutable::readExecutableOutput()
 */
void GTestExecutable::standardErrorAvailable() {
	outputLock.lock();
	gtest->setReadChannel(QProcess::StandardError);
	readExecutableOutput(standardError);
	outputLock.unlock();
}

/*! \brief Reads the current readChannel data into the appropriate QBuffer.
 *
 * This function is called when either standard output or standard error
 * data is available from the gtest executable process. Setup is done prior
 * to calling this function, so that the correct buffer is populated.
 * \see GTestExecutable::standardErrorAvailable()
 * \see GTestExecutable::standardOutputAvailable()
 */
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
			lineLength = gtest->readLine(buffer, BUF_SIZE);
			if(lineLength > 0 && lineLength <= BUF_SIZE) {
				standardChannel.write(buffer);
			}
		}
	}
	standardChannel.close();
}

/*! \brief Slot to be called when the QProcess has finished execution.
 *
 * This function is called after a QProcess has finished either generating
 * a listing, or running the tests in the gtest executable. It populates
 * the exit statuses which can be subsequently checked by the test runner.
 */
void GTestExecutable::executableFinished(int exitCode, QProcess::ExitStatus exitStatus) {
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

/*! \brief Retrieves the state of the executable.
 *
 * \return The state of the executable (one of VALID, FILE_NOT_FOUND,
 * 		   or INSUFFICIENT_PRIVILEGES).
 */
GTestExecutable::STATE GTestExecutable::getState() {
	state = VALID;
	QFile file(objectName());
	if(!file.exists()) {
		state = FILE_NOT_FOUND;
	}
	else if(!(file.permissions() & QFile::ExeUser)) {
		state = INSUFFICIENT_PRIVILEGES;
	}
	return state;
}


