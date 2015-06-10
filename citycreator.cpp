#include <QtCore/QCoreApplication>
#include <QTime>
#include <iostream>
#include <fstream>
#include <math.h>
#include <cstdio>
#include <cstdlib>

#include "citycreator.h"

// constructor?
CityCreator::CityCreator()
{
	// is such variable good to initialize in constructor? no idea :)
	numOfNames = 0;
	// how close the objects can be to the edge of the map.
	ObjEdgeOffset = 25;
}


// no destructor?
/*
CityCreator::~CityCreator()
{
}
*/


/*

	this is like the old int main() :)

*/
void CityCreator::RunMe(int argc, char *argv[])
{
	// initialize random seed:
	QTime timmy(0, 0, 0);
	qsrand(timmy.secsTo(QTime::currentTime()));

	// just get the time
	QTime t;
	t.start();

	// check parameters and reads config file
	Check_Parameters(argc, argv);

	printf("reading VegetationObjectsFile... ");
	// read vegetation objects list. yes this is badly named hehe
	ReadObjectsFile(VegetationObjectsFile);
	
	printf("reading ReadVillageConfigFile... ");
	// read village objects list.
	VillageBuildings = ReadVillageConfigFile(VillageObjectsFile);

	printf("reading ReadCityConfigFile... ");
	// read city objects list.
	CityBuildings = ReadCityConfigFile(CityObjectsFile);

	// TODO this could be, anywhere?
	NumberOfCities = MinCity + (MaxCity * qrand() / (RAND_MAX + 1.0));
	printf("NumberOfCities randomly chosen: %i\n", NumberOfCities);

	// objects export / import visitor 3 TEXT file
	FILE *output = fopen (output_file, "wt");
	if (!output)
	{
		printf ("error at %s file\n", output_file);
		exit (1);
	}

	// write random vegetation
	Write_Objects(output);

	// create and write city / village objects
	for (int i = 1; i < (NumberOfCities); i++)
	{
		MakeTheCity(output);
	}

	Close_Files(output, t);
}


/*

	Checks command line parameters,
	opens vegetation and city building objects list text file and reads them into array

*/
void CityCreator::Check_Parameters(int argc, char *argv[])
{
	if (argc < 3)
	{
		printf("Incorrect parameters supplied.\nUsage:\n\nexe DESTINATION.txt ConfigFile\n\n");
		exit(1);
	}

	// 1 copy the output file parameter into variable
	strcpy(output_file, argv[1]);
	printf("Created Objects file: %s\n", output_file); // was argv[1]

	// read config file
	ReadConfigFile(argv[2]);
}


/*

*/
void CityCreator::ReadConfigFile(char *filename)
{
	char temp[80];
	
	// City buildings configuration file
	FILE *configf;
	configf = fopen(filename, "rt");
	if (!configf)
	{
		printf("error at %s!\n", filename);
		exit(1);
	}

	// read lines from config file
	// 1 - map size
	fgets(temp, 80, configf);
	MapSize = atoi(temp);
	printf("MapSize: %i\n", MapSize);

	// 2 - cell size
	fgets(temp, 80, configf);
	CellSize = atoi(temp);
	printf("CellSize: %i\n", CellSize);

	// 3 - number of vegetation objects to randomly place
	fgets(temp, 80, configf);
	NumberOfObjects = atoi(temp);
	printf("Number of vegetation objects: %i\n", NumberOfObjects);

	// 4 - min number of cities created randomly
	fgets(temp, 80, configf);
	MinCity = atoi(temp);
	printf("Minimum number of cities: %i\n", MinCity);

	// 5 - max number of cities created randomly
	fgets(temp, 80, configf);
	MaxCity = atoi(temp);
	printf("Maximum number of cities: %i\n", MaxCity);

	// 6 - vegetation objects list text file path and name
	fgets(temp, 80, configf);
	strcpy(VegetationObjectsFile, temp);
	// this is so gay that we have to strip the ending \n away from the string hehe
	// if we dont strip, then lateron fopen cannot read this file name for some reason.
	VegetationObjectsFile[strlen(VegetationObjectsFile) - 1] = 0;
	printf("Vegetation Objects File: %s\n", VegetationObjectsFile);
	
	// 7 - village (small/poor buildings) objects list text file path and name
	fgets(temp, 80, configf);
	strcpy(VillageObjectsFile, temp);
	VillageObjectsFile[strlen(VillageObjectsFile) - 1] = 0;
	printf("Village Objects File: %s\n", VillageObjectsFile);

	// 8 - city (large/fancy buildings) objects list text file path and name
	fgets(temp, 80, configf);
	strcpy(CityObjectsFile, temp);
	CityObjectsFile[strlen(CityObjectsFile) - 1] = 0;
	printf("City Objects File: %s\n", CityObjectsFile);

	// 9 - building size
	fgets(temp, 80, configf);
	building_size = atoi(temp);
	printf("Building size: %i\n", building_size);

	// 10 - object spacing (distance between two buildings)
	fgets(temp, 80, configf);
	objectspacing = atoi(temp);
	printf("Building object spacing: %i\n", objectspacing);

	printf("Closing config file.\n");
	fclose(configf);
}

