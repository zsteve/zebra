#ifndef ZERROR_H
#define ZERROR_H

#include <string>

using namespace std;

class ZError{
    private:
    string buffer;
    int flushType;
    public:
    enum flush{FLUSH_IMMEDIATE, FLUSH_ONDEMAND};
	enum errorLevel{ERROR_FATAL, ERROR_NONFATAL};
    ZError() {buffer=""; flushType=FLUSH_IMMEDIATE;}
    ~ZError(){}
    void addError(string errorMsg);
	void addError(string errorMsg, int errorLevel);
    void setBufferFlush(int val);
    void flushBuffer();
};

#endif
