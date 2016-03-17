/**
 * This file contains all self-written mathematic functions.
 * @author Peifeng Yin, Lu Ou, Michael Hunter, Sy-Miin Chow
 * @create Feb. 19, 2014
 */
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <math.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>
#include "math_function.h"

/**
 * This method computes the log-likelihood of a multivariate normal distribution.
 * @param x the variable (column) vector
 * @param det the determinate of the cov_matrix
 * @param inv_cov_matrix the covariance matrix
 * @return the negative log-likelihood
 */
double mathfunction_negloglike_multivariate_normal_invcov(const gsl_vector *x, const gsl_matrix *inv_cov_matrix, double det){
    /*printf("x(0)=%f\n", gsl_vector_get(x, 0));*/
    double result=0;
    gsl_vector *y=gsl_vector_calloc(x->size); /* y will save inv_cov_matrix*x*/
    double mu; /* save result of x'*inv_cov_matrix*x*/
    
    /** compute the log likelihood **/
    result=(x->size/2.0)*log(M_PI*2);
    result+=log(det)/2.0;
    
    gsl_blas_dgemv(CblasNoTrans, 1.0, inv_cov_matrix, x, 1.0, y); /* y=1*inv_cov_matrix*x+y*/
    gsl_blas_ddot(x, y, &mu);
    /*if(mu!=mu){
     printf("%f %f\n", gsl_vector_get(x, 0), gsl_vector_get(y, 0));
     }*/
    result+=mu/2.0;
    
    /** free allocated space **/
    gsl_vector_free(y);
    
    /*if(1){
     for(ri=0; ri<inv_cov_matrix->size1; ri++){
     for(ci=0; ci<inv_cov_matrix->size2; ci++)
     printf("%.3f ", gsl_matrix_get(inv_cov_matrix, ri, ci));
     printf("\n");
     }
     printf("x: ( ");
     for(ci=0; ci<x->size; ci++)
     printf("%.3f ", gsl_vector_get(x, ci));
     printf(")\n");
     }*/
    /*result=isfinite(result)?result:1e-4;*/
    return result;
}

/**
 * This method checks that a matrix is invertible by computing its determinant
 * It is assumed that gsl_linalg_LU_decomp(mat, per, &sing);
 * has been run on mat prior to checking invertibility
 */
int mathfunction_check_inv(const gsl_matrix *mat){
	double eps = 1.0e-6;
	double det = gsl_linalg_LU_det(mat, 1);
	if( fabs(det) > eps ){
		return 0;
	}
	else {
		return 1;
	}
}

/**
 * compute the inverse of a given matrix
 * @param mat the given matrix
 * @param inv_mat the matrix where the inverse one is stored.
 */
void mathfunction_inv_matrix(const gsl_matrix *mat, gsl_matrix *inv_mat){
    /** conduct LR decomposition **/
    gsl_permutation *per=gsl_permutation_alloc(mat->size1);
    gsl_matrix *cp_mat=gsl_matrix_alloc(mat->size1, mat->size2);
    gsl_matrix_memcpy(cp_mat, mat);
    int sing;
    /*size_t ri, ci;*/
    /*for(ri=0; ri<mat->size1; ri++){
     for(ci=0; ci<mat->size2; ci++)
     printf("%.3f ",gsl_matrix_get(mat, ri, ci));
     printf("\n");
     }*/
    gsl_linalg_LU_decomp(cp_mat, per, &sing);
    
    /*for(ri=0; ri<cp_mat->size1; ri++){
     for(ci=0; ci<cp_mat->size2; ci++)
     printf("%.3f ", gsl_matrix_get(cp_mat, ri, ci));
     printf("\n");
     }*/
    int rStatus;
    rStatus = mathfunction_check_inv(cp_mat);
    if(rStatus != 0){
        /*printf("Singular matrix found by mathfunction_inv_matrix().\n");*/
        gsl_matrix_set_all(inv_mat, 10000.0);
    } else {
        gsl_linalg_LU_invert(cp_mat, per, inv_mat);
    }
    
    /** free allocated space **/
    gsl_permutation_free(per);
    gsl_matrix_free(cp_mat);
}

/**
 * compute the inverse of a given matrix and returns the determinant
 * @param mat the given matrix
 * @param inv_mat the matrix where the inverse one is stored.
 */

