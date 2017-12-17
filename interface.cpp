#include <stdlib.h>
#include <sqlite3.h>
#include <time.h>
#include <fstream>
#include <ctime>
#include "Timestamp.h"

using namespace std;

//To compile: g++ String.h Student.h Timestamp.h String.cpp Student.cpp Timestamp.cpp interface.cpp -lsqlite3 -o interface

//signature
void attMenu(); //attendance menu
void cleanup(); //cleanup database and log file
void deleteHelper(String criteria); //delete student from database based on given criteria
void deleteStudent(); // delete menu
void editorMenu(); //FingerPrint database editor menu
void insertHelper(Student student); //insert student into database
void insertStudent(); //insert menu
void logWrite(int severityLvl, String level); //write log entries to "logFile.txt"
int prepareStudent(void* data, int numOfEntries, char** entry, char** column); //prepare a student object from entries returned from SQL search
int prepareTimestamp(void* data, int numOfEntries, char** entry, char** column); //prepare a timestamp object from entries returned from SQL search
void menu(); //main menu
void searchHelper(String criteria); //search database for student based on given criteria
void searchStudent(); //search menu
void setup(); //setup database and logfile
bool stringToInt(String string, int& value); //converts string to int if possible
void view(String criteria); //view attendance logs based on given criteria

//global variables
sqlite3* db1;
sqlite3* db2;
int resultCounter = 0;
Student* students = NULL;
Timestamp* timestamps = NULL;
ofstream outfile;

//attendance menu
void attMenu() {
  system("clear");
  cout << "ATTENDANCE LOG VIEWER\n" << endl;
  cout << "1. View a student's attendance 'log' on a given day" << endl;
  cout << "2. View all attendance logs of a given 'student'" << endl;
  cout << "3. View all attendance logs on a given 'day'" << endl;
  cout << "4. View 'all' attendance logs'" << endl;
  cout << "5.'Return' to main 'menu'\n" << endl;
  String choice;
  cin >> choice;
  bool done = false;
  while (!done) {
    if (choice == "1" || choice == "Log" || choice == "log") {
      String criteria = "Both";
      view(criteria);
      done = true;
    }
    else if (choice == "2" || choice ==  "Student" || choice == "student") {
      String criteria = "Student";
      view(criteria);
      done = true;
    }
    else if (choice == "3" || choice ==  "Day" || choice == "day") {
      String criteria = "Day";
      view(criteria);
      done = true;
    }
    else if (choice == "4" || choice ==  "All" || choice == "all") {
      String criteria = "None";
      view(criteria);
      done = true;
    }
    else if (choice == "5" || choice == "Return" || choice == "return" || choice == "Menu" || choice == "menu") {
      menu();
      done = true;
    }
    else {
      cout << "\nInvalid command, try again" << endl;
      logWrite(0, "Invalid command input by user\r\n");
      cin >> choice;
    }
  }
  cout << "\nWould you like to view other attendance logs? (yes/no)" << endl;
  cin >> choice;
  done = false;
  while (!done) {
    if (choice == "N" || choice == "n" || choice == "No" || choice == "no") {
      menu();
      done = true;
    }
    else if (choice == "Y" || choice == "y" || choice == "Yes" || choice == "yes") {
      attMenu();
      done = true;
    }
    else {
      cout << "\nInvalid command, try again" << endl;
      logWrite(0, "Invalid command input by user\r\n");
      cin >> choice;
    }
  }
}

//cleanup database
void cleanup() {
  if (sqlite3_close(db1)) {
    logWrite(3, "Disconnection failed\r\n");
  }
  logWrite(2, "Disconnected from FingerPrint database\r\n");

  if (sqlite3_close(db2)) {
    logWrite(3, "Disconnection failed\r\n");
  }
  logWrite(2, "Disconnected from Timestamp database\r\n");
  outfile << "END OF LOG FILE.\r\n";
  outfile.close();
}

