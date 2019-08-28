#pragma once

// #ifdef RLU_EXPORT
  #define RLU_DLL __declspec(dllexport)
// #endif

#ifdef RLU_IMPORT
  #define RLU_DLL __declspec(dllimport)
#endif

#ifdef RLU_NOTHING
  #define RLU_DLL
#endif
