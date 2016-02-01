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

/* This class implements a very simplified directoy spider. This opeation
    is OS specific, so this class encapsulates the details fom the rest
    of the classifier. Currently only a Windows implemenation is available */
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include "fileFinder.h"
#include "baseException.h"
#include "windows.h"

using namespace std;

/* Finds all files in a directoy hiearchy and adds their paths to the file
    list. Note that the list is not cleared first, so this method may be
    called on multiple roots. If the level paramters are set, files found
    outside that depth in the directoy heirarchy (with the root as zero)
    will be ignored */
void FileFinder::findFiles(const vector<string>& roots, vector<string>& fileList,
                           short minLevel, short maxLevel)
{
    vector<string>::const_iterator index;
    for (index = roots.begin(); index != roots.end(); index++)
        findFiles(*index, fileList, minLevel, maxLevel);
}


// Same operation for a single directoy root
void FileFinder::findFiles(const string& root, vector<string>& fileList,
                           short minLevel, short maxLevel)
{
    if (minLevel > maxLevel)
        cerr << "WARNING: No files found in " << root << ". Max dir level below min level" << endl;
    else if (maxLevel < 0)
        cerr << "WARNING: No files found in " << root << ". Max dir level below zero" << endl;
    else {
        // If passed name is a file, retun it if the minLevel is zero.
        DWORD fileData = GetFileAttributes(root.c_str());
        if (fileData == INVALID_FILE_ATTRIBUTES) {
            stringstream errorMessage;
            errorMessage << "ERROR: directory or file to fetch " << root << " does not exist";
            THROW_BASE_EXCEPTION(errorMessage.str().c_str());
        }
        if (fileData & FILE_ATTRIBUTE_DIRECTORY)
            // Directory. Files are at least one level below root, so start count there
            findFiles(root, fileList, 1, minLevel, maxLevel);
        else if (minLevel <= 0)
            fileList.push_back(root);
    } // Level setup allows file fetch
}

// Find all files starting at a given point in the directoy tree
void FileFinder::findFiles(const string& dirName, vector<string>& fileList,
                          short currLevel, short minLevel, short maxLevel)
{
    // Adapted from: http://stackoverflow.com/questions/2314542/listing-directory-contents-using-c-and-windows
    WIN32_FIND_DATA fileData;
    HANDLE file = NULL;
    bool foundSomething = false;

    string searchPath(dirName + "\\*"); // Get everything
    file = FindFirstFile(searchPath.c_str(), &fileData);
    if (file == INVALID_HANDLE_VALUE) {
        // Serious problem
        stringstream errorMessage;
        errorMessage << "ERROR, directory of files to fetch " << dirName << " does not exist";
        THROW_BASE_EXCEPTION(errorMessage.str().c_str());
    }

    do {
        // Skip 'this' and 'parent' directories. Note that the names are C strings
        if ((strcmp(fileData.cFileName, ".") != 0) &&
            (strcmp(fileData.cFileName, "..") != 0)) {
            // Assemble the full path
            string fullPathName(dirName + "\\");
            fullPathName.append(fileData.cFileName);

            if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                // If above maximum level, recurse
                if (currLevel < maxLevel)
                    findFiles(fullPathName, fileList, currLevel + 1,
                              minLevel, maxLevel); // Recursive call
            }
            // If below minimum level, record the file
            else if (currLevel >= minLevel)
                fileList.push_back(fullPathName);
            foundSomething = true;
        } // Not this or parent
    }
    while(FindNextFile(file, &fileData)); //Find the next file.

    FindClose(file); // Ensure no file leaks
    if (!foundSomething)
        cerr << "WARNING: File directoy " << dirName << " skipped, empty" << endl;
}
