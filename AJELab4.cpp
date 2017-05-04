// Andrew Estronick

#include<iostream>
#include<vector>
#include<fstream>
#include<sstream>
#include <algorithm>

struct record
{
	unsigned ID;
	float latitude;
	float longitude;
};

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		std::cout << "Usage: " << argv[0] << " {input file}\n";
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
		record r;
		buffer >> r.ID;
		buffer >> r.latitude;
		buffer >> r.longitude;

		if (0 == r.latitude || 0 == r.longitude)
			continue;

		coordinates.emplace_back(r);
	}

	unsigned size = coordinates.size();
	unsigned *ID = new unsigned[size];
	float *latitude = new float[size];
	float *longitude = new float[size];

	for (unsigned i = 0; i < size; ++i)
	{
		ID[i] = coordinates[i].ID;
		latitude[i] = coordinates[i].latitude;
		longitude[i] = coordinates[i].longitude;
	}

	std::cout << "Size = " << size << std::endl;

}