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
#include <iostream>
#include "porterStemmer.h"

using namespace std;

/* C++ can efficiently search strings for values in other strings. These
    provide the values to search for */
const string PorterStemmer::_vowels("aeiou");
const string PorterStemmer::_vowelsAndY("aeiouy");

// Private inline methods. Must be declared before they are used
inline bool PorterStemmer::isConsonant(char letter)
{
    /* This method assumes all lowercase and no punctuation.
        Since texts are treated as bags of words, it should
        be valid */
    return (_vowelsAndY.find(letter) == string::npos);
}

// Returns true if the word has a vowel before a certain position
inline bool PorterStemmer::stemHasVowel(const string& word, size_t pos)
{
    // This is tricker than it sounds, because the vowel may be a 'y'
    if (word.find_last_of(_vowels, pos - 1) != string::npos)
        return true;
    else {
        // Any Y preceded by a consonant is a vowel
        size_t index = word.rfind('y', pos - 1);
        while ((index != string::npos) && (index != 0) &&
               (!isConsonant(word[index - 1])))
            index = word.rfind('y', index - 1);
        return ((index != string::npos) && (index != 0));
    }
}

// Returns true if the second string is a suffix of the first
inline bool PorterStemmer::hasSuffix(const string& word, const string& suffix)
{
    if (word.length() <= suffix.length())
        return false;
    else
        // Suffix must match the end of word
        /* NOTE: Depending on how compare() is implemented, this may be faster
            with custom code to compare from the end of both strings */
        return (word.compare(word.length() - suffix.length(), suffix.length(),
                             suffix) == 0);
}

/* Returns true if the current stem has at least the given number of syllables
    after the suffix under evaluation is removed */
inline bool PorterStemmer::hasSyllableCount(const string& stem, const vector<size_t>& syllables,
                                            size_t wantSyllable, size_t suffixSize)
{
    /* To pass, the original word must have had the wanted number of syllables,
        and the wanted syllable must still be in the stem after the suffix is
        removed. Remember that the syllable list is indexed from zero and does
        not include the first */
    if (wantSyllable == 1)
        return true;
    else
        return ((syllables.size() >= (wantSyllable - 1)) &&
                (syllables[wantSyllable - 2] < stem.length() - suffixSize));
}

/* If the word has the first suffix, replace it with the second suffix.
    Returns TRUE if a replacement takes place */
inline bool PorterStemmer::replaceSuffix(string& word, const string& suffix,
                                         const string& replacement)
{
    // The suffix can't be the entire word, hence the minimum stem length of 1
    return replaceSuffix(word, suffix, 1, replacement);
}

/* If the word has the first suffix, and the stem is larger than the passed size,
    replace the suffix with the passed suffix Returns TRUE if a replacement takes
    place */
inline bool PorterStemmer::replaceSuffix(string& word, const string& suffix,
                                         size_t stemLength, const string& replacement)
{
    if (((suffix.length() + stemLength) > word.length()) ||
        (!hasSuffix(word, suffix)))
        return false;
    else {
        word.replace(word.length() - suffix.length(), suffix.length(), replacement);
        return true;
    }
}

/* Tests a word for a series of suffixes. The first one that matches beyond
    the location of the specified syllable is replaced as given in the rule.
    If none match, the word is returned unedited. Retuns true if a replacement
    took place */
