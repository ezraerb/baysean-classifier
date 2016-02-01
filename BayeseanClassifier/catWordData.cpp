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
#include "CatWordData.h"

/* This class represents data about a catgegory of documents. It's
    found once and then cached */
CatWordData::CatWordData()
{
    clear();
}

// Add a new document to the category results
void CatWordData::addDocument(const DocumentWordMap& docData)
{
    // Merge word data first, to handle the unlikely case it throws
    _wordData.mergeMap(docData);
    _docCount++;
    _wordCount += docData.size(); // Number of different words
    _totalWordCount += docData.getTotalWordCount();
}

// Merge other category data into this data
void CatWordData::mergeData(const CatWordData& other)
{
    // Merge word data first, to handle the unlikely case it throws
    _wordData.mergeMap(other._wordData);
    _docCount += other._docCount;
    _wordCount += other._wordCount;
    _totalWordCount += other._totalWordCount;
}
