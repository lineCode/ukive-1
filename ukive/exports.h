#pragma once

#ifdef UKIVE_EXPORTS
#define UKIVE_API __declspec(dllexport)
#else
#define UKIVE_API __declspec(dllimport)
#endif