void deleteHelper(String criteria) {
  system("clear");
  cout << "FINGERPRINT DATABASE EDITOR\n" << endl;
  cout << "STUDENT DELETION\n" << endl;
  cout << "What is the '" + criteria + "' of the student you wish to delete from the database?" << endl;
  String value;
  cin >> value;
  String sql = "SELECT * FROM FingerPrint WHERE " + criteria + " = '" + value + "';";
  char* errorMessage = NULL;
  const char* data = "Callback function called";
  sqlite3_exec(db1, sql.getChPtr(), prepareStudent, (void*)data, &errorMessage);
  if (errorMessage) {
    String error = "error: ";
    logWrite(4, error + errorMessage + "\r\n");
  }
  system("clear");
  cout << "FINGERPRINT DATABASE EDITOR\n" << endl;
  cout << "STUDENT DELETION\n" << endl;
  if (!resultCounter) {
    cout << "\nNo student with the '" + criteria + "' '" + value + "' is currently in the database, perhaps the student has already been deleted" << endl;
    logWrite(2, "Record for deletion not found, for: " + criteria + "' '" + value + "\r\n");
  }
  else {
    for (int i = 0; i < resultCounter; i++) {
      cout << "\nStudent " << (i + 1) << endl;
      cout << students[i] << endl;
    }
    cout << "\nAre you sure you want to delete the above student from the database? (yes/no)" << endl;
    String choice;
    cin >> choice;
    bool done = false;
    while (!done) {
      if (choice == "N" || choice == "n" || choice == "No" || choice == "no") {
        cout << "\nDeletion cancelled" << endl;
        done = true;
        logWrite(2, "Record for deletion found but NOT deleted for: " + criteria + "' '" + value + "\r\n");
      }
      else if (choice == "Y" || choice == "y" || choice == "Yes" || choice == "yes") {
        sql = "DELETE FROM FingerPrint WHERE " + criteria + " = '" + value + "';";
        char* errorMessage = NULL;
        sqlite3_exec(db1, sql.getChPtr(), NULL, NULL, &errorMessage);
        cout << "\nStudent deleted" << endl;
        if (errorMessage) {
          String error = "error: ";
          logWrite(4, error + errorMessage + "\r\n");
        }
        logWrite(2, "Record for deletion found AND deleted for: " + criteria + "' '" + value + "\r\n");
        sqlite3_free(errorMessage);
        done = true;
      }
      else {
        cout << "\nInvalid command, try again" << endl;
        logWrite(0, "Invalid command input by user\r\n");
        cin >> choice;
      }
    }
  }
  resultCounter = 0;
  //cerr << "Error: " << errorMessage << endl;
  sqlite3_free(errorMessage);
}

//delete student from database
void deleteStudent() {
  system("clear");
  cout << "FINGERPRINT DATABASE EDITOR\n" << endl;
  cout << "STUDENT DELETION\n" << endl;
  cout << "1. Delete by 'StudentID'" << endl;
  cout << "2. Delete by 'Username'\n" << endl;
  String choice;
  cin >> choice;
  bool done = false;
  while (!done) {
    if (choice == "1" || choice == "StudentID" || choice == "studentID") {
      String criteria = "StudentID";
      deleteHelper(criteria);
      done = true;
    }
    else if (choice == "2" || choice == "Username" || choice == "UserName" || choice == "username") {
      String criteria = "Username";
      deleteHelper(criteria);
      done = true;
    }
    else {
      cout << "\nInvalid command, try again" << endl;
      logWrite(0, "Invalid command input by user\r\n");
      cin >> choice;
    }
  }
  cout << "\nWould you like to delete another student? (yes/no)" << endl;
  cin >> choice;
  done = false;
  while (!done) {
    if (choice == "N" || choice == "n" || choice == "No" || choice == "no") {
      editorMenu();
      done = true;
    }
    else if (choice == "Y" || choice == "y" || choice == "Yes" || choice == "yes") {
      deleteStudent();
      done = true;
    }
    else {
      cout << "\nInvalid command, try again" << endl;
      logWrite(0, "Invalid command input by user\r\n");
      cin >> choice;
    }
  }
}

