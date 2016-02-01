#ifndef PORTER_STEMMER_H
#define PORTER_STEMMER_H
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

using std::string;
using std::vector;
using std::pair;

/* This class implements a stemmer, which converts words into their roots. Very
   important in text processing, it allows code to handle different variants of
   a word as though they were the same, leading to cleaner results. It also
   reduces compute, by reducing the number of dimension of the word-space
   spanned by documents.

   Many different stemmer algorithms exist. All have different tradeoffs
   between speed, false positives, and false negatives. This code implements
   the Porter algorithm, the most widely used. Its quite fast and resonably
   accurate.

   The Porter algorithm works based on pattern recognition. It looks for words
   matching certain patterns, and then manipulates the word based on the
   results. The matching is done in multiple phases, so a word may be
   manipulated multiple times. The goal is to remove all suffuxes, leaving just
   the root. The root may not be an actual English word, which does not affect
   subsequent processing as long as it is consistent.

   The original paper is available at:
   http://tartarus.org/martin/PorterStemmer/def.txt
*/

class PorterStemmer
{
    private:
        /* Suffix replacement rules. If the string ends with the first
            string, replace it with the second */
        typedef vector<pair<string, string> > SuffixReplacements;

        /* C++ can efficiently search strings for values in other strings. These
            provide the values to search for */
        static const string _vowels;
        static const string _vowelsAndY;

        // Y is a vowel only if preceeded by a consonant. This test defines it efficiently
        static bool isConsonant(char letter);

        // Returns true if the word has a vowel before a certain position
        static bool stemHasVowel(const string& word, size_t pos);

        /* Returns true if the current stem has at least the given number of syllables
            after the suffix under evaluation is removed */
        static bool hasSyllableCount(const string& stem, const vector<size_t>& syllables,
                                     size_t wantSyllable, size_t suffixSize);

        // Returns true if the second string is a suffix of the first
        static bool hasSuffix(const string& word, const string& suffix);

        /* If the word has the first suffix, replace it with the second suffix.
            Returns TRUE if a replacement takes place */
        static bool replaceSuffix(string& word, const string& suffix,
                                  const string& replacement);

        /* If the word has the first suffix, and the stem is larger than the passed size,
            replace the suffix with the passed suffix Returns TRUE if a replacement takes
            place */
        static bool replaceSuffix(string& word, const string& suffix,
                                  size_t stemLength, const string& replacement);

        /* Tests a word for a series of suffixes. The first one that matches beyond
            the location of the specified syllable is replaced as given in the rule.
            If none match, the word is returned unedited. Retuns true if a replacement
            took place */
        static bool replaceSuffix(string& word, const vector<size_t>& syllables,
                                  size_t wantSyllable,
                                  const SuffixReplacements& suffixes);

        /* Test for a series of suffuxes in order, and replace the first one that matches
            and the stem is equal to or larger than the passed size. Returns TRUE if a
            replacement took place */
        static bool replaceSuffix(string& word, int reqStemLength,
                                  const SuffixReplacements& suffixes);

        // Retuns the location of next syllable in a word
        static size_t nextSyllable(const string& word, size_t pos);

        // Returns the location of the second through fourth syllables in a word
        static void getSyllables(const string& word, vector<size_t>& syllables);

    public:
        /* Get the stem for a word. Must be all lowercase with no
            punctuation except for dashes */
        static string getStem(const string& word);

        // A function to test the stemmer. Failed tests go to standard out
        static void testStemmer();
};

#endif // PORTER_STEMMER_H
