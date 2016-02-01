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
#include <cstring>

#include "documentClassifier.h"
#include "baseException.h"

using namespace std;

// Class to parse arguments. Used to reduce method scope
/* Format of input is a switch followed by values for that switch.
    Specifying multiple switches is allowed; the values are merged.
    or overwritten as appropriate. If an overwrite takes place or a
    switch has no arguments, a warning is issued. An invalid switch,
    or specifying the help option, triggers a usage printout and
    the program terminates */
class ArgumentParser
{
public:
// Parse arguments, returns true if they are valid
static bool parse(int argc, char** argv, vector<string>& trainingDirs,
                  vector<string>& classifyFiles, string& stopwordsFile,
                  bool& traceInfo)
{
    // Set default values
    trainingDirs.clear();
    classifyFiles.clear();
    stopwordsFile = string("stopwords.txt");
    traceInfo = false;

    bool seenStopwords = false;

    int index = 1; // 0 is the program name
    bool valid = true;
    while ((index < argc) && valid) {
        /* Argument values are parsed with the arguments, so only options
            should be seen in this loop. Programs should have few enough
            of them that just compare linearly */
        if (strcmp(argv[index], "--training-dirs") == 0) {
            index++;
            int valueCount = getValues(argc, argv, index, trainingDirs);
            if (!valueCount)
                cerr << "WARNING: --training-dirs option specified with no values" << endl;
            index += valueCount;
        }
        else if (strcmp(argv[index], "--classify-docs") == 0) {
            index++;
            int valueCount = getValues(argc, argv, index, classifyFiles);
            if (!valueCount)
                cerr << "WARNING: --classify-docs option specified with no values" << endl;
            index += valueCount;
        }
        else if (strcmp(argv[index], "--stopwords-file") == 0) {
            index++;
            // Declare that stopwords file name can not start with '--'
            if ((index == argc) || isOption(argc, argv, index)) {
                cerr << "ERROR: Stopwords option specified without file name" << endl;
                valid = false;
            }
            else {
                if (seenStopwords)
                    cerr << "WARNING: --stopwords-file specified twice, previous value ignored" << endl;
                stopwordsFile = string(argv[index]);
                seenStopwords = true;
                index++;
            }
        } // Stopwoards file
        else if (strcmp(argv[index], "--trace-info") == 0) {
            traceInfo = true;
            index++;
        }
        else if (strcmp(argv[index], "--help") == 0)
            /* Since any error causes the help message, declaring this to be
                an error will produce the wanted result */
            valid = false;
        else if (isOption(argc, argv, index)) {
            cerr << "ERROR: unknown option " << argv[index] << " specified" << endl;
            valid = false;
        }
        else {
            /* Values are processed with the options, so getting to here means a value was
                found without an option first. This is an error */
            cerr << "ERROR: value " << argv[index] << " found without a preceeding option" << endl;
            valid = false;
        }
    } // While loop through values
    // Verify that mandatory values have been read.
    if (valid) {
        if (trainingDirs.empty()) {
            cerr << "ERROR: No directories for training classification files specified" << endl;
            valid = false;
        }
        if (classifyFiles.empty()) {
            cerr << "ERROR: No files to classify specified" << endl;
            valid = false;
        }
    }

    if (!valid)
        // Append the usage to any argument error message (or the help flag)
        usage();
    return valid;
}

private:

// Returns true if the given argument is an option
static bool isOption(int argc, char** argv, int argument)
{
    return ((argument > 0) && (argument < argc) &&
            // Works only due to conditional AND, ensures string non-empty
            (argv[argument][0] == '-') &&
            (argv[argument][1] == '-'));
}

/* Extracts values for a given argument into the passed vector
    returns the number found */
static int getValues(int argc, char** argv, int firstValue,
                     vector<string>& values)
{
    int startSize = values.size();
    int valueIndex = firstValue;
    while ((valueIndex < argc) && (!isOption(argc, argv, valueIndex))) {
        values.push_back(string(argv[valueIndex]));
        valueIndex++;
    }
    return values.size() - startSize;
}

// Prints usage
static void usage()
{
    cerr << "Usage: BayseanClassifier.exe flag values flag values [flag] [values]" << endl
         << "Mandatory flags:" << endl
         << "--training-dirs  Directories to find training documents organized into directories by category" << endl
         << "                 Multiple are allowed" << endl
         << "--classify-docs  Documents to classify based on training data. If a directory is specified, every" << endl
         << "                 file in it will be clssified. Mutiple are allowed" << endl
         << "Optional flags:" << endl
         << "--stopwords-file File to load stopwords from. Defaults to 'stopwords.txt' in current directory" << endl
         << "--trace-info     Traces probability data about documents used by the classifier. Will produce huge" << endl
         << "                 output on any resonable sized document set" << endl
         << "--help           Prints this message and exits" << endl;
}

};

// The driver for the Baysean Classifier
int main(int argc, char** argv)
{
    try {
        // Assemble arguments
        vector<string> trainingDirs;
        vector<string> classifyFiles;
        string stopwordsFile;
        bool traceInfo;

        if (ArgumentParser::parse(argc, argv, trainingDirs, classifyFiles, stopwordsFile,
                                  traceInfo)) {

            if (traceInfo) {
                // Print training data input
                vector<string>::const_iterator dirIndex;
                cout << "Training dirs:";
                for (dirIndex = trainingDirs.begin(); dirIndex != trainingDirs.end(); dirIndex++)
                    cout << " " << *dirIndex;
                cout << endl;
                cout << "Stop words file: " << stopwordsFile << endl;
                cout << "Files to classify:";
                for (dirIndex = classifyFiles.begin(); dirIndex != classifyFiles.end(); dirIndex++)
                    cout << " " << *dirIndex;
                cout << endl;
            }

            DocumentClassifier classifier(trainingDirs, stopwordsFile, traceInfo);
            DocClassifyMap results;
            classifier.classify(classifyFiles, results);

            // Print out documents and categories
            DocClassifyMap::const_iterator index;
            for (index = results.begin(); index != results.end(); index++)
                cout << index->first << ": " << index->second << endl;
        } // Arguments are valid
    }
    catch (exception& e) {
        cerr << "Classification Failed. Caught exception " << e.what() << endl;
    }
}
