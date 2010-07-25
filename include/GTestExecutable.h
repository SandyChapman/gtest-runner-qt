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

#ifndef GTEST_H_
#define GTEST_H_

#include <QBuffer>
#include <QObject>
#include <QProcess>
#include <QMetaType>
#include <QMutex>

class GTestExecutable : public QObject {

Q_OBJECT

public:
	enum STATE {
		VALID = 0x0,
		FILE_NOT_FOUND = 0x1,
		INSUFFICIENT_PRIVILEGES = 0x2
	};

private:
	QString filePath;
	STATE state;
	QMutex processLock;
	QProcess *gtest;
	QMutex outputLock;
	QBuffer standardOutput;
	QBuffer standardError;
	QStringList listing;

	QProcess::ProcessError error;
	QProcess::ExitStatus exitStatus;
	int exitCode;

	void setupExecutable();
	void runExecutable(QStringList args);
	void readExecutableOutput(QBuffer& standardChannel);

signals:
	void listingReady(GTestExecutable* sender);
	void testResultsReady(GTestExecutable* sender);

public slots:
	void standardErrorAvailable();
	void standardOutputAvailable();
	void executableFinished(int exitCode, QProcess::ExitStatus exitStatus);
	void runTest();
	void parseListing(int exitCode, QProcess::ExitStatus exitStatus);
	void parseTestResults(int exitCode, QProcess::ExitStatus exitStatus);

public:

	GTestExecutable(QObject* parent = 0, QString executablePath = QString());
	GTestExecutable(const GTestExecutable& other);
	~GTestExecutable();

	void produceListing();
	QStringList getListing();

	void setExecutablePath(QString executablePath);
	QString getExecutablePath();

	QProcess::ProcessError getError();
	QProcess::ExitStatus getExitStatus();
	int getExitCode();
	STATE getState();

};

Q_DECLARE_METATYPE(GTestExecutable);
Q_DECLARE_METATYPE(GTestExecutable*);


inline QStringList GTestExecutable::getListing() {
	return listing;
}

inline QString GTestExecutable::getExecutablePath() { return filePath; }

inline QProcess::ProcessError GTestExecutable::getError() { return error; }

inline QProcess::ExitStatus GTestExecutable::getExitStatus() { return exitStatus; }

inline int GTestExecutable::getExitCode() { return exitCode; }

inline void GTestExecutable::setExecutablePath(QString executablePath) { this->filePath = executablePath; }


#endif /* GTEST_H_ */
