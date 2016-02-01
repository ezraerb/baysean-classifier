#ifndef CAT_WORD_DATA_FACTORY_H
#define CAT_WORD_DATA_FACTORY_H

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
#include <map>
#include <vector>
#include "catWordData.h"
#include "documentWordMapFactory.h"

using std::map;
using std::string;

/* This class takes a directory tree of documents sorted by category, and
    converts them into data about each category. It assumes the directory is
    organized as follows:
    [root]
        [category 1]
            documents
        [category 2]
            documents
        etc.
*/

typedef map<string, CatWordData> InfoByCategory;

class CatWordDataFactory
{
private:
    const DocumentWordMapFactory _docProcessor;

    // Trace how files are processed
    bool _traceInfo;

    // Process a single category directory of documents
    void processCategory(const string& filesRoot, const string& category,
                         InfoByCategory& info) const;

public:
    CatWordDataFactory(const Stopwords& stopwords, bool traceInfo);

    // Use default copy constructor, destructor, and assignment operator

    // Generate information about the words in a set of docments.
    void generateInfo(const string& filesRoot, InfoByCategory& info) const;

    // Generate information about the words in multiple sets of documents
    void generateInfo(const vector<string>& filesRoot, InfoByCategory& info) const;

    // Utility method to print of data by category
    static string infoByCategoryToString(const InfoByCategory& info);
};

#endif // CAT_WORD_DATA_FACTORY_H
