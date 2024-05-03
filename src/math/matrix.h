#ifndef MATRIX_H_
#define MATRIX_H_

#include <math.h>

typedef struct {
	float data[4 * 4];
} mat4f;

static const float pi = 3.14159265358979323846;

static const mat4f mat4f_identity = {
	.data = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1,
	}
};

static inline mat4f mat4f_multiply(const mat4f a, const mat4f b) {
	return (mat4f) {
		.data = {
			a.data[0] * b.data[0] + a.data[4] * b.data[1] + a.data[8] * b.data[2] + a.data[12] * b.data[3],

			a.data[1] * b.data[0] + a.data[5] * b.data[1] + a.data[9] * b.data[2] + a.data[13] * b.data[3],
			a.data[2] * b.data[0] + a.data[6] * b.data[1] + a.data[10] * b.data[2] + a.data[14] * b.data[3],
			a.data[3] * b.data[0] + a.data[7] * b.data[1] + a.data[11] * b.data[2] + a.data[15] * b.data[3],

			a.data[0] * b.data[4] + a.data[4] * b.data[5] + a.data[8] * b.data[6] + a.data[12] * b.data[7],
			a.data[1] * b.data[4] + a.data[5] * b.data[5] + a.data[9] * b.data[6] + a.data[13] * b.data[7],
			a.data[2] * b.data[4] + a.data[6] * b.data[5] + a.data[10] * b.data[6] + a.data[14] * b.data[7],
			a.data[3] * b.data[4] + a.data[7] * b.data[5] + a.data[11] * b.data[6] + a.data[15] * b.data[7],

			a.data[0] * b.data[8] + a.data[4] * b.data[9] + a.data[8] * b.data[10] + a.data[12] * b.data[11],
			a.data[1] * b.data[8] + a.data[5] * b.data[9] + a.data[9] * b.data[10] + a.data[13] * b.data[11],
			a.data[2] * b.data[8] + a.data[6] * b.data[9] + a.data[10] * b.data[10] + a.data[14] * b.data[11],
			a.data[3] * b.data[8] + a.data[7] * b.data[9] + a.data[11] * b.data[10] + a.data[15] * b.data[11],

			a.data[0] * b.data[12] + a.data[4] * b.data[13] + a.data[8] * b.data[14] + a.data[12] * b.data[15],
			a.data[1] * b.data[12] + a.data[5] * b.data[13] + a.data[9] * b.data[14] + a.data[13] * b.data[15],
			a.data[2] * b.data[12] + a.data[6] * b.data[13] + a.data[10] * b.data[14] + a.data[14] * b.data[15],
			a.data[3] * b.data[12] + a.data[7] * b.data[13] + a.data[11] * b.data[14] + a.data[15] * b.data[15]
		}
	};
}

static inline mat4f mat4f_scale(const float x, const float y, const float z) {
	return (mat4f){
		.data = {
			x, 0, 0, 0,
			0, y, 0, 0,
			0, 0, z, 0,
			0, 0, 0, 1,
		}
	};
}

static inline mat4f mat4f_translation(const float x, const float y, const float z) {
	return (mat4f) {
		.data = {
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			x, y, z, 1,
		}
	};
}

static inline mat4f mat4f_rotate_z(const float theta) {
	return (mat4f) {
		.data = {
			cos(theta), sin(theta),  0, 0,
			-sin(theta), cos(theta), 0, 0,
					  0,		  0, 1, 0,
					  0,		  0, 0, 1,
		}
	};
}

static inline mat4f mat4f_rotate_y(const float theta) {
	return (mat4f) {
		.data = {
			cos(theta), 0, -sin(theta), 0,
				     0, 1,           0, 0,
			sin(theta), 0,  cos(theta), 0,
					 0, 0,           0, 1,
		}
	};
}

static inline mat4f mat4f_rotate_x(const float theta) {
	return (mat4f) {
		.data = {
			1,			0,			0,  0,
			0, cos(theta), sin(theta),  0,
			0, -sin(theta), cos(theta), 0,
			0,			 0,			 0, 1,
		}
	};
}

static inline mat4f mat4f_perspective(const float r, const float t, const float n, const float f) {
	return (mat4f) {
		.data = {
			n / r,	   0,				      0,  0,
			    0, n / t,	    			  0,  0,
				0,	   0,	 (-f - n) / (f - n), -1,
				0,	   0,   2 * f * n / (n - f),  0,
		}
	};
}

#endif // MATRIX_H_