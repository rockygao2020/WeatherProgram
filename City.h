#ifndef CITY_H
#define CITY_H

#include <string>
using namespace std;

class City {
	public:
		//constructor
		City(string inputName = "", string inputCountry = "", string inputState = "", float inputLat = 0, float inputLon = 0);
		//destructor
		~City();

		//accessors
		string getName() const;
		string getCountry() const;
		string getState() const;
		float getLat() const;
		float getLon() const;

		//mutators
		void setName(string name);
		void setCountry(string country);
		void setState(string state);
		void setLat(float lat);
		void setLon(float lon);

	private:
		string name;
		string country;
		string state;
		float lat;
		float lon;

};












#endif
