// main.cpp : This file contains the 'main' function. Program execution begins and ends there.

#pragma warning(disable : 4996)


#define CURL_STATICLIB
#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include <string>
#include <vector>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <queue>

#include "curl.h"				//API utilities: static library I needed to download and then build locally

#include "City.h"				//stores location (from which to take weather data from)
#include "Today.h"				//stores weather data for current day (until midnight)
#include "NextDay.h"			//stores weather data for future days (from 12:00AM - 11:59PM)
#include "Forecast.h"			//stores weather data from 3-hr interval
#include "WeatherType.h"		//simplifies max/min-heap sorting

using namespace std;

//helper function to convert string to all caps
string allCaps(string input) {
	string result;

	for (int i = 0; i < input.length(); i++) {
		result += toupper(input[i]);
	}
	return result;
}


//helper function for transforming strings
string spaceToUnderscore(string name)
{
	replace(name.begin(), name.end(), ' ', '_');
	return name;
}

//helper function for transforming strings
string underscoreToSpace(string name)
{
	replace(name.begin(), name.end(), '_', ' ');
	return name;
}

//helper function for CURL
static size_t my_write(void* buffer, size_t size, size_t nmemb, void* param)
{
	std::string& text = *static_cast<std::string*>(param);
	size_t totalsize = size * nmemb;
	text.append(static_cast<char*>(buffer), totalsize);
	return totalsize;
}

//helper function for storing 5-day/3hr forecasts as individual string literals
int forecastToVector(string result, vector<string>& list) {
	int count = 0;
	// get the starting and ending point
	
	size_t start = result.find("{\"dt\":");
	if (start == string::npos)  return -1; 

	size_t end = result.find("{\"dt\":", start+1);
	if (end == string::npos)  return -1; 

	while (end != string::npos) {
		list.push_back(result.substr(start+1, (end-3) - (start+1) + 1));	//second param: (end index - start index) + 1 = size of the desired substring
		count++;
		//increment the 2 params
		start = end;
		end = result.find("{\"dt\":", start+1);
	}
	
	//get the final forecast
	end = result.find("],\"city\":", start + 1);
	if (end != string::npos) {
		list.push_back(result.substr(start + 1, (end - 2) - (start + 1) + 1));
	}


	return count + 1;
}

//helper function for storing individual string literals as objects
int vectorToClass(vector<string>& oldList, vector<Forecast>& newList) {
	int count = 0;

	time_t date;
	double maxTemp;
	double minTemp;
	string weather;

	size_t start, end;

	//for each element in oldList -> convert into a Forecast object and insert into newList
	for (auto it : oldList) {
		//get date
		start = it.find("\"dt\":");
		end = it.find(",", start + 1);
		if ((start == string::npos) || (end == string::npos)) return -1;
		date = stoll(it.substr(start + 5, (end - 1) - (start + 5) + 1)); //same second param logic as the function above (end-start + 1) = size
		
		//get maxTemp;
		start = it.find("\"temp_max\":");
		end = it.find(",", start + 1);
		if ((start == string::npos) || (end == string::npos)) return -1;
		maxTemp = stod(it.substr(start + 11, (end - 1) - (start + 11) + 1));
		
		//get minTemp;
		start = it.find("\"temp_min\":");
		end = it.find(",", start + 1);
		if ((start == string::npos) || (end == string::npos)) return -1;
		minTemp = stod(it.substr(start + 11, (end - 1) - (start + 11) + 1));
		
		//get weather
		start = it.find("\"main\":", start + 1);
		end = it.find(",", start + 1);
		if ((start == string::npos) || (end == string::npos)) return -1;
		weather = it.substr(start + 8, (end - 2) - (start + 8) + 1);

		//cout << "\t\n" << setw(30) << left << date << setw(20) << left << maxTemp << setw(20) << left << minTemp << setw(20) << left << weather << endl;
		Forecast tempForecast{date, maxTemp, minTemp, weather};
		newList.push_back(tempForecast);
		count++;
	}
	return count;
}


