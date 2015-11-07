#include "global.h"

list<ProgramObject *> Global::shaders;
Matrix44f Global::modelview, Global::projection;
stack<Matrix44f> Global::mvStack, Global::projStack;

void Global::set_mv(const Matrix44f &mat) {
	Matrix44f m;
	mat.GetInverse(m);
	m = m.GetTranspose();
	list<ProgramObject *>::iterator p;
	for (p = shaders.begin(); p != shaders.end(); p++) {
		ProgramObject *po = *p;
		po->Use();
		po->ModelView(mat.data);
		po->NormalMatrix(m.data);
	}
}

void Global::set_proj(const Matrix44f &mat) {
	list<ProgramObject *>::iterator p;
	for (p = shaders.begin(); p != shaders.end(); p++) {
		ProgramObject *po = *p;
		po->Use();
		po->Projection(mat.data);
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