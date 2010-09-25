/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * GTestExecutable.h - Created on 2010-07-23                                                           *
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

#ifndef GTESTEXECUTABLE_H_
#define GTESTEXECUTABLE_H_

#include <QBuffer>
#include <QObject>
#include <QProcess>
#include <QMetaType>
#include <QMutex>
#include <QSet>

#include "GTestExecutableResults.h"
#include "GTestSuite.h"

/*! \brief This class logically represens a gtest executable file.
 *
 * This class attempts to mirror what an actual gtest executable is comprised of.
 * For example, it contains a name (filepath) and contains a collection
 * of unit test suites. Thus, this structure is reflected in objects of this
 * type hold a number of GTestSuites which hold the actual GTests.
 * \note This file may separate into two or more classes to separate the
 * 		 process functionality from the logic. However, as it stands now,
 *       I think this is acceptable (for the current size).
 */
class GTestExecutable : public GTestSuite {

Q_OBJECT

public:

	/*! The state of the file executable as determined by the name
	 * instance variable (which is actually the file path).
	 */
	enum STATE {
		VALID = 0x0,					/*!< The file exists and is a valid gtest
										 *   executable */
		FILE_NOT_FOUND = 0x1,			//!< The file does not exist
		INSUFFICIENT_PRIVILEGES = 0x2	/*!< The file exists, but the user has
										 *   insufficient privileges to view it */
	};

private:
	STATE state;			//!< The state of the executable defined by 'name'
	QMutex processLock;		//!< A lock to ensure only a single process is active
	QMutex outputLock;		/*!< A lock to ensure that the output buffers only
							 *   have a single writer. */
	QProcess *gtest;		//!< The process thread for running tests.
	QBuffer standardOutput;	//!< The buffer to hold the stdout text.
	QBuffer standardError;	//!< The buffer to hold the stderr text.
	QSet<QString> listingSet;//!< The set of unit test suites provided.
	QSet<QString> oldListingSet;//!< The set of elements prior to an update.
	QStringList testsToRun;	//!< The list of tests to run.
	bool runOnSignal;		/*!< A flag to indicate whether a process should be
							 *   run when the runTest slot is called */

	QProcess::ProcessError error;	//!< The state of the QProcess after its run
	QProcess::ExitStatus exitStatus;//!< The exit status of the QProcess
	int exitCode;					//!< The exit code of the QProcess

	void runExecutable(QStringList args);
	void readExecutableOutput(QBuffer& standardChannel);

signals:
	void listingReady(GTestExecutable* sender); //!< Sends notification that a new listing has been received.

public slots:
	void standardErrorAvailable();
	void standardOutputAvailable();
	void executableFinished(int exitCode, QProcess::ExitStatus exitStatus);
	void runTest();
	void parseListing(int exitCode, QProcess::ExitStatus exitStatus);
	void parseTestResults(int exitCode, QProcess::ExitStatus exitStatus);
	void resetRunState();

public:

	GTestExecutable(QObject* parent = 0, QString executablePath = QString());
	virtual ~GTestExecutable();

//GETS:
	QProcess::ProcessError getError() const;
	QString getExecutablePath() const;
	int getExitCode() const;
	QProcess::ExitStatus getExitStatus() const;
	const QSet<QString>& getListing() const;
	const QSet<QString>& getOldListing() const;
	STATE getState();
	void setRunFlag(bool runOnSignal);

//SETS:
	void setExecutablePath(QString executablePath);

//METHODS:
	void produceListing();

	virtual void run();
};

Q_DECLARE_METATYPE(GTestExecutable*);

/*! \brief Retrieves the listing (not necessarily populated).
 *
 * The caller should ensure that produceListing() has been called first and
 * that the QProcess has successfully completed.
 * \see GTestExecutable::produceListing()
 */
inline const QSet<QString>& GTestExecutable::getListing() const { return listingSet; }

/*! \brief Retrieves the listing prior to an update (not necessarily populated).
 *
 * If nothing has changed between sets, oldListing should be equal to listing.
 */
inline const QSet<QString>& GTestExecutable::getOldListing() const { return oldListingSet; }

/*! \brief Retrieves the executable path (i.e. name) of the executable file
 *
 * \note This function duplicates the functionality of getName(), however,
 * 		 this is semantically clearer. This should either be removed, or
 * 		 perhaps a 'name' and 'filepath' could be two separate variables
 * 		 where 'filepath' is the full path and 'name' is simply the filename.
 * \todo TODO::create a new instance variable 'filePath'.
 */
inline QString GTestExecutable::getExecutablePath() const { return objectName(); }

/*! \brief Retrieves the error produced from running the gtest executable.
 *
 * This is only populated after either produceListing() or runTest() is called
 * while the state is STATE::VALID.
 */
inline QProcess::ProcessError GTestExecutable::getError() const { return error; }

/*! \brief Retrieves the exit status from running the gtest executable.
 *
 * This is only populated after either produceListing() or runTest() is called
 * while the state is STATE::VALID.
 */
inline QProcess::ExitStatus GTestExecutable::getExitStatus() const { return exitStatus; }

/*! \brief Retrieves the exit code from running the gtest executable.
 *
 * This is only populated after either produceListing() or runTest() is called
 * while the state is STATE::VALID.
 */
inline int GTestExecutable::getExitCode() const { return exitCode; }

/*! \brief Sets the executable's path.
 *
 * This will set the executables path. One can check to see if the path is
 * valid and that produceListing() and runTest() will work by calling
 * GTestExecutable::getState()
 * \param executablePath The fully qualified path to a gtest executable.
 * \todo TODO::set the return value of this function to return STATE
 */
inline void GTestExecutable::setExecutablePath(QString executablePath) { setObjectName(executablePath); }

/*! \brief Sets the flag that determines whether this GTestExecutable
 *  responds to the runTest() slot.
 *
 *  \param runOnSignal whether the flag should allow the signal (true), or not.
 */
inline void GTestExecutable::setRunFlag(bool runOnSignal) { this->runOnSignal = runOnSignal; }

/*! \brief Slot that resets the run state of this executable.
 *
 * Resetting the executable prepares it to be run again and produce new
 * test results.
 * \todo TODO::This should be clearing the buffers, runList, etc.
 */
inline void GTestExecutable::resetRunState() { setRunFlag(false); }

#endif /* GTESTEXECUTABLE_H_ */
