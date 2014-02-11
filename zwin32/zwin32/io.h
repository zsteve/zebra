#ifndef IO_H
#define IO_H

#include <windows.h>
#include <list>
#include <string>

using namespace std;

class IO{
	// implemented as singleton class
private:
	static const int MAX_LINES = 100;

	static bool hasInstance;
	static IO* single;
	IO(){
		out.clear();
		in.clear();
		inputReady=false;
	}

	list<string> out;
	string in;

	bool inputReady;

public:
	static IO* getInstance();
	~IO(){
		hasInstance=false;
	}

	list<string>& getOutBuffer(){return out;}

	void print(char* s){
		int i=0;
		string str;
		do{
			if(s[i]=='\r' || s[i]=='\n'){
				// one line
				out.push_back(str);
				if(out.size()>MAX_LINES){
					out.pop_front();
				}
				str.clear();
			}
			str+=s[i];
		}while(s[++i]!=NULL);
		out.back()+=str;
	}

	void scan(char* in){
		while(!inputReady){
			Sleep(20);
		}
		strcpy(in, this->in.c_str());
		this->in.clear();
		inputReady=false;
	}

	bool has_input(){
		return !in.empty();
	}

	void put_in(char c){
		in+=c;
	}

	void unput_in(){
		in.pop_back();
	}

	void end_input(){
		inputReady=true;
	}

	void put_out(char c){
		out.back()+=c;
	}

	void unput_out(){
		out.back().pop_back();
	}
};

#endif