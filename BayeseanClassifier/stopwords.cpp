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
#include <string>
#include <set>
#include <fstream>
#include <sstream>
#include "stopwords.h"
#include "baseException.h"

using namespace std;

/* This class manages a list of stop words. These are words that are so
   common in documents that they are useless for English test analysis.
   No agreed on list exists; it gets tweaked by each application. This
   code deals with the issue by reading the list from a configuration
   file specified at construction time */

/* Initialize the stopword list from the supplied file. Not finding
    it causes an exception */
Stopwords::Stopwords(const string& dataFileName)
    : _wordList()
{
    // Read file in a try..catch block to ensure it is closed on error
    ifstream dataFile;
    try {
        dataFile.open(dataFileName.c_str(), ios_base::in);
        if (!dataFile.is_open()) {
            stringstream errorMessage;
            errorMessage << "Error: Stopword file " << dataFileName << " could not be opened";
            THROW_BASE_EXCEPTION(errorMessage.str().c_str());
        }

        string buffer;
        while (dataFile >> buffer) {
            // Words are specified on one line split by commas or spaces
            size_t index = buffer.find_first_not_of(", ");
            while (index != string::npos) {
                size_t end = buffer.find_first_of(", ", index);
                if (end == string::npos) // Last word on line
                    end = buffer.length();
                _wordList.insert(buffer.substr(index, end - index));
                index = buffer.find_first_not_of(", ", end);
            }
        } // While rows in the file to process

        // If the word list is empty at this point, have a corrupted word file
        if (_wordList.empty()) {
            stringstream errorMessage;
            errorMessage << "Error: Stopword file " << dataFileName << " has no data";
            THROW_BASE_EXCEPTION(errorMessage.str().c_str());
        }
        dataFile.close();
    } // Try block
    catch (...) {
        /* Close file, and delete word list to ensure class is always in
            a consistent state */
        if (dataFile.is_open())
            dataFile.close();
        _wordList.clear();
        throw;
    }
}

/* Return all stop words as a comma seperated string, used for debugging */
string Stopwords::allStopwords() const
{
    ostringstream buffer;
    set<string>::const_iterator index;
    for (index = _wordList.begin(); index != _wordList.end(); index++)
        buffer << *index << " ";
    return buffer.str();
}
