#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPMessage.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/URI.h"
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/StreamCopier.h>
#include <Poco/String.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <mariadb/conncpp.hpp>
#include <mariadb/conncpp/SQLString.hpp>
#include <nlohmann/json.hpp>
#include <sql.h>
#include <string>
#include <vector>
#define MAX_LIVES 5

using json = nlohmann::json;
using namespace std;
using namespace Poco;
using namespace Poco::Net;

json getData(URI uri) {
  std::string path(uri.getPathAndQuery());
  if (path.empty())
    path = "/";

  try {
    const Context::Ptr context =
        new Context(Context::CLIENT_USE, "", "", "", Context::VERIFY_NONE, 9,
                    false, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");
    HTTPSClientSession session(uri.getHost(), uri.getPort(), context);
    HTTPRequest request(HTTPRequest::HTTP_GET, path, HTTPMessage::HTTP_1_1);
    HTTPResponse response;
    session.sendRequest(request);
    std::istream &rs = session.receiveResponse(response);
    auto parsedJson = json::parse(rs);
    return parsedJson;
  } catch (Poco::Exception &e) {
    cout << "Error in getting quiz data: " << e.displayText() << endl;
    cout << "Check your internet connection and try again.\nExiting..\n";
    exit(0);
    return {};
  }
}

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
  if (choiceCount > categoryLength) {
    cout << "Please enter a number between 1 and " << categoryLength << endl;
    goto getInput;
  }
  string chosenCategory = categoryArray[choiceCount - 1];
  replace(chosenCategory.begin(), chosenCategory.end(), ' ', '_');
  return chosenCategory;
}

sql::Driver *driver = sql::mariadb::get_driver_instance();
sql::SQLString url("jdbc:mariadb://localhost:3306/test");
sql::Properties properties({{"user", "db_user"},
                            {"password", "db_user_password"}});
std::unique_ptr<sql::Connection> conn(driver->connect(url, properties));

void addScoreToDb(string initialName, int score) {
  string playerName = initialName;
  printf("Enter name to be saved(default = %s, enter x to skip save): ",
         initialName.c_str());
  cin >> playerName;
  if (playerName == "x") {
    cout << "Skipping..\n";
    exit(0);
  }
  std::shared_ptr<sql::PreparedStatement> stmnt(
      conn->prepareStatement("INSERT INTO test.quizscores(name, "
                             "score) VALUES (?, ?)"));
  try {
    stmnt->setString(1, playerName);
    stmnt->setInt(2, score);
    stmnt->execute();
    cout << "Name and score saved to database" << endl;
  } catch (sql::SQLException &e) {
    std::cerr << "Error adding name and score to database: " << e.what()
              << std::endl;
  }
}

void showHighScores() {
  shared_ptr<sql::Statement> stmnt(conn->createStatement());
  int counter = 1;
  cout << "All time High scores:\n";
  try {
    std::unique_ptr<sql::ResultSet> res(stmnt->executeQuery(
        "SELECT name, score FROM test.quizscores order by score desc"));
    while (res->next()) {
      std::cout << counter++ << ". " << res->getString("name") << ": "
                << res->getString("score") << endl;
    }
  } catch (sql::SQLException &e) {
    std::cerr << "Error printing scores: " << e.what() << std::endl;
    exit(0);
  }
}

int main() {
  string playerName;
  cout << "Enter your name: ";
  cin >> playerName;
  cout << "Hello " << playerName << ", Welcome to cQuiz!\n\n";
  cout << "Loading categories available...\n";

  URI categoryUri("https://the-trivia-api.com/api/categories");
  json categoriesJson = getData(categoryUri);
  string preferredCategory = getPreferredCategory(categoriesJson);
  // string preferredCategory = "music";
  URI questionUri("https://the-trivia-api.com/api/"
                  "questions?categories=" +
                  preferredCategory +
                  "&limit=10&region=IN&"
                  "difficulty=easy");

  json questions = getData(questionUri);
  int qCounter = 0, lives = MAX_LIVES, score = 0, choice;

  // for (qCounter = 0; qCounter < questions.size() && lives > 0; qCounter++) {
  for (qCounter = 0; qCounter < 2 && lives > 0; qCounter++) {
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
    if (choice > options.size()) {
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
  conn->close();
}