#ifndef __LOADER__
#define __LOADER__

#include <vector>
#include <iostream>
#include <sstream>
#include <istream>
#include <fstream>
#include <iterator>
#include <iomanip>

#include "glm\glm.hpp"

unsigned int max_structures = 0;
unsigned int max_triples = 0;
unsigned int grouplength = 0;

bool get_group_length(const std::string path, unsigned int* gl);


vector<string> split(const char *str, char c = ' ')
{
	vector<string> result;

	do
	{
		const char *begin = str;

		while (*str != c && *str)
			str++;

		result.push_back(string(begin, str));
	} while (0 != *str++);

	return result;
}


unsigned int load_structures(
		std::vector<std::vector<glm::vec3>*>* structures, 
		const std::string path, 
		std::vector<std::string> metadata, 
		std::map<std::string, float>* freq
	)
{

	unsigned int tot_structures;
	unsigned int tot_triples = 0;
	std::string line;

	// read the total number of structures
	std::string structures_path(path);
	structures_path += "\\structures";
	std::ifstream struct_file(structures_path.c_str());
	if (!struct_file.is_open()) {
		perror("Cannot open structures file, aborting.");
		exit(-2);
	}
	getline(struct_file, line);
	tot_structures = atoi(line.c_str());
	printf("   %d structures will be loaded.\n", tot_structures);

	if (tot_structures > max_structures)  max_structures = tot_structures;


	// read the structures for this snapshot from the separate files
	// all structures are stored in the same vector
	structures->clear();
	for (unsigned int i = 0; i<tot_structures; i++) {

		std::string newpath(path);
		newpath += "\\coordinate";
		newpath += std::to_string(i);

		FILE* fp;
		#pragma warning (disable : 4996)	
		fp = fopen(newpath.c_str(), "r");
		
		structures->push_back(new std::vector<glm::vec3>());

		// read values
		float var1, var2, var3;
		std::string line;
		while (fscanf(fp, "%f\t%f\t%f", &var1, &var2, &var3) != EOF) {
			structures->back()->push_back(glm::vec3(var1, var2, var3));
		}
		fclose(fp);
		
		tot_triples += structures->back()->size();
	}

	float z = 1.f / tot_triples;

	// read the indices of the structures for this snapshot from the separate files
	// use the indices to retrieve the types of grains
	// create a dictionary (freq) with hits for each grain type
	for (unsigned int i = 0; i<tot_structures; i++) {

		std::string newpath(path);
		newpath += "\\struttura";
		newpath += std::to_string(i);

		FILE* fp;
#pragma warning (disable : 4996)	
		fp = fopen(newpath.c_str(), "r");

		// read values
		unsigned int var1, var2, var3;
		while (fscanf(fp, "%d\t%d\t%d", &var1, &var2, &var3) != EOF) {

			auto success = freq->try_emplace(metadata[var1], 1);
			if (!success.second) freq->at(metadata[var1]) ++;
			
			success = freq->try_emplace(metadata[var2], 1);
			if (!success.second) freq->at(metadata[var2]) ++;
			
			success = freq->try_emplace(metadata[var3], 1);
			if (!success.second) freq->at(metadata[var3]) ++;
		}
		fclose(fp);

	}

	/*std::cout << " DUMP STRUCTURES\n";
	for (std::map<std::string, float>::iterator it = freq->begin(); it != freq->end(); ++it) {
		auto val = (it->second / tot_triples ) * 100;
		std::cout << it->first << "\t" << it->second << "\t" << val << std::endl;
	}*/

	for (std::map<std::string, float>::iterator it = freq->begin(); it != freq->end(); ++it) {
		it->second /= tot_triples;				
		it->second *= 100;
	}


	return tot_triples;

}

std::string number_with_leading_zeros(int number, int howmany=3) {
	std::stringstream ss;
	ss << std::setw(howmany) << std::setfill('0') << number;
	std::string s = ss.str();
	return s;
}


/* load the point cloud from the file */
unsigned int load_point_cloud(
	std::vector<glm::vec3>* ndp, 
	std::vector<std::string>* meta, 
	std::vector<std::string>* meta2, 
	std::vector<std::string>* meta3, 
	const std::string path
) {
	
	std::cout << " * Loading " << path.c_str() << std::endl;

	FILE* fp;
	#pragma warning (disable : 4996)	
	fp = fopen(path.c_str(), "r");	

	unsigned int number_of_lines = 0;
    int ch;

	if (fp==NULL) {
		perror("Cannot open snapshot");
		exit(100);
	}

	// count rows
	while (EOF != (ch=getc(fp)))  if ('\n' == ch) ++number_of_lines;
	rewind(fp);

	std::string type;
	std::string boh;
	int id = 0;
    float var1 = 0, var2 = 0, var3 = 0;
	unsigned int row =0;
	
	while(row<number_of_lines-1)  {			
		char line [100];
		char line2 [100];
		if (row<2) {			
			fgets(line, sizeof line, fp);
		} else {

			//  line = grain name
			fscanf(fp, "%s %s %d %f %f %f", &line, &line2, &id, &var1, &var2, &var3);
			ndp->push_back(glm::vec3(var1,var2,var3));
			
			std::string stringline = std::string(line);

			unsigned int i;
			for (i = 0; i < stringline.length(); i++) {
				if (!isdigit(line[i])) break;
			}
			
			std::string s = number_with_leading_zeros(atoi(line));

			// printf("%s (%d)\t", s.c_str(),i);

			meta->push_back(
				// s + "-" + 
				stringline.substr(i, stringline.size())
			);		
			if (meta->back().size() > max_length_grain_name)
				max_length_grain_name = meta->back().size();

			meta2->push_back(line2);		
			meta3->push_back( std::to_string( (row-2) / grouplength ));

			if(var1>max_coord.x) max_coord.x=var1;
			if(var2>max_coord.y) max_coord.y=var2;
			if(var3>max_coord.z) max_coord.z=var3;

			if(var1<min_coord.x) min_coord.x=var1;
			if(var2<min_coord.y) min_coord.y=var2;
			if(var3<min_coord.z) min_coord.z=var3;
			
		}
		row ++;		
	}
	fclose(fp);	

	initial_translation =  (max_coord+min_coord)*-.5f;

	return ndp->size();
		
}


#endif