#include <iostream>
using namespace std;

class RespParser {
  string status; // init,finish,parsing,error
  string message;
  string decode();
  void reset();
  void feed(string str);
  string status();
  void getMessage();
};

void RespParser::reset() {
  status = "init";
  message.clear()
}

string RespParser::status() { return status; }
void RespParser::feed() {}
// bool RespParser::isEnd(string str) {
//   string::iterator it;
//   for (it = str.begin(); it != str.end(); it++) {
//     if ('\n' == it && '\r' == subStr.back()) {
//       subStr.pop_back();
//       val += this->decode(subStr);
//       subStr.clear();
//     }
//     subStr.push_back(it);
//   }
// }
