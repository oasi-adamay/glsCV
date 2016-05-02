#ifndef _GLS_FBO_H_
#define _GLS_FBO_H_

class glsFBO{
private:
	GLuint fbo;
public:
	glsFBO(const int attachment_num = 1);
	~glsFBO(void);
};

#endif
