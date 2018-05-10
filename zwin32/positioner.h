#ifndef POSITIONER_H
#define POSITIONER_H

class Positioner{
public:
	int GetAbsPosX(double f);
	int GetAbsPosY(double f);

	int GetAbsWidth(double f);
	int GetAbsHeight(double f);

	void SetW(int w){sw=w;}
	void SetH(int h){sh=h;}
private:
	int sw, sh;
protected:
};

#define CENTER_X(xPos, w)\
	xPos-(w/2)

#define CENTER_Y(yPos, h)\
	yPos-(h/2)

#endif