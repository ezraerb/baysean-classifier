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

/* This program expects documents to be sorted into categories
   as follows:
   root1
      category1
          document1
          document2
          etc.
      category2
          etc.
  root2
      etc.

  It builds the expected categories for each file in each
  category directoy
*/
#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <iostream>
#include "expectedResults.h"
#include "fileFinder.h"
#include "baseException.h"

using namespace std;

ExpectedResults::ExpectedResults(const vector<string>& dirRoots)
{
    if (dirRoots.size() <= 0) {
        stringstream errorMessage;
        errorMessage << "Error, results directory tree list empty";
        THROW_BASE_EXCEPTION(errorMessage.str().c_str());
    }

    /* Search each directoy tree for the files it contains. Given the required
        directory tree structure, the files will be found on level 2 */
    vector<string> fileList;
    FileFinder::findFiles(dirRoots, fileList, 2, 2);

    vector<string>::const_iterator index;

    for (index = fileList.begin(); index != fileList.end(); index++) {
        /* With the required directoy setup, the last directory above
            the file name is the category. Find it in the path. If not
            found, its an error */
        unsigned int secondLastSlash = string::npos;
        unsigned int lastSlash = index->find_last_of("/\\");
        if ((lastSlash != string::npos) && (lastSlash != 0))
            secondLastSlash = index->find_last_of("/\\", lastSlash - 1);
        if ((secondLastSlash == string::npos) ||
            (lastSlash - secondLastSlash <= 1)) {
            // Serious problem, file fetch did not set paths properly
            stringstream errorMessage;
            errorMessage << "ERROR: could not extact category from file path " << *index;
            THROW_BASE_EXCEPTION(errorMessage.str().c_str());
        }

        // NOTE: The extra 1 to avoid the slash before the category
        string category(index->substr(secondLastSlash + 1, lastSlash - secondLastSlash - 1));
        _expectedResults.insert(make_pair(*index, category));
        _validCategories.insert(category);

    } // Loop on found files

    // If no files at all were found, issue an exception for bad directories
    if (_validCategories.size() <= 0) {
        stringstream errorMessage;
        errorMessage << "ERROR: results directories contained no files";
        THROW_BASE_EXCEPTION(errorMessage.str().c_str());
    }
}

// Logs the object conents to standard out for debugging
void ExpectedResults::debugOutput() const
{
    map<string, string>::const_iterator resultsIndex;
    for (resultsIndex = _expectedResults.begin(); resultsIndex != _expectedResults.end();
         resultsIndex++)
        cout << resultsIndex->first << ": " << resultsIndex->second << endl;
    set<string>::const_iterator catIndex;
    cout << "Valid categories:";
    for (catIndex = _validCategories.begin(); catIndex != _validCategories.end();
         catIndex++)
        cout << " " << *catIndex;
    cout << endl;
}

