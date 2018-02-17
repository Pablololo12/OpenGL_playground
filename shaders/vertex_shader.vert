#version 330 core
layout(location=0) in vec3 vpos;
layout(location=1) in vec3 vnos;
uniform mat4 view;
uniform vec3 light_pos;
uniform vec3 light_color;
uniform vec3 camera;
uniform vec3 obj_color;
uniform vec3 das;
out vec3 f_color;
void main()
{
	gl_Position = view*vec4(vpos,1.0);
	vec3 vec_light = normalize(light_pos-vpos);
	vec3 vec_v = normalize(camera-vpos);
	float l_power = 1/pow(distance(light_pos,vpos),2);
	float dot_l = dot(vec_light,vnos);
	vec3 difs = dot_l*l_power*obj_color*light_color; //Kd(N*L)Cd
	vec3 amb = obj_color*light_color; //ka Cd Cl
	vec3 spe = dot_l*pow(dot(reflect(-light_pos,vnos),vec_v),3)*obj_color*light_color*l_power; //ks (N*L)(R*V)Cs Cl
   f_color = difs*das.x+amb*das.y+spe*das.z;
}