bool PorterStemmer::replaceSuffix(string& word, const vector<size_t>& syllables,
                                  size_t wantSyllable,
                                  const SuffixReplacements& suffixes)
{
    /* If the word never had the wanted number of syllables, no
        replacement is possible. Remember that the first is not indexed */
    if (syllables.size() + 1 < wantSyllable)
        return false;
    else {
        /* If the first syllable was specified, it starts at the beginning
            of the string, otherwise need to look it up syllable before the
            one to replace */
        size_t wantStemSize = 1;
        if (wantSyllable > 1)
            // Syllables indexed from zero, add 1 to get size
            wantStemSize = syllables[wantSyllable - 2] + 1;

        SuffixReplacements::const_iterator index = suffixes.begin();
        bool haveMatch = false;
        while ((!haveMatch) && (index != suffixes.end())) {
            haveMatch = replaceSuffix(word, index->first, wantStemSize,
                                      index->second);
            index++;
        }
        return haveMatch;
    }
}
// Retuns the location of next syllable in a word
size_t PorterStemmer::nextSyllable(const string& word, size_t pos)
{
    /* A syllable here is defined as one or more consecutive vowels, optionally
        preceeded by one or more consecutive consonants. This does not match up
        with the linguistic defintion but works well enough for the stemmer.
        The letter 'y' can be either a constant or a vowel, depending on context.
        If preceeded by a constant other than 'y', its a vowel, otherwise its a
        constant.

        With this definition, the start of the next group of consecutive
        consonants after a group of consecutive vowels defines the start of
        the next syllable. */
    bool haveVowel = false;
    size_t index = word.find_first_of(_vowelsAndY, pos);
    while ((!haveVowel) && (index != string::npos)) {
        if ((word[index] != 'y') ||
            ((index != 0) && (isConsonant(word[index - 1]))))
            haveVowel = true;
        else
            index = word.find_first_of(_vowelsAndY, index + 1);
    } // While vowel not found

    if (index == string::npos) // Not found
        return index;

    bool haveConsonant = false;
    /* NOTE: Its tempting here to exclude 'y' as a consonant if it was
        found above as a vowel, but this won't work. The 'y' can have
        other vowels after it, which would make the next 'y' a consonant */
    index = word.find_first_not_of(_vowels, index + 1);
    while ((!haveConsonant) && (index != string::npos)) {
        /* If have a 'y', the previous letter is guarenteed to be a
            vowel, unless it is also a 'y', in which case it was the
            vowel found above and this 'y' is also a vowel
            NOTE: The index is guarenteed to not be 0 at this point,
            but test for it anywsay */
        if ((word[index] != 'y') ||
            ((index != 0) && (word[index - 1] != 'y')))
            haveConsonant = true;
        else
            index = word.find_first_not_of(_vowels, index + 1);
    }
    return index;
}

// Returns the location of the second through fourth syllables in a word
void PorterStemmer::getSyllables(const string& word, vector<size_t>& syllables)
{
    /* Certain stemming operations depend on the number of syllables a word
        will have after the stemming operation. The locations are calculated
        so they only need to be done once; comparing the wanted syllable to
        the overall length will show whether it would survive the stemming.
        The first syllable is ignored here because its location is obvious.
        Only three are extracted because the stemmer only cares about four
        syllables at the most. */
    size_t index = 0;
    syllables.clear();
    while ((index != string::npos) && (syllables.size() < 4)) {
        index = nextSyllable(word, index);
        if (index != string::npos) { // found one
            syllables.push_back(index);
            index++;
        }
    }
}

