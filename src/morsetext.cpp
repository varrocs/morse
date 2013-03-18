#include "morsetext.h"

// For character check
#include <ctype.h>

// For debugging
#include <iostream>

using namespace std;
// http://en.wikipedia.org/wiki/Morse_code
map<char, vector<MorseText::Signal> > MorseText::signalMap = {
    {'a', {SHORT, LONG} },
    {'b', {LONG, SHORT, SHORT, SHORT} },
    {'c', {LONG, SHORT, LONG, SHORT} },
    {'d', {LONG, LONG, SHORT} },
    {'e', {SHORT} },
    {'f', {SHORT, SHORT, LONG, SHORT} },
    {'g', {LONG, LONG, SHORT} },
    {'h', {SHORT, SHORT, SHORT, SHORT} },
    {'i', {SHORT, SHORT} },
    {'j', {SHORT, LONG, LONG, LONG} },
    {'k', {LONG, SHORT, LONG} },
    {'l', {SHORT, LONG, SHORT, SHORT} },
    {'m', {LONG, LONG} },
    {'n', {LONG, SHORT} },
    {'o', {LONG, LONG, LONG} },
    {'p', {SHORT, LONG, LONG, SHORT} },
    {'q', {LONG, LONG, SHORT, LONG} },
    {'r', {SHORT, LONG, SHORT} },
    {'s', {SHORT, SHORT, SHORT} },
    {'t', {LONG} },
    {'u', {SHORT, SHORT, LONG} },
    {'v', {SHORT, SHORT, SHORT, LONG} },
    {'w', {SHORT, LONG, LONG} },
    {'x', {LONG, SHORT, SHORT, LONG} },
    {'x', {LONG, SHORT, LONG, LONG} },
    {'z', {LONG, LONG, SHORT, SHORT} },

    {'1', {SHORT, LONG, LONG, LONG, LONG} },
    {'2', {SHORT, SHORT, LONG, LONG, LONG} },
    {'3', {SHORT, SHORT, SHORT, LONG, SHORT} },
    {'4', {SHORT, SHORT, SHORT, SHORT, LONG} },
    {'5', {SHORT, SHORT, SHORT, SHORT, SHORT} },
    {'6', {LONG, SHORT, SHORT, SHORT, SHORT} },
    {'7', {LONG, LONG, SHORT, SHORT, SHORT} },
    {'8', {LONG, LONG, LONG, SHORT, SHORT} },
    {'9', {LONG, LONG, LONG, LONG, SHORT} },
    {'0', {LONG, LONG, LONG, LONG, LONG} }
};

MorseText::MorseText() : rawText() {}

/// Expects raw string
MorseText::MorseText(const string& rawString) : rawText(rawString) {}


class ParseState {
	bool prevState;
	int count;

	MorseText::Signal lastSignal;

public:

	MorseText::Signal getLastSignal() {return lastSignal;}

	ParseState() : prevState(false), count(0), lastSignal(MorseText::PAUSE_WORD) {}

	bool feed(bool signal) {
		if (signal xor prevState) { // The signal has changed
			//cout << "Signal changed to "<< signal << ", count: " << count << endl;
			if (prevState) {
				if (count == 1 || count == 2) {
					lastSignal = MorseText::SHORT;
				} else {
					lastSignal = MorseText::LONG;
				}
			} else {
				if (count == 1) {
					lastSignal = MorseText::PAUSE_SIGNAL;
				} else if ( count < 4){
					lastSignal = MorseText::PAUSE_LETTER;
				} else {
					lastSignal = MorseText::PAUSE_WORD;
				}
			}
			count = 1;
			prevState = signal;
			return true;
		} else {
			count++;
			return false;
		}
	}
};

MorseText::MorseText(const std::vector<bool>& signalVector) {
	// Interpret a letter
	vector<Signal> letter;
	ParseState state;

	for (auto s : signalVector) {
		//cout << "Feeding: " << s << endl;
		bool changed  = state.feed(s);
		if (changed) {
			letter.push_back(state.getLastSignal());
			//cout << "last signal: " << state.getLastSignal() << endl;
		}
	}

	// Find the next big pause
	auto beginit = letter.begin();
	auto endit = letter.begin();

	while (true) {
		while (endit != letter.end()) {
			if (*endit == PAUSE_LETTER || (*endit == PAUSE_WORD)) {
				++endit;
				break;
			}
			++endit;
		}

		// Find match
		auto word = vector < Signal > (beginit, endit);
		//Clear the pauses
		auto wordClear = vector<Signal>();
		for (auto s : word) {
			if (s == SHORT || s == LONG) {
				wordClear.push_back(s);
			}
		}
		/*
		cout << "WordClear: ";
		for (auto s : wordClear) {
			cout << s << " ";
		}
		cout << endl;
		*/
		bool found = false;
		for (auto l : signalMap) {
			if (l.second == wordClear) {
				rawText.append(1, l.first);
				//cout << "Found: "<< l.first << endl;
				found = true;
				break;
			}
		}
		if (found) {
			rawText.append(1, ' ');
		}

		if (endit == letter.end() ) {
			break; // The endless loop
		} else {
			beginit = endit;
		}

	}

}

/// Returns raw string representation
string  MorseText::toString() {
    return rawText;
}

void MorseText::toSignals(vector<Signal>& appendTo) {
    for (auto c : rawText) {
        encode(c, appendTo);
    }
}

void MorseText::toSignals(std::vector<bool>& appendTo) {
	vector<Signal> temp;
	toSignals(temp);
	for (auto s : temp) {
		switch (s) {
		case PAUSE_SIGNAL:
			appendTo.push_back(false);
			break;
		case PAUSE_LETTER:
			appendTo.push_back(false);
			appendTo.push_back(false);
			appendTo.push_back(false);
			break;
		case PAUSE_WORD:
			appendTo.push_back(false);
			appendTo.push_back(false);
			appendTo.push_back(false);
			appendTo.push_back(false);
			appendTo.push_back(false);
			appendTo.push_back(false);
			break;

		case SHORT:
			appendTo.push_back(true);
			appendTo.push_back(false);
			break;
		case LONG:
			appendTo.push_back(true);
			appendTo.push_back(true);
			appendTo.push_back(true);
			appendTo.push_back(false);
			break;
		}
	}
}

MorseText& MorseText::operator<<(const string& rawString) {
    rawText += rawString;
    return *this;
}

MorseText& MorseText::operator<<(const char c) {
    rawText += c;
    return *this;
}

void MorseText::encode(char c, vector<Signal>& signal) {
    // Precheck
    if (isspace(c)) {
        signal.push_back(PAUSE_WORD);
        return;
    }

    if (!isalnum(c)) {
        return;
    }

    c = tolower(c);

    // Get from the map
    for (auto s : signalMap[c]) {
        signal.push_back(s);
    }
    signal.push_back(PAUSE_LETTER);
}

