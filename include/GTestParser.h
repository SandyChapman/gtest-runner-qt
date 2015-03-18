/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * GTestParse.h - Created on 2010-07-18                                                      *
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

#ifndef GTESTPARSER_H_
#define GTESTPARSER_H_

#include <QDir>
#include <QFile>
#include <QXmlStreamAttributes>
#include "GTestExecutableResults.h"

/*! \brief This class parses an .xml file outputted by a gtest executable.
 *
 * This parser takes a generic QIODevice and parses the results out of it.
 * The iodevice enables a user to pass anything from a file handle (QFile)
 * to a buffer (QBuffer). The parsed results are returned as a tree of
 * GTestResults objects.
 */
class GTestParser {

private:
	QIODevice *xmlSource;	//!< A pointer to the gtest .xml source

public:
	GTestParser(QIODevice *inStream);
	GTestExecutableResults* parse();
    void parseAttributes(GTestResults* testResults, QXmlStreamAttributes attributes);

};

#endif /* GTESTPARSER_H_ */
