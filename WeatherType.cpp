#include "WeatherType.h"

//constructor
WeatherType::WeatherType(string weather, int frequency) : weather(weather), frequency(frequency) {}

//destructor
WeatherType::~WeatherType() {}

//accessors
string WeatherType::getWeather() const {
	return weather;
}
int WeatherType::getFrequency() const {
	return frequency;
}

//mutators
void WeatherType::setWeather(string weather) {
	this->weather = weather;
}
void WeatherType::setFrequency(int frequency) {
	this->frequency = frequency;
}
void WeatherType::increment() {
	this->frequency += 1;
}

//overloaded comparison operators
bool WeatherType::operator==(const WeatherType& other) const {
	if (this->frequency == other.frequency) {
		return true;
	}
	else {
		return false;
	}
}

bool WeatherType::operator<(const WeatherType& other) const {
	if (this->frequency < other.frequency) {
		return true;
	}
	else {
		return false;
	}
}

bool WeatherType::operator>(const WeatherType& other) const {
	if (this->frequency > other.frequency) {
		return true;
	}
	else {
		return false;
	}
}