#ifndef TODAY_H
#define TODAY_H

#include <string>
#include <ctime>
using namespace std;

#pragma warning(disable : 4996)

class Today {
public:
	//constructor
	Today(time_t date = time(NULL), double currTemp = 0.0, double maxTemp = 0.0, double minTemp = 0.0, string avgWeather = "", double windSpeed = 0.0, double windDir = 0.0, double humidity = 0.0);
	//destructor
	~Today();

	//accessors
	time_t getDate() const;
	string getStrDate() const;
	string getDay() const;
	double getCurrTemp() const;
	double getMaxTemp() const;
	double getMinTemp() const;
	string getAvgWeather() const;
	double getWindSpeed() const;
	double getWindDir() const;
	double getHumidity() const;
	

	//mutators
	void setDate(time_t date);
	void setCurrTemp(double currTemp);
	void setMaxTemp(double maxTemp);
	void setMinTemp(double minTemp);
	void setAvgWeather(string avgWeather);
	void setWindSpeed(double windSpeed);
	void setWindDir(double windDir);
	void setHumidity(double humidity);

private:
	time_t date;
	double currTemp;
	double maxTemp;				//NOTE - this data comes from the 5-day forecast, not the current forecast
	double minTemp;				//NOTE - this data comes from the 5-day forecast, not the current forecast
	string avgWeather;			//NOTE - this data comes from the 5-day forecast, not the current forecast
	double windSpeed;
	double windDir;
	double humidity;


};

#endif