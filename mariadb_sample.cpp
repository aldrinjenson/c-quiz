// Includes
#include <iostream>
#include <mariadb/conncpp.hpp>
// Function to Add Contact
void addContact(std::shared_ptr<sql::PreparedStatement> &stmnt,
                sql::SQLString first_name, sql::SQLString last_name,
                sql::SQLString email) {
  try {
    // Bind Values
    stmnt->setString(1, first_name);
    stmnt->setString(2, last_name);
    stmnt->setString(3, email);
    // Execute Statement
    stmnt->execute();
  }
  // Handle Exceptions
  catch (sql::SQLException &e) {
    std::cerr << "Error adding contact to database: " << e.what() << std::endl;
  }
}
// Main Process
int main(int argc, char **argv) {
  try {
    // Instantiate Driver
    sql::Driver *driver = sql::mariadb::get_driver_instance();
    // Configure Connection
    sql::SQLString url("jdbc:mariadb://localhost:3306/test");
    sql::Properties properties(
        {{"user", "db_user"}, {"password", "db_user_password"}});
    // Establish Connection
    std::unique_ptr<sql::Connection> conn(driver->connect(url, properties));
    std::shared_ptr<sql::PreparedStatement> stmnt(
        conn->prepareStatement("INSERT INTO test.contacts(first_name, "
                               "last_name, email) VALUES (?, ?, ?)"));

    // Use prepared statement to add data
    addContact(stmnt, "John", "Smith", "john.smith@example.com");

    // Close Connection
    conn->close();
  }
  // Catch Exceptions
  catch (sql::SQLException &e) {
    std::cerr << "Error Connecting to MariaDB Platform: " << e.what()
              << std::endl;
    return 1;
  }
  return 0;
}