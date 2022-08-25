#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

int main() {
  std::ifstream f("data.json");
  json questions = json::parse(f);

  int qCounter = 0;
  int lives = 5;
  int score = 0;
  int choice;
  for (qCounter = 0; qCounter < questions.size(); qCounter++) {
    json currQues = questions[qCounter];
    json options = currQues["options"];
    cout << "\n\n" << qCounter + 1 << "). " << currQues["q"] << endl;
    cout << "Options:\n";
    for (int i = 0; i < options.size(); i++) {
      auto op = options[i];
      cout << i + 1 << ") " << op << endl;
    }
  getOptionInput:
    cout << "\nEnter your choice: ";
    cin >> choice;
    if (choice > options.size()) {
      cout << "Invalid option!\nPlease try again";
      goto getOptionInput;
    }
    auto correctAns = currQues["a"];
    auto chosenAns = options[choice - 1];
    if (chosenAns == correctAns) {
      score += 5;
      cout << "Right Answer!\nScore = " << score << endl;
    } else {
      cout << "Wrong Answer!\nCorrect answer is: " << correctAns;
      cout << "\nLives left = " << --lives;
    }
  }
}