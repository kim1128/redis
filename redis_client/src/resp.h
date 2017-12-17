#include <iostream>
#include <string>
#include <vector>
using namespace std;

enum RESPTYPE { SIMPLESTRING, BULKSTRING, INTEGER, ERROR, ARRAY, AUTO };

class RESPMsg {
public:
  virtual string encode() { return ""; };
  static RESPMsg *decode(int sfd);
  virtual string toString() { return ""; };
  virtual void show() { return; };
  virtual int length() { return 0; };
  virtual RESPMsg *list(int inidex) { return NULL; }
};

class RESPString : public RESPMsg {
private:
  string val;
  RESPTYPE type;

public:
  RESPString(string strVal, RESPTYPE type = AUTO);
  static RESPMsg *decode(int sfd, RESPTYPE type = AUTO);
  virtual string encode();
  virtual string toString();
  virtual void show();
};

class RESPError : public RESPString {
public:
  RESPError(string strVal) : RESPString(strVal, ERROR){};
};

class RESPBulkString : public RESPString {
public:
  RESPBulkString(string strVal) : RESPString(strVal, BULKSTRING){};
};

class RESPSimpleString : public RESPString {
public:
  RESPSimpleString(string strVal) : RESPString(strVal, SIMPLESTRING){};
};

class integer : public RESPMsg {
private:
  int val;

public:
  integer(int value);
  virtual string encode();
};

class array : public RESPMsg {
private:
  vector<RESPMsg *> val;

public:
  array(string *aval, int len);
  array(){};
  virtual string encode();
  static RESPMsg *decode(int sfd);
  virtual string toString();
  virtual int length();
  virtual RESPMsg *list(int i);
  virtual void show();
};

string respSendMsg(string argv[], int argc);
