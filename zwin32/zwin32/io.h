#ifndef IO_H
#define IO_H

#include <windows.h>
#include <list>
#include <string>

#include <iostream>

#include "../../num2ascii/num2ascii/num2ascii.h"

using namespace std;

class IO{
	// implemented as singleton class
private:

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

	static int MAX_ROWS;
	static int MAX_COLS;

	int nRows;

	static IO* getInstance();
	~IO(){
		hasInstance=false;
	}

	list<string>& getOutBuffer(){return out;}

	void print(char* s){
		int i=0;
		string str;
		do{
			if(str.size()>MAX_COLS){
				// exceeded max columns
				// we will have to clip...
				if(!out.empty()){
					out.back()+=str;
				}else{
					out.push_back(str);
				}
				out.push_back("");
				if(out.size()>=MAX_ROWS){
					while(out.size()>=MAX_ROWS){
						out.pop_front();
					}
				}
				str.clear();
				if(s[i]=='\r' || s[i]=='\n'){
					if(!out.empty()){
						out.back()+=str;
					}else{
						out.push_back(str);
					}
					out.push_back("");
					continue;
				}
			}
			if(s[i]=='\r' || s[i]=='\n'){
				// one line
				if(!out.empty()){
					out.back()+=str;
				}else{
					out.push_back(str);
				}
				out.push_back("");
				if(out.size()>=MAX_ROWS){
					while(out.size()>=MAX_ROWS){
						out.pop_front();
					}
				}
				str.clear();
				continue;
			}
			str+=s[i];
		}while(s[++i]);
		out.back()+=str;
		nRows=out.size();
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

	string& get_input(){
		return in;
	}

	std::list<string>& get_output(){
		return out;
	}
};

#endif
