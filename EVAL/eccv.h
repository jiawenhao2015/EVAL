static const int C = 320;
static const int R = 240;
static const int NPOINTS = C*R;

//struct Joint {
//  int32_t id;
//  float x,y,z;
//};
//struct Marker {
//  char name[255];
//  float x,y,z;
//};
//
//struct Frame {
//  int32_t   MAGIC_NUMBER;
//  int32_t   frame_number;
//  float     points[NPOINTS][3];
//  
//  int32_t   nmarkers;
//  struct Marker    markers[32]; // first nmarkers will be filled with markers
//  
//  int32_t   njoints;
//  struct Joint     joints[32];  // first njoints will be filled with joints
//};

struct Joint {
	int id;
	float x,y,z;
};
struct Marker {
	char name[255];
	float x,y,z;
};

struct Frame {
	int   MAGIC_NUMBER;
	int   frame_number;
	float     points[NPOINTS][3];

	int   nmarkers;
	struct Marker    markers[32]; // first nmarkers will be filled with markers

	int   njoints;
	struct Joint     joints[32];  // first njoints will be filled with joints
};


