#include "Today.h"


//constructor
Today::Today(time_t date, double currTemp, double maxTemp, double minTemp, string avgWeather, double windSpeed, double windDir, double humidity) {
	this->date = date;
	this->currTemp = currTemp;
	this->maxTemp = maxTemp;
	this->minTemp = minTemp;
	this->avgWeather = avgWeather;
	this->windSpeed = windSpeed;
	this->windDir = windDir;
	this->humidity = humidity;

}

//destructor
Today::~Today() {

}

//accessors
time_t Today::getDate() const {
	return date;
}
string Today::getStrDate() const {
	/*
	string strDate = std::asctime(std::localtime(&date)); //converts &time_t (date) into string ending w/ newline
	strDate.pop_back();									  //delete newline
	return strDate; */

	struct tm *timeinfo;
	char buffer[80];
	timeinfo = localtime(&date);

	strftime(buffer, 80, "%a %b %d %r %G", timeinfo);
	return buffer;


}
string Today::getDay() const {
	struct tm* timeinfo;
	char buffer[80];
	timeinfo = localtime(&date);

	strftime(buffer, 80, "%A", timeinfo);
	return buffer;
}

double Today::getCurrTemp() const {
	return currTemp;
}
double Today::getMaxTemp() const {
	return maxTemp;
}
double Today::getMinTemp() const {
	return minTemp;
}
string Today::getAvgWeather() const {
	return avgWeather;
}
double Today::getWindSpeed() const {
	return windSpeed;
}
double Today::getWindDir() const {
	return windDir;
}
double Today::getHumidity() const {
	return humidity;
}


//mutators
void Today::setDate(time_t date) {
	this->date = date;
}
void Today::setCurrTemp(double currTemp) {
	this->currTemp = currTemp;
}
void Today::setMaxTemp(double maxTemp) {
	this->maxTemp = maxTemp;
}
void Today::setMinTemp(double minTemp) {
	this->minTemp = minTemp;
}
void Today::setAvgWeather(string avgWeather) {
	this->avgWeather = avgWeather;
}
void Today::setWindSpeed(double windSpeed) {
	this->windSpeed = windSpeed;
}
void Today::setWindDir(double windDir) {
	this->windDir = windDir;
}
void Today::setHumidity(double humidity) {
	this->humidity = humidity;
}