/*

	Reads Vegetation Objects File

*/
void CityCreator::ReadObjectsFile(char *filename)
{
	// vegetation configuration file & strings
	FILE *vege;
	vege = fopen(filename, "rt");
	if (!vege)
	{
		printf("error at %s!\n", filename);
		exit(1);
	}

	int rx = 0;
	while (!feof(vege))
	{
		fgets(Rnd3dObjN[rx], 80, vege);
		Rnd3dObjN[rx][strlen(Rnd3dObjN[rx]) - 1] = 0;
		rx++;
	}
	// we come back one nodge because we read the EOF...
	rx--;
	printf("Read %i vegetation objects\n", rx);

	fclose(vege);
}


/*

	Read Village Config File

*/
int CityCreator::ReadVillageConfigFile(char *filename)
{
	// buildings configuration file
	FILE *citys;
	citys = fopen(filename, "rt");
	if (!citys)
	{
		printf("error at %s!\n", filename);
		exit(1);
	}

	int rx = 0;
	while (!feof(citys))
	{
		fgets(VillageObjects[rx], 80, citys);
		VillageObjects[rx][strlen(VillageObjects[rx]) - 1] = 0;
		rx++;
	}
	// we come back one nodge because we read the EOF...
	rx--;
	printf("Read %i village building objects\n", rx);

	fclose(citys);
	
	// returns the number of objects
	return rx;
}


/*

	Read City Config File

*/
int CityCreator::ReadCityConfigFile(char *filename)
{
	// buildings configuration file
	FILE *citys;
	citys = fopen(filename, "rt");
	if (!citys)
	{
		printf("error at %s!\n", filename);
		exit(1);
	}

	int rx = 0;
	while (!feof(citys))
	{
		fgets(CityObjects[rx], 80, citys);
		CityObjects[rx][strlen(CityObjects[rx]) - 1] = 0;
		rx++;
	}
	// we come back one nodge because we read the EOF...
	rx--;
	printf("Read %i city building objects\n", rx);

	fclose(citys);
	
	// returns the number of objects
	return rx;
}


