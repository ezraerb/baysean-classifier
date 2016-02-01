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
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include "stopwords.h"
#include "documentWordMapFactory.h"
#include "CatWordDataFactory.h"
#include "baseException.h"
#include "fileFinder.h"

using namespace std;

/* This class takes a directory tree of documents sorted by category, and
    converts them into data about each category */

// Construct with stopwords to filter out. Does not take ownership
CatWordDataFactory::CatWordDataFactory(const Stopwords& stopwords, bool traceInfo)
    : _docProcessor(stopwords), _traceInfo(traceInfo)
    {}

// Generate information about the words in a set of documents
void CatWordDataFactory::generateInfo(const string& filesRoot, InfoByCategory& info) const
{
    info.clear();

    /* Extract all files to generate classification data. Given the required
        directoy setup, they will all appear at level 2 of the hierarchy */
    vector<string> fileList;
    FileFinder::findFiles(filesRoot, fileList, 2, 2);

    string category;
    CatWordData results;
    DocumentWordMap data;

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
        string currCategory(index->substr(secondLastSlash + 1, lastSlash - secondLastSlash - 1));

        if ((index == fileList.begin()) || (category != currCategory)) {
            // Start of a new category
            if (index != fileList.begin()) // Have existing categoy to finish processing
                info.insert(make_pair(category, results));
            results.clear();
            category = currCategory;
            if (_traceInfo)
                cout << category << endl;
        } // Change of category

        if (_traceInfo)
            cout << *index << endl;
        data.clear(); // Ensure previous results do not carry over
        _docProcessor.getWordMap(*index, data);
        if (_traceInfo)
            cout << data.allMapData() << endl;
        results.addDocument(data);
    } // Loop on returned files
    // Process final categoy
    if (fileList.size() > 0)
        info.insert(make_pair(category, results));
}

// Generate information about the words in multiple sets of documents
void CatWordDataFactory::generateInfo(const vector<string>& filesRoot, InfoByCategory& info) const
{
    info.clear();
    InfoByCategory newInfo;
    vector<string>::const_iterator dirIndex;
    try {
        for (dirIndex = filesRoot.begin(); dirIndex != filesRoot.end(); dirIndex++) {
            generateInfo(*dirIndex, newInfo);

            // Merge into overall results
            InfoByCategory::const_iterator catIndex;
            for (catIndex = newInfo.begin(); catIndex != newInfo.end(); catIndex++) {
                // Look up category in this map. If found, merge data, otherwise insert
                InfoByCategory::iterator entry = info.lower_bound(catIndex->first);
                if (entry != info.end() && (entry->first == catIndex->first)) // Already present
                    entry->second.mergeData(catIndex->second);
                else
                    // lower_bound returned where the new word should be inserted
                    info.insert(entry, *catIndex);
            }
            // Don't need to clear newInfo, data load routine handles it
        }
    } // Try block
    catch (...) {
        // Ensure consistent state on exception
        info.clear();
        throw;
    }
}

// Utility method to print of data by category
string CatWordDataFactory::infoByCategoryToString(const InfoByCategory& info)
{
    InfoByCategory::const_iterator index;
    ostringstream buffer;
    for (index = info.begin(); index != info.end(); index++)
        buffer << index->first << ": Doc: " << index->second.getDocCount()
                << " Unique word: " << index->second.getWordCount()
                << " Total word: " << index->second.getTotalWordCount()
                << index->second.getWordData().allMapData();
    return buffer.str();
}
