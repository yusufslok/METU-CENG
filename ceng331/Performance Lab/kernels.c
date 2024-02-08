/********************
 * Kernels to be optimized for the CS:APP Performance Lab
 ********************/

#include <stdio.h>
#include <stdlib.h>
#include "defs.h"
/*
 * Please fill in the following team_t struct
 */
team_t team = {
        "HY",    /* Team Name */

        "e2580751",    /* First student ID */
        "Hasan Kureli",    /* First student name */
        
        "e2521748",        /* Second student ID */
        "Yusuf Sami Lök"         /* Second student name */

};


/*******************
 * BATCHED MATRIX MULTIPLICATION \W SUM REDUCTION KERNEL
 *******************/

/***************************
 * Your different versions of the batched matrix multiplication functions go here
 ***************************/

/*
 * naive_batched_mm - The naive baseline version of batched matrix multiplication
 */
char naive_batched_mm_descr[] = "naive_batched_mm: Naive baseline implementation";
void naive_batched_mm(int dim, int *b_mat, int *mat, int *dst) {
    int i,j,k,l;
    
    for (i = 0; i < dim; i++) {
        for (j = 0; j < dim; j++) {
            for (k = 0; k < dim; k++) {
            	if (i == 0){
            	    dst[RIDX(j, k, dim)] = 0;
            	}
            	for (l = 0; l < dim; l++){
                    dst[RIDX(j, k, dim)] += b_mat[RIDX(i*dim+j, l, dim)] * mat[RIDX(l, k, dim)];
                }
            }
        }
    }
}

/*
 * batched_mm - Your current working version of batched matrix multiplication
 * IMPORTANT: This is the version you will be graded on
 */
char batched_mm_descr[] = "Batched MM with sum reduction: Current working version";

void batched_mm(int dim, int *b_mat, int *mat, int *dst)
{
    int i, j, k, l;
    int dim2 = dim * dim;
    int *temp = (int *)malloc(dim2 * sizeof(int));
    int *transpose = (int *)malloc(dim2 * sizeof(int));
    int *temp1 = temp;

    for (i = 0; i < dim2; i++)
    {
        *temp1++ = 0;
    }

    int *b_mat_temp = b_mat;

    for (i = 0; i < dim; i++)
    {
        temp1 = temp;
        for (j = 0; j < dim2; j++)
        {
            *temp1++ += *b_mat_temp++;
        }
    }
    int *transpose_temp = transpose;
    for (i = 0; i < dim; i++)
    {   
        int* temp_mat = mat+i;
        for (j = 0; j < dim; j++)
        {
            *transpose_temp++ = *temp_mat;
            temp_mat+=dim;
        }
    }    

    int *dst1 = dst;
    int acc0 = 0, acc1 = 0, acc2 = 0, acc3 = 0;
    int jdimll = 0;
    for (j = 0; j < dim; j++)
    {
        int kdim = 0;
        
        for (k = 0; k < dim; k++)
        {

            acc0 = 0;acc1 = 0;acc2 = 0;acc3 = 0;
            
            for (l = 0;l < dim; l += 32)
            {   
                int jdiml = jdimll + l;
                acc0 += temp[jdiml] * transpose[kdim] + temp[jdiml + 1] * transpose[kdim + 1 ] +
                        temp[jdiml + 2] * transpose[kdim + 2 ] + temp[jdiml + 3] * transpose[kdim + 3 ] +
                        temp[jdiml + 4] * transpose[kdim + 4 ] + temp[jdiml + 5] * transpose[kdim + 5 ] +
                        temp[jdiml + 6] * transpose[kdim + 6 ] + temp[jdiml + 7] * transpose[kdim + 7 ];

                acc1 += temp[jdiml + 8] * transpose[kdim + 8 ] + temp[jdiml + 9] * transpose[kdim + 9 ] +
                        temp[jdiml + 10] * transpose[kdim + 10 ] + temp[jdiml + 11] * transpose[kdim + 11 ] +
                        temp[jdiml + 12] * transpose[kdim + 12 ] + temp[jdiml + 13] * transpose[kdim + 13 ] +
                        temp[jdiml + 14] * transpose[kdim + 14 ] + temp[jdiml + 15] * transpose[kdim + 15 ] ;
            
                acc2 += temp[jdiml + 16] * transpose[kdim + 16 ] + temp[jdiml + 17] * transpose[kdim + 17 ] +
                        temp[jdiml + 18] * transpose[kdim + 18 ] + temp[jdiml + 19] * transpose[kdim + 19 ] +
                        temp[jdiml + 20] * transpose[kdim + 20 ] + temp[jdiml + 21] * transpose[kdim + 21 ] +
                        temp[jdiml + 22] * transpose[kdim + 22 ] + temp[jdiml + 23] * transpose[kdim + 23 ] ;

                acc3 += temp[jdiml + 24] * transpose[kdim + 24 ] + temp[jdiml + 25] * transpose[kdim + 25 ] +
                        temp[jdiml + 26] * transpose[kdim + 26 ] + temp[jdiml + 27] * transpose[kdim + 27 ] +
                        temp[jdiml + 28] * transpose[kdim + 28 ] + temp[jdiml + 29] * transpose[kdim + 29 ] +
                        temp[jdiml + 30] * transpose[kdim + 30 ] + temp[jdiml + 31] * transpose[kdim + 31 ] ;
                kdim += 32;
            }
            
            *dst1 = acc0 + acc1 + acc2 + acc3;

            dst1++;
        }
        jdimll += dim;
    }
    free(temp);
    free(transpose);
}
/***********************
 * register_batched_mm_functions - Register all of your different versions
 *     of the batched matrix multiplication functions  with the driver by calling the
 *     add_batched_mm_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.
 ***********************/

void register_batched_mm_functions() {
    add_batched_mm_function(&naive_batched_mm, naive_batched_mm_descr);
    add_batched_mm_function(&batched_mm, batched_mm_descr);
    /* ... Register additional test functions here */
}




/********
 * POINT REFLECTION KERNEL
 ********/

/*******************
 * Your different versions of the point reflection go here
 *******************/

/*
 * naive_reflect - The naive baseline version of point reflection
 */
char naive_reflect_descr[] = "Naive Point Reflection: Naive baseline implementation";
void naive_reflect(int dim, int *src, int *dst) {
    int i, j;

    for (i = 0; i < dim; i++) {
	for (j = 0; j < dim; j++) {
	    dst[RIDX(dim-1-i, dim-1-j, dim)] = src[RIDX(i, j, dim)];
	}
    }
}


/*
 * reflect - Your current working version of reflect
 * IMPORTANT: This is the version you will be graded on
 */
char reflect_descr[] = "Point Reflection: Current working version";
void reflect(int dim, int *src, int *dst)
{

    int i;
    int dim1 = dim*dim-1;
    int *dst1 = dst+dim1;
    int *src1 = src;
    for (i = 0; i <= dim1; i+=2) {
        *dst1 = *src1;
        *(dst1-1) = *(src1+1);
        dst1-=2;
        src1+=2;
        
    }

}

/**************************
 * register_reflect_functions - Register all of your different versions
 *     of the reflect with the driver by calling the
 *     add_reflect_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 **************************/

void register_reflect_functions() {
    add_reflect_function(&naive_reflect, naive_reflect_descr);
    add_reflect_function(&reflect, reflect_descr);
    /* ... Register additional test functions here */
}