#include <stdlib.h>
#include <sqlite3.h>
#include <time.h>
#include <fstream>
#include <ctime>
#include <unistd.h>
#include "Timestamp.h"

using namespace std;

//To compile: g++ String.h Student.h Timestamp.h String.cpp Student.cpp Timestamp.cpp attendance.cpp -lsqlite3 -o attendance

//simple class for temporary storage of entry data
class TempEntry {
private:
  int _studentID;
  int _rawTime;
  String _date;
public:
  TempEntry() {_studentID = 0; _rawTime = 0; _date = "";}
  ~TempEntry() {}
  int getStudentID() {return _studentID;}
  void setStudentID(int studentID) {_studentID = studentID;}
  int getRawTime() {return _rawTime;}
  void setRawTime(int rawTime) {_rawTime = rawTime;}
  String getDate() {return _date;}
  void setDate(String date) {_date = date;}
};

//simple struct for link list of entry data
struct Node {
  TempEntry* data;
  Node* next;
};

//signature
void cleanup(); //cleanup databases and linked list
void entry(int studentID); //mark entry of student in entryList
void exitt(int studentID); //compile student's attendance and remove entry from entryList
void insertTimestamp(TempEntry* entry); //prepare a Timestamp object and insert it into database
int prepareStudent(void* data, int numOfEntries, char** entry, char** column); //prepare student object from entries returned from SQL search
void logWrite(int severityLvl, String logMsg); //write log entries to "logFile.txt"
void setup(); //setup databases
Node* stored(int studentID); //return node of tempEntry of given student from entryList if it exists

//global variables
sqlite3* db1;
sqlite3* db2;
Node* entryList = NULL;
int resultCounter = 0;
Student* students = NULL;
ofstream outfile;

