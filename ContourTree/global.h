#ifndef GLOBAL_H
#define GLOBAL_H

typedef unsigned int uint;

//typedef float DataType; //data is stored like this
//#define ABS(x) fabs(x)

typedef unsigned char DataType;
//typedef unsigned short DataType;

inline bool compareLess( unsigned char a, unsigned char b ) { return a < b; }
//inline bool compareLess( unsigned short int a, unsigned short int b ) { return a < b; }
//inline bool compareLess( float a, float b ) { return a < b; }
//inline bool compareLess( double a, double b ) { return a < b; }

inline bool compareEqual( unsigned char a, unsigned char b ) { return a == b; }
//inline bool compareEqual( unsigned short int a, unsigned short int b ) { return a == b; }
//inline bool compareEqual( float a, float b ) { return a == b; }
//inline bool compareEqual( double a, double b ) { return a == b; }

#endif


