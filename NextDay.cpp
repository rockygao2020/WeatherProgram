#include "NextDay.h"

//constructor
NextDay::NextDay(time_t date, double maxTemp, double minTemp, string avgWeather) : date(date), maxTemp(maxTemp), minTemp(minTemp), avgWeather(avgWeather) {}
//destructor
NextDay::~NextDay() {}

//accessors
time_t NextDay::getDate() const {
	return date;
}
string NextDay::getStrDate() const {
	/*
	string strDate = std::asctime(std::localtime(&date)); //converts &time_t (date) into string ending w/ newline
	strDate.pop_back();									  //delete newline
	return strDate; */

	struct tm* timeinfo;
	char buffer[80];
	timeinfo = localtime(&date);

	strftime(buffer, 80, "%a %b %d", timeinfo);
	return buffer;
}
string NextDay::getDay() const {
	struct tm* timeinfo;
	char buffer[80];
	timeinfo = localtime(&date);

	strftime(buffer, 80, "%A", timeinfo);
	return buffer;
}
double NextDay::getMaxTemp() const {
	return maxTemp;
}
double NextDay::getMinTemp() const {
	return minTemp;
}
string NextDay::getAvgWeather() const {
	return avgWeather;
}


//mutators
void NextDay::setDate(time_t date) {
	this->date = date;
}
void NextDay::setMaxTemp(double maxTemp) {
	this->maxTemp = maxTemp;
}
void NextDay::setMinTemp(double minTemp) {
	this->minTemp = minTemp;
}
void NextDay::setAvgWeather(string avgWeather) {
	this->avgWeather = avgWeather;
}