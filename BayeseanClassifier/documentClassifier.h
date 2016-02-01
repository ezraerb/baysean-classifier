#ifndef DOCUMENT_CLASSIFIER_H
#define DOCUMENT_CLASSIFIER_H

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
#include <map>
#include <string>
#include <vector>
#include "classifier.h"
#include "documentWordMapFactory.h"
#include "stopwords.h"

using std::map;
using std::string;
using std::vector;

typedef map<string, string> DocClassifyMap;

class DocumentClassifier
{
public:
    // Construct the classifier from a set of training data directories
    DocumentClassifier(const vector<string>& trainingDirs, const string& stopwordsFile,
                       bool traceInfo);

    // Classify documents in a set of files or directories
    void classify(const vector<string>& classifyList, DocClassifyMap& results) const;

    // Classify documents in a file or directory
    void classify(const string& classifyDir, DocClassifyMap& results) const;

private:
    // Stop words for all documents. In class to ensure consistency
    const Stopwords _stopwords;

    // Classifiers per category
    CategoryClassifiers _classifiers;

    // Factory to convert documents to classify into word data
    const DocumentWordMapFactory _wordDataFactory;

    // Trace classification operations
    bool _traceInfo;

    // Classify a directory tree of documents
    void classifyDirs(const string& dirName, DocClassifyMap& results) const;

    // Classify a single document
    void classifyFile(const string& fileName, DocClassifyMap& results) const;
};

#endif // DOCUMENT_CLASSIFIER_H