//FingerPrint database editor menu
void editorMenu() {
  system("clear");
  cout << "FINGERPRINT DATABASE EDITOR\n" << endl;
  cout << "1.'Insert' a new student into the database" << endl;
  cout << "2.'Delete' a student from the database" << endl;
  cout << "3.'Search' for a student in the database" << endl;
  cout << "4.'Return' to main 'menu'\n" << endl;
  String choice;
  cin >> choice;
  bool done = false;
  while (!done) {
    if (choice == "1" || choice == "Insert" || choice == "insert") {
      insertStudent();
      done = true;
    }
    else if (choice == "2" || choice ==  "Delete" || choice == "delete") {
      deleteStudent();
      done = true;
    }
    else if (choice == "3" || choice ==  "Search" || choice == "search") {
      searchStudent();
      done = true;
    }
    else if (choice == "4" || choice == "Return" || choice == "return" || choice == "Menu" || choice == "menu") {
      menu();
      done = true;
    }
    else {
      cout << "\nInvalid command, try again" << endl;
      logWrite(0, "Invalid command input by user\r\n");

      cin >> choice;
    }
  }
}

//insert student into database
void insertHelper(Student student) {
  String string;
  String sql = "INSERT INTO FingerPrint(StudentID, FirstName, LastName, Username) VALUES("
   + string.intToString(student.getStudentID()) + ", '" + student.getFirstName() + "', '"
    + student.getLastName() + "', '" + student.getUsername() + "');";
  char* errorMessage = NULL;
  sqlite3_exec(db1, sql.getChPtr(), NULL, NULL, &errorMessage);
  cout << "\nStudent " << student.getStudentID() << " registered" << endl;
  if (errorMessage) {
    String error = "error: ";
    logWrite(4, error + errorMessage + "\r\n");
  }
  logWrite(2, "Record successfully inserted\r\n");
  sqlite3_free(errorMessage);
}

//insert student into database
void insertStudent() {
  system("clear");
  cout << "FINGERPRINT DATABASE EDITOR\n" << endl;
  cout << "STUDENT INSERTION\n" << endl;
  int studentID;
  String IDstring;
  cout << "Please enter userID" << endl; //temporary
  cin >> IDstring;
  while (IDstring.length() != 8 || !stringToInt(IDstring, studentID)) {
    cout << "\nInvalid input, must be an integer with exactly 8 digits, please review ID and try again" << endl;
    cin >> IDstring;
  }
  cout << "\nPlease enter first name (Note: avoid middle names)" << endl;
  String firstName;
  cin >> firstName;
  while (firstName.length() > 24) {
    cout << "\nFirst name exceeds character limit of 24, please consider entering an abbreviated version" << endl;
    cin >> firstName;
  }
  cout << "\nPlease enter last name" << endl;
  String lastName;
  cin >> lastName;
  while (lastName.length() > 24) {
    cout << "\nLast name exceeds character limit of 24, please consider entering an abbreviated version" << endl;
    cin >> lastName;
  }
  cout << "\nPlease enter username" << endl;
  String username;
  cin >> username;
  while (username.length() != 8) {
    cout << "\nusername must be exactly 8 characters, please review username and try again" << endl;
    cin >> username;
  }
  system("clear");
  cout << "FINGERPRINT DATABASE EDITOR\n" << endl;
  cout << "STUDENT INSERTION\n" << endl;
  Student student(studentID, firstName, lastName, username);
  cout << student << endl;
  cout << "Is the above information correct? (yes/no)" << endl;
  String choice;
  cin >> choice;
  bool done = false;
  while (!done) {
    if (choice == "N" || choice == "n" || choice == "No" || choice == "no") {
      insertStudent();
      done = true;
    }
    else if (choice == "Y" || choice == "y" || choice == "Yes" || choice == "yes") {
      insertHelper(student);
      done = true;
    }
    else {
      cout << "\nInvalid command, try again" << endl;
      logWrite(0, "Invalid command input by user\r\n");
      cin >> choice;
    }
  }
  cout << "\nWould you like to insert another student? (yes/no)" << endl;
  cin >> choice;
  done = false;
  while (!done) {
    if (choice == "N" || choice == "n" || choice == "No" || choice == "no") {
      editorMenu();
      done = true;
    }
    else if (choice == "Y" || choice == "y" || choice == "Yes" || choice == "yes") {
      insertStudent();
      done = true;
    }
    else {
      cout << "\nInvalid command, try again" << endl;
      logWrite(0, "Invalid command input by user\r\n");
      cin >> choice;
    }
  }
}

