#ifndef DOCUMENT_WORD_MAP_FACTORY_H
#define DOCUMENT_WORD_MAP_FACTORY_H

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
/* This class defines a map representing a document as a bag of words.
    It gets generated by a factory that takes as input a file with
    the document */
#include <string>
#include <map>
#include "stopwords.h"

using std::string;
using std::map;

/* This is a wrapper around the standard word map, with some additional
    methods for ease of handling. */
// Assume not dealing with War and Peace, so a short should be plenty
class DocumentWordMap : public map<string, unsigned short>
{
private:
    // Add a count for the given word into the map
    void addWordCount(const string& word, unsigned short count);

public:
    // Use default constuctor, destructor, and copy operator

    // Add a word into the map
    void addWord(const string& word);

    // Return the total word count of the map
    unsigned int getTotalWordCount() const;

    // Merge another word map into this one
    void mergeMap(const DocumentWordMap& other);

    // Return a atring containing all data in the map
    // WARNING: Likely to be huge
    string allMapData() const;
};

// Add a count for the given word into the map
inline void DocumentWordMap::addWordCount(const string& word, unsigned short count)
{
    DocumentWordMap::iterator entry = lower_bound(word);
    if (entry != end() && (entry->first == word)) // Already present
        entry->second += count;
    else
        // lower_bound returned where the new word should be inserted
        insert(entry, make_pair(word, count));
}

// Add a word into the map
inline void DocumentWordMap::addWord(const string& word)
{
    addWordCount(word, 1);
}

// Merge another word map into this one
inline void DocumentWordMap::mergeMap(const DocumentWordMap& other)
{
    DocumentWordMap::const_iterator index;
    for (index = other.begin(); index != other.end(); index++)
        addWordCount(index->first, index->second);
}

class DocumentWordMapFactory {
private:
    static const string _letters;
    const Stopwords& _stopwords;

public:
    // Construct with the list of stopwords to use. Does not take ownership
    explicit DocumentWordMapFactory(const Stopwords& stopwords);

    // Convert the specified file into a document word map
    void getWordMap(const string& fileName, DocumentWordMap& wordMap) const;
};

#endif // DOCUMENT_WORD_MAP_FACTORY_H
