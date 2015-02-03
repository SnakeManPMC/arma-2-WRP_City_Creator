#include <QtCore/QCoreApplication>
#include "citycreator.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	
	CityCreator city;
	city.RunMe(argc, argv);
	
	//return a.exec();
	return 0;
}
