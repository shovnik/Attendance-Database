#include "Student.h"

using namespace std;

struct Time {
  int hours;
  int minutes;
  int seconds;
};

class Timestamp {
private:
  Student _student;
  int _duration;
  String _date;
  Time _time;

public:
  Timestamp();
  Timestamp(String date, int duration, Student student);
  ~Timestamp();
  String getDate();
  int getDuration();
  int getStudentID();
  String getFirstName();
  String getLastName();
  String getUsername();
  Timestamp& operator=(const Timestamp timestamp);
  friend ostream& operator<<(ostream& out, const Timestamp& output);
};
