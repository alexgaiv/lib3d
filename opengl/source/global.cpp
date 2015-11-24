#include "global.h"

GLuint Global::curProgram = 0;
GLenum Global::curTextureUnit = 0;

list<ProgramObject *> Global::shaders;
Matrix44f Global::modelview, Global::projection;
stack<Matrix44f> Global::mvStack, Global::projStack;

void Global::set_mv(const Matrix44f &mat)
{
	Matrix44f mvp = projection * mat;
	Matrix44f normalMatrix;
	bool fnorm = true;

	list<ProgramObject *>::iterator p;
	for (p = shaders.begin(); p != shaders.end(); p++)
	{
		ProgramObject *po = *p;

		if (po->HasNormalMatrix()) {
			if (fnorm) {
				mat.GetInverse(normalMatrix);
				normalMatrix = normalMatrix.GetTranspose();
				fnorm = false;
			}
			po->NormalMatrix(normalMatrix);
		}

		po->ModelView(mat);
		po->ModelViewProjection(mvp);
	}
}

void Global::set_proj(const Matrix44f &mat)
{
	Matrix44f mvp = mat * modelview;
	list<ProgramObject *>::iterator p;
	for (p = shaders.begin(); p != shaders.end(); p++)
	{
		ProgramObject *po = *p;
		po->Projection(mat);
		po->ModelViewProjection(mvp);
	}
}

void Global::SetModelView(const Matrix44f &mat)
{
	modelview = mat;
	if (shaders.size()) {
		set_mv(modelview);
	}
	else {
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(mat.data);
	}
}

void Global::SetProjection(const Matrix44f &mat)
{
	projection = mat;
	if (shaders.size()) {
		set_proj(projection);
	}
	else {
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(mat.data);
	}
}

void Global::MultModelView(const Matrix44f &mat)
{
	modelview *= mat;
	if (shaders.size()) {
		set_mv(modelview);
	}
	else {
		glMatrixMode(GL_MODELVIEW);
		glMultMatrixf(mat.data);
	}
}

void Global::MultProjection(const Matrix44f &mat)
{
	projection *= mat;
	if (shaders.size()) {
		set_proj(projection);
	}
	else {
		glMatrixMode(GL_PROJECTION);
		glMultMatrixf(mat.data);
	}
}