//write log entries to "logFile.txt"
void logWrite(int severityLvl, String logMsg) {
  String level;
    switch (severityLvl) {
      case 0:
        level = "TRACE";
        break;
      case 1:
        level = "DEBUG";
        break;
      case 2:
        level = "INFO";
        break;
      case 3:
        level = "WARNING";
        break;
      case 4:
        level = "ERROR";
        break;
      case 5:
        level = "FATAL";
        break;
    }
  time_t t = time(0);
  struct tm * now = localtime( & t );
  outfile << "[" << now->tm_mday << '-'<< (now->tm_mon + 1) << '-' << (now->tm_year + 1900)
   << ", " << now->tm_hour << ':' << now->tm_min << ':' << now->tm_sec << "] (" << level
    << ") : " << logMsg;
}

//main menu
void menu() {
  system("clear");
  cout << "MAIN MENU\n" << endl;
  cout << "1.'Edit' FingerPrint database" << endl;
  cout << "2.'View' student 'attendance' logs" << endl;
  cout << "3. Exit\n" << endl;
  String choice;
  cin >> choice;
  bool done = false;
  while (!done) {
    if (choice == "1" || choice == "Edit" || choice == "edit") {
      editorMenu();
      done = true;
    }
    else if (choice == "2" || choice ==  "Attendance" || choice == "attendance" || choice == "View" || choice == "view") {
      attMenu();
      done = true;
    }
    else if (choice == "3" || choice == "Exit" || choice == "exit" || choice == "Quit" || choice == "quit") {
      cout << "\nExiting" << endl;
      done = true;
    }
    else {
      cout << "\nInvalid command, try again" << endl;
      logWrite(0, "Invalid command input by user\r\n");
      cin >> choice;
    }
  }
}

//prepare a student object from entries returned from SQL search
int prepareStudent(void* data, int numOfEntries, char** entry, char** column) {
  if (numOfEntries < 4 || entry == NULL || column == NULL) {
    return -1;
  }
  int studentID = atoi(entry[0]);
  String firstName = entry[1];
  String lastName = entry[2];
  String username = entry[3];
  Student student(studentID, firstName, lastName, username);
  Student* temp;
  if (resultCounter) {
    temp = new Student[resultCounter];
    for (int i = 0; i < resultCounter; i++) {
      temp[i] = students[i];
    }
  }
  students = new Student[++resultCounter];
  for (int i = 0; i < resultCounter - 1; i++) {
    students[i] = temp[i];
  }
  students[resultCounter - 1] = student;
  return 0;
}

//prepare a timestamp object from entries returned from SQL search
int prepareTimestamp(void* data, int numOfEntries, char** entry, char** column) {
  if (numOfEntries < 4 || entry == NULL || column == NULL) {
    return -1;
  }
  String date = entry[0];
  int duration = atoi(entry[1]);
  int studentID = atoi(entry[2]);
  String firstName = entry[3];
  String lastName = entry[4];
  String username = entry[5];
  Student student(studentID, firstName, lastName, username);
  Timestamp timestamp(date, duration, student);
  Timestamp* temp;
  if (resultCounter) {
    temp = new Timestamp[resultCounter];
    for (int i = 0; i < resultCounter; i++) {
      temp[i] = timestamps[i];
    }
  }
  timestamps = new Timestamp[++resultCounter];
  for (int i = 0; i < resultCounter - 1; i++) {
    timestamps[i] = temp[i];
  }
  timestamps[resultCounter - 1] = timestamp;
  return 0;
}

//search database based on given criteria
void searchHelper(String criteria) {
  system("clear");
  cout << "FINGERPRINT DATABASE EDITOR\n" << endl;
  cout << "STUDENT SEARCH\n" << endl;
  cout << "What is the '" + criteria + "' of the student you are looking for?" << endl;
  String value;
  cin >> value;
  String sql = "SELECT * FROM FingerPrint WHERE " + criteria + " = '" + value + "';";
  char* errorMessage = NULL;
  const char* data = "Callback function called";
  sqlite3_exec(db1, sql.getChPtr(), prepareStudent, (void*)data, &errorMessage);
  system("clear");
  cout << "FINGERPRINT DATABASE EDITOR\n" << endl;
  cout << "SEARCH RESULTS\n" << endl;
  if (!resultCounter) {
    cout << "No student with the '" + criteria + "' '" + value + "' could be found in the FingerPrint database" << endl;
    logWrite(2, "Record not found\r\n");
  }
  else {
    for (int i = 0; i < resultCounter; i++) {
      cout << "Student " << (i + 1) << endl;
      cout << students[i] << endl;
    }
    cout << "All the students with the '" + criteria + "' '" + value + "' found in the database are listed above" << endl;
    logWrite(2, "Record successfully found\r\n");
  }
  resultCounter = 0;
  if (errorMessage) {
    String error = "error: ";
    logWrite(4, error + errorMessage + "\r\n");
  }
  sqlite3_free(errorMessage);
}

