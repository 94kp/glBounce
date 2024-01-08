#include "..\lib\karan.hpp"


void drawSphere(void)
{
	    // *** bind vao ***
    glBindVertexArray(gVao_sphere);

    // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

    // *** unbind vao ***
    glBindVertexArray(0);
}

void drawCube(void)
{
	
	glBindVertexArray(gVao_cube);

	// here there be dragons (drawing code)

	// glDrawArrays(GL_TRIANGLES, 0, 3);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

	glBindVertexArray(0);
}

void karan_code(mat4 translationMatrix, mat4 modelMatrix, mat4 viewMatrix, mat4 rotationMatrix)
{
	void push(mat4);
	mat4 pop();
	void drawSphere(void);
	void drawCube(void);

	translationMatrix = vmath::translate(0.0f, 0.0f, -3.0f);
	modelMatrix = translationMatrix;

	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	// modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	push(modelMatrix);

	// Provided You Already Had Done Matrices Related Task Up Till Here

	rotationMatrix = vmath::rotate((GLfloat)sphereRoll, 0.0f, 0.0f, 1.0f);
	modelMatrix = pop() * rotationMatrix;
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	drawSphere();

	translationMatrix = vmath::translate(0.0f, -2.0f, -3.0f);
	modelMatrix = translationMatrix;

	// glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	// glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	// glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	push(modelMatrix);

	mat4 scaleMatrix = vmath::scale(4.0f, 1.0f, 1.0f);

	modelMatrix = pop() * scaleMatrix;

	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	drawCube();
}

