#ifndef FILE_FINDER_H
#define FILE_FINDER_H

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
#include <climits>

using std::string;
using std::vector;

class FileFinder
{
    public:
        /* Finds all files in a directoy hiearchy and adds their paths to the file
            list. Note that the list is not cleared first, so this method may be
            called on multiple roots. If the level paramters are set, files found
            outside that depth in the directoy heirarchy (with the root as zero)
            will be ignored */
        static void findFiles(const vector<string>& roots, vector<string>& fileList,
                              short minLevel = 0, short maxLevel = SHRT_MAX);

        static void findFiles(const string& root, vector<string>& fileList,
                              short minLevel = 0, short maxLevel = SHRT_MAX);

    private:
        // Find all files starting at a given point in the directoy tree
        static void findFiles(const string& dirName, vector<string>& fileList,
                              short currLevel, short minLevel, short maxLevel);
};

#endif // FILE_FINDER_H
