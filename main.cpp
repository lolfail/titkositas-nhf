#include <fstream>

#include "GenericTests.hpp"
#include "IOUtils.hpp"

/**
 * Megjegyzés:
 * A tesztesetek jelenleg statikusak, olyan értelemben, hogy még nem fájlból olvasódnak be a tesztadatok,
 * hanem a tesztekbe vannak "égetve". Azonban ez csak egyelőre van így.
 *
 * Verziókövetés:
 * https://github.com/lolfail/titkositas-nhf
 */

int main() {
  std::ifstream stream;
  stream.open("default_tests.txt");
  TestCaseResource testcase_file(stream, ", ", "\n");

  testing::indexing::call_tests(testcase_file);
  testing::assignation::call_tests(testcase_file);
  testing::concatenation::call_tests(testcase_file);
  testing::logic::call_tests(testcase_file);
  testing::iterator::call_tests(testcase_file);

  return 0;
}
