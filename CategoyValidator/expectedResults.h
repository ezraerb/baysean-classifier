#ifndef EXPECTED_RESULTS_H
#define EXPECTED_RESULTS_H

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

#include <vector>
#include <map>
#include <set>
#include <string>

using std::vector;
using std::map;
using std::set;
using std::string;

class ExpectedResults
{
public:
    /* Construct with the vector of roots of directory trees
        of documents by category. If any are invalid, construction
        fails and an exception is thrown */
    explicit ExpectedResults(const vector<string>& dirRoots);

    // Returns the category for a file. Returns the empty string if none found
    string getCorrectCategory(const string& fileName) const;

    // Returns true if a category is valid
    bool isCatValid(const string& category) const;

    // Logs the object conents to standard out for debugging
    void debugOutput() const;

private:
    map<string, string> _expectedResults;
    set<string> _validCategories;
};

inline string ExpectedResults::getCorrectCategory(const string& fileName) const
{
    map<string, string>::const_iterator index = _expectedResults.find(fileName);
    if (index != _expectedResults.end())
        return index->second;
    else
        return string("");
}

inline bool ExpectedResults::isCatValid(const string& category) const
{
    return _validCategories.find(category) != _validCategories.end();
}

#endif // EXPECTED_RESULTS_H
