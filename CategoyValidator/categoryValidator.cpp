/* This file is part of Cagtegoy Validator for BayseanClassifier.
    It verifies the classification of documents against expected
    results and computes the accuacy of classification.

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

/* This program expects documents to be sorted into categories
   as follows:
   root1
      category1
          document1
          document2
          etc.
      category2
          etc.
  root2
      etc.
  It compares this to the categories as assigned by a classifier. The
  results file must have the following format:
  path to document from root: category
  path to document from root: category

  It compares the categories to those from the data structure to
  calculate the effectiveness of the classifier. Note that any
  classified file not in the expected results is ignored with a
  warning. A file classified to a category not in the expected
  results is treated as misclassified.

  Expected input: resultgs file root1 root2 ...
*/
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string>
#include <vector>
#include "baseException.h"
#include "expectedResults.h"

using namespace std;

struct CatStats {
    CatStats() : _correct(0), _misclassToThis(0), _misclassToOther(0) {}

    // Use default copy constructor, copy operator and destructor

    // Documents correctly classified in this category
    unsigned short _correct;

    // Documents for some other category classified in this one
    unsigned short _misclassToThis;

    // Documents for this category classifed in some other one
    unsigned short _misclassToOther;
};

typedef map<string, CatStats> CategoryStatsMap;


// Given a results file and map of expected results, calculate classification success stats
void getClassifyResultStats(const string& resultsFile, const ExpectedResults& expected,
                            CategoryStatsMap& stats)
{
    ifstream input;
    string buffer;
    stats.clear();
    unsigned short lineCount = 0;
    try { // Process in a try...catch block to ensure file does not leak
        input.open(resultsFile.c_str());
        if (!input.is_open()) {
            stringstream errorMessage;
            errorMessage << "Error, results file " << resultsFile << " could not be opened";
            THROW_BASE_EXCEPTION(errorMessage.str().c_str());
        }
        // In Windows file paths can have spaces, so need to read line by line
        while ((!input.fail()) && (!input.eof())) {
            getline(input, buffer);
            lineCount++;
            /* Split into file path and category. To handle spaces in file
                names, look for ': ' */
            unsigned int splitPos = buffer.rfind(": ");
            // To be valid, must be found with text on either side
            if ((splitPos == string::npos) || (splitPos == 0) ||
                (splitPos + 2 >= buffer.length()))
                cout << "WARNING: " << resultsFile << " line " << lineCount
                     << " ignored, missing file or category" << endl;
            else {
                string classifyFile(buffer.substr(0, splitPos));
                string category(buffer.substr(splitPos + 2, buffer.length() - splitPos - 2));

                /* Look up the file in the expected results map. Not finding it indicates
                    a problem. Most likely, relative paths were specified for the
                    diretory hierarchy and they do not match up to the origianl
                    classification program */
                string expectedCategory(expected.getCorrectCategory(classifyFile));
                if (expectedCategory.empty()) {
                    cout << "WARNING: Expected results not found for file " << classifyFile << endl;
                    // Ignore it
                }
                else {
                    // Locate the statistics for the correct category. Initialize if not found
                    CatStats& correctStats = stats[expectedCategory];
                    if (expectedCategory != category) { // Missclassified
                        correctStats._misclassToOther++;
                        /* If the category it was classifed under is a valid result,
                            note this document as classifed in that category by mistake */
                        if (expected.isCatValid(category))
                            stats[category]._misclassToThis++;
                    } // Misclassifed
                    else
                        correctStats._correct++;
                } // Expected results found
            } // Valid classification result
        } // Lines to process
        input.close();

        /* If have no results at this point, the expected results directories
            were likely specified with paths that did not match the original
            classification. This is an error */
        if (stats.size() <= 0) {
            stringstream errorMessage;
            errorMessage << "ERROR: results file " << resultsFile << " contained no files in expected category directories";
            THROW_BASE_EXCEPTION(errorMessage.str().c_str());
        }
    }
    catch (...) {
        if (input.is_open())
            input.close();
        throw;
    }
}

int main(int argc, char** argv)
{
    /* Need at least two arguments, a results file and the directory
        organized by expected categories. Note that the program name
        is also an argument! */
    if (argc < 3) {
        cerr << "ERROR: Not enough arguments" << endl
             << "Usage: CategoryValidator.exe results_file directory_of_classified_files [additional_directories]" << endl;
    }
    else {
        try {
            // Assemble expected results
            vector<string> resultsDirs;
            int index;
            for (index = 2; index < argc; index++) // Program is zero, results file is 1
                resultsDirs.push_back(argv[index]);
            ExpectedResults expectedResults(resultsDirs);

            CategoryStatsMap stats;
            getClassifyResultStats(argv[1], expectedResults, stats);

            CategoryStatsMap::const_iterator statIndex;
            for (statIndex = stats.begin(); statIndex != stats.end(); statIndex++) {
                /* When evaluating a classification algorithm, people care about two things,
                    precision and recall. Precision is the percentage of documents classified
                    for a given category that actually belong there. Recall is the percentage
                    of documents in a category that were classified there. These are normaly
                    combined into a statistic called the balanced F-mesaure: F = PR/2(P+R)
                    This code reports this statistic per category */
                cout << statIndex->first << ": _correct: " << statIndex->second._correct
                     << " _misclassToThis: " << statIndex->second._misclassToThis
                     << " _misclassToOther: " << statIndex->second._misclassToOther << endl;

                /* NOTE: To appear in the categoy list, a document must have either been classified
                    in the category, or supposed to be clssified in it. If ALL documents were classified in
                    error for a category, precision or recall is zero. */
                double precision = 0.0;
                if ((statIndex->second._correct != 0) || (statIndex->second._misclassToThis != 0))
                    precision = ((double)statIndex->second._correct /
                                 ((double)statIndex->second._correct + (double)statIndex->second._misclassToThis));
                double recall = 0.0;
                if ((statIndex->second._correct != 0) || (statIndex->second._misclassToThis != 0))
                    recall = ((double)statIndex->second._correct /
                              ((double)statIndex->second._correct + (double)statIndex->second._misclassToOther));
                double fmeasure = 0.0;
                // Avoid a divide by zero for truly horrible classifiers
                if (precision + recall)
                    fmeasure = precision * recall * 2 / (precision + recall);

                cout << statIndex->first << ": " << "Balance F measure: " << fmeasure
                     << " precision: " << precision << " recall: " << recall << endl;
            } // For each category
        }
        catch (exception& e) {
            cerr << "ERROR: Caught exception " << e.what() << endl;
        }
    }
}
