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
#include <cmath> // For log()
#include <sstream>

#include "documentWordMapFactory.h"
#include "catWordData.h"
#include "classifier.h"

using namespace std;
/* This class calculates the likelyhood that a given document
    is part of its category, given the probability data that it holds.

    It calculates the probability using the classic Naive Bayes
    probability algorithm. It has known limitations, but is resonably
    accurate on general document sets and fast. In particular, it assumes
    that the probability of a given word being in a document is independent
    of both its position and the other words in the document.

    With the size of the documents, calculating with actual probabilities
    will cause numeric underflow. Taking the natural log of the algorithm
    calculation solves this problem */

/* Constructor. Requires data about the words in documents
    in this category, the overall number of documents, and
    a tuning parameter used to handle unknwon words */
Classifier::Classifier(const CatWordData& trainingData,
                       unsigned short totalDocCount,
                       double knownWordWeight)
{
    /* Document probability: number of documents in category divided by total.
        number of documents. Note cast to double so divide is done at high
        precision */
    _docProbability = log((double)trainingData.getDocCount() / (double)totalDocCount);

    /* Total word count adjusted by the word weight. Note the cast to ensure the
        calculation is done as doubles */
    double adjustedWordCount = (double)trainingData.getTotalWordCount() +
        ((double)trainingData.getWordCount() * knownWordWeight);

    /* Probability for each word. Number in document set adjusted by the known word
        weight divided by the adjusted number of words in the documents */
    const DocumentWordMap& wordData = trainingData.getWordData();
    DocumentWordMap::const_iterator index;
    for (index = wordData.begin(); index != wordData.end(); index++) {
        double wordProbability = log(((double)index->second + knownWordWeight) /
                                     adjustedWordCount);
        _wordProbability.insert(make_pair(index->first, wordProbability));
    }

    /* Probability of an unknown word is the same as a known word with a frequency
        of zero */
    _unknownWordProbability = log(knownWordWeight / adjustedWordCount);
}

/* Given data about the words in a document, return the scaled log probability
    that it belongs to this category */
double Classifier::getCategoryProbability(const DocumentWordMap& document) const
{
    /* This method implements the classic Baysean algorithm for calculating the probability
        a given document is in the class. Its calculated using logarythms to avoid numeric
        underflow. The wanted probability is the sum of the log probability of the category
        plus the log probability that each word in the document signals it is in the category.

        Technically, to get the probability need to subtract the log probability that each word
        appears in ANY category, called the evidence. This value is the same for every category
        this document could belong to, so it makes no difference for classification. Leaving it
        out makes the code faster */
    double probability = _docProbability;

    DocumentWordMap::const_iterator index;
    map<string, double>::const_iterator wordIndex;
    for (index = document.begin(); index != document.end(); index++) {
        wordIndex = _wordProbability.find(index->first);
        if (wordIndex == _wordProbability.end())
            // Unknown word
            probability += (_unknownWordProbability * index->second);
        else
            // Add the probability per times the word appears
            probability += (wordIndex->second * index->second);
    }
    return probability;
}

/* Return a string containing the probability data in this class,
    used for debugging.
    WARNING: Likely to be very long */
string Classifier::classifierToString() const
{
    ostringstream buffer;
    buffer << "_docProability:" << _docProbability << " _unknownWordProbability:"
            << _unknownWordProbability << "Words:";

    map<string, double>::const_iterator index;
    for (index = _wordProbability.begin(); index != _wordProbability.end(); index++)
        buffer << " " << index->first << ": " << index->second;
    return buffer.str();
}