//helper function that takes inputs: 2 time_t boundaries, vector of type Forecast, and 3 priority queues for max temp, min temp, and class WeatherType
//and does the following action:     runs through each Forecast object in the vector, and adds its 3 variables to the corresponding queues
int vectorToHeap(time_t start, time_t end, vector<Forecast>& list, priority_queue<double>& maxTemps,
				  priority_queue<double, vector<double>, greater<double>>& minTemps, priority_queue<class WeatherType>& maxWeatherTypes) {

	//TESTING ONLY
	//std::cout << "\n\nVECTORTOHEAP() FUNC IS RUNNING:";
	
	//first, make sure all the 3 heaps are empty
	while (!maxTemps.empty()) { maxTemps.pop(); }
	while (!minTemps.empty()) { minTemps.pop(); }
	while (!maxWeatherTypes.empty()) { maxWeatherTypes.pop(); }

	int count = 0; //return count at end of program (number of valid forecasts that fit between the bounds)

	//run through the vector/list of Forecast objects, adding max and min temp to their heaps
	//weathertypes heap is tricker: need to interate through ALL valid forecasts before filling up the WeatherType heap
	vector<WeatherType> weatherTypes;
	string weather;
	bool weatherExists;

	for (auto it : list) {
		if ((it.getDate() >= start) && (it.getDate() <= end)) { //if the forecast (it) fits within the time_t boundaries, store its 3 data vars
			//TESTING ONLY
			//std::cout << "\n\t\tDATE: " << it.getDate();
			
			count++;
			maxTemps.push(it.getMaxTemp());
			minTemps.push(it.getMinTemp());
			
			//deal with weather data; see if it's already added or not into the weatherTypes vector
			weatherExists = FALSE;
			weather = it.getWeather();

			for (int i = 0; i < weatherTypes.size(); i++) {
				if (weather.compare(weatherTypes.at(i).getWeather()) == 0) {	    //if the current (it) forecast has the same weather as another in the vector, increment the frequency
					weatherTypes.at(i).increment();												    //and set weatherExists bool to TRUE
					weatherExists = TRUE;
					break;
				}
			}
			if (!weatherExists) {						       //if after going through the entire vector, there is no instance of this weather type, bool will remain
				WeatherType temp{weather, 1};			       //FALSE, indicating to add the new weather type
				weatherTypes.push_back(temp);
			}
			/*

			//TESTING ONLY:
			std::cout << "\n\t\twhat the current vector<WeatherType> looks like :\n";
			for (auto x : weatherTypes) {
				std::cout << "\t\t\t" << setw(10) << left << "type: " << setw(10) << left << x.getWeather() << setw(10) << left << "freq: " << setw(10) << left << x.getFrequency() << endl;
			}
			*/
		}
	}

	//update - maxTemp and minTemp heaps are filled w/ all the forecasts within the interval; weathertype vector holds weather data from same batch of forecasts
	//fill up the WeatherType heap now
	for (auto x : weatherTypes) {
		maxWeatherTypes.push(x);
	}

	return count;
}


/*helper function that takes input : max heap of type WeatherType, and outputs : the (biased) most frequent WeatherType based on which object has the
  the max frequency data member. 
  The bias is such that for hazardous weather types (snow, rain, mist, thunder), even if they are tied or in a close-second to a less dangerous type,
  the function still returns the hazardous weather type */

WeatherType getBiasedMaxWeather(priority_queue<class WeatherType>& maxWeatherTypes) {
	vector<string> hazardList = {"Rain", "Thunderstorm", "Snow", "Mist"};
	
	
	//get the unbiased max weather type
	WeatherType currWeather = maxWeatherTypes.top();
	int currFrequency = currWeather.getFrequency();
	
	//save the unbiased max weather type for later
	const WeatherType MAX_WEATHER = currWeather;
	const int MAX_FREQUENCY = currFrequency;

	//if the unbiased max weather is already a hazard type, then simply return it
	for (auto it : hazardList) {
		if (it.compare(currWeather.getWeather()) == 0) { return currWeather; }
	}

	//if the literal max weather is not a hazard, then search for ties or seond-places that are hazards
	maxWeatherTypes.pop();
	bool isHazard;
	bool isBounded;

	//go through each maximum (descending order) and find one that is also a hazard; if there are none, then the loop runs through the entire heap
	while (!maxWeatherTypes.empty()) {
		currWeather = maxWeatherTypes.top();

		//check to see if current WeatherType is a hazard
		isHazard = FALSE;
		for (auto it : hazardList) {
			if (it.compare(currWeather.getWeather()) == 0) {
				isHazard = TRUE;
				break;
			}
		}

		//check to see if the current WeatherType's frequency is within 2 of the unbiased maximum frequency
		if (currWeather.getFrequency() >= MAX_FREQUENCY - 3) {
			isBounded = TRUE;
		} else {
			isBounded = FALSE;
		}

		//if both conditions are met, then return this "new" max weather instead of the unbiased max weather
		if (isHazard && isBounded) { return currWeather; }
		
		//decrement
		maxWeatherTypes.pop();
	}
	
	//if the code reaches the end of the loop here: the unbiased max is not a hazard AND there are no hazards 
	//that have enough frequency to be considered a replacement; return the unbiased max
	
	return MAX_WEATHER;

} 





