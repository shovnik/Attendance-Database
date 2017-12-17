#include "Timestamp.h"

Timestamp::Timestamp() {
  _duration = 0;
  _time.hours = 0;
  _time.minutes = 0;
  _time.seconds = 0;
}

Timestamp::Timestamp(String date, int duration, Student student) {
  _student = student;
  _duration = duration;
  _date = date;
  _time.hours = duration / 3600;
  _time.minutes = (duration - _time.hours * 3600) / 60;
  _time.seconds = duration - _time.hours * 3600 - _time.minutes * 60;
}

Timestamp::~Timestamp() {
}

String Timestamp::getDate() {
  return _date;
}

int Timestamp::getDuration() {
  return _duration;
}

int Timestamp::getStudentID() {
  return _student.getStudentID();
}

String Timestamp::getFirstName() {
  return _student.getFirstName();
}

String Timestamp::getLastName() {
  return _student.getLastName();
}

String Timestamp::getUsername() {
  return _student.getUsername();
}

Timestamp& Timestamp::operator=(const Timestamp timestamp) {
  _date = timestamp._date;
  _duration = timestamp._duration;
  _student = timestamp._student;
  _time = timestamp._time;
  return *this;
}

ostream& operator<<(ostream& out, const Timestamp& output) {
  out << "[" << output._date << " for ";
  if (output._time.hours) {
    out << output._time.hours << " hours | ";
  }
  if (output._time.minutes) {
    out << output._time.minutes << " minutes | ";
  }
  out << output._time.seconds << " seconds]" << endl;
}
