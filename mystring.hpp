#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

class MyString{
public:
static std::vector<std::string> SplitString(const std::string& s, const std::string& c)
{
  std::vector<std::string> v;
  std::string::size_type pos1, pos2;
  pos2 = s.find(c);
  pos1 = 0;
  while(std::string::npos != pos2)
  {
    v.push_back(s.substr(pos1, pos2-pos1));
 
    pos1 = pos2 + c.size();
    pos2 = s.find(c, pos1);
  }
  if(pos1 != s.length())
    v.push_back(s.substr(pos1));
  return v;
}

static float string_to_float(std::string str){
	return atof(str.c_str());
}

template <typename T>
static std::string data_to_string(T f){
	std::stringstream strStream;
	strStream<<f;
	return strStream.str();
}

};

class MyFile{
public:
	static std::vector<std::string> readlines(std::string file_name){
        std::ifstream f;
        std::vector<std::string> v;
        f.open(file_name.c_str());
        std::string s;
        while(getline(f, s)){
            v.push_back(s);
        }
        return v;
	}

	static void file_append_line(std::string file_name, std::string line){
        std::ofstream f;
        f.open(file_name.c_str(), std::ios::app);
        f<<line<<std::endl;
        f.close();
	}
};
