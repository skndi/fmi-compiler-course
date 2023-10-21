#include <iostream>
#include <string>

int main() {
  std::string stringToParse{"bacab"};
  std::string pattern{"ab"};
  bool found = false;
  char previousLetter{};

  for (int stringIndex = 0, patternIndex = 0;
       stringIndex < stringToParse.size(); stringIndex++) {

    std::cout << "String index: " << stringIndex
              << " Pattern index: " << patternIndex << '\n';
    std::cout << "Compare: " << stringToParse[stringIndex] << " and "
              << pattern[patternIndex] << '\n';

    if (pattern[patternIndex + 1] == '?') {
      if (stringToParse[stringIndex] != pattern[patternIndex]) {
        std::cout << "Didn't match before ?\n";
        patternIndex += 2;
        continue;
      } else {
        std::cout << "Matched before ?\n";
        patternIndex += 2;
        continue;
      }
    }
    if (stringToParse[stringIndex] == pattern[patternIndex]) {
      if (patternIndex == pattern.size() - 1) {
        found = true;
        break;
      }

      patternIndex++;
    }
  }
  if (found) {
    std::cout << "Found!\n";
  }
}
