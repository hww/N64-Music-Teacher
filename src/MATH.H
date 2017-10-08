/* 
 * Just some general maths stuff
 *
 * All angles are in degrees so have to convert to radians
 */

typedef float vec3_t[3];

#define VectorCopy(A,B) B[0]=A[0]; B[1]=A[1]; B[2]=A[2];
#define VectorScale(A,B,C) C[0]=A[0]*(B); C[1]=A[1]*(B); C[2]=A[2]*(B);
#define VectorAdd(A,B,C) C[0]=A[0]+B[0]; C[1]=A[1]+B[1]; C[2]=A[2]+B[2];

void AngleVectors (vec3_t angles, vec3_t out);

extern vec3_t vec3_origin;
