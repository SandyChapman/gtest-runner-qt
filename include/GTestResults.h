/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * GTestResults.h - Created on 2010-07-27
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

#ifndef GTESTRESULTS_H_
#define GTESTRESULTS_H_

#include <QStringList>

class GTestResults {

public:
	enum STATUS {
		RUN,
		UNDEFINED
	};

private:
	QString name;
	uint time;
	QStringList failureMessages;
	STATUS status;

public:
	GTestResults(QString name);
	void setStatus(QString status);
	void setRunningTime(uint runTime);
	void addFailureMessage(QString failureMsg);
	STATUS getStatus() const;
	QString getName() const;
	uint getRunningTime() const;
	uint getFailureMessageCount() const;
	QString getFailureMessage(uint num) const;
	QStringList getFailureMessages() const;

};

inline void GTestResults::setStatus(QString statusString) {
	if(statusString == "run")
		status = RUN;
}

inline void GTestResults::setRunningTime(uint runTime) {
	time = runTime;
}

inline void GTestResults::addFailureMessage(QString failureMsg) {
	failureMessages.append(failureMsg);
}

inline GTestResults::STATUS GTestResults::getStatus() const { return status; }

inline QString GTestResults::getName() const {
	return name;
}

inline uint GTestResults::getRunningTime() const {
	return time;
}

inline uint GTestResults::getFailureMessageCount() const {
	return failureMessages.size();
}

inline QString GTestResults::getFailureMessage(uint num) const {
	return failureMessages.at(num);
}

inline QStringList GTestResults::getFailureMessages() const {
	return failureMessages;
}

#endif /* GTESTRESULTS_H_ */