//search menu
void searchStudent() {
  system("clear");
  cout << "FINGERPRINT DATABASE EDITOR\n" << endl;
  cout << "STUDENT SEARCH\n" << endl;
  cout << "1. Search by 'FirstName'" << endl;
  cout << "2. Search by 'LastName'" << endl;
  cout << "3. Search by 'Username'\n" << endl;
  String choice;
  cin >> choice;
  bool done = false;
  while (!done) {
    if (choice == "1" || choice == "FirstName" || choice == "Firstname" || choice == "firstName" || choice == "firstname") {
      String criteria = "FirstName";
      searchHelper(criteria);
      done = true;
    }
    else if (choice == "2" || choice == "LastName" || choice == "Lastname" || choice == "lastName" || choice == "lastname") {
      String criteria = "LastName";
      searchHelper(criteria);
      done = true;
    }
    else if (choice == "3" || choice == "username" || choice == "username") {
      String criteria = "Username";
      searchHelper(criteria);
      done = true;
    }
    else {
      cout << "\nInvalid command, try again" << endl;
      logWrite(0, "Invalid command input by user\r\n");
      cin >> choice;
    }
  }
  cout << "\nWould you like to search for another student? (yes/no)" << endl;
  cin >> choice;
  done = false;
  while (!done) {
    if (choice == "N" || choice == "n" || choice == "No" || choice == "no") {
      editorMenu();
      done = true;
    }
    else if (choice == "Y" || choice == "y" || choice == "Yes" || choice == "yes") {
      searchStudent();
      done = true;
    }
    else {
      cout << "\nInvalid command, try again" << endl;
      logWrite(0, "Invalid command input by user\r\n");
      cin >> choice;
    }
  }
}

//setup database
void setup() {
  outfile.open("logFile.txt", ios::out);
  outfile << "***START OF LOG***\r\n";
  if (sqlite3_open("FingerPrint.db", &db1)) {
    logWrite(5, "Cannot open FingerPrint database\r\n");
  }
  else {
    logWrite(2, "Connected to FingerPrint database\r\n");
  }
  String sql = "CREATE TABLE IF NOT EXISTS FingerPrint(StudentID INTEGER, FirstName TEXT, LastName TEXT, Username TEXT);";
  char* errorMessage = NULL;
  sqlite3_exec(db1, sql.getChPtr(), NULL, NULL, &errorMessage);
  if (errorMessage) {
    String error = "error: ";
    logWrite(4, error + errorMessage + "\r\n");
  }
  logWrite(2, "Table successfully created\r\n");
  sqlite3_free(errorMessage);
  if (sqlite3_open("Timestamp.db", &db2)) {
    logWrite(5, "Cannot open Timestamp database\r\n");
  }
  else {
    logWrite(2, "Connected to Timestamp database\r\n");
  }
}

//converts string to int if possible
bool stringToInt(String string, int& value) {
  int i = 0;
  const char* input = string.getChPtr();
  value = 0;
  bool positive = true;
  if (input[0] == 43) {
    i++;
  }
  else if (input[0] == 45) {
    positive = false;
    i++;
  }
  while (i < string.length()) {
    if (input[i] > 47 && input[i] < 58) {
      if ((2147483647 - (input[i] - 48)) / 10 < value) {
        return false;
      }
      value *= 10;
      value += input[i] - 48;
    }
    else {
      return false;
    }
    i++;
  }
  if (!positive) {
    value *= -1;
  }
  return true;
}

