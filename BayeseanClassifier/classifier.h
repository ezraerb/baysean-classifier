#ifndef CLASSIFIER_H
#define CLASSIFIER_H

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
#include "documentWordMapFactory.h"
#include "catWordData.h"

using std::map;
using std::string;

/* This class calculates the likelyhood that a given
    document is part of its category, given the probability
    data that it holds.

    It calculates the probability using the classic Naive Bayes
    probability algorithm. It has known limitations, but is resonably
    accurate on general document sets and fast. This class calculates
    needed algorithm data up front and caches it for additional speed.

    With the size of the documents, calculating with actual probabilities
    will cause numeric underflow. Taking the natural log of the algorithm
    calculation solves this problem */
class Classifier
{
private:
    /* Log probability that a document chosen at random from
        the set falls in this category */
    double _docProbability;

    /* Log probability that a given word is from a document in
        this category */
    map<string, double> _wordProbability;

    /* Log probability that a previously unknown word is from a
        document in this category */
    double _unknownWordProbability;

public:
    /* Constructor. Requires data bout the words in documents
        in this category, the overall number of documents, and
        a tuning parameter used to handle unknwon words */
    Classifier(const CatWordData& trainingData, unsigned short totalDocCount,
               double knownWordWeight);

    // Use default copy constructo, assignment operator, and destructor

    /* Given data about the words in a document, return the scaled log
        probability that it belongs to this category */
    double getCategoryProbability(const DocumentWordMap& document) const;

    /* Return a string containing the probability data in this class,
        used for debugging
        WARNING: Likely to be very long */
    string classifierToString() const;
};

typedef map<string, Classifier> CategoryClassifiers;

#endif // CLASSIFIER_H
