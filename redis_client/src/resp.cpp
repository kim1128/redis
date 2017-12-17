#include <cstring>

#include <algorithm>
#include <sstream>

#include <unistd.h>

#include "resp.h"

using namespace std;

RESPString::RESPString(string strVal, RESPTYPE type) {
  this->val = strVal;
  this->type = type;
}

string RESPString::encode() {
  stringstream stream;
  switch (type) {
  case SIMPLESTRING:
    stream << "+" << val << "\r\n";
    break;
  case ERROR:
    stream << "-" << val << "\r\n";
    break;
  case BULKSTRING:
    stream << "$" << val.size() << "\r\n" << val << "\r\n";
    break;
  default:
    stream << "$-1\r\n";
  }
  return stream.str();
}

RESPMsg *RESPString::decode(int sfd, RESPTYPE type) {
  char buf[1024];
  string str;
  while (1) {
    read(sfd, &buf, 1);
    if ('\n' == buf[0] && '\r' == str.back()) {
      str.push_back(buf[0]);
      break;
    }
    str.push_back(buf[0]);
  }
  str.pop_back();
  str.pop_back();

  if (BULKSTRING == type) {
    int len = stoi(str);
    read(sfd, buf, len + 2);
    str = string(buf, len);
  }
  RESPString *respString = new RESPString(str, type);
  return respString;
}
string RESPString::toString() { return val; }
void RESPString::show() { cout << val << endl; }

integer::integer(int value) { val = value; }

string integer::encode() {
  stringstream stream;
  stream << ":" << val << "\r\n";
  return stream.str();
}

array::array(string *aval, int len) {
  for (int i = 0; i < len; i++)
    val.push_back(new RESPString(aval[i], BULKSTRING));
}

string array::encode() {
  stringstream stream;
  stream << "*" << val.size() << "\r\n";
  for (int i = 0; i < val.size(); i++) {
    stream << val[i]->encode();
  }
  return stream.str();
}

RESPMsg *array::decode(int sfd) {
  char buf;
  string str;
  while (1) {
    read(sfd, &buf, 1);
    if ('\n' == buf && '\r' == str.back()) {
      str.push_back(buf);
      break;
    }
    str.push_back(buf);
  }

  str.pop_back();
  str.pop_back();
  int len = stoi(str);

  array *arr = new array;
  for (int i = 0; i < len; i++) {
    arr->val.push_back(RESPMsg::decode(sfd));
  }
  return arr;
}

string array::toString() { throw string("no tostring method"); }

void array::show() {
  int len = this->length();
  for (int i = 0; i < len; i++) {
    RESPMsg *base2 = this->list(i);
    string str = base2->toString();
    cout << (i + 1) << ")\t" << str << endl;
  }
}
int array::length() { return val.size(); }

RESPMsg *array::list(int i) { return val.at(i); }

string respSendMsg(string argv[], int argc) {
  array arr(argv, argc);
  RESPMsg *base = &arr;
  string buf = base->encode();
  cout << buf << "\n";
  return buf;
}

RESPMsg *RESPMsg::decode(int sfd) {
  char buf;
  string val;
  RESPMsg *base;
  read(sfd, &buf, 1);

  if ('+' == buf) {
    return RESPString::decode(sfd, SIMPLESTRING);
  }
  if ('$' == buf) {
    return RESPString::decode(sfd, BULKSTRING);
  }
  if ('-' == buf) {
    return RESPString::decode(sfd, ERROR);
  }
  if ('*' == buf) {
    return array::decode(sfd);
  }
  return NULL;
}