//view attendance logs based on given criteria
void view(String criteria) {
  system("clear");
  cout << "ATTENDANCE LOG VIEWER\n" << endl;
  if (criteria == "Both") {
    cout << "Please enter the Student ID of the student you are looking for" << endl;
    String studentID;
    cin >> studentID;
    cout << "\nPlease enter the date you which to check in the form DD/MM/YY" << endl;
    String date;
    cin >> date;
    String sql = "SELECT * FROM Timestamp WHERE Date = '" + date + "' AND StudentID = " + studentID + ";";
    char* errorMessage = NULL;
    const char* data = "Callback function called";
    sqlite3_exec(db2, sql.getChPtr(), prepareTimestamp, (void*)data, &errorMessage);
    if (errorMessage) {
      String error = "error: ";
      logWrite(4, error + errorMessage + "\r\n");
    }
    system("clear");
    cout << "ATTENDANCE LOG VIEWER\n" << endl;
    if (!resultCounter) {
      cout << "Student " + studentID + " does not have any attendance logs on " + date << endl;
      logWrite(2, "No attendance log: " + studentID + ", " + date + "\r\n");
    }
    else {
      cout << "Student " + studentID + " attended:\n" << endl;
      for (int i = 0; i < resultCounter; i++) {
        cout << timestamps[i];
      }
      logWrite(2, "Attendance log found: " + studentID + ", " + date + "\r\n");
    }
  }
  else if (criteria == "Student") {
    cout << "Please enter the Student ID of the student you are looking for" << endl;
    String studentID;
    cin >> studentID;
    String sql = "SELECT * FROM Timestamp WHERE StudentID = " + studentID + ";";
    char* errorMessage = NULL;
    const char* data = "Callback function called";
    sqlite3_exec(db2, sql.getChPtr(), prepareTimestamp, (void*)data, &errorMessage);
    if (errorMessage) {
      String error = "error: ";
      logWrite(4, error + errorMessage + "\r\n");
    }
    system("clear");
    cout << "ATTENDANCE LOG VIEWER\n" << endl;
    if (!resultCounter) {
      cout << "Student " + studentID + " does not have any attendance logs" << endl;
      logWrite(2, "No attendance log: " + studentID + "\r\n");
    }
    else {
      cout << "Student " + studentID + " attended:\n" << endl;
      for (int i = 0; i < resultCounter; i++) {
        cout << timestamps[i];
      }
      logWrite(2, "Attendance log found: " + studentID + "\r\n");
    }
  }
  else if (criteria == "Day") {
    cout << "Please enter the date you which to check in the form DD/MM/YY" << endl;
    String date;
    cin >> date;
    String sql = "SELECT * FROM Timestamp WHERE Date = '" + date + "';";
    char* errorMessage = NULL;
    const char* data = "Callback function called";
    sqlite3_exec(db2, sql.getChPtr(), prepareTimestamp, (void*)data, &errorMessage);
    if (errorMessage) {
      String error = "error: ";
      logWrite(4, error + errorMessage + "\r\n");
    }
    system("clear");
    cout << "ATTENDANCE LOG VIEWER\n" << endl;
    if (!resultCounter) {
      cout << "There are no attendance logs on " + date << endl;
      logWrite(2, "No attendance log: " + date + "\r\n");
    }
    else {
      cout << "Attendance logs on " + date + ":\n" << endl;
      for (int i = 0; i < resultCounter; i++) {
        cout << timestamps[i].getStudentID() << " " << timestamps[i];
      }
      logWrite(2, "Attendance log found: " + date + "\r\n");
    }
  }
  else if (criteria == "None") {
    String sql = "SELECT * FROM Timestamp;";
    char* errorMessage = NULL;
    const char* data = "Callback function called";
    sqlite3_exec(db2, sql.getChPtr(), prepareTimestamp, (void*)data, &errorMessage);
    if (errorMessage) {
      String error = "error: ";
      logWrite(4, error + errorMessage + "\r\n");
    }
    system("clear");
    cout << "ATTENDANCE LOG VIEWER\n" << endl;
    if (!resultCounter) {
      cout << "There are currently no attendance logs" << endl;
      logWrite(2, "General view: no attendance logs in database\r\n");
    }
    else {
      cout << "All attendance logs to date:\n" << endl;
      for (int i = 0; i < resultCounter; i++) {
        cout << timestamps[i].getStudentID() << " " << timestamps[i];
      }
      logWrite(2, "General view: All attendance logs in database shown\r\n");
    }
  }
  resultCounter = 0;
}

int main(int argc, char const *argv[]) {
  system("clear");
  setup();
  menu();
  cleanup();
  return 0;
}
