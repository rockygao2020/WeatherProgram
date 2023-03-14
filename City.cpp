//using namespace std;

#include "City.h"

//constructor
City::City(string inputName, string inputCountry, string inputState, float inputLat, float inputLon) : name(inputName), country(inputCountry), state(inputState), lat(inputLat), lon(inputLon) {}

//destructor
City::~City() {}

//accessors
string City::getName() const {
	return name;
}

string City::getCountry() const {
	return country;
}

string City::getState() const {
	return state;
}

float City::getLat() const {
	return lat;
}

float City::getLon() const {
	return lon;
}

//mutators
void City::setName(string name) {
	this->name = name;
}

void City::setCountry(string country) {
	this->country = country;
}

void City::setState(string state) {
	this->state = state;
}

void City::setLat(float lat) {
	this->lat = lat;
}

void City::setLon(float lon) {
	this->lon = lon;
}




