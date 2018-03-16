#ifndef UKIVE_UTILS_HRESULT_UTILS_H_
#define UKIVE_UTILS_HRESULT_UTILS_H_


#define RH(x)    {HRESULT hr=(x); if(FAILED(hr)){ return hr;} }
#define RB(x)    {HRESULT hr=(x); if(FAILED(hr)){ return false;} }

#endif  // UKIVE_UTILS_HRESULT_UTILS_H_