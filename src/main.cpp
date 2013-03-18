#include <iostream>
#include <vector>
#include "morsetext.h"

using namespace std;

int main()
{
    MorseText text("Hello World!");
    cout << text.toString() << endl;
    vector<MorseText::Signal> signals;
    text.toSignals(signals);

    for (auto s : signals) {
        switch (s) {
        case MorseText::PAUSE_WORD:
            cout << " ";
            break;
        case MorseText::SHORT:
            cout << ".";
            break;
        case MorseText::LONG:
            cout << "-";
            break;
        };
    }
    cout << endl;

    return 0;
}

