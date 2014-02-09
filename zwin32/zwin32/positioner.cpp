#include "positioner.h"

int Positioner::GetAbsPosX(double f){
	if(f>1.0){
		return this->sw;
	}
	return (int)((double)this->sw)*f;
}

int Positioner::GetAbsPosY(double f){
	if(f>1.0){
		return this->sh;
	}
	return (int)((double)this->sh)*f;
}

int Positioner::GetAbsWidth(double f){
	return GetAbsPosX(f);
}

int Positioner::GetAbsHeight(double f){
	return GetAbsPosY(f);
}