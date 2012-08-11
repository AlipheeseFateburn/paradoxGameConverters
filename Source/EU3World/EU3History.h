#ifndef EU3HISTORY_H_
#define EU3HISTORY_H_


#include "..\Date.h"
#include "..\Mappers.h"
#include <fstream>


class CK2History;
class EU3Ruler;
class EU3Advisor;

class EU3History {
	public:
		EU3History(date when);
		EU3History(CK2History*);

		void				output(FILE*);

		date				when;
		EU3Ruler*		monarch;
		EU3Ruler*		regent;
		EU3Ruler*		heir;
		EU3Advisor*		advisor;
		string			capital;
		double			population;
		string			owner;
		string			culture;
		string			religion;
		vector<string>	discoverers;

	private:
		EU3History();
};


#endif // EU3HISTORY_H_