string PorterStemmer::getStem(const string& word)
{
    /* This code implements the classic Porter stemmer. For each step,
        apply patterns in order until one is matched, then replace as
        needed.

        The original Porter stemmer paper is available at:
        http://tartarus.org/martin/PorterStemmer/def.txt

        OPTIMIZATION: Since this code deals with suffixes only, words that
        could potentially need one type of manipulation will not qualify for
        any other type of manipulation for the same step. This allows
        very convenient branching based on the final chars of a word
        NOTE: Strings are indexed from zero, so length - 1 is the last
        char, etc. */
    vector<size_t> syllables;
    getSyllables(word, syllables);

    // By default, the word is the stem
    string stem(word);

    /* Convert plural to singular.
        WARNING: Not all words that end in 's' are plural */
    if (stem[stem.length() - 1] == 's') {
        if (!replaceSuffix(stem, "sses", "ss"))
            if (!replaceSuffix(stem, "ies", "i"))
                /* If neither of the above applied and the second to last chaacter is
                    not an 's', remove the last 's' */
                if ((stem.length() > 1) && (stem[stem.length() - 2] != 's'))
                    stem.erase(stem.length() - 1);
    } // String ends with 's'

    // Convert verbs to the present tense.
    else if (hasSuffix(stem, "eed")) {
        // If multiple syllables will exist after the suffix removal, convert
        if (hasSyllableCount(stem, syllables, 2, 3))
            stem.erase(stem.length() - 1);
    }
    else {
        /* Test for verb tense conversion. If either succeed, have
            additional processing afterward */
        bool haveTenseConv = false;
        if (hasSuffix(stem, "ed") && stemHasVowel(stem, stem.length() - 2)) {
            stem.erase(stem.length() - 2);
            haveTenseConv = true;
        }
        else if (hasSuffix(stem, "ing") && stemHasVowel(stem, stem.length() - 3)) {
            stem.erase(stem.length() - 3);
            haveTenseConv = true;
        }
        if (haveTenseConv) {
            /* Converting verb tense may change the stem before adding
                the suffix. These tests reverse those changes */

            /* If an 'e' was dropped before adding the suffix, add
                it back. */
            if (hasSuffix(stem, "at") || hasSuffix(stem, "bl") ||
                hasSuffix(stem, "iz"))
                stem.append("e");

            /* Check for constant doubling before the suffix was added.
               If it exists, remove it
               NOTE: Keep in mind that some stems have double letter endings
               and don't qualify here */
            else {
                char lastChar = stem[stem.length() - 1];
                if ((lastChar != 'l') && (lastChar != 's') &&
                    (lastChar != 'z') && (stem.length() > 1) &&
                    (lastChar == stem[stem.length() - 2]))
                    stem.erase(stem.length() - 1);
                    /* This next test is tricky. The word must end with the pattern
                        'consonant-vowel-consonant' and have exactly two syllables.
                        Note that 'y' counts as a vowel here because it is next to a
                        consonant. The last letter of that pattern defined the start
                        of a syllable before the suffix was removed, so the second
                        syllable location must match that spot for this test to pass.
                        Note that any further syllables were removed with the suffix */
                else if ((stem.length() >= 3) && hasSyllableCount(stem, syllables, 2, 0) &&
                         (!hasSyllableCount(stem, syllables, 3, 0)) &&
                         isConsonant(stem[stem.length() - 1]) &&
                         (!isConsonant(stem[stem.length() - 2])) &&
                         isConsonant(stem[stem.length() - 3]))
                    stem.append("e");
            } // Not a simple case for adding an 'e'
        } // Complex tense conversion needed
    } // Not simple tense conversion or plural conversion

    /* If a word containing a non-y vowel ends in a 'y', convert it to a 'i' so it
        matches the stem from the plural change above */
    if ((stem[stem.length() - 1] == 'y') && (stem.length() > 1)) {
        /* Find the last non-y in the string. In nearly all
            cases this will be the second to last letter, so just do
            a linear search */
        size_t lastIndex = stem.length() - 2;
        while ((lastIndex >= 0) && (stem[lastIndex] == 'y'))
            lastIndex--;
        if (lastIndex >= 0) {
            if (stemHasVowel(stem, lastIndex + 1))
                stem.replace(stem.length() - 1, 1, "i");
        }
    } // Last letter is a 'y'


    /* Remove suffixes that create adjectives and adverbs. In order to
        have a suffix, the string must have at least two syllables after
        it is removed
        OPTIMIZATION: The suffixes sort beautifully based on their second
        to last letter. Test this in the word to find the appropriate ones
        OPTIMIZATION: Do the syllable test on the shortest suffix up front */
    if ((stem.length() > 3) && hasSyllableCount(stem, syllables, 2, 3)) {
        SuffixReplacements suffixes;
        switch (stem[stem.length() - 2]) {
        case 'a':
            suffixes.push_back(make_pair(string("ational"), string("ate")));
            suffixes.push_back(make_pair(string("tional"), string("tion")));
            break;
        case 'c':
            suffixes.push_back(make_pair(string("enci"), string("ence")));
            suffixes.push_back(make_pair(string("anci"), string("ance")));
        break;
        case 'e':
            suffixes.push_back(make_pair(string("izer"), string("ize")));
            break;
        case 'l':
            suffixes.push_back(make_pair(string("abli"), string("able")));
            suffixes.push_back(make_pair(string("alli"), string("al")));
            suffixes.push_back(make_pair(string("entli"), string("ent")));
            suffixes.push_back(make_pair(string("eli"), string("e")));
            suffixes.push_back(make_pair(string("ousli"), string("ous")));
            break;
        case 'o':
            suffixes.push_back(make_pair(string("ization"), string("ize")));
            suffixes.push_back(make_pair(string("ation"), string("ate")));
            suffixes.push_back(make_pair(string("ator"), string("ate")));
            break;
        case 's':
            suffixes.push_back(make_pair(string("alism"), string("al")));
            suffixes.push_back(make_pair(string("iveness"), string("ive")));
            suffixes.push_back(make_pair(string("fulness"), string("ful")));
            suffixes.push_back(make_pair(string("ousness"), string("ous")));
            break;

        case 't':
            suffixes.push_back(make_pair(string("aliti"), string("al")));
            suffixes.push_back(make_pair(string("iviti"), string("ive")));
            suffixes.push_back(make_pair(string("biliti"), string("ble")));
            break;

        default:
            // Do nothing
            break;
        } // Switch on second to last letter

        if (!suffixes.empty()) // Apply the list
            replaceSuffix(stem, syllables, 2, suffixes);
    } // Word may have a suffix to remove


    /* More adjective and adverb suffixes, some of which may be removed
        from the stems found above
        OPTIMIZATION: Split on the last letter this time */
    if ((stem.length() > 2) && hasSyllableCount(stem, syllables, 2, 3)) {
        SuffixReplacements suffixes;
        switch (stem[stem.length() - 1]) {
        case 'e':
            suffixes.push_back(make_pair(string("icate"), string("ic")));
            suffixes.push_back(make_pair(string("ative"), string("")));
            suffixes.push_back(make_pair(string("alize"), string("al")));
            break;
        case 'i':
            suffixes.push_back(make_pair(string("iciti"), string("ic")));
            break;
        case 'l':
            suffixes.push_back(make_pair(string("ical"), string("ic")));
            suffixes.push_back(make_pair(string("ful"), string("")));
            break;
        case 's':
            suffixes.push_back(make_pair(string("ness"), string("")));
            break;
        default:
            // Do nothing
            break;
        } // Switch on second to last letter

        if (!suffixes.empty()) // Apply the list
            replaceSuffix(stem, syllables, 2, suffixes);

    } // Word may have suffix to remove

    /* Yet more adjective and adverb suffixes, some of which may be removed
        from the stems found above. At least three syllables must remain after
        removal of the suffix
        OPTIMIZATION: Split on the second to last letter */
    if ((stem.length() > 3) && hasSyllableCount(stem, syllables, 3, 2)) {
        SuffixReplacements suffixes;
        switch (stem[stem.length() - 2]) {
        case 'a':
            suffixes.push_back(make_pair(string("al"), string("")));
            break;
        case 'c':
            suffixes.push_back(make_pair(string("ance"), string("")));
            suffixes.push_back(make_pair(string("ence"), string("")));
            break;
        case 'e':
            suffixes.push_back(make_pair(string("er"), string("")));
            break;
        case 'i':
            suffixes.push_back(make_pair(string("ic"), string("")));
            break;
        case 'l':
            suffixes.push_back(make_pair(string("able"), string("")));
            suffixes.push_back(make_pair(string("ible"), string("")));
            break;
        case 'n':
            suffixes.push_back(make_pair(string("ant"), string("")));
            suffixes.push_back(make_pair(string("ement"), string("")));
            suffixes.push_back(make_pair(string("ment"), string("")));
            suffixes.push_back(make_pair(string("ent"), string("")));
            break;
        case 'o':
            suffixes.push_back(make_pair(string("sion"), string("s")));
            suffixes.push_back(make_pair(string("tion"), string("t")));
            suffixes.push_back(make_pair(string("ou"), string("")));
            break;
        case 's':
            suffixes.push_back(make_pair(string("ism"), string("")));
            break;
        case 't':
            suffixes.push_back(make_pair(string("ate"), string("")));
            suffixes.push_back(make_pair(string("iti"), string("")));
            break;
        case 'u':
            suffixes.push_back(make_pair(string("ous"), string("")));
            break;
        case 'v':
            suffixes.push_back(make_pair(string("ive"), string("")));
            break;
        case 'z':
            suffixes.push_back(make_pair(string("ize"), string("")));
            break;
        default:
            // Do nothing
            break;
        } // Switch on second to last letter

        if (!suffixes.empty()) // Apply the list
            replaceSuffix(stem, syllables, 3, suffixes);
    } // More than two syllables in word

    // Clean up stems after suffix removal
    if (stem[stem.length() - 1] == 'e') {
        if (hasSyllableCount(stem, syllables, 3, 1))
            // At least three syllables
            stem.erase(stem.length() - 1, 1);
        else if (hasSyllableCount(stem, syllables, 2, 1)) {
            // Exactly two syllables remaining
            /* Strip the trailing 'e' unless the final four chars are
                constant-vowel-consanent-e with the second consanent NOT
                'w' or 'x' Note that 'y' counts as a vowell here because it is
                next to a conanent */
            char testChar = stem[stem.length() - 2];
            if ((stem.length() < 4) ||
                (!isConsonant(stem[stem.length() - 4])) ||
                isConsonant(stem[stem.length() - 3]) ||
                (!isConsonant(testChar)) || (testChar == 'w') ||
                (testChar == 'x'))
                stem.erase(stem.length() - 1, 1);
        }
    }

    if (hasSyllableCount(stem, syllables, 3, 1) &&
        hasSuffix(stem, string("ll")))
        stem.erase(stem.length() - 1);

    return stem;
}

