#ifndef FORECAST_H
#define FORECAST_H

#include <string>
#include <ctime>
using namespace std;

class Forecast {
public:
	//constructor
	Forecast(time_t inputDate = time(NULL), double inputMaxTemp = 0.0, double inputMinTemp = 0.0, string inputWeather = "");
	
	//destructor
	~Forecast();

	//accessors
	time_t getDate() const;
	double getMaxTemp() const;
	double getMinTemp() const;
	string getWeather() const;

	//mutators
	void setDate(time_t date);
	void setMaxTemp(double maxTemp);
	void setMinTemp(double minTemp);
	void setWeather(string weather);

private:
	time_t date;
	double maxTemp;
	double minTemp;
	string weather;


};

#endif
