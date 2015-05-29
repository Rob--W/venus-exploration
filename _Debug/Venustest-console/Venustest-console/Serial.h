#pragma once
#include <iostream>
#include <string>

using namespace std;


class _Serial
{
public:
	_Serial();
	~_Serial();

	void print(string);
	void print(double);
	void println(string);
	void println(double);
	void begin(int address);

};

void delay(int);