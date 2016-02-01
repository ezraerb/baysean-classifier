#ifndef STOPWORDS_H
#define STOPWORDS_H

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
#include <set>
#include <string>

using namespace std;

/* This class manages a list of stop words. These are words that are so
   common in documents that they are useless for English test analysis.
   No agreed on list exists; it gets tweaked by each application. This
   code deals with the issue by reading the list from a configuration
   file specified at construction time */
class Stopwords
{
    public:
        // Constructor. Needs full path to file
        explicit Stopwords(const string& dataFileName);

        // Use default destuctor

        // Return true if a given word is a stopword
        bool isStopword(const string& word) const;

        /* Return all stop words as a comma seperated string, used
            for debugging */
        string allStopwords() const;


    private:
        // The words
        set<string> _wordList;

        // Make non-copyable, ensures all clients use the same list
        Stopwords(const Stopwords& other);
        Stopwords& operator=(const Stopwords& other);
};

inline bool Stopwords::isStopword(const string& word) const
{
    // The classic set search
    return (_wordList.find(word) != _wordList.end());
}

#endif // STOPWORDS_H
