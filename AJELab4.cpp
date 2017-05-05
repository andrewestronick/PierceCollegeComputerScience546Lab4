// Estronick, Andrew CS546 Section 3122  May 23, 2017
// Lab Assignment 4 – Array Processing With Intel AVX Instructions 

#define _USE_MATH_DEFINES

#include<iostream>
#include<vector>
#include<fstream>
#include<sstream>
#include<algorithm>
#include <cmath>


struct record
{
	unsigned ID;
	float latitude;
	float longitude;
	float distance;
	float dlatitude;
	float dlongitude;
};

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		std::cout << "Usage: " << argv[0] << " {input file}\n";
		exit(-1);
	}

	const float PI_180 = 3.14159 / 180.0;
	const float earth = 3963.19;
	std::vector<record> coordinates;
	std::ifstream fin(argv[1]);
	std::string line;
	float dlat1;
	float dlong1;
	float dlat2;
	float dlong2;
	float dLong;
	float dLat;
	float aHarv;
	float cHarv;

	if (!fin.is_open())
	{
		std::cout << "Cannot open file " << argv[1] << std::endl;
		exit(-1);
	}

	while (getline(fin, line))
	{
		std::stringstream buffer(line);
		unsigned ID;
		float latitude, longitude;

		buffer >> ID;
		buffer >> latitude;
		buffer >> longitude;

		if (0 == latitude && 0 == longitude)
			continue;

		coordinates.emplace_back(record{ ID, latitude, longitude, 0.0, (latitude * PI_180), (longitude * PI_180) });
	}

	size_t size = coordinates.size();

	// std::sort(coordinates.begin(), coordinates.end(), [](record &a, record &b) { return a.latitude < b.latitude; });

	unsigned *ID = new unsigned[size];
	float *latitude = new float[size];
	float *longitude = new float[size];

	/*
	for (unsigned i = 0; i < size; ++i)
	{
		ID[i] = coordinates[i].ID;
		latitude[i] = coordinates[i].latitude;
		longitude[i] = coordinates[i].longitude;
	}
	*/

	while (true)
	{
		std::cout << "Enter input location (latitude and longitude), or -999 -999 to end. ";

		float latitude1, longitude1;

		std::cin >> latitude1;
		std::cin >> longitude1;

		if (latitude1 <= -999 && longitude1 <= -999)
			break;

		if (latitude1 < -90 || latitude1 > 90 || longitude1 < -180 || longitude1 > 180)
		{
			std::cout << "Values " << latitude1 << " and " << longitude1 << " are not acceptable, re-enter.\n";
			continue;
		}

		for (record &r : coordinates)
		{
			dlat1 = latitude1 * PI_180;
			dlong1 = longitude1 * PI_180;

			dlat2 = r.dlatitude;
			dlong2 = r.dlongitude;

			dLong = longitude1 - dlong2;
			dLat = latitude1 - dlat2;

			aHarv = pow(sin(dLat / 2.0), 2.0) + cos(dlat1) * cos(dlat2) * pow(sin(dLong / 2), 2);
			cHarv = 2 * atan2(sqrt(aHarv), sqrt(1.0 - aHarv));
		
			r.distance = earth * cHarv;
		}

		std::sort(coordinates.begin(), coordinates.end(), [](record &a, record &b) { return a.distance < b.distance; });

		for (unsigned i = 0; i < 5; ++i)
			std::cout << "Location " << coordinates[i].ID  << " at " << coordinates[i].latitude << ", " << coordinates[i].longitude << " is " << coordinates[i].distance << " {units} away. \n";
	}

	std::cout << "Ending execution. \n";

	delete ID;
	delete latitude;
	delete longitude;
}


/*

// LabExercise1.cpp

// This is a lab exercise to measure CPU time used by an algorithm.
// The user enters a prime number limit and the program calculates all the prime numbers
// up to that limit.

// This program uses several Windows-defined structures and data types, including:
// ULONGLONG
// FILETIME and LPFILETIME
// HANDLE
// SYSTEMTIME
// BOOL
// DWORD
// ULARGE_INTEGER

// Windows functions used include:
// GetCurrentProcess
// GetProcessTimes
// GetLastError
// GetLocalTime

#define _WIN32_WINNT 0x0501
#include <iostream>
#include "windows.h"
using namespace std;
ULONGLONG getTime64(LPFILETIME a);

int main(int argc, char* argv[])
{
FILETIME creationTime, exitTime, kernelTime, userTime;
LPFILETIME creation = &creationTime, exit = &exitTime, kernel = &kernelTime, user = &userTime;
ULONGLONG c1, c2, e1, e2, k1, k2, u1, u2;
HANDLE myProcess = GetCurrentProcess();
BOOL gotTime1, gotTime2;
DWORD failReason;
SYSTEMTIME loct;
ULONGLONG primeLimit = 0, primeCount = 0, lastPrime;

do {
cout << "Enter prime number limit:";
cin >> primeLimit;
cout << endl;
} while (primeLimit < 1);

gotTime1 = GetProcessTimes(myProcess, creation, exit, kernel, user);
if (!gotTime1) {
failReason = GetLastError();
cout << "GetProcessTimes failed, Failure reason:" << failReason << endl;
return 0;
}
c1 = getTime64(creation);
e1 = getTime64(exit);
k1 = getTime64(kernel);
u1 = getTime64(user);

// do something long...
GetLocalTime(&loct);
cout << "The starting local time is:" << loct.wHour << ':' << loct.wMinute << ':' << loct.wSecond << '.' << loct.wMilliseconds << endl;
double fStartTime = loct.wHour * 3600 + loct.wMinute * 60 + loct.wSecond + (loct.wMilliseconds / 1000.0);
// simple-minded prime number calculation
for (int i = 2; i <= primeLimit; i++) {
bool isPrime = true;
for (int j = 2; j < i; j++)
if ((i % j) == 0) isPrime = false;
if (isPrime) {
// cout << i << " is a prime number." << endl;
primeCount++;
lastPrime = i;
}
}

gotTime2 = GetProcessTimes(myProcess, creation, exit, kernel, user);
if (!gotTime2) {
failReason = GetLastError();
cout << "GetProcessTimes failed, Failure reason:" << failReason << endl;
return 0;
}
GetLocalTime(&loct);
cout << "The   ending local time is:" << loct.wHour << ':' << loct.wMinute << ':' << loct.wSecond << '.' << loct.wMilliseconds << endl;
double fEndTime = loct.wHour * 3600 + loct.wMinute * 60 + loct.wSecond + (loct.wMilliseconds / 1000.0);
u2 = getTime64(user);
double workUserTime = (u2 - u1) * 100.0 / 1000000000.0;
cout << "Process user time:" << workUserTime << " (" << u2 - u1 << ')' << endl;

double workTime = fEndTime - fStartTime;
cout << "Elapsed clock time:" <<  workTime << endl;
cout << "CPU busy percentage:" << (workUserTime / workTime) * 100 << endl;
cout << primeCount << " prime numbers were calculated and the last one was:" << lastPrime << endl;
return 0;
}

// convert a FILETIME structure to a 64-bit integer
ULONGLONG getTime64(LPFILETIME a) {
ULARGE_INTEGER work;
work.HighPart = a->dwHighDateTime;
work.LowPart = a->dwLowDateTime;
return work.QuadPart;
}
*/
