#ifndef _GLS_VAO_H_
#define _GLS_VAO_H_

class glsVAO{
private:
	GLuint vao;
	GLuint vbo;
public:
	glsVAO(GLuint attrLoc);
	~glsVAO(void);
};



#endif
