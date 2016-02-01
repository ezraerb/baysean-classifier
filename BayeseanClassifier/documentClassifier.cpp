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
#include <vector>
#include <map>
#include <iostream>
#include <sstream>

#include "documentClassifier.h"
#include "stopwords.h"
#include "documentWordMapFactory.h"
#include "catWordDataFactory.h"
#include "classifier.h"
#include "baseException.h"
#include "fileFinder.h"

using namespace std;

// Construct the classifier from a set of training data directories
DocumentClassifier::DocumentClassifier(const vector<string>& trainingDirs,
                                       const string& stopwordsFile,
                                       bool traceInfo)
    : _stopwords(stopwordsFile), _wordDataFactory(_stopwords), _traceInfo(traceInfo)
{
    try {
        CatWordDataFactory trainingDataSource(_stopwords, _traceInfo);
        InfoByCategory trainingData;
        trainingDataSource.generateInfo(trainingDirs, trainingData);

        /* At least two categories must be found in the training data or
            classification is not possible */
        if (trainingData.size() < 1) {
            stringstream errorMessage;
            errorMessage << "Error, no training data found in specified directories";
            THROW_BASE_EXCEPTION(errorMessage.str().c_str());
        }
        else if (trainingData.size() < 2) {
            stringstream errorMessage;
            errorMessage << "Error, training data found only for categoy " << trainingData.begin()->first;
            THROW_BASE_EXCEPTION(errorMessage.str().c_str());
        }

        // Need the total document count
        InfoByCategory::const_iterator trainIndex;
        unsigned short totalDocCount = 0;
        for (trainIndex = trainingData.begin(); trainIndex != trainingData.end(); trainIndex++)
            totalDocCount += trainIndex->second.getDocCount();

        /* For each category, create a classifier from the training document data
            for each category. Need to do after all are read in because the total
            documents read affects the classification
            NOTE: A known word weight of 1 works well for medium sized documents and above */
        for (trainIndex = trainingData.begin(); trainIndex != trainingData.end(); trainIndex++)
            _classifiers.insert(make_pair(trainIndex->first,
                                          Classifier(trainIndex->second, totalDocCount, 1.0)));

        CategoryClassifiers::const_iterator classifierIndex;
        if (_traceInfo) {
            cout << "Classifiers:" << endl;
            for (classifierIndex = _classifiers.begin(); classifierIndex != _classifiers.end();
                 classifierIndex++)
                cout << classifierIndex->first << ": " << classifierIndex->second.classifierToString() << endl;
        } // _traceInfo
    }
    catch (...) {
        // Ensure consistent state on exception
        _classifiers.clear();
        throw;
    }
}

// Classify documents in a set of files or directories
void DocumentClassifier::classify(const vector<string>& classifyList, DocClassifyMap& results) const
{
    if (_classifiers.empty()) {
        // Serious problem. Construction failed and exception not handled
        stringstream errorMessage;
        errorMessage << "Internal error: attempt to classify documents with invalid classifier";
        THROW_BASE_EXCEPTION(errorMessage.str().c_str());
    }
    results.clear();
    vector<string>::const_iterator index;
    for (index = classifyList.begin(); index != classifyList.end(); index++)
        classifyDirs(*index, results);
}

// Classify documents in a file or directory
void DocumentClassifier::classify(const string& classifyDir, DocClassifyMap& results) const
{
    if (_classifiers.empty()) {
        // Serious problem. Construction failed and exception not handled
        stringstream errorMessage;
        errorMessage << "Internal error: attempt to classify documents with invalid classifier";
        THROW_BASE_EXCEPTION(errorMessage.str().c_str());
    }
    results.clear();
    classifyDirs(classifyDir, results);
}

// Classify a single document or directory of documents
void DocumentClassifier::classifyDirs(const string& dirName, DocClassifyMap& results) const
{
    vector<string> fileList;
    // Fetch all files in the directoy tree
    FileFinder::findFiles(dirName, fileList);

    if (fileList.empty()) {
        stringstream errorMessage;
        errorMessage << "ERROR, directory or file to classify " << dirName << " contains no files";
        THROW_BASE_EXCEPTION(errorMessage.str().c_str());
    }
    vector<string>::const_iterator index;
    for (index = fileList.begin(); index != fileList.end(); index++)
        classifyFile(*index, results);
}

// Classify a single document
void DocumentClassifier::classifyFile(const string& fileName, DocClassifyMap& results) const
{
    if (_traceInfo)
        cout << "File to classify: " << fileName << endl;

    // Convert the file to word statistics
    DocumentWordMap wordMap;
    _wordDataFactory.getWordMap(fileName, wordMap);

    /* Iterate through the classifiers and score the file with each.
        Highest score indicates highest probability, so it wins */
    CategoryClassifiers::const_iterator index = _classifiers.begin();
    string category(index->first);
    double logProbability = index->second.getCategoryProbability(wordMap);
    if (_traceInfo)
        cout << "Category: " << index->first << " Log probability: " << logProbability << endl;
    index++;
    while (index != _classifiers.end()) {
        double newProbability = index->second.getCategoryProbability(wordMap);
        if (_traceInfo)
            cout << "Category: " << index->first << " Log probability: " << newProbability << endl;
        if (newProbability > logProbability) {
            logProbability = newProbability;
            category = index->first;
        }
        index++;
    } // While loop

    results.insert(make_pair(fileName, category));
}
