// Andrew Estronick


#include<iostream>
#include<vector>
#include<fstream>
#include<sstream>
#include <algorithm>

class record
{
public:
	record(unsigned ID, float latitude, float longitude);
	bool compareLatitude(const record &a, const record &b);
	bool compareLongitude(const record &a, const record &b);
	unsigned getID();
	float getLatitude();
	float getLongitude();
private:
	unsigned ID;
	float latitude;
	float longitude;
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
		int ID;
		float latitude, longitude;
		buffer >> ID;
		buffer >> latitude;
		buffer >> longitude;


		coordinates.emplace_back(record(ID, latitude, longitude));
	}

	size_t size = coordinates.size();

	// std::sort(coordinates.begin(), coordinates.end(), [](record &a, record &b) { return a.getLatitude() < b.getLatitude(); });

	for (unsigned i = 0; i < size; ++i)
	{
		std::cout << i << " " << coordinates[i].getID() << " " << coordinates[i].getLatitude() << ", " << coordinates[i].getLongitude() << std::endl;
	}


	unsigned *ID = new unsigned[size];
	float *latitude = new float[size];
	float *longitude = new float[size];

	for (unsigned i = 0; i < size; ++i)
	{
		ID[i] = coordinates[i].getID();
		latitude[i] = coordinates[i].getLatitude();
		longitude[i] = coordinates[i].getLongitude();
	}

	std::cout << "Size = " << size << std::endl;

}

record::record(unsigned ID, float latitude, float longitude)
{
}

bool record::compareLatitude(const record & a, const record & b)
{
	return a.latitude < b.latitude;
}

bool record::compareLongitude(const record & a, const record & b)
{
	return a.longitude < b.longitude;
}

unsigned record::getID()
{
	return ID;
}

float record::getLatitude()
{
	return latitude;
}

float record::getLongitude()
{
	return longitude;
}