/*

	create names text file (config.cpp compatible) for this city.

*/
void CityCreator::Names_Creator(float xcoord, float zcoord, int citysizex, int citysizez)
{
        FILE *names;

        // size limit for city being a village, ie small
        int villageSizeLimit = 250;

        // increment out name "name" for one digit
        numOfNames ++;

        // open file append mode, create if not exist.
        names = fopen ("names.hpp", "at");
        if (!names)
        {
        	printf ("error at names.hpp file\n");
                exit (1);
        }

        // seek to end of file so we don't overwrite existing citys.
        fseek (names, 0, SEEK_SET);

        // do we have small village or big city?
        if (citysizex < villageSizeLimit && citysizez < villageSizeLimit)
        {
                // small village - size less than 0-100
                fprintf(names, "\tclass village%i\n\t{\n\t\tname = \"Village %i\";\n\t\tposition[] = {%f, %f, 0};\n\t\ttype = \"NameVillage\";\n\t\tradiusA = %i;\n\t\tradiusB = %i;\n\t\tangle = 0;\n\t};\n\n", numOfNames, numOfNames, xcoord, zcoord, citysizex, citysizez);
                // ALICE stuff
                fprintf(names, "\tclass AcityCvillage%i\n\t{\n\t\tname = \"\";\n\t\ttype = \"CityCenter\";\n\t\tneighbors[] = {};\n\t\tdemography[] = {};\n\t\tposition[] = {%f, %f, 0};\n\t\tradiusA = 100;\n\t\tradiusB = 100;\n\t\tangle = 0;\n\t};\n\n", numOfNames, xcoord, zcoord);
        }
        else
        {
                // big city - size more than 100-500
                fprintf(names, "\tclass city%i\n\t{\n\t\tname = \"City %i\";\n\t\tposition[] = {%f, %f, 0};\n\t\ttype = \"NameCity\";\n\t\tradiusA = %i;\n\t\tradiusB = %i;\n\t\tangle = 0;\n\t};\n\n", numOfNames, numOfNames, xcoord, zcoord, citysizex, citysizez);

                // ALICE stuff
                fprintf(names, "\tclass AcityCcity%i\n\t{\n\t\tname = \"\";\n\t\ttype = \"CityCenter\";\n\t\tneighbors[] = {};\n\t\tdemography[] = {};\n\t\tposition[] = {%f, %f, 0};\n\t\tradiusA = %i;\n\t\tradiusB = %i;\n\t\tangle = 0;\n\t};\n\n", numOfNames, xcoord, zcoord, citysizex, citysizez);
        }
        fclose (names);
}


/*

	create random city stuff

*/
void CityCreator::MakeTheCity(FILE *output)
{
        int ran;
	int citysizex = (int) (building_size + objectspacing) + (1500 * qrand() / (RAND_MAX + 1.0));
	int citysizez = (int) (building_size + objectspacing) + (1500 * qrand() / (RAND_MAX + 1.0));
        float xcoord = RandomCoordinate(MapSize, CellSize);
        float zcoord = RandomCoordinate(MapSize, CellSize);
	int westEastLooper = 0;
	int southNorthLooper = 0;

        // create names txt file for this city.
        Names_Creator(xcoord, zcoord, citysizex, citysizez);

        float pos_x = xcoord;
        float pos_z = zcoord;

	// randomize rotation
	float rotation = buildingRotation();

        // X is west to east
        // Z is south to north
        pos_z++;
        while (pos_z != zcoord)
        {
        	// check if we have village or city
        	// village 250 or less, city larger ...
        	// except right now this has no meaning as all buildings are considered the same in CityObject array :(
		// TODO, randomize the angle of building to be 0, 90, 180, or 270.
        	if (citysizex && citysizez < 250)
        	{
	                ran = (qrand() % VillageBuildings);
			WriteP3D(VillageObjects[ran], pos_x, pos_z, rotation, output);
        	}
        	else
        	{
	                ran = (qrand() % CityBuildings);
			WriteP3D(CityObjects[ran], pos_x, pos_z, rotation, output);

			// random walls around the building
			ran = (qrand() % 75);
			if (ran > 50) WriteWalls(pos_x, pos_z, output);
        	}

                // advance west to east slot
                pos_x = (pos_x + building_size + objectspacing);

		// increment one for our checked how many buildings we placed from west to east
		westEastLooper ++;
		if (westEastLooper == 2)
		{
			westEastLooper = 0;
			pos_x += (building_size);
		}

                // if we have reached "east" end of our city size box.
                if (pos_x >= (xcoord + citysizex))
                {
                        // lift the coordinates one notch towards north.
			pos_z = (pos_z + building_size + objectspacing);
                        // roll back our west to east position to west.
                        pos_x = xcoord;

			// reset this so it wont "overflow" its on two building line, eh cant explain :)
			westEastLooper = 0;

			// increment one for our checked how many buildings we placed from south to north
			southNorthLooper ++;
			if (southNorthLooper == 4)
			{
				southNorthLooper = 0;
				pos_z += (building_size / 2);
			}
		}

                // if we have reached the north end of our city size box.
                if (pos_z >= (zcoord + citysizez)) break;
        }
}


