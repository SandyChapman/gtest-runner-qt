/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * GTestResults.cpp - Created on 2010-07-27
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

#include "GTestResults.h"

/*! \brief Constructor
 *
 * \param name The name of the unit test to which this object belongs.
 */
GTestResults::GTestResults()
: failureMessages()
{}

/*! \brief Destructor
 *
 */
GTestResults::~GTestResults()
{}


QString GTestResults::serialiseAttributes(){
    QString attribstr;
    foreach(const QString &attr, attributes.keys()){
        attribstr.append(attr);
        attribstr.append(": ");
        attribstr.append(attributes.value(attr));
        attribstr.append("\n\r");
    }
    return attribstr;
}
