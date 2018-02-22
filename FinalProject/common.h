#ifndef __COMMON_H
#define __COMMON_H
#include<iostream>

using namespace std;

/*
* Here you can find all the common functionalities or data types neccessary
*/

enum TypeObject { INACTIVE, CUBE, CYLINDER, SCOPE, PLAIN, SOFA, TABLE, CABINET, CHAIR, TOY };

class vector3d {
private:
	// Variables
	float x;
	float y;
	float z;

public:
	// Methods
	vector3d();
	vector3d(float x, float y, float z); // Constructor
	vector3d copyVector();
	float getElement(int axis); // Get elements of a specific axis (X=0, Y=1, Z=2)
	void setElement(int axis, float value);
	void addElement(vector3d& that);
	float getX();
	float getY();
	float getZ();
	void setVector(vector3d& that);
};


#endif