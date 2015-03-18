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

#include <QSharedPointer>
#include <QStringList>

/* \brief This class logically represents the results of running a unit test.
 *
 * The class is the analogue to a gtest unit test result. It consists of the name of
 * the test (which is used to assign the result to the correct GTest), the running
 * time of the test, any failure messages that were produced and a summary result
 * status (pass/fail). In the subclasses, these variables also apply, but differ
 * in that a GTestSuiteResults comprises the results of all tests within the suite.
 */
class GTestResults {

protected:
	QStringList failureMessages;	//!< The list of failure messages --unparsed.
    QHash<QString, QString> attributes;

public:
    GTestResults();
	virtual ~GTestResults();

	void addFailureMessage(QString failureMsg);
    void addAttribute(QString name, QString value) {attributes.insert(name, value);}
    QString serialiseAttributes();

    QString get(QString iname){ return attributes.value(iname);}

    virtual uint getFailureCount() const;
	QString getFailureMessage(uint index) const;
	QStringList getFailureMessages() const;

	virtual GTestResults* getTestResults(QString name);

};

/*! \brief Adds a failure message to the list of failure messages.
 *
 * This appends a failure message to the end of the list of failure messages.
 * \param failureMsg The failure message as retrieved from the gtest executable.
 */
inline void GTestResults::addFailureMessage(QString failureMsg) {
	failureMessages.append(failureMsg);
}

/*! \brief Retrieves the total number of failure messages for this test.
 *
 * \note There can be multiple failures per test.
 * \return The number of failures.
 */
inline uint GTestResults::getFailureCount() const {
	return failureMessages.size();
}

/*! \brief Retrieves the failure message indexed by 'index'.
 *
 * This retrieves the index'th failure string from the list of
 * failure strings.
 * \note If you wish to iterate through all failure messages, it
 * 		 is recommended that you retrieve a copy of the entire list
 * 		 of messages \see GTestResults::getFailureMessage().
 * \return The failure message at index 'index.
 */
inline QString GTestResults::getFailureMessage(uint index) const {
	return failureMessages.at(index);
}

/*! \brief Retrieves the entire list of failure messages.
 *
 * If you wish to iterate through all messages, this function is the
 * prefered method to call.
 * \return A copy of the entire list of failure messages.
 */
inline QStringList GTestResults::getFailureMessages() const {
	return failureMessages;
}

/*! \brief Retrieves the test result object with name 'name'.
 *
 * This function is primarily for subclasses which may hold references
 * to other test results.
 * \return A pointer to a GTestResult.
 */
inline GTestResults* GTestResults::getTestResults(QString name) { return (name == attributes.value("name") ? this : 0); }

#endif /* GTESTRESULTS_H_ */
