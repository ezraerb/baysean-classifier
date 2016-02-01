/* This file is part of BayseanClassifier. It classifies documents into
    categories based on the classic Baysean classification algorithm.

    Copyright (C) 2016   Ezra Erb

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    I'd appreciate a note if you find this program useful or make
    updates. Please contact me through LinkedIn (my profile also has
    a link to the code depository)
*/
#include <cstring>
#include <cstdio>
#include "baseException.h"

using std::exception;

BaseException::BaseException(const char* file, int line, const char* message) throw()
{
    // Copy the file name into the error message
    _message[0] = '[';
    strncpy(_message + 1, file, 99);
    unsigned short length = strlen(_message); // Returns position of NUL at end of string
    if (length < MessageSize) {
        /* Convert the line number to a string and copy. To avoid a memory allocation,
            use C style conversion on the stack and copy */
        char temp[15];
        sprintf(temp, " %d] ", line);
        // NOTE: Nasty C style pointer arithmetic to get the copy to start at the right place
        strncpy(_message + length, temp, MessageSize - length);
        length = strlen(_message);
    }

    /* More important to get the message than the location, so if not enough
        room, take the message */
    if ((length + strlen(message)) >= MessageSize)
        strncpy(_message, message, MessageSize);
    else
        strncpy(_message + length, message, MessageSize - length);
}

// Destructor
BaseException::~BaseException() throw()
{
    // All on stack, so nothing to do!
}

// Assignment and copy, required by class exception
BaseException& BaseException::operator=(const BaseException& other) throw()
{
    strncpy(_message, other._message, MessageSize);
    return *this;
}
BaseException::BaseException(const BaseException& other) throw()
{
    *this=other;
}
