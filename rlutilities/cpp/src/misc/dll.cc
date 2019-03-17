#define NOMINMAX

#include <string>
#include <iostream>
#include <windows.h>

#include "simulation/field.h"
#include "mechanics/drive_path.h"

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {

  // Perform actions based on the reason for calling.
  switch(fdwReason) {
  
    case DLL_PROCESS_ATTACH: {
    
      std::cout << "Initializing rlutilities...\n";
      
      char path[256];
      GetModuleFileName(hinstDLL, path, 256);

      std::string dll_directory = std::string(path);

      auto last_slash = dll_directory.rfind("\\");
      dll_directory.erase(last_slash + 1);

      std::cout << "Located dll in directory: " << dll_directory << '\n';

      DrivePath::read_files(dll_directory);
      Field::read_mesh_files(dll_directory);
      
      break;
    
    }
    
    case DLL_THREAD_ATTACH: {
      
      // Do thread-specific initialization.
      
      break;
    
    }
    
    case DLL_THREAD_DETACH: {
      
      // Do thread-specific cleanup.
      
      break;
    
    }
    
    case DLL_PROCESS_DETACH: {
      
      // Perform any necessary cleanup.
      
      break;
    
    }
  
  }
  
  // Successful DLL_PROCESS_ATTACH.
  
  return TRUE;

}