/*

	Write the P3D file into a file

*/
void CityCreator::WriteP3D(char *P3DType, float dDirX, float dDirZ, float rotation, FILE *output)
{
        float elevation = 0;
        // strip the last 3 characters out of the p3d name string
        char tmpName[76];
        strcpy (tmpName,"");
        strncat (tmpName, P3DType, strlen(P3DType) - 4);

        fprintf(output,"\"%s\";%f;%f;%f;%f;\n", tmpName, dDirX, dDirZ, elevation, rotation);
}


/*

Temporary Wall makes

*/
void CityCreator::WriteWalls(float dDirX, float dDirZ, FILE *output)
{
/*
	"house_l_1_ep1";115;111.5;0;-90;	<- house, middle
	1 "wall_l_5m_ep1";125;100;0;-0;		<- wall, SE corner (front yard)
	2 "wall_l_5m_ep1";128;103;0;-90;		<- wall, south to north (start)
	3 "wall_l_5m_ep1";128;108;0;-90;		<- wall, south to north, south middle
	4 "wall_l_5m_ep1";128;113;0;-90;		<- wall, south to north, north middle
	5 "wall_l_5m_ep1";128;118;0;-90;		<- wall, south to north (end)
	6 "wall_l_5m_ep1";125;121;0;-0;		<- wall, east to west, eastern (start)
	7 "wall_l_5m_ep1";120;121;0;-0;
	8 "wall_l_5m_ep1";115;121;0;-0;
	9 "wall_l_5m_ep1";110;121;0;-0;
	10 "wall_l_5m_ep1";105;121;0;-0;		<- wall, east to west, western (end)
	11 "wall_l_5m_ep1";102;118;0;-90;		<- wall, north to south (start)
	12 "wall_l_5m_ep1";102;113;0;-90;
	13 "wall_l_5m_ep1";102;108;0;-90;
	14 "wall_l_5m_ep1";102;103;0;-90;		<- wall, north to south (end)
	15 "wall_l_5m_ep1";105;100;0;-0;		<- wall, SW corner (front yard)
*/
	char tmpName[] = "ca\\structures_e\\Wall\\Wall_L\\wall_l_5m_ep1";
	// 1
	dDirX = dDirX + 10;
	dDirZ = dDirZ - 11.5;
	fprintf(output,"\"%s\";%f;%f;%f;%f;\n", tmpName, dDirX, dDirZ, (double) 0, (double) 0);

	// 2
	dDirX = dDirX + 3;
	dDirZ = dDirZ + 3;
	fprintf(output,"\"%s\";%f;%f;%f;%f;\n", tmpName, dDirX, dDirZ, (double) 0, (double) -90);

	// 3
	dDirZ = dDirZ + 5;
	fprintf(output,"\"%s\";%f;%f;%f;%f;\n", tmpName, dDirX, dDirZ, (double) 0, (double) -90);

	// 4
	dDirZ = dDirZ + 5;
	fprintf(output,"\"%s\";%f;%f;%f;%f;\n", tmpName, dDirX, dDirZ, (double) 0, (double) -90);

	// 5
	dDirZ = dDirZ + 5;
	fprintf(output,"\"%s\";%f;%f;%f;%f;\n", tmpName, dDirX, dDirZ, (double) 0, (double) -90);

	// 6
	dDirX = dDirX - 3;
	dDirZ = dDirZ + 3;
	fprintf(output,"\"%s\";%f;%f;%f;%f;\n", tmpName, dDirX, dDirZ, (double) 0, (double) 0);

	// 7
	dDirX = dDirX - 5;
	fprintf(output,"\"%s\";%f;%f;%f;%f;\n", tmpName, dDirX, dDirZ, (double) 0, (double) 0);

	// 8
	dDirX = dDirX - 5;
	fprintf(output,"\"%s\";%f;%f;%f;%f;\n", tmpName, dDirX, dDirZ, (double) 0, (double) 0);

	// 9
	dDirX = dDirX - 5;
	fprintf(output,"\"%s\";%f;%f;%f;%f;\n", tmpName, dDirX, dDirZ, (double) 0, (double) 0);

	// 10
	dDirX = dDirX - 5;
	fprintf(output,"\"%s\";%f;%f;%f;%f;\n", tmpName, dDirX, dDirZ, (double) 0, (double) 0);

	// 11
	dDirX = dDirX - 3;
	dDirZ = dDirZ - 3;
	fprintf(output,"\"%s\";%f;%f;%f;%f;\n", tmpName, dDirX, dDirZ, (double) 0, (double) -90);

	// 12
	dDirZ = dDirZ - 5;
	fprintf(output,"\"%s\";%f;%f;%f;%f;\n", tmpName, dDirX, dDirZ, (double) 0, (double) -90);

	// 13
	dDirZ = dDirZ - 5;
	fprintf(output,"\"%s\";%f;%f;%f;%f;\n", tmpName, dDirX, dDirZ, (double) 0, (double) -90);

	// 14
	dDirZ = dDirZ - 5;
	fprintf(output,"\"%s\";%f;%f;%f;%f;\n", tmpName, dDirX, dDirZ, (double) 0, (double) -90);

	// 15
	dDirX = dDirX + 3;
	dDirZ = dDirZ - 3;
	fprintf(output,"\"%s\";%f;%f;%f;%f;\n", tmpName, dDirX, dDirZ, (double) 0, (double) 0);
}


