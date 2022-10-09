#include "../headers/utils.h"

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::vector;
using json = nlohmann::json;

string getPreferredCategory(json categories) {
  cout << "Choose category\n";
  vector<string> categoryArray;
  int categoryIndex = 0;

  for (auto &it : categories.items()) {
    json currCategory = it.key();
    cout << categoryIndex + 1 << ". " << currCategory << endl;
    categoryArray.push_back(currCategory);
    categoryIndex++;
  }

  int categoryLength = categoryArray.size();
  int choiceCount;

getInput:
  cout << "Enter your choice (1-" << categoryLength << "): ";
  cin >> choiceCount;
  if (!cin || choiceCount > categoryLength) {
    // to handle cases when characters other than numbers are entered
    cin.clear(); // reset failbit
    cin.ignore(std::numeric_limits<std::streamsize>::max(),
               '\n'); // skip bad input
    cout << "Please enter a number between 1 and " << categoryLength << endl;
    goto getInput;
  }
  
  string chosenCategory = categoryArray[choiceCount - 1];
  replace(chosenCategory.begin(), chosenCategory.end(), ' ', '_');
  return chosenCategory;
}
