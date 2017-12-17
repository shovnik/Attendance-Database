#include "String.h"

String::String() {
  _length = 0;
  _string = NULL;
}

String::String(const char* input) {
  _length = 0;
  while (input[_length] != '\0') {
    _length++;
  }
   if (!_length)
       _string = NULL;
   else
       _string = new char[_length];

   for (int i = 0; i < _length; i++) {
     _string[i] = input[i];
   }
}

String::~String() {
}

int String::length() {
  return _length;
}

const char* String::getChPtr() {
  return _string;
}

String String::intToString (int integer) {
    int n = integer;
    if (integer == 0) {
      _length = 1;
      _string = new char[1];
      _string[0] = '0';
    }
    else {
      int digits = 0;
      char* temp = new char[10];
      while (n > 0) {
          temp[digits] = n % 10 + '0';
          n = (n - n % 10) / 10;
          digits++;
      }
      _length = digits;
      _string = new char[digits];
      for (int i = 0; i < digits; i++) {
          _string[i] = temp[digits - i - 1];
      }
      delete temp;
    }
    return *this;
}

String& String::operator=(const String& input) {
  if (_string) {
    delete[] _string;
  }
  _length = input._length;
  _string = new char[_length];
  for (int i = 0; i < _length; i++) {
    _string[i] = input._string[i];
  }
  return *this;
}

bool String::operator==(const String& input) {
  if (_length == input._length) {
    for (int i = 0; i < _length; i++) {
      if (_string[i] != input._string[i]) {
        return false;
      }
    }
    return true;
  }
  return false;
}

bool String::operator!=(const String& input) {
  if (_length == input._length) {
    for (int i = 0; i < _length; i++) {
      if (_string[i] != input._string[i]) {
        return true;
      }
    }
    return false;
  }
  return true;
}

String String::operator+(const String& input) {
  String* concatenated = new String;
  concatenated->_length = _length + input._length;
  concatenated->_string = new char[concatenated->_length];
  for (int i = 0; i < concatenated->_length; i++) {
    if (i < _length) {
      concatenated->_string[i] = _string[i];
    }
    else {
      concatenated->_string[i] = input._string[i - _length];
    }
  }
  return *concatenated;
}

String String::operator+(const char* input) {
  int inputLength = 0;
  while (input[inputLength] != '\0') {
    inputLength++;
  }
  String* concatenated = new String;
  concatenated->_length = _length + inputLength;
  concatenated->_string = new char[concatenated->_length];
  for (int i = 0; i < concatenated->_length; i++) {
    if (i < _length) {
      concatenated->_string[i] = _string[i];
    }
    else {
      concatenated->_string[i] = input[i - _length];
    }
  }
  return *concatenated;
}

ostream& operator<<(ostream& out, const String& output) {
  for (int i = 0; i < output._length; i++) {
    out << output._string[i];
  }
  return out;
}

istream& operator>>(istream& in, String& input) {
  char* temp = new char[1000];
  in >> temp;
  input._length = 0;
  while (input._length < 1000 && temp[input._length] != '\0') {
    input._length++;
  }
  input._string = new char[input._length];
  for (int i = 0; i < input._length; i++) {
    input._string[i] = temp[i];
  }
  delete temp;
  return in;
}

String operator+(const char* input, const String& str) {
  int inputLength = 0;
  while (input[inputLength] != '\0') {
    inputLength++;
  }
  String* concatenated = new String;
  concatenated->_length = inputLength + str._length;
  concatenated->_string = new char[concatenated->_length];
  for (int i = 0; i < concatenated->_length; i++) {
    if (i < inputLength) {
      concatenated->_string[i] = input[i];
    }
    else {
      concatenated->_string[i] = str._string[i - inputLength];
    }
  }
  return *concatenated;
}
