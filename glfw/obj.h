#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <fstream>

using namespace std;

class OBJ
{
private:
	vector<glm::vec3>	v;
	vector<glm::vec3>	vn;
	vector<glm::vec3>	fv;
	vector<glm::vec3>	fvn;

	void load_v(ifstream& is);
	void load_vn(ifstream& is);
	void load_f(ifstream& is);

	void make_normals();

public:
	OBJ();
	OBJ(const string& name,const glm::mat4& xf=glm::mat4(1.0f));
	~OBJ();

	bool load(const string& name,const glm::mat4& xf=glm::mat4(1.0f));
	void normalize();

	const vector<glm::vec3>& faces() const { return fv; }
	const vector<glm::vec3>& normals() const { return fvn; }
};
