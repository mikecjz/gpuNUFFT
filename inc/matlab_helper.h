#ifndef __MATLAB_HELPER_H
#define __MATLAB_HELPER_H

#include "mex.h"
#include "matrix.h"

#include "config.hpp" 

#ifdef __unix__ 
#include <unistd.h>
#elif defined _WIN32 
# include <windows.h>
#endif

#ifdef __unix__ 
	#include <sys/time.h>
#elif defined _WIN32 
	#include <time.h>
#endif

#define GET_TIME(now) { \
   struct timeval t; \
   gettimeofday(&t, NULL); \
   now = t.tv_sec + t.tv_usec/1000000.0; \
}

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))

template <typename TType>
void readMatlabInputArray(const mxArray *prhs[], int input_index, int highest_varying_dim, const char* name,TType** data, int* data_entries)
{
	int dummy;
	readMatlabInputArray<TType>(prhs, input_index, highest_varying_dim,name,data, data_entries,2,&dummy);
}

template <typename TType>
void readMatlabInputArray(const mxArray *prhs[], int input_index, int highest_varying_dim, const char* name,TType** data, int* data_entries, int max_nd, int* n_coils)
{
	int nd = mxGetNumberOfDimensions(prhs[input_index]); /* get coordinate dimensions */
	
	if (MATLAB_DEBUG)
		mexPrintf("number of dims %d\n",nd);

	const mwSize *dims = mxGetDimensions(prhs[input_index]);
    *n_coils = 1;
	if (nd == 2)
	{
		if(dims[0] != highest_varying_dim)//total: highest_varying_dim x N = 2
		{
			mexPrintf("dimensions of '%s' input array don't fit. Need to be %d x N but are %d x %d\n",name, highest_varying_dim, dims[0],dims[1]);
			mexErrMsgTxt ("Error occured!\n");
		}
	}
	else if (max_nd == 3 && nd == 3)
	{
		//multiple coil data passed
		*n_coils = dims[2];
		if (MATLAB_DEBUG)
			mexPrintf("number of coils %d\n",*n_coils);
	}
	else
	{
		mexPrintf("dimensions of '%s' input array don't fit. Need to be %d x N but are ",name, highest_varying_dim);
			for (int i=0; i<nd-1; i++)
				mexPrintf(" %d x ", dims[i]);
			if (nd > 1) mexPrintf(" %d\n", dims[nd-1]);
			else mexPrintf(" 0\n");
		mexErrMsgTxt ("Error occured!\n");
	}

	if (MATLAB_DEBUG)
	{
		mexPrintf("%s dimensions: ",name);
		for (int i=0; i<nd; i++)
			mexPrintf(" %d ",dims[i]);
		mexPrintf("\n");
	}
	
	*data_entries = dims[1];

	const mxArray *matlabData;
    matlabData = prhs[input_index];
	bool is_int = false;

	if (mxIsInt32(matlabData) || mxIsUint32(matlabData))
	{
		is_int = true;
		*data = ( TType*) mxGetData(matlabData);		
	}
	else
	{
		*data = ( TType*) mxGetPr(matlabData);
	}
	if (MATLAB_DEBUG)
	{
		for (int i = 0; i < MIN((highest_varying_dim * (*data_entries)),100); i++)//re, im
			if (is_int)
				mexPrintf("%s: %d, ",name,(*data)[i]);
			else
				mexPrintf("%s: %f, ",name,(*data)[i]);

		mexPrintf("\n");
	}
}

template <typename TType>
inline TType getParamField(const mxArray* params, const char* fieldname)
{
	const mxArray* data = mxGetField(params, 0, fieldname);
	if (mxIsInt32(data))
	{
		return (TType)(((TType*)mxGetData(data))[0]); 
	}
	else
	{
		return (TType)(((TType*)mxGetPr(data))[0]); 
	}
}

#endif