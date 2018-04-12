#include <iostream>
#include <fstream>

using namespace std;

int main() {
	ofstream myfile;
 	myfile.open ("ReadSemanticGroundTruth/labels.txt");

 	string line;
	ifstream file ("ReadSemanticGroundTruth/labels_inst.txt");
	int count = 1;
	if (file.is_open())
	{
		while ( file.good() )
		{
			getline (file,line);
			if (line.find("bgcolor")!= std::string::npos) {
				myfile << count << "\t";
				count++;
				getline (file,line);
				for (int i = 0; i < 3; i++) {
					getline (file,line);
					myfile << line.substr(4, line.length()-9) << "\t";
				}
				myfile << "\n";
			}
		}
		file.close();
	}
 	myfile.close();
}