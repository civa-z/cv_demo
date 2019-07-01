#ifndef USER_H
#define USER_H

typedef unsigned char u8;

typedef struct {
    u8* input_data;
    int input_cols;
    int input_rows;
    float* M_data;
    u8* output_data;
    int output_cols;
    int output_rows;
    int start_line;
    int end_line;
} AffineTransInfo;

void* affine_transformation(void* affine_trans_info);

#endif

