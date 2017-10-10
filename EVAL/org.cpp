/////*
////* =====================================================================================
////*
////*       Filename:  main_eccv_data.cpp
////*
////*    Description:  plays back depth mocap data for ECCV 2012
////*        Version:  1.0
////*        Created:  11/10/2012
////*         Author:  Varun Ganapathi (varung@gmail.com),
////*
////*
////*         USAGE:   pass prefix of data files, e.g., seq_a, and it will play back all
////*                  items in that sequences, i.e., seq_a_0.bin, seq_1.bin, and so on.
////*
////*                  on ubuntu:
////*         COMPILE: g++ main_eccv_data.cpp -lglut
////* =====================================================================================
////*/
////#include <Windows.h>
////#include <vector>
////#include <cassert>
////#include <iostream>
////#include <fstream>
////#include <sstream>
////
////#ifdef __APPLE__ & __MACH__
////#define OSX
////#endif
////
////#ifdef OSX
////#include <GLUT/glut.h>
////#else
////// NOTE: you can change this to the standard GL headers if you don't have GLEW
//////#include <GL/glew.h>
////#include <GL/gl.h>
////#include <GL/freeglut.h>
//////#include <GL/glut.h>
////#endif
////
////using namespace std;
////
////int             paused = false;
////float           amp_lo = 4000;
////float           x_angle = 0,
////y_angle = 0;
////int             cur_file = 0;
////char           *data_set_dir = NULL;
////extern float    color_map[][3];
////extern int      color_map_size;
////
////
////float intrinsic[] = {
////	2.844376, 0.000000, 0.000000, 0.000000,
////	0.000000, 2.327216, 0.000000, 0.000000,
////	0.000000, 0.000000, -1.000020, -1.000000,
////	0.000000, 0.000000, -0.200002, 0.000000
////};
////
////#include "eccv.h"
////
////int win_h() { return glutGet(GLUT_WINDOW_HEIGHT); }
////int win_w() { return glutGet(GLUT_WINDOW_WIDTH); }
////
////void reshape(int w, int h){
////	glutPostRedisplay();
////}
////
////inline float clamp(float x, float lo, float hi){
////	if (x < lo) return lo;
////	if (x > hi) return hi;
////	return x;
////}
////
////Frame frame;
////
////ifstream points_ifs, markers_ifs, joints_ifs;
////
////void draw(Frame& f) {
////	glMatrixMode(GL_PROJECTION);
////	glLoadIdentity();
////	glLoadMatrixf(intrinsic);
////
////	// draw sensor
////	glEnable(GL_DEPTH_TEST);
////	glPointSize(4.0);
////	glMatrixMode(GL_MODELVIEW);
////	glLoadIdentity();
////
////	glTranslatef(0, 0, -2);
////	glRotatef(y_angle, 0, 1.0, 0);
////	glRotatef(x_angle, 1.0, 0, 0);
////	glTranslatef(0, 0, 2);
////
////	glColor3f(.7, .3, .3);
////	glBegin(GL_POINTS);
////
////	for (int i = 0; i<NPOINTS; ++i) {
////		int index = clamp(-(f.points[i][2] + 3.0), 0.0, 1.0) * (color_map_size - 1);
////		glColor3fv(color_map[index]);
////		glVertex3fv(f.points[i]);
////	}
////	glEnd();
////
////
////	// draw markers
////	glDisable(GL_DEPTH_TEST);
////	for (int i = 0; i<f.nmarkers; ++i) {
////		glPushMatrix();
////		glTranslatef(f.markers[i].x, f.markers[i].y, f.markers[i].z);
////		glColor3f(0, 0, 1);
////		glutWireSphere(.01, 10, 10);
////
////		glRasterPos3f(0, 0, 0);
////#ifndef OSX
////		glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)f.markers[i].name); // requires openglut or freeglut
////#endif
////		glPopMatrix();
////	}
////
////	for (int i = 0; i<f.njoints; ++i) {
////		glPushMatrix();
////		glTranslatef(f.joints[i].x, f.joints[i].y, f.joints[i].z);
////		glColor3f(1, 0, 0);
////		glutWireSphere(.01, 10, 10);
////
////		glColor3f(0, 0, 0);
////		glRasterPos3f(0, 0, 0);
////		char buf[255];
////		sprintf(buf, "%d", i);
////#ifndef OSX
////		glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)buf); // requires openglut or freeglut
////#endif
////		glPopMatrix();
////	}
////}
////
////bool read_frame() {
////	points_ifs.read((char*)&frame, sizeof(frame));
////
////	// open the next one
////	if (!points_ifs) {
////		points_ifs.clear();
////		return false;
////	}
////	return true;
////}
////
////void next_file() {
////	static int id = -1;
////	++id;
////	char buf[1024];
////	sprintf(buf, "%s_%d.bin", data_set_dir, id);
////	printf("trying to open: %s\n", buf);
////	points_ifs.clear();
////	if (points_ifs.is_open()) points_ifs.close();
////
////	points_ifs.open(buf, ifstream::binary);
////
////	if (!points_ifs) {
////		printf("ERROR: exiting. could not open: %s\n", buf);
////		exit(-1);
////	}
////	read_frame();
////}
////
////void idle() {
////	if (!paused) {
////		if (!read_frame()) { next_file(); }
////	}
////	glutPostRedisplay();
////}
////
////void rewind_one() {
////	points_ifs.clear();
////	long off = sizeof(Frame); off *= -2;
////	points_ifs.seekg(off, ifstream::cur);
////	if (!points_ifs){
////		points_ifs.clear();
////		points_ifs.seekg(0, ios_base::beg);
////	}
////	if (!read_frame()){
////		printf("failed to read file\n");
////	}
////}
////
////void key(unsigned char k, int x, int y) {
////	switch (k) {
////	case 'q':
////	case 27:
////		//glutLeaveMainLoop(); 
////		exit(0);
////		break;
////	case 'f':
////		if (!read_frame()) next_file();
////		break;
////	case 'b':
////		rewind_one();
////		break;
////
////	case 'p': paused = !paused; break;
////	case 'x': x_angle += .5; break;
////	case 'X': x_angle -= .5; break;
////	case 'y': y_angle += .5; break;
////	case 'Y': y_angle -= .5; break;
////	}
////}
////
////void display() {
////	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
////	glViewport(0, 0, win_w(), win_h());
////	draw(frame);
////	glutSwapBuffers();
////}
////
////int main(int argc, char* argv[])
////{
////	// INIT
////	glutInit(&argc, argv);
////	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
////	glutCreateWindow("TOF MOCAP PLAYER-Varun Ganapathi");
////
////	// IF YOU HAVE GLEW
////	// glewInit();
////
////	if (argc < 2){
////		printf("please specify file prefix for the sequences containing Stanford TOF MOCAP data (e.g., 'seq_a' to play 'seq_a_0.bin', 'seq_a_1.bin', etc.\n");
////		return -1;
////	}
////	data_set_dir = argv[1];
////	printf("Key commands: \n");
////	printf("q : quit \n");
////	printf("p : pause \n");
////	printf("f : next frame\n");
////	printf("b : previous frame\n");
////	printf("xXyY: rotate about axis\n");
////
////	glutReshapeWindow(512, 512);
////	glutDisplayFunc(display);
////	//glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
////	glutReshapeFunc(reshape);
////	glutKeyboardFunc(key);
////	glutIdleFunc(idle);
////	glClearColor(1, 1, 1, 1);
////	next_file();
////	glutMainLoop();
////	return 0;
////}
////
////float color_map[][3] = {
////	0, 0, 0.5625,
////	0, 0, 0.6250,
////	0, 0, 0.6875,
////	0, 0, 0.7500,
////	0, 0, 0.8125,
////	0, 0, 0.8750,
////	0, 0, 0.9375,
////	0, 0, 1.0000,
////	0, 0.0625, 1.0000,
////	0, 0.1250, 1.0000,
////	0, 0.1875, 1.0000,
////	0, 0.2500, 1.0000,
////	0, 0.3125, 1.0000,
////	0, 0.3750, 1.0000,
////	0, 0.4375, 1.0000,
////	0, 0.5000, 1.0000,
////	0, 0.5625, 1.0000,
////	0, 0.6250, 1.0000,
////	0, 0.6875, 1.0000,
////	0, 0.7500, 1.0000,
////	0, 0.8125, 1.0000,
////	0, 0.8750, 1.0000,
////	0, 0.9375, 1.0000,
////	0, 1.0000, 1.0000,
////	0.0625, 1.0000, 0.9375,
////	0.1250, 1.0000, 0.8750,
////	0.1875, 1.0000, 0.8125,
////	0.2500, 1.0000, 0.7500,
////	0.3125, 1.0000, 0.6875,
////	0.3750, 1.0000, 0.6250,
////	0.4375, 1.0000, 0.5625,
////	0.5000, 1.0000, 0.5000,
////	0.5625, 1.0000, 0.4375,
////	0.6250, 1.0000, 0.3750,
////	0.6875, 1.0000, 0.3125,
////	0.7500, 1.0000, 0.2500,
////	0.8125, 1.0000, 0.1875,
////	0.8750, 1.0000, 0.1250,
////	0.9375, 1.0000, 0.0625,
////	1.0000, 1.0000, 0,
////	1.0000, 0.9375, 0,
////	1.0000, 0.8750, 0,
////	1.0000, 0.8125, 0,
////	1.0000, 0.7500, 0,
////	1.0000, 0.6875, 0,
////	1.0000, 0.6250, 0,
////	1.0000, 0.5625, 0,
////	1.0000, 0.5000, 0,
////	1.0000, 0.4375, 0,
////	1.0000, 0.3750, 0,
////	1.0000, 0.3125, 0,
////	1.0000, 0.2500, 0,
////	1.0000, 0.1875, 0,
////	1.0000, 0.1250, 0,
////	1.0000, 0.0625, 0,
////	1.0000, 0, 0,
////	0.9375, 0, 0,
////	0.8750, 0, 0,
////	0.8125, 0, 0,
////	0.7500, 0, 0,
////	0.6875, 0, 0,
////	0.6250, 0, 0,
////	0.5625, 0, 0,
////	0.5000, 0, 0
////};
////
////int color_map_size = sizeof(color_map) / (sizeof(color_map[0]));