//cleanup database and linked list
void cleanup() {
  Node* cleaner = entryList;
  while (entryList != NULL) {
    entryList = entryList->next;
    delete cleaner->data;
    delete cleaner;
    cleaner = entryList;
  }
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

//mark entry of student in entryList
void entry(int studentID) {
  if (!stored(studentID)) {
    time_t rawTime = time(0);
    struct tm* timeTM = localtime(&rawTime);
    int y = timeTM->tm_year + 1900;
    int m = timeTM->tm_mon + 1;
    int d = timeTM->tm_mday;
    int y1 = y % 10;
    int y2 = y % 100 / 10;
    int m1 = m % 10;
    int m2 = m % 100 / 10;
    int d1 = d % 10;
    int d2 = d % 100 / 10;
    Node* temp = new Node;
    temp->next = entryList;
    entryList = temp;
    temp->data = new TempEntry;
    String string;
    temp->data->setStudentID(studentID);
    temp->data->setRawTime(rawTime);
    temp->data->setDate(string.intToString(d2) + string.intToString(d1)
     + "/" + string.intToString(m2) + string.intToString(m1) + "/"
      + string.intToString(y2) + string.intToString(y1));
  }
  else {
    String error = "error: entry not marked because student ";
    String error1 = "\'s entrance has already been marked";
    String string;
    logWrite(4, error + string.intToString(studentID) + error1 + "\r\n");
  }
}

//compile student's attendance and remove entry from entryList
void exitt(int studentID) {
  Node* entryNode = stored(studentID);
  if (entryNode) {
    insertTimestamp(entryNode->data);
    if (entryNode == entryList) {
      entryList = entryList->next;
    }
    else {
      Node* traverse = entryList;
      while (traverse->next != entryNode) {
        traverse = traverse->next;
      }
      traverse->next = traverse->next->next;
      traverse = NULL;
    }
    delete entryNode->data;
    delete entryNode;
    entryNode = NULL;
  }
  else {
    String string;
    logWrite(4, "error: entry not marked because student " + string.intToString(studentID)
     + "'s entrance has no marked entrance\r\n");
  }
}

//prepare a Timestamp object and insert it into database
void insertTimestamp(TempEntry* entry) {
  time_t rawTime = time(0);
  String date = entry->getDate();
  int duration = rawTime - entry->getRawTime();
  String string;
  String sql1 = "SELECT * FROM FingerPrint WHERE StudentID = " + string.intToString(entry->getStudentID()) + ";";
  char* errorMessage1 = NULL;
  const char* data = "Callback function called";
  sqlite3_exec(db2, sql1.getChPtr(), prepareStudent, (void*)data, &errorMessage1);
  if (!resultCounter) {
    String error = "StudentID given does not correspond to any student in FingerPrint database";
    logWrite(4, error + "\r\n");
    //cerr << "StudentID given does not correspond to any student in FingerPrint database" << endl;
  }
  else {
    resultCounter = 0;
    Timestamp timestamp(date, duration, students[0]);
    String sql2 = "INSERT INTO Timestamp(Date, Duration, StudentID, FirstName, LastName, Username) VALUES('"
     + date + "', " + string.intToString(duration) + ", " + string.intToString(timestamp.getStudentID()) + ", '"
      + timestamp.getFirstName() + "', '" + timestamp.getLastName() + "', '" + timestamp.getUsername() + "');";
    char* errorMessage2 = NULL;
    sqlite3_exec(db1, sql2.getChPtr(), NULL, NULL, &errorMessage2);
    String string;
    logWrite(2, "\nTimestamp " + string.intToString(timestamp.getStudentID()) + " inserted\r\n");
    if (errorMessage2) {
      String error = "error: ";
      logWrite(4, error + errorMessage2 + "\r\n");    }
    sqlite3_free(errorMessage2);
  }
  if (errorMessage1) {
    String error = "error: ";
    logWrite(4, error + errorMessage1 + "\r\n");  }
  sqlite3_free(errorMessage1);
}

//setup database and logfile
void setup () {
  outfile.open("logFile.txt", ios::out);
  outfile << "***START OF LOG***\r\n";
  if (sqlite3_open("Timestamp.db", &db1)) {
    logWrite(5, "Cannot open Timestamp database\r\n");
  }
  else{
    logWrite(2, "Connected to Timestamp database\r\n");
  }
  String sql = "CREATE TABLE IF NOT EXISTS Timestamp(Date TEXT, Duration INT, StudentID INTEGER, FirstName TEXT, LastName TEXT, Username TEXT);";
  char* errorMessage = NULL;
  sqlite3_exec(db1, sql.getChPtr(), NULL, NULL, &errorMessage);
  if (errorMessage) {
    String error = "error: ";
    logWrite(4, error + errorMessage + "\r\n");
  }
  logWrite(2, "Table successfully created\r\n");
  sqlite3_free(errorMessage);
  if (sqlite3_open("FingerPrint.db", &db2)) {
    logWrite(5, "Cannot open FingerPrint database\r\n");
  }
  else {
    logWrite(2, "Connected to FingerPrint database\r\n");
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
      default:
        break;
    }
  time_t t = time(0);
  struct tm * now = localtime( & t );
  outfile << "[" << now->tm_mday << '-'<< (now->tm_mon + 1) << '-' << (now->tm_year + 1900)
   << ", " << now->tm_hour << ':' << now->tm_min << ':' << now->tm_sec << "] (" << level << ") : " << logMsg;
}

//prepare student object from entries returned from SQL search
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

//return tempEntry of given student from temporary storage if it exists
Node* stored(int studentID) {
  Node* traverse = entryList;
  while (traverse != NULL) {
    if (traverse->data->getStudentID() == studentID) {
      return traverse;
    }
    traverse = traverse->next;
  }
  return NULL;
}

int main(int argc, char const *argv[]) {
  system("clear");
  setup();
  int studentID1 = 20743126;
  int studentID2 = 20713261;
  int studentID3 = 20702904;
  int studentID4 = 20719725;
  entry(studentID1);
  entry(studentID2);
  entry(studentID3);
  entry(studentID4);
  entry(studentID3);
  entry(studentID4);
  sleep(1);
  exitt(studentID1);
  sleep(4);
  exitt(studentID2);
  sleep(1);
  exitt(studentID3);
  sleep(2);
  exitt(studentID4);
  exitt(studentID4);
  exitt(studentID4);
  cleanup();
  return 0;
}