int main()
{
	const string API_KEY = "2545169a3f0f1c836108f7a40a5744b0"; //USE OWN API CODE (requires the free plan)
	const string GEOCODE_LIMIT = "5";
	
	//***** STEP 1 - given: city's name -> get: the correct city and its lat/long coords

	//CURL set up
	string result; /*
	CURL* curl;
	CURLcode res;
	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init(); */
	
	//store all city options (5 total) in a vector
	vector<City> cityList;

	//get the city name
	string cityName;
	std::cout << "ENTER A CITY:" << endl;
	getline(cin, cityName);
	cityName = spaceToUnderscore(cityName);

	bool isValid = FALSE;

	while (!isValid) {
		
		//geocoding GET request using city name
		string geocodingURL = "http://api.openweathermap.org/geo/1.0/direct?q=" + cityName + "&limit=" + GEOCODE_LIMIT + "&appid=" + API_KEY;
		std::cout << "USING THE FOLLOWING URL: " << geocodingURL << endl;

		/*
		std::string result; */
		CURL* curl;
		CURLcode res;
		curl_global_init(CURL_GLOBAL_DEFAULT);
		curl = curl_easy_init(); 

		if (curl) {

			//curl_easy_setopt(curl, CURLOPT_URL, openWeatherURL.c_str()); //CURLOPT_URL expects a char* pointer to a null-terminated C string, NOT a string
			curl_easy_setopt(curl, CURLOPT_URL, geocodingURL.c_str());

			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my_write);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);
			curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
			res = curl_easy_perform(curl);
			curl_easy_cleanup(curl);
			if (CURLE_OK != res) {
				std::cerr << "CURL error: " << res << '\n';
			}
		}
		curl_global_cleanup();
		//std::cout << result << "\n\n";    //TESTING ONLY

		//Parse the result string to create a list of possible cities that the user wanted to select
		//vector<City> cityList;

		string currName;
		string currCountry;
		string currState;
		string currLat;
		string currLon;

		int i = 0;
		while (i < result.length()) {
			if (result[i] == '{') {			//indicates start of a new location; parse the { location data } and make a new object
				i += 9;						//set i to the start of the name

				//get the name
				currName = "";
				while (result[i] != '\"') {
					currName += result[i];
					i++;
				}

				if (result[i + 16] == '{') {	//if there exists the local names {} section: skip it
					while (result[i] != '}') {
						i++;
					}
				}
				i += 8;							//set i to the start of the lat

				//get the lat and lon
				currLat = "";
				currLon = "";

				while (result[i] != ',') {
					currLat += result[i];
					i++;
				}
				i += 7;						//set i to the start of the long

				while (result[i] != ',') {
					currLon += result[i];
					i++;
				}
				i += 12;					//set i to the start of the country

				//get the country
				currCountry = "";
				while (result[i] != '\"') {
					currCountry += result[i];
					i++;
				}

				if (result[i + 1] == ',') {	//there exists a state
					i += 11;				//set i to the start of the state

					//get the state
					currState = "";
					while (result[i] != '\"') {
						currState += result[i];
						i++;
					}
				}
				/* TESTING
				cout << "\t" << "CITY:\t" << setw(30) << left << currName << setw(30) << left << currCountry
					<< setw(30) << left << currState << setw(30) << left << currLat + ", " + currLon << endl; */

					//create the City object
				City temp{ currName, currCountry, currState, stof(currLat), stof(currLon) };
				cityList.push_back(temp);

			}

			i++;
		}

		//check to see if the name given was valid
		if (cityList.size() <= 0) {
			std::cout << "\nINVALID INPUT: GIVE A VALID CITY NAME\n";
			std::cout << "Enter a city:" << endl;
			getline(cin, cityName);
			cityName = spaceToUnderscore(cityName);
		}
		else {
			isValid = TRUE;			//vector contains real cities from the API get result -> stops the loop
		}
	}


	//get the exact location (city) user wants
	std::cout << "\nCHOOSE THE DESIRED CITY BY ENTERING ITS CORRESPONDING INTEGER:\n\t"
		 << setw(10) << left << "INTEGER" << setw(30) << left << "NAME" << setw(10) << left << "COUNTRY" << setw(30) << left << "STATE" << endl;

	for (int i = 0; i < cityList.size(); i++) {
		City temp = cityList.at(i);
		std::cout << "\t" << setw(10) << left << i+1 << setw(30) << left << temp.getName() << setw(10) << left << temp.getCountry() << setw(30) << left << temp.getState() << endl;
	}

	int userInt;
	cin >> userInt;
	
	while (cityList.size() < userInt || userInt <= 0) {
		std::cout << "INVALID INPUT: INTEGER MUST BE FROM 1-" << cityList.size() << endl;
		cin >> userInt;
	}

	//at this point: obtained a valid integer/city to use for the rest of the code
	City userCity = cityList.at(userInt - 1);

	std::cout << "CHOSEN CITY:\n\t"
		<< setw(10) << left << userInt << setw(30) << left << userCity.getName() << setw(10) << left << userCity.getCountry() << setw(30) << left << userCity.getState() << endl << endl;

	

	//TESTING ONLY: time

	/*
	auto test = std::chrono::system_clock::now();									//return "time point" class value representing current point in time
	std::time_t test_time = std::chrono::system_clock::to_time_t(test);				//converts time point value to time_t type
	std::cout << "finished computation at " << std::ctime(&test_time) << endl;		//ctime converts time_t to str

	std::time_t test2 = std::time(nullptr);
	std::cout << "Using time_t and localtime() func: " << std::asctime(std::localtime(&test2)) << "a"; //does same thing, but there's an automatic endline
	*/


	//***** STEP 2 - given the exact City object, create the TodayWidget object using the GET call for the current weather

	//current weather GET request
	string openWeatherURL = "https://api.openweathermap.org/data/2.5/weather?lat=" + to_string(userCity.getLat()) + "&lon=" + to_string(userCity.getLon()) + "&appid=2545169a3f0f1c836108f7a40a5744b0&units=imperial";
	std::cout << "USING THE FOLLOWING URL: " << openWeatherURL << endl;

	result = "";
	CURL* curl;
	CURLcode res;
	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();
	if (curl) {

		//curl_easy_setopt(curl, CURLOPT_URL, openWeatherURL.c_str()); //CURLOPT_URL expects a char* pointer to a null-terminated C string, NOT a string
		curl_easy_setopt(curl, CURLOPT_URL, openWeatherURL.c_str());

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my_write);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		if (CURLE_OK != res) {
			std::cerr << "CURL error: " << res << '\n';
		}
	}
	curl_global_cleanup();
	//std::cout << result << "\n\n";    //TESTING ONLY

	//parse the (second) API GET request to get data and create the dataToday instance object
	
	string currTemp = "";
	string windSpeed = "";
	string windDir = "";
	string humidity = "";

	/* use str find() function to grab the data quickly */

	string strTemp = "temp\":";
	string strHumidity = "humidity\":";
	string strSpeed = "speed\":";
	string strDeg = "deg\":";

	
	//get current temperature
	size_t found = result.find(strTemp);
	if (found != string::npos) {
		int i = found + 6;
		while ((result[i] != ',') && (result[i] != '}')) {
			currTemp += result[i];
			i++;
		} 
	}
	//get humidity
	found = result.find(strHumidity);
	if (found != string::npos) {
		int i = found + 10;
		while ( (result[i] != ',') && (result[i] != '}') ) {
			humidity += result[i];
			i++;
		}
	}
	//get wind speed
	found = result.find(strSpeed);
	if (found != string::npos) {
		int i = found + 7;
		while( (result[i] != ',') && (result[i] != '}') ) {
			windSpeed += result[i];
			i++;
		}
	}
	//get wind direction
	found = result.find(strDeg);
	if (found != string::npos) {
		int i = found + 5;
		while ( (result[i] != ',') && (result[i] != '}') ) {
			windDir += result[i];
			i++;
		}
	}
	
	//add the 4 variables to Today object
	Today dataToday = Today();
	dataToday.setCurrTemp(stod(currTemp));
	dataToday.setHumidity(stod(humidity));
	dataToday.setWindSpeed(stod(windSpeed));
	dataToday.setWindDir(stod(windDir));

	
	//***** STEP 3 - given the exact City object, create 4 NextDayWidget objects using the Get call for the 5-day forecast; also complete TodayWidget


	//forecasted weather GET request
	std::string openWeatherURL2 = "api.openweathermap.org/data/2.5/forecast?lat=" + to_string(userCity.getLat()) + "&lon=" + to_string(userCity.getLon()) + "&appid=2545169a3f0f1c836108f7a40a5744b0&units=imperial";
	std::cout << "USING THE FOLLOWING URL: " << openWeatherURL2 << std::endl;

	result = "";
	//CURL* curl;
	//CURLcode res;
	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();
	if (curl) {

		//curl_easy_setopt(curl, CURLOPT_URL, openWeatherURL.c_str()); //CURLOPT_URL expects a char* pointer to a null-terminated C string, NOT a string
		curl_easy_setopt(curl, CURLOPT_URL, openWeatherURL2.c_str());

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my_write);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		if (CURLE_OK != res) {
			std::cerr << "CURL error: " << res << '\n';
		}
	}
	curl_global_cleanup();
	//std::cout << result << "\n\n";    //TESTING ONLY

	//need to group each 3-hr forecast using unix timestamps (make 4 timestamps total)
	//given tempTime (time_t holding current time from epoch in seconds) -> find another time_t value that indicates the start of the next day (12AM)
	//GETTING THE DATE FROM THE TODAY OBJECT
	time_t tempTime = dataToday.getDate();
	string strDate = std::asctime(std::localtime(&tempTime));
	strDate.pop_back();
	
	
	time_t nextday1;
	time_t nextday2;
	time_t nextday3;
	time_t nextday4;


	struct tm* timeinfo;
	timeinfo = localtime(&tempTime);		//converts the time_t tempTime (in seconds since epoch) into struct tm (easily breaks up months, days, etc)
	timeinfo->tm_sec = 0;
	timeinfo->tm_min = 0;
	timeinfo->tm_hour = 0;
	timeinfo->tm_mday += 1;
	timeinfo->tm_isdst = -1;				//check locally if DST is on or not
	nextday1 = mktime(timeinfo);			//this auto-rearranges out-of-bounds or impossible days, etc ( can't have 40 days, 25 hrs, etc)
											//nextday1 is 1 or less days ahead of current day (starts at 12AM next day)
	//string strNextDay1 = std::asctime(std::localtime(&nextday1));
	//strNextDay1.pop_back();
	
	timeinfo->tm_mday += 1;
	timeinfo->tm_isdst = -1;
	nextday2 = mktime(timeinfo);			//nextday2 is 2 days ahead of current day

	timeinfo->tm_mday += 1;
	timeinfo->tm_isdst = -1;
	nextday3 = mktime(timeinfo);

	timeinfo->tm_mday += 1;
	timeinfo->tm_isdst = -1;
	nextday4 = mktime(timeinfo);



	/* TESTING
	cout << "TIME TESTING:\n\t" << setw(30) << left << " time [readable/local]: " << setw(30) << left << strDate
		<< "\n\t" << setw(30) << left << " time [epoch]: " << setw(30) << left << tempTime
		//<< "\n\t" << setw(30) << left << " time [struct tm/local]: " << setw(30) << left << asctime(timeinfo)
		//<< "\n\t" << setw(30) << left << " time [struct tm/local]: " << setw(30) << left << asctime(timeinfo)
		<< "\n\t" << setw(30) << left << " time [epoch/nextday]: " << setw(30) << left << nextday1
		<< "\n\t" << setw(30) << left << " time [epoch/nextday]: " << setw(30) << left << nextday2
		<< "\n\t" << setw(30) << left << " time [epoch/nextday]: " << setw(30) << left << nextday3
		<< "\n\t" << setw(30) << left << " time [epoch/nextday]: " << setw(30) << left << nextday4
		<< "\n\t";
		*/
				
	//***** UPDATE - obtained time_t vars: tempTime (current time), and 4 nextdays (deadline of midnight of the following day, consecutively)
	
	//***** STEP 3.1 - break up the 3-hr forecasts by curly braces; 
		
	vector<string> strForecastList;					//stores 20+ forecasts as strings; each measured consecutively after 3-hr intervals
	vector<Forecast> forecastList;					//stores 20+ forecasts as objects; better way to organize the data within each forecast

	//call helper function to fill up 1rst vector
	int j = forecastToVector(result, strForecastList);
	
	
	if (j >= 0) {
		/*
		cout << "TESTING STRING VECTOR:";
		for (auto it : strForecastList) {
			cout << "\n\t" << it;
		}
		cout << "\n\n"; */
	} else { 
		std::cout << "ERROR: FIND FUNCTION FAILED\n";
	}
	
	//call helper function to convert 1rst to 2nd vector
	int k = vectorToClass(strForecastList, forecastList);
	if (k >= 0) {
		/*cout << "TESTING OBJECT VECTOR:";
		for (auto it : forecastList) {
			cout << "\n\t" << setw(30) << left << it.getDate() << setw(30) << left << it.getMaxTemp() << setw(30) << left << it.getMinTemp()
				<< setw(30) << left << it.getWeather(); 
		} */
	} else {
		std::cout << "ERROR: FIND FUNCTION FAILED\n";
	}
	
	//***** STEP 3.2 - use all forecasts before the end of the current day to complete the TODAY object (get avg weather, max, and min temps)
	
	priority_queue<double> maxTemps;
	priority_queue<double, vector<double>, greater<double>> minTemps;
	priority_queue<class WeatherType> maxWeatherTypes;					//stores WeatherTypes objects (weather as a string, int frequency of it occuring)
																		//taking the top() returns the MAX WeatherType; the object w/ highest frequency
																		//possible because I overloaded the comparison operators for WeatherType class

	//call helper function to take a set interval[current time - midnight] of forecasts and input their 3 values into the 3 respective containers
	int h = vectorToHeap(tempTime, nextday1, forecastList, maxTemps, minTemps, maxWeatherTypes);

	//complete the data members for the Today object "dataToday"
	dataToday.setMaxTemp(maxTemps.top());
	dataToday.setMinTemp(minTemps.top());
	dataToday.setAvgWeather(getBiasedMaxWeather(maxWeatherTypes).getWeather());

	//TESTING:
	//cout << "\n" << setw(20) << left << "MAX TEMP: " << maxTemps.top();
	//cout << "\n" << setw(20) << left << "MIN TEMP: " << minTemps.top();
	//cout << "\n" << setw(20) << left << "MAX TYPE: " << maxWeatherTypes.top().getWeather() << ", " << maxWeatherTypes.top().getFrequency();
	
	std::cout << "\n" << "TODAY'S WEATHER FORECAST:"
		<< "\n\t" << setw(20) << left << "DATE:" << setw(30) << left << dataToday.getStrDate()
		<< "\n\t" << setw(20) << left << "CURR_TEMP:" << setw(30) << left << dataToday.getCurrTemp()
		<< "\n\t" << setw(20) << left << "MAX_TEMP:" << setw(30) << left << dataToday.getMaxTemp()
		<< "\n\t" << setw(20) << left << "MIN_TEMP:" << setw(30) << left << dataToday.getMinTemp()
		<< "\n\t" << setw(20) << left << "AVG_WEATHER:" << setw(30) << left << dataToday.getAvgWeather()
		<< "\n\t" << setw(20) << left << "HUMIDITY:" << setw(30) << left << dataToday.getHumidity()
		<< "\n\t" << setw(20) << left << "WD_SPEED:" << setw(30) << left << dataToday.getWindSpeed()
		<< "\n\t" << setw(20) << left << "WD_DIRECTION:" << setw(30) << left << dataToday.getWindDir() << endl;


	//***** STEP 3.3 - sort the forecasts into the next 3 days and complete the NextDay objects respectively; 
	
	NextDay nextDay1;
	NextDay nextDay2;
	NextDay nextDay3;

	//get all forecasts for the next day (tommorrow) - from time_t nextday1 to time_t nextday2 - stored into the 3 heaps
	h = vectorToHeap(nextday1, nextday2, forecastList, maxTemps, minTemps, maxWeatherTypes);
	//complete data members for object of type NextDay called "NextDay1"
	nextDay1.setDate(nextday1);
	nextDay1.setMaxTemp(maxTemps.top());
	nextDay1.setMinTemp(minTemps.top());
	nextDay1.setAvgWeather(getBiasedMaxWeather(maxWeatherTypes).getWeather());

	std::cout << "\n" << allCaps( nextDay1.getDay() ) << "\'S WEATHER FORECAST:"
		<< "\n\t" << setw(20) << left << "DATE:" << setw(30) << left << nextDay1.getStrDate()
		<< "\n\t" << setw(20) << left << "MAX_TEMP:" << setw(30) << left << nextDay1.getMaxTemp()
		<< "\n\t" << setw(20) << left << "MIN_TEMP:" << setw(30) << left << nextDay1.getMinTemp()
		<< "\n\t" << setw(20) << left << "AVG_WEATHER:" << setw(30) << left << nextDay1.getAvgWeather() << endl;

	//get all forecasts for the next day (after tommorrow) - from time_t nextday2 to time_t nextday3 - stored into the 3 heaps
	h = vectorToHeap(nextday2, nextday3, forecastList, maxTemps, minTemps, maxWeatherTypes);
	//complete data members for object of type NextDay called "NextDay1"
	nextDay2.setDate(nextday2);
	nextDay2.setMaxTemp(maxTemps.top());
	nextDay2.setMinTemp(minTemps.top());
	nextDay2.setAvgWeather(getBiasedMaxWeather(maxWeatherTypes).getWeather());

	std::cout << "\n" << allCaps(nextDay2.getDay()) << "\'S WEATHER FORECAST:"
		<< "\n\t" << setw(20) << left << "DATE:" << setw(30) << left << nextDay2.getStrDate()
		<< "\n\t" << setw(20) << left << "MAX_TEMP:" << setw(30) << left << nextDay2.getMaxTemp()
		<< "\n\t" << setw(20) << left << "MIN_TEMP:" << setw(30) << left << nextDay2.getMinTemp()
		<< "\n\t" << setw(20) << left << "AVG_WEATHER:" << setw(30) << left << nextDay2.getAvgWeather() << endl;

	//get all forecasts for the next day (after-after-tommorrow) - from time_t nextday3 to time_t nextday4 - stored into the 3 heaps
	h = vectorToHeap(nextday3, nextday4, forecastList, maxTemps, minTemps, maxWeatherTypes);
	//complete data members for object of type NextDay called "NextDay1"
	nextDay3.setDate(nextday3);
	nextDay3.setMaxTemp(maxTemps.top());
	nextDay3.setMinTemp(minTemps.top());
	nextDay3.setAvgWeather(getBiasedMaxWeather(maxWeatherTypes).getWeather());

	std::cout << "\n" << allCaps(nextDay3.getDay()) << "\'S WEATHER FORECAST:"
		<< "\n\t" << setw(20) << left << "DATE:" << setw(30) << left << nextDay3.getStrDate()
		<< "\n\t" << setw(20) << left << "MAX_TEMP:" << setw(30) << left << nextDay3.getMaxTemp()
		<< "\n\t" << setw(20) << left << "MIN_TEMP:" << setw(30) << left << nextDay3.getMinTemp()
		<< "\n\t" << setw(20) << left << "AVG_WEATHER:" << setw(30) << left << nextDay3.getAvgWeather() << endl;


	return 0;
}






