#ifndef CAT_WORD_DATA_H
#define CAT_WORD_DATA_H

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
#include "documentWordMapFactory.h"

// This class represents data about a catgegory of documents.
class CatWordData
{
private:
    unsigned short _docCount; // Number of documents in category
    unsigned int _wordCount; // Number of different words in category documents
    unsigned int _totalWordCount; // Overall number of words in documents
    DocumentWordMap _wordData; // Counts of individual words

public:
    CatWordData();

    // Use default copy constructor, assignment operator, and destructor

    // Add a new document to the category results
    void addDocument(const DocumentWordMap& docData);

    // Merge other category data into this data
    void mergeData(const CatWordData& other);

    // Reset all infomation in the class
    void clear();

    // Number of documents in category
    unsigned short getDocCount() const;

    // Number of different words in category documents
    unsigned int getWordCount() const;

    // Overall number of words in documents
    unsigned int getTotalWordCount() const;

    // Counts of individual words
    const DocumentWordMap& getWordData() const;
};

// Reset all infomation in the class
inline void CatWordData::clear()
{
    _docCount = 0;
    _wordCount = 0;
    _totalWordCount = 0;
    _wordData.clear();
}

// Number of documents in category
inline unsigned short CatWordData::getDocCount() const
{
    return _docCount;
}

// Number of different words in category documents
inline unsigned int CatWordData::getWordCount() const
{
    return _wordCount;
}

// Overall number of words in documents
inline unsigned int CatWordData::getTotalWordCount() const
{
    return _totalWordCount;
}

// Counts of individual words
inline const DocumentWordMap& CatWordData::getWordData() const
{
    return _wordData;
}

#endif // CAT_WORD_DATA_H
