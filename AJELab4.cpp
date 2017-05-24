// Estronick, Andrew CS546 Section 3122  May 23, 2017
// Lab Assignment 4 – Array Processing With Intel AVX Instructions 

#define _USE_MATH_DEFINES
#define _WIN32_WINNT 0x0501

#include<iostream>
#include<vector>
#include<fstream>
#include<sstream>
#include<algorithm>
#include <cmath>
#include "windows.h"

ULONGLONG getTime64(LPFILETIME a);

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
	const float PI_180 = (float) (M_PI / 180);
	const float earth = 3963.19f;
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
	FILETIME creationTime, exitTime, kernelTime, userTime;
	LPFILETIME creation = &creationTime, exitT = &exitTime, kernel = &kernelTime, user = &userTime;
	ULONGLONG u1, u2;
	HANDLE myProcess = GetCurrentProcess();
	BOOL gotTime1, gotTime2;
	DWORD failReason;
	SYSTEMTIME loct;
	double fStartTime, fEndTime, workTime, workUserTime;

	if (argc != 2)
	{
		std::cout << "Usage: " << argv[0] << " {input file}\n";
		exit(-1);
	}

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

	fin.close();

	size_t size = coordinates.size();

	if (size < 5)
	{
		std::cout << "Error data file has less than five usables entires.\n";
		exit(-1);
	}

	unsigned *ID = new unsigned[size];
	float *latitude = new float[size];
	float *longitude = new float[size];
	float *dlatitude = new float[size];
	float *dlongitude = new float[size];
	float *distance = new float[size];
	unsigned *ID_top5 = new unsigned[5];
	float *latitude_top5 = new float[5];
	float *longitude_top5 = new float[5];
	float *distance_top5 =  new float[5];

	for (unsigned i = 0; i < size; ++i)
	{
		ID[i] = coordinates[i].ID;
		latitude[i] = coordinates[i].latitude;
		longitude[i] = coordinates[i].longitude;
		dlatitude[i] = coordinates[i].dlatitude; 
		dlongitude[i] = coordinates[i].dlongitude;

	}

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

		gotTime1 = GetProcessTimes(myProcess, creation, exitT, kernel, user);
		if (!gotTime1) {
			failReason = GetLastError();
			std::cout << "GetProcessTimes failed, Failure reason:" << failReason << std::endl;
			exit(-1);
		}
		u1 = getTime64(user);
		GetLocalTime(&loct);
		fStartTime = loct.wHour * 3600 + loct.wMinute * 60 + loct.wSecond + (loct.wMilliseconds / 1000.0);

		// dlat1 = latitude1 * PI_180;
		// dlong1 = longitude1 * PI_180;

		alignas(32) static float coord[2];
		alignas(32) static float multi[2];
		// alignas(32) static float result[2];

		coord[0] = latitude1;
		coord[1] = longitude1;

		multi[0] = PI_180;
		multi[1] = PI_180;


		__asm
		{
			movaps xmm1, [coord]
			movaps xmm2, [multi]
			mulps xmm1 ,xmm2
			movaps [coord], xmm1

		}

		dlat1 = coord[0];
		dlong1 = coord[1];

		for (unsigned i = 0; i < size; ++i)
		{
			dlat2 = dlatitude[i];
			dlong2 = dlongitude[i];

			dLat = PI_180 * (latitude1 - latitude[i]);
			dLong = PI_180 * (longitude1 - longitude[i]);

			aHarv = (float) (pow(sin(dLat / 2.0), 2.0) + cos(dlat1) * cos(dlat2) * pow(sin(dLong / 2), 2));
			cHarv = (float) (2 * atan2(sqrt(aHarv), sqrt(1.0 - aHarv)));
		
			distance[i] = earth * cHarv;
		}

		for (unsigned i = 0; i < 5; ++i)
			distance_top5[i] = FLT_MAX;

		for (unsigned i = 0; i < size; ++i)
		{
			if (distance[i] < distance_top5[4]) // is value less than largest top 5
			{
				for (unsigned j = 0; j < 5; ++j)
				{
					if (distance[i] < distance_top5[j]) // find where to insert value
					{
						for (int k = 4, insert = ((j == 0) ? 1 : j); k >= insert; --k)
						{
							ID_top5[k] = ID_top5[k -1];
							latitude_top5[k] = latitude_top5[k - 1];
							longitude_top5[k] = longitude_top5[k - 1];
							distance_top5[k] = distance_top5[k - 1];
						}
						ID_top5[j] = ID[i];
						latitude_top5[j] = latitude[i];
						longitude_top5[j] = longitude[i];
						distance_top5[j] = distance[i];
						break;
					}
				}
			}
		}

		gotTime2 = GetProcessTimes(myProcess, creation, exitT, kernel, user);
		if (!gotTime2) {
			failReason = GetLastError();
			std::cout << "GetProcessTimes failed, Failure reason:" << failReason << std::endl;
			exit(-1);
		}

		GetLocalTime(&loct);
		fEndTime = loct.wHour * 3600 + loct.wMinute * 60 + loct.wSecond + (loct.wMilliseconds / 1000.0);
		u2 = getTime64(user);
		workUserTime = (u2 - u1) * 100.0 / 1000000000.0; // = usertime
		workTime = fEndTime - fStartTime; // = system CPU time

		for (unsigned i = 0; i < 5; ++i)
			std::cout << "Location " << ID_top5[i] << " at " << latitude_top5[i] << ", " << longitude_top5[i] << " is " << distance_top5[i] << " Miles away. \n";
		
		std::cout << "search took " << workTime << " seconds of system CPU time and "<< workUserTime << " seconds of user CPU time.\n\n\n";
	}

	std::cout << "Ending execution. \n";

	delete ID;
	delete latitude;
	delete longitude;
	delete dlatitude;
	delete dlongitude;
	delete distance;
	delete ID_top5;
	delete distance_top5;
}

ULONGLONG getTime64(LPFILETIME a) {
	ULARGE_INTEGER work;
	work.HighPart = a->dwHighDateTime;
	work.LowPart = a->dwLowDateTime;
	return work.QuadPart;
}
