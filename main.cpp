#include "Poco/URI.h"
#include "headers/dbFuncs.h"
#include "headers/requests.h"
#include "headers/utils.h"
#include <nlohmann/json.hpp>

#define MAX_LIVES 5

using json = nlohmann::json;
using Poco::URI;
using std::cin;
using std::cout;
using std::endl;
using std::string;

int main() {
  string playerName;
  cout << "Enter your name: ";
  cin >> playerName;
  cout << "Hello " << playerName << ", Welcome to cQuiz!\n\n";
  cout << "Loading categories available...\n";

  URI categoryUri("https://the-trivia-api.com/api/categories");
  json categoriesJson = getData(categoryUri);
  string preferredCategory = getPreferredCategory(categoriesJson);
  URI questionUri("https://the-trivia-api.com/api/"
                  "questions?categories=" +
                  preferredCategory +
                  "&limit=10&region=IN&"
                  "difficulty=easy");

  json questions = getData(questionUri);
  int qCounter = 0, lives = MAX_LIVES, score = 0, choice;

  for (qCounter = 0; qCounter < questions.size() && lives > 0; qCounter++) {
    // for (qCounter = 0; qCounter < 2 && lives > 0; qCounter++) {
    json currQues = questions[qCounter];
    json options = currQues["incorrectAnswers"];
    string correctAns = currQues["correctAnswer"];
    options.push_back(correctAns);
    sort(options.begin(), options.end());

    cout << "\nQ" << qCounter + 1 << ". " << currQues["question"] << endl;
    cout << "Options:\n";
    for (int i = 0; i < options.size(); i++) {
      auto op = options[i];
      cout << i + 1 << ") " << op << endl;
    }

  getOptionInput:
    cout << "\nEnter your choice: ";
    cin >> choice;
    if (!cin || choice > options.size()) {
      cin.clear(); // reset failbit
      cin.ignore(std::numeric_limits<std::streamsize>::max(),
                 '\n'); // skip bad input
      cout << "Invalid option!\nPlease try again";
      goto getOptionInput;
    }

    auto chosenAns = options[choice - 1];

    if (chosenAns == correctAns) {
      score += 5;
      cout << "Right Answer!\nScore = " << score << endl;
    } else {
      cout << "Wrong Answer!\nCorrect answer is: " << correctAns;
      cout << "\nLives left = " << --lives << endl;
    }
  }

  cout << "\n\nGame over!\n\tStatus report of " << playerName
       << "\nQuestions solved = " << qCounter
       << "\nLives used = " << MAX_LIVES - lives << "\nFinal Score = " << score
       << endl;
  addScoreToDb(playerName, score);
  showHighScores();
}