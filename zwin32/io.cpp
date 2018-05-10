#include "io.h"
#include <cstdlib>

int IO::MAX_ROWS = 100;
int IO::MAX_COLS = 100;

bool IO::hasInstance=false;
IO* IO::single = NULL;
IO* IO::getInstance(){
	if(!hasInstance){
		single= new IO();
		hasInstance=true;
		return single;
	}else{
		return single;
	}
}