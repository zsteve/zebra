#include "io.h"
#include <cstdlib>

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