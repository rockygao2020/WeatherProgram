#ifndef NEXTDAY_H
#define NEXTDAY_H

#include <string>
#include <ctime>
using namespace std;

#pragma warning(disable : 4996)

class NextDay {
public:
	//constructor
	NextDay(time_t date = time(NULL), double maxTemp = 0.0, double minTemp = 0.0, string avgWeather = "");
	//destructor
	~NextDay();

	//accessors
	time_t getDate() const;
	string getStrDate() const;
	string getDay() const;
	double getMaxTemp() const;
	double getMinTemp() const;
	string getAvgWeather() const;


	//mutators
	void setDate(time_t date);
	void setMaxTemp(double maxTemp);
	void setMinTemp(double minTemp);
	void setAvgWeather(string avgWeather);


private:
	time_t date;
	double maxTemp;				//NOTE - this data comes from the 5-day forecast, not the current forecast
	double minTemp;				//NOTE - this data comes from the 5-day forecast, not the current forecast
	string avgWeather;			//NOTE - this data comes from the 5-day forecast, not the current forecast


};

#endif