double mathfunction_inv_matrix_det_lu(const gsl_matrix *mat, gsl_matrix *inv_mat){
    /** conduct LR decomposition **/
    gsl_permutation *per=gsl_permutation_alloc(mat->size1);
    gsl_matrix *cp_mat=gsl_matrix_alloc(mat->size1, mat->size2);
    gsl_matrix_memcpy(cp_mat, mat);
    int sing;
    double det=0;
    /*size_t ri, ci;*/
    /*for(ri=0; ri<mat->size1; ri++){
     for(ci=0; ci<mat->size2; ci++)
     printf("%.3f ",gsl_matrix_get(mat, ri, ci));
     printf("\n");
     }*/
    gsl_linalg_LU_decomp(cp_mat, per, &sing);
    det=gsl_linalg_LU_det(cp_mat, sing);
    /*printf("cp_mat:\n");*/
    /*for(ri=0; ri<cp_mat->size1; ri++){
     for(ci=0; ci<cp_mat->size2; ci++)
     printf("%.3f ", gsl_matrix_get(cp_mat, ri, ci));
     printf("\n");
     }*/
    /*if(det!=det){
     for(ri=0; ri<cp_cov_matrix->size1; ri++){
     for(ci=0; ci<cp_cov_matrix->size2; ci++)
     printf("%.2f ", gsl_matrix_get(cov_matrix, ri, ci));
     printf("\n");
     }
     }*/
    if(fabs(det) < 1.0e-6){
        /* printf("Singular matrix found by mathfunction_inv_matrix_det().\n"); */
        gsl_matrix_set_all(inv_mat, 10000.0);
    }
    else {
        gsl_linalg_LU_invert(cp_mat, per, inv_mat);
    }
    
    /** free allocated space **/
    gsl_permutation_free(per);
    gsl_matrix_free(cp_mat);
    return det;
}

/**

 * compute the inverse of a given matrix and returns the determinant
 * @param mat the given matrix
 * @param inv_mat the matrix where the inverse one is stored.
 */

double mathfunction_inv_matrix_det(const gsl_matrix *mat, gsl_matrix *inv_mat){
	gsl_set_error_handler_off();
	if(mat->size1 != mat->size2 || mat->size1 != inv_mat->size1 || inv_mat->size1 != inv_mat->size2){
		printf("Matrix for inversion is not square or not equal in size to inverse matrix.\n");
	}
	gsl_matrix_memcpy(inv_mat, mat);
	double det=0.0;
	int info = gsl_linalg_cholesky_decomp(inv_mat);
	det = mathfunction_cholesky_det(inv_mat);
	if(fabs(det) < 1.0e-6 || info == GSL_EDOM){
		/* printf("Singular or non-positive definite matrix found by mathfunction_inv_matrix_det().\n"); */
		gsl_matrix_set_all(inv_mat, 10000.0);
		det = 0.0;
	}
	else {
		gsl_linalg_cholesky_invert(inv_mat);
	}
	return det;
}

/**
 * compute the determinant of a Cholesky matrix
 * It's just the square of the product of the diagonal elements
 */
double mathfunction_cholesky_det(const gsl_matrix *mat){
	size_t i=0;
	double d=gsl_matrix_get(mat, i, i); /*d is the determinant*/
	for(i=1; i < mat->size1; i++){
		d*=gsl_matrix_get(mat, i, i);
	}
	d*=d;
	return d;
}


/**
 * print the given vector's value to the console
 * format (v1, v2, ... )
 */
void print_vector(const gsl_vector *y){
    if(y==NULL){
        printf("( NULL )");
        return;
    }
    size_t index;
    if(y->size<=0)
        return;
    printf("(%.3f",gsl_vector_get(y, 0));
    for(index=1; index<y->size; index++)
        printf(", %.3f", gsl_vector_get(y, index));
    printf(")");
}

/**
 * print given array to the console
 * format is [v1, ..., vn]
 */
void print_array(const double *v, int n){
    size_t index;
    if(n<=0)
        return;
    printf("[%.3f", v[0]);
    for(index=1; index<n; index++)
        printf(", %.3f", v[index]);
    printf("]");
}

