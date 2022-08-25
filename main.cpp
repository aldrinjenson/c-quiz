#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPMessage.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/URI.h"
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/StreamCopier.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#define MAX_LIVES 5

// https://the-trivia-api.com/api/categories
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
    std::cout << response.getStatus() << " " << response.getReason()
              << std::endl;

    // Poco::JSON::Object obj;
    auto parsedJson = json::parse(rs);
    return parsedJson;
  } catch (Poco::Exception &e) {
    cout << "Error in GET Request: " << e.displayText() << endl;
    return {};
  }
}

string getPreferredCategory(json categories) { return "sport_and_leisure"; }

int main() {

  URI categoryUri("https://the-trivia-api.com/api/categories");
  json categories = getData(categoryUri);
  cout << categories;
  string preferredCategory = getPreferredCategory(categories);
  // string preferredCategory = "sport_and_leisure";
  URI questionUri("https://the-trivia-api.com/api/"
                  "questions?categories=" +
                  preferredCategory +
                  "&limit=10&region=IN&"
                  "difficulty=easy");
  json questions = getData(questionUri);
  std::ifstream f("data.json");
  // json questions = json::parse(f);
  int qCounter = 0, lives = MAX_LIVES, score = 0, choice;
  string playerName;
  cout << "Categories: ";

  cout << "Enter your name: ";
  cin >> playerName;
  cout << "Hello " << playerName << ", Welcome to cQuiz!\n\n";
  for (qCounter = 0; qCounter < questions.size() && lives > 0; qCounter++) {
    json currQues = questions[qCounter];
    json options = currQues["incorrectAnswers"];
    auto correctAns = currQues["correctAnswer"];
    options.push_back(correctAns);
    sort(correctAns.begin(), correctAns.end());

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
      cout << "\nLives left = " << --lives;
    }
  }

  cout << "\n\nGame over!\n\tStatus report of " << playerName
       << "\nQuestions solved = " << qCounter
       << "\nLives used = " << MAX_LIVES - lives << "\nFinal Score = " << score
       << endl;
}