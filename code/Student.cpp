#include "Student.h"

Student::Student() {
  _studentID = 0;
  _firstName = "";
  _lastName = "";
  _username = "";
}

Student::Student(int studentID, String firstName, String lastName, String username) {
  _studentID = studentID;
  _firstName = firstName;
  _lastName = lastName;
  _username = username;
}

Student::~Student() {
}

int Student::getStudentID() {
  return _studentID;
}

String Student::getFirstName() {
  return _firstName;
}

String Student::getLastName() {
  return _lastName;
}

String Student::getUsername() {
  return _username;
}

Student& Student::operator=(const Student student) {
  _studentID = student._studentID;
  _firstName = student._firstName;
  _lastName = student._lastName;
  _username = student._username;
  return *this;
}

ostream& operator<<(ostream& out, const Student& output) {
  out << "Student ID: " << output._studentID << endl;
  out << "First Name: " << output._firstName << endl;
  out << "Last Name: " << output._lastName << endl;
  out << "Username: " << output._username << endl;
  return out;
}