/**
 * print the given matrix;
 */
void print_matrix(const gsl_matrix *mat){
    size_t ri, ci;
    if(mat->size1<=0 || mat->size2<=0)
        return;
    for(ri=0; ri<mat->size1; ri++){
        printf("  %.7f", gsl_matrix_get(mat, ri, 0));
        for(ci=1; ci<mat->size2; ci++)
            printf(", %.7f", gsl_matrix_get(mat, ri, ci));
        printf("\n");
    }
}

/**
 * This method computes C=A*B or C=A'*B or C=A*B' or C=A'*B'
 * @param mat_a the matrix of A
 * @param mat_b the matrix of B
 * @param transpose_a whether A will be transposed
 * @param transpose_b whether B will be transposed
 * @param mat_c the result of multiplication.
 */
void mathfunction_matrix_mul(const gsl_matrix *mat_a, const gsl_matrix *mat_b, bool transpose_a, bool transpose_b, gsl_matrix *mat_c){
    size_t ri=0, ci=0, index=0;
    double v=0, mul=0;
    size_t end=transpose_a?mat_a->size1:mat_a->size2;
    for(ri=0; ri<mat_c->size1; ri++){
        for(ci=0; ci<mat_c->size2; ci++){
            v=0;
            for(index=0; index<end; index++){
                if(transpose_a)
                    mul=gsl_matrix_get(mat_a, index, ri);
                else
                    mul=gsl_matrix_get(mat_a, ri, index);
                if(transpose_b)
                    mul*=gsl_matrix_get(mat_b, ci, index);
                else
                    mul*=gsl_matrix_get(mat_b, index, ci);
                v+=mul;
            }
            gsl_matrix_set(mat_c, ri, ci, v);
        }
    }
}

/**
 * This function sums the given vector
 */
double mathfunction_sum_vector(const gsl_vector *vec){
    double sum=0;
    size_t index=0;
    for(index=0; index<vec->size; index++)
        sum+=gsl_vector_get(vec, index);
    return sum;
}

/**
 * This function caculates the min of three numbers.
 */
double mathfunction_min(const double x,const double y,const double z){
    double mininmum;
    if (x<y){
        mininmum=x<z?x:z;
    }else{
        mininmum=y<z?y:z;
    }

    return mininmum;
}


/**
 * convert a matrix (e.g.,
 * [1 4 5
 *  4 2 6
 *  5 6 3]) to a vector (e.g., [1 2 3 4 5 6])
 * @param mat the given matrix
 */

void mathfunction_mat_to_vec(const gsl_matrix *mat, gsl_vector *vec){
    size_t i,j;
    size_t nx=mat->size1;
    /*convert matrix to vector*/
    for(i=0; i<nx; i++){
            gsl_vector_set(vec,i,gsl_matrix_get(mat,i,i));

    	for (j=i+1;j<nx;j++){
                gsl_vector_set(vec,i+j+nx-1,gsl_matrix_get(mat,i,j));
    	    /*printf("%lu",i+j+nx-1);}*/
    	}
    }
}

/**
 * convert a vector (e.g., [1 2 3 4 5 6]) to a matrix (e.g.,
 * [1 4 5
 *  4 2 6
 *  5 6 3])
 * @param vec the given vector
 */

void mathfunction_vec_to_mat(const gsl_vector *vec, gsl_matrix *mat){
    size_t i,j;
    size_t nx=mat->size1;
    /*convert vector to matrix*/
    for(i=0; i<nx; i++){
        gsl_matrix_set(mat,i,i,gsl_vector_get(vec,i));
    	for (j=i+1;j<nx;j++){
    	    gsl_matrix_set(mat,i,j,gsl_vector_get(vec,i+j+nx-1));
    	    gsl_matrix_set(mat,j,i,gsl_vector_get(vec,i+j+nx-1));
    	}
    }
}
/**
 * generate a random number ranges from 0 to 1
 */
double drand(){
    return (rand()+1.0)/(RAND_MAX+1.0);
}

/**
 * generate a random number satisfies standard normal distribution.
 */
double random_std_normal()
{
  return sqrt(-2*log(drand())) * cos(2*M_PI*drand());
}

/**
 * generate a random number satisfies any normal distribution
 */
