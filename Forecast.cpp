#include "Forecast.h"

//constructor
Forecast::Forecast(time_t inputDate, double inputMaxTemp, double inputMinTemp, string inputWeather) : date(inputDate), maxTemp(inputMaxTemp), minTemp(inputMinTemp), weather(inputWeather) {}

//destructor
Forecast::~Forecast() {}

//accessors
time_t Forecast::getDate() const {
	return date;
}
double Forecast::getMaxTemp() const {
	return maxTemp;
}
double Forecast::getMinTemp() const {
	return minTemp;
}
string Forecast::getWeather() const {
	return weather;
}

//mutators
void Forecast::setDate(time_t date) {
	this->date = date;
}
void Forecast::setMaxTemp(double maxTemp) {
	this->maxTemp = maxTemp;
}
void Forecast::setMinTemp(double minTemp) {
	this->minTemp = minTemp;
}
void Forecast::setWeather(string weather) {
	this->weather = weather;
}