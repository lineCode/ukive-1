#ifndef UKIVE_UTILS_HRESULT_UTILS_H_
#define UKIVE_UTILS_HRESULT_UTILS_H_


#define RH(x)    {HRESULT hr=(x); if(FAILED(hr)){ return hr;} }

#endif  // UKIVE_UTILS_HRESULT_UTILS_H_