double random_normal(double mu, double sigma){
    return random_std_normal()*sigma+mu;
}

/**
 * generate a vector of random numbers satisfies given normal distribution with zero mean.
 */
void white_noise(const gsl_vector *sigma, gsl_vector *noise){
    size_t index;
    for(index=0; index<sigma->size; index++)
        gsl_vector_set(noise, index, random_normal(0, gsl_vector_get(sigma, index)));
}

void random_pos_id_mat(gsl_matrix *mat){
    size_t row_index;

    gsl_matrix_set_zero(mat);
    for(row_index=0; row_index<mat->size1; row_index++){
        gsl_matrix_set(mat, row_index, row_index, abs(random_std_normal()));
        /*for(col_index=row_index+1; col_index<mat->size2; col_index++){
            v=abs(random_std_normal());
            gsl_matrix_set(mat, row_index, col_index, v);
            gsl_matrix_set(mat, col_index, row_index, v);
        }*/
    }
}

/**
 * This function scales vector A and store the result in B
 * @param vec_a vector A
 * @param vec_b the A*c of scaling
 */
void mathfunction_vec_scale(const gsl_vector *vec_a, const double x, gsl_vector *vec_b){
    size_t index=0;
    for(index=0; index<vec_a->size; index++){
        gsl_vector_set(vec_b, index, gsl_vector_get(vec_a,index)*x);
    }
}
/**
 * This function scales matrix A and store the result in B
 * @param mat_a vector A
 * @param mat_b the A*c of scaling
 */
void mathfunction_mat_scale(const gsl_matrix *mat_a, const double x, gsl_matrix *mat_b){
    size_t ri=0, ci=0;
    for(ri=0; ri<mat_a->size1; ri++){
        for(ci=0; ci<mat_a->size2; ci++){
            gsl_matrix_set(mat_b, ri, ci, gsl_matrix_get(mat_a,ri,ci)*x);
        }
    }
}
/**
 * This function convert a vector A into a matrix B so that the diagonal of matrix B is A*x
 * @param vec_a vector A
 * @param mat_b matrix B
 */
void mathfunction_diagin_scale(const gsl_vector *vec_a, const double x, gsl_matrix *mat_b){
    size_t ri=0;
    for(ri=0; ri<vec_a->size; ri++){
            gsl_matrix_set(mat_b, ri, ri, gsl_vector_get(vec_a,ri)*x);
    }
}

/**
 * This function convert a the diag of matrix A into a vector B so that B = diag(A)*x
 * @param mat_a matrix A
 * @param vec_b vector B
 */
void mathfunction_diagout_scale(const gsl_matrix *mat_a, const double x, gsl_vector *vec_b){
    size_t ri=0;
    for(ri=0; ri<mat_a->size1; ri++){
            gsl_vector_set(vec_b, ri, gsl_matrix_get(mat_a,ri,ri)*x);
    }
}

/**
 * Given a matrix of log-values, this method normalize each element to v~(0,1) and the sum of them will be equal to 1.
 * For example, given (-1, -2), the resulted value will be (e^{-1}, e^{-2})/(e^{-1}+e^{-2}).
 * @param log_v the target matrix values. After this function, the values in original one will be modified.
 * @return the normalizer
 */
double mathfunction_normalize_log(gsl_matrix *log_v){
    double max_v, min_v, sum=0;
    double temp_value;
    size_t row_index, col_index;
    gsl_matrix_minmax(log_v, &min_v, &max_v);
    gsl_matrix_add_constant(log_v, -1*(min_v+max_v)/2);
    
    /* now compute the exponential of each element as well as the sum*/
    for(row_index=0; row_index<log_v->size1; row_index++)
        for(col_index=0; col_index<log_v->size2; col_index++){
            temp_value=gsl_matrix_get(log_v, row_index, col_index);
            temp_value=exp(temp_value);
            sum+=temp_value;
            gsl_matrix_set(log_v, row_index, col_index, temp_value);
        }
    
    /* normalize all values*/
    gsl_matrix_scale(log_v, 1.0/sum);
    return sum;
}

/**
 * Given a vector of log-values, this method normalize each element to v~(0,1) and the sum of them will be equal to 1.
 * For example, given (-1, -2), the resulted value will be (e^{-1}, e^{-2})/(e^{-1}+e^{-2}).
 * @param log_v the target vector values. After this function, the values in original one will be modified.
 * @return the normalizer
 */
