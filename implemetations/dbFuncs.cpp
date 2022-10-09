#include "../headers/dbFuncs.h"
#include <mariadb/conncpp.hpp>

using namespace std;

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
    conn->close();
  } catch (sql::SQLException &e) {
    std::cerr << "Error printing scores: " << e.what() << std::endl;
    exit(0);
  }
}
