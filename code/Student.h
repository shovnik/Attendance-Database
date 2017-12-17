#include "String.h"

using namespace std;

class Student {
private:
  int _studentID;
  String _firstName;
  String _lastName;
  String _username;

public:
  Student();
  Student(int studentID, String firstName, String lastName, String username);
  ~Student();
  int getStudentID();
  String getFirstName();
  String getLastName();
  String getUsername();
  Student& operator=(const Student student);
  friend ostream& operator<<(ostream& out, const Student& output);
};
