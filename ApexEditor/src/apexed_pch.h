#pragma once

#define APEX_EDITOR

#include <iostream>
#include <fstream>
#include <iomanip>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <string>
#include <sstream>
#include <list>
#include <array>
#include <queue>
#include <stack>
#include <vector>
#include <map>
#include <filesystem>
namespace fs = std::filesystem;

#include "Apex/Core/Core.h"
#include "Apex/Core/Log.h"
#include "Apex/Utils/HashFNV1a.h"

#ifdef APEX_PLATFORM_WINDOWS
//	#include <Windows.h>
#endif
