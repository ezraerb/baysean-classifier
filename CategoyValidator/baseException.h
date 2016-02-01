#ifndef BASE_EXCEPTION_H
#define BASE_EXCEPTION_H

/* This file is part of Cagtegoy Validator for BayseanClassifier.
    It verifies the classification of documents against expected
    results and computes the accuacy of classification.

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

#include <exception>
using std::exception;

/* Macro for calling exceptions with line information. Can't have this as a
    Constructor because it would reflect THIS file, not where the exception was
    actually thrown */
#define THROW_BASE_EXCEPTION(message) throw BaseException(__FILE__, __LINE__, message)

// Base exception class. Reports errors and little else
class BaseException: public exception
{
    /* Exceptions must not throw their own exceptions at any time! Memory
        allocation on the heap can throw out of memory, so this class has
        it on the stack. The size was picked to be larger than any possible
        error message */
private:
    /* TRICKY NOTE: Static constants can't be used for array sizes. The use of an
        enum is the standard way around the problem */
    enum {MessageSize = 200};
    char _message[MessageSize];

public:
    BaseException(const char* file, int line, const char* message) throw();

    // Report error message
    virtual const char* what() const throw();

    // Assignment and copy, required by class exception
    BaseException& operator=(const BaseException& other) throw();
    BaseException(const BaseException& other) throw();

    virtual ~BaseException() throw();
};

inline const char* BaseException::what() const throw()
{
    return _message;
}

#endif // BASE_EXCEPTION_H