// A function to test the stemmer. Failed tests go to standard out
void PorterStemmer::testStemmer()
{
    /* Create a long list of words to test. Nearly all of them come
        from the original paper. Note that for some the word should NOT
        change. Compare the stems to the expected results, and report the
        mismatches */
    vector<pair<string, string> > tests;
    tests.push_back(make_pair(string("caresses"), string("caress")));
    tests.push_back(make_pair(string("ponies"), string("poni")));
    tests.push_back(make_pair(string("ties"), string("ti")));
    tests.push_back(make_pair(string("caress"), string("caress")));
    tests.push_back(make_pair(string("cats"), string("cat")));

    tests.push_back(make_pair(string("syllables"), string("syllabl")));
    tests.push_back(make_pair(string("feed"), string("feed")));
    tests.push_back(make_pair(string("agreed"), string("agre")));
    tests.push_back(make_pair(string("plastered"), string("plaster")));
    tests.push_back(make_pair(string("bled"), string("bled")));

    tests.push_back(make_pair(string("sing"), string("sing")));
    tests.push_back(make_pair(string("flying"), string("fly")));
    tests.push_back(make_pair(string("conflated"), string("conflat")));
    tests.push_back(make_pair(string("troubled"), string("troubl")));
    tests.push_back(make_pair(string("sized"), string("size")));

    tests.push_back(make_pair(string("hopping"), string("hop")));
    tests.push_back(make_pair(string("falling"), string("fall")));
    tests.push_back(make_pair(string("hissing"), string("hiss")));
    tests.push_back(make_pair(string("failing"), string("fail")));
    tests.push_back(make_pair(string("filing"), string("file")));

    tests.push_back(make_pair(string("sky"), string("sky")));
    tests.push_back(make_pair(string("relational"), string("relat")));
    tests.push_back(make_pair(string("conditional"), string("condition")));
    tests.push_back(make_pair(string("rational"), string("ration")));
    tests.push_back(make_pair(string("valency"), string("valenc")));

    tests.push_back(make_pair(string("digitizer"), string("digit")));
    tests.push_back(make_pair(string("conformably"), string("conform")));
    tests.push_back(make_pair(string("differently"), string("differ")));
    tests.push_back(make_pair(string("analogously"), string("analog")));
    tests.push_back(make_pair(string("authorization"), string("author")));

    tests.push_back(make_pair(string("predication"), string("predic")));
    tests.push_back(make_pair(string("operator"), string("oper")));
    tests.push_back(make_pair(string("feudalism"), string("feudal")));
    tests.push_back(make_pair(string("decisiveness"), string("decis")));
    tests.push_back(make_pair(string("hopefulness"), string("hope")));

    tests.push_back(make_pair(string("callousness"), string("callous")));
    tests.push_back(make_pair(string("formality"), string("formal")));
    tests.push_back(make_pair(string("sensitivity"), string("sensit")));
    tests.push_back(make_pair(string("sensibility"), string("sensibl")));
    tests.push_back(make_pair(string("ability"), string("abil")));

    tests.push_back(make_pair(string("triplicate"), string("triplic")));
    tests.push_back(make_pair(string("formative"), string("form")));
    tests.push_back(make_pair(string("formalize"), string("formal")));
    tests.push_back(make_pair(string("electricity"), string("electr")));
    tests.push_back(make_pair(string("electrical"), string("electr")));

    tests.push_back(make_pair(string("revival"), string("reviv")));
    tests.push_back(make_pair(string("allowance"), string("allow")));
    tests.push_back(make_pair(string("inference"), string("infer")));
    tests.push_back(make_pair(string("airliner"), string("airlin")));
    tests.push_back(make_pair(string("adjustable"), string("adjust")));

    tests.push_back(make_pair(string("defensible"), string("defens")));
    tests.push_back(make_pair(string("replacement"), string("replac")));
    tests.push_back(make_pair(string("element"), string("elem")));
    tests.push_back(make_pair(string("dependent"), string("depend")));
    tests.push_back(make_pair(string("activate"), string("activ")));

    tests.push_back(make_pair(string("effective"), string("effect")));
    tests.push_back(make_pair(string("rate"), string("rate")));
    tests.push_back(make_pair(string("cease"), string("ceas")));
    tests.push_back(make_pair(string("controller"), string("control")));
    tests.push_back(make_pair(string("roll"), string("roll")));

    vector<pair<string, string> >::iterator index;
    int failureCount = 0;
    for (index = tests.begin(); index != tests.end(); index++) {
        string result(getStem(index->first));
        if (result != index->second) {
            cout << "Stem test failed. Word: " << index->first
                 << " Expected stem:" << index->second
                 << " Actual stem: " << result << endl;
            failureCount++;
        }
    }
    if (failureCount > 0)
        cout << failureCount << " tests failed" << endl;
    else
        cout << "All tests passed" << endl;
}
