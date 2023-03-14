#ifndef WEATHERTYPE_H
#define WEATHERTYPE_H

#include <string>
#include <ctime>
using namespace std;


class WeatherType {
public:
	//constructor
	WeatherType(string weather = "", int frequency = 0);

	//destructor
	~WeatherType();

	//accessors
	string getWeather() const;
	int getFrequency() const;
	

	//mutators
	void setWeather(string weather);
	void setFrequency(int frequency);
	void increment();

	//overloaded comparison operators
	bool operator==(const WeatherType& other) const;
	bool operator<(const WeatherType& other) const;
	bool operator>(const WeatherType& other) const;
	

private:
	string weather;
	int frequency;


};

#endif