double mathfunction_normalize_log_vector(gsl_vector *log_v){
    double max_v=0, min_v=0, sum=0;
    double temp_value, temp_value2;
    size_t col_index;
    
    gsl_vector_minmax(log_v, &min_v, &max_v); /* find a bug for this function, returns minv as -nan*/
    /*printf("%f\n", min_v);*/
    if(min_v!=min_v){ /* sometimes the returned min_v is nan. But recall this function will solve the problem*/
        gsl_vector_minmax(log_v, &min_v, &max_v);
        /*printf("%f\n", min_v);*/
    }
    /*max_v=gsl_vector_get(log_v, 0);
     min_v=max_v;
     for(col_index=1; col_index<log_v->size; col_index++){
     temp_value=gsl_vector_get(log_v, col_index);
     if(max_v<temp_value)
     max_v=temp_value;
     if(min_v>temp_value)
     min_v=temp_value;
     }*/
    
    gsl_vector_add_constant(log_v, -1*(min_v+max_v)/2);
    
    /* now compute the exponential of each element as well as the sum*/
    for(col_index=0; col_index<log_v->size; col_index++){
        temp_value2=gsl_vector_get(log_v, col_index);
        temp_value=exp(temp_value2);
        /*printf("%f\n",temp_value);*/
        sum+=temp_value;
        gsl_vector_set(log_v, col_index, temp_value);
    }
    /* normalize all values*/
    gsl_vector_scale(log_v, 1.0/sum);
    return sum;
}

/**
 * This method normalize the given matrix's values so that the sum of them is equal to 1.
 * @param v the given matrix to be normalized.
 * @return the normalizer
 *
 */
double mathfunction_matrix_normalize(gsl_matrix *v){
    double sum=0;
    size_t row_index, col_index;
    for(row_index=0; row_index<v->size1; row_index++){
        for(col_index=0; col_index<v->size2; col_index++)
            sum+=gsl_matrix_get(v, row_index, col_index);
    }
    /*printf("%f",sum);*/
    gsl_matrix_scale(v, 1.0/sum);
    return sum;
}

/**
 * This method normalizes the given vector's values so that sum of them is equal to 1.
 * @param v the given vector to be normalized.

 */
double mathfunction_vector_normalize(gsl_vector *v){
    size_t index;
    double sum=0;
    for(index=0; index<v->size; index++)
        sum+=gsl_vector_get(v, index);
    gsl_vector_scale(v, 1.0/sum);
    return sum;
}

/**
 * This method computes the trace of the given matrix
 * @param mat the target matrix, make sure the matrix is a square one.
 * @return the trace
 */
double mathfunction_mat_trace(const gsl_matrix *mat){
    double tr=0;
    size_t index=0;
    for(index=0; index<mat->size1; index++)
        tr+=gsl_matrix_get(mat, index, index);
    return tr;
}

/**
 * This function takes a double and gives back a double
 * It computes the logistic function (i.e. the inverse of the logit link function)
 * @param x, the double value e.g. a normally distributed number
 * @return logistic(x), the double value e.g. a number between 0 and 1
 */
double mathfunction_logistic(const double x){
	double value = 1.0/(1.0 + exp(-x));
	return value;
}

/**
 * This function takes a gsl_vector and modifies its second argument (another gsl_vector)
 * It computes the softmax function (e.g. for multinomial logistic regression)
 * @param x, the double value e.g. a vector of normally distributed numbers
 * @param result, softmax(x), e.g. a numbers between 0 and 1 that sum to 1
 */
void mathfunction_softmax(const gsl_vector *x, gsl_vector *result){
	/* Elementwise exponentiation */
	size_t index=0;
	for(index=0; index < x->size; index++){
		gsl_vector_set(result, index, exp(gsl_vector_get(x, index)));
	}
	
	/* Sum for the scaling coeficient */
	double scale = 0.0;
	for(index=0; index < x->size; index++){
		scale += gsl_vector_get(result, index);
	}
	
	/* Multiply all elements of result by 1/scale */
	gsl_blas_dscal(1/scale, result);
}


