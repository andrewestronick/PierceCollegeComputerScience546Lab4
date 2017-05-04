// Andrew Estronick
//
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
};

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		std::cout << "Usage: " << argv[0] << " {input file}\n";
		std::cout << argv[1] << std::endl;
		exit(-1);
	}

	std::vector<record> coordinates;
	std::ifstream fin(argv[1]);
	std::string line;

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

		coordinates.emplace_back(record{ ID, latitude, longitude, 0.0 });
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

		float latitude, longitude;

		std::cin >> latitude;
		std::cin >> longitude;

		if (latitude <= -999 && longitude <= -999)
			break;

		if (latitude < -90 || latitude > 90 || longitude < -180 || longitude > 180)
		{
			std::cout << "Values " << latitude << " and " << longitude << " are not acceptable, re-enter.\n";
			continue;
		}

		for (record &r : coordinates)
		{
			float lat1 = latitude;
			float long1 = longitude;
			float lat2 = r.latitude;
			float long2 = r.longitude;

			float PI = M_PI;

			float dlat1 = lat1*(PI / 180);

			float dlong1 = long1*(PI / 180);
			float dlat2 = lat2*(PI / 180);
			float dlong2 = long2*(PI / 180);

			float dLong = dlong1 - dlong2;
			float dLat = dlat1 - dlat2;

			float aHarv = pow(sin(dLat / 2.0), 2.0) + cos(dlat1)*cos(dlat2)*pow(sin(dLong / 2), 2);
			float cHarv = 2 * atan2(sqrt(aHarv), sqrt(1.0 - aHarv));
		
			const float earth = 3963.19;
			r.distance = earth*cHarv;
		}

		std::sort(coordinates.begin(), coordinates.end(), [](record &a, record &b) { return a.distance < b.distance; });

		for (record &r : coordinates)
		{
			std::cout << r.ID << "::" << r.latitude << ", " << r.longitude << ", " << r.distance << "\n";
		}
		/*
		double PI = M_PI;

		double dlat1 = lat1*(PI / 180);

		double dlong1 = long1*(PI / 180);
		double dlat2 = lat2*(PI / 180);
		double dlong2 = long2*(PI / 180);

		double dLong = dlong1 - dlong2;
		double dLat = dlat1 - dlat2;

		double aHarv = pow(sin(dLat / 2.0), 2.0) + cos(dlat1)*cos(dlat2)*pow(sin(dLong / 2), 2);
		double cHarv = 2 * atan2(sqrt(aHarv), sqrt(1.0 - aHarv));
		//earth's radius from wikipedia varies between 6,356.750 km - 6,378.135 km (~3,949.901 - 3,963.189 miles)
		//The IUGG value for the equatorial radius of the Earth is 6378.137 km (3963.19 mile)
		const double earth = 3963.19;//I am doing miles, just change this to radius in kilometers to get distances in km
		double distance = earth*cHarv;

		cout << ''...Distance is '' << distance << '' miles.\n'';
		*/
	}

	std::cout << "Ending execution. \n";

	delete ID;
	delete latitude;
	delete longitude;
}
