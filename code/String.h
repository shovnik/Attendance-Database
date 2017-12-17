#include <iostream>

using namespace std;

class String {
private:
  int _length;
  char* _string;
public:
  String();
  String(const char* input);
  ~String();
  int length();
  const char* getChPtr();
  String intToString (int integer);
  String& operator=(const String& input);
  bool operator==(const String& input);
  bool operator!=(const String& input);
  String operator+(const String& input);
  String operator+(const char* input);
  friend String operator+(const char* input, const String& str);
  friend ostream& operator<<(ostream& out, const String& output);
  friend istream& operator>>(istream& in, String& input);
};
