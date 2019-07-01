#include <stdio.h>
#include <math.h>
#include "affine_transformation.h"

void* affine_transformation(void* affine_trans_info) {
    u8 *inPlane =     ((AffineTransInfo*)affine_trans_info)->input_data;
    int width =       ((AffineTransInfo*)affine_trans_info)->input_cols;
    int height =      ((AffineTransInfo*)affine_trans_info)->input_rows;
    float* M =        ((AffineTransInfo*)affine_trans_info)->M_data;
    u8 *outPlane =    ((AffineTransInfo*)affine_trans_info)->output_data;
    int ow =          ((AffineTransInfo*)affine_trans_info)->output_cols;
    int oh =          ((AffineTransInfo*)affine_trans_info)->output_rows;
    int oy_start =    ((AffineTransInfo*)affine_trans_info)->start_line;
    int oy_end =      ((AffineTransInfo*)affine_trans_info)->end_line;
    
	float M00 = M[0];
	float M01 = M[1];
	float M02 = M[2];
	float M10 = M[3];
	float M11 = M[4];
	float M12 = M[5];

	int ix, iy, ox, oy;
	float ifx, ify;
    float fx0, fx1, fy0, fy1;

    u8* ip1 = inPlane;
    u8* op1 = outPlane;

    for(oy = oy_start; oy < oy_end; ++oy)
    {
        for(ox = 0; ox < ow; ++ox)
        {
            ifx = M00 * ox + M01 * oy + M02;
            ify = M10 * ox + M11 * oy + M12;
            ix = (int)ifx;
            iy = (int)ify;
            
            if (ix < 0 || iy < 0 || ix > width || iy > height)
                continue;

            fx0 = ifx - ix;
            fx1 = 1.0 - fx0;
            fy0 = ify - iy;
            fy1 = 1.0 - fy0;

            //Y
#if 1
            op1[oy*3 * ow + ox*3] =
                ip1[(iy+0) * 3 * width+(ix+0) * 3] * fx1*fy1 +
                ip1[(iy+0) * 3 * width+(ix+1) * 3] * fx0*fy1 +
                ip1[(iy+1) * 3 * width+(ix+0) * 3] * fx1*fy0 +
                ip1[(iy+1) * 3 * width+(ix+1) * 3] * fx0*fy0;
            
            op1[oy*3 * ow + ox*3 +1] =
                ip1[(iy+0) * 3 * width+(ix+0) * 3 +1] * fx1*fy1 +
                ip1[(iy+0) * 3 * width+(ix+1) * 3 +1] * fx0*fy1 +
                ip1[(iy+1) * 3 * width+(ix+0) * 3 +1] * fx1*fy0 +
                ip1[(iy+1) * 3 * width+(ix+1) * 3 +1] * fx0*fy0;
            
            op1[oy*3 * ow + ox*3 +2] =
                ip1[(iy+0) * 3 * width+(ix+0) * 3 +2] * fx1*fy1 +
                ip1[(iy+0) * 3 * width+(ix+1) * 3 +2] * fx0*fy1 +
                ip1[(iy+1) * 3 * width+(ix+0) * 3 +2] * fx1*fy0 +
                ip1[(iy+1) * 3 * width+(ix+1) * 3 +2] * fx0*fy0;
#else
            op1[oy* 3 * ow + ox * 3] =     ip1[iy* 3 * width + ix * 3];
            op1[oy* 3 * ow + ox * 3 + 1] = ip1[iy* 3 * width + ix * 3 + 1];
            op1[oy* 3 * ow + ox * 3 + 2] = ip1[iy* 3 * width + ix * 3 + 2];
#endif
        }
    }
}

