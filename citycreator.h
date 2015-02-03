#ifndef CITYCREATOR_H
#define CITYCREATOR_H

#include <QTime>

class CityCreator
{
public:
	CityCreator();
	
	FILE *output;
	char output_file[80];
	char VegetationObjectsFile[80];
	char VillageObjectsFile[80];
	char CityObjectsFile[80];
	int MapSize, NumberOfObjects, NumberOfCities, CellSize, MinCity, MaxCity, numOfNames, ObjEdgeOffset, Buildings;
	int CityBuildings,VillageBuildings;
	int objectspacing, building_size;
	// arrays
	char Rnd3dObjN[255][80];
	char CityObjects[255][80];
	char VillageObjects[255][80];

	void Do_Time_Stuff();
	void Check_Parameters(int argc, char *argv[]);
	void Write_Objects(FILE *output);
	void Close_Files(FILE *output, QTime t);
	void Names_Creator(float xcoord, float zcoord, int citysizex, int citysizez);
	void MakeTheCity(FILE *output);
	void WriteP3D(char *P3DType, float dDirX, float dDirZ, float rotation, FILE *output);
	void RunMe(int argc, char *argv[]);
	void ReadObjectsFile(char *filename);
	int ReadVillageConfigFile(char *filename);
	int ReadCityConfigFile(char *filename);
	float RandomCoordinate(int MapSize, int CellSize);
	void ReadConfigFile(char *filename);
	void WriteWalls(float dDirX, float dDirZ, FILE *output);
	float buildingRotation();
};

#endif // CITYCREATOR_H
