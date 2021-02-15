#ifndef __DATA__
#define __DATA__

#include <map>
#include <vector>
#include "glm\glm.hpp"

std::vector< std::vector<glm::vec3> > point_clouds;
std::vector< std::vector<std::vector<glm::vec3>*> > vec_structures;
std::vector< int > triples_per_snapshot;
std::vector< std::vector<std::string> > vec_metadata;
std::vector< std::vector<std::string> > vec_metadata2;
std::vector< std::vector<std::string> > vec_metadata3;
std::vector< float > violet_green_ratio;
std::vector < std::map <std::string, float> > vec_freq;

// vector of snapshots, vector of boxes, vector of indices of grains in the boxes
std::vector < std::vector<  std::vector< unsigned int>* > > vec_boxes;
unsigned int  max_length_grain_name = 0;

#endif