/*

	write random vegetation objects

*/
void CityCreator::Write_Objects(FILE *output)
{
        char dObjName[76];
        float dDirX = 0, dDirZ = 0;
	int i = 0, ObjArrayCount = 0, rnd3d = 0;

	printf("\nMap x,y coord max size is: %i\n", (MapSize * CellSize));
        while (strlen(Rnd3dObjN[i]) > 0)
        {
                i++;
                ObjArrayCount++;
        }
	printf("Vegetation objects count. i: %i, ObjArrayCount: %i\n", i, ObjArrayCount);

        for (int cx = 0; cx < NumberOfObjects; cx++)
        {
		dDirX = RandomCoordinate(MapSize, CellSize);
		dDirZ = RandomCoordinate(MapSize, CellSize);

                // add the 3d objects randomly
                //rnd3d = 0 + (int) (ObjArrayCount * qrand() / (RAND_MAX + 1.0));
		rnd3d = 0 + (int) (qrand() % ObjArrayCount);

                sprintf(dObjName, Rnd3dObjN[rnd3d]);
                // write the p3d name into the export file
                // p3d, x, z, rotation, filename
                WriteP3D(dObjName, dDirX, dDirZ, (qrand() % 360), output);
        }
}


/*

	close files and display time used.

*/
void CityCreator::Close_Files(FILE *output, QTime t)
{
        fclose(output);

	printf("All done. Time elapsed: %d ms\n", t.elapsed());
}


/*

	Grab the time for the time used thing

*/
void CityCreator::Do_Time_Stuff()
{
        // initialize random seed:
        QTime timmy(0, 0, 0);
        qsrand(timmy.secsTo(QTime::currentTime()));

        // just get the time
        QTime t;
        t.start();
}

float CityCreator::RandomCoordinate(int MapSize, int CellSize)
{
	float coord;
	coord = ObjEdgeOffset + ( (qrand() * qrand()) % (MapSize * CellSize - (ObjEdgeOffset * 2)));
	return coord;
}

// no fucking idea why this doesnt work ON VISITOR3 (it works here in C++ ok).
float CityCreator::buildingRotation()
{
	float rotation;
	int ro = (qrand() % 4);
	switch (ro)
	{
		case 0:
		{
		    rotation = 0;
		    break;
		}
		case 1:
		{
		    rotation = 90;
		    break;
		}
		case 2:
		{
		    rotation = 180;
		    break;
		}
		case 3:
		{
		    rotation = -90; // this is supposed to be 270 but homo visitor / depew you know...
		    break;
		}
	}
	//printf("ro: %i, rotation: %f\n", ro, rotation);

	return rotation;
}
