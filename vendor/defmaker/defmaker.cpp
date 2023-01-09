// defmaker - creates a DEF file from a list of libraries.
// The output DEF file will contain all _declspec(dllexport) symbols from the libraries.
// /def:<def file> must be added to the linker options for the DLL/EXE.
//
// Parameters:
//   defmaker <output.def> <library1.lib> <library2.lib> ...
//
// Part of the Plugin System tutorial
//
///////////////////////////////////////////////////////////////////////////////

#pragma warning( disable: 4786 ) // Identifier was truncated to 255 characters in the debug info

#include <stdio.h>
#include <windows.h>
#include <string>
#include <set>
#include <Dbghelp.h>

struct StrNCmp
{
	bool operator()(const std::string &s1, const std::string &s2) const
	{
		return _stricmp(s1.c_str(),s2.c_str())<0;
	}
};

std::set<std::string,StrNCmp> g_Names;

static const char *EXPORT_TAG[]=
{
	"/EXPORT:", // VC6 SP5, VC7.1, VC8.0
	"-export:", // VC6 SP6
};

static bool CmpTag( const char *data )
{
	for (int i=0;i<sizeof(EXPORT_TAG)/sizeof(EXPORT_TAG[0]);i++)
		if (_strnicmp(EXPORT_TAG[i],data,strlen(EXPORT_TAG[i]))==0)
			return true;
	return false;
}

static bool ParseLIB( const char *fname )
{
	int len=strlen(EXPORT_TAG[0]);

	bool err=true;
	// create a memory mapping of the LIB file
	HANDLE hFile=CreateFile(fname,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL|FILE_FLAG_RANDOM_ACCESS,0);
	if (hFile!=INVALID_HANDLE_VALUE) {
		HANDLE hFileMap=CreateFileMapping(hFile,NULL,PAGE_READONLY,0,0,0);
		if (hFileMap!=INVALID_HANDLE_VALUE) {
			const char *data=(const char *)MapViewOfFile(hFileMap,FILE_MAP_READ,0,0,0);
			if (data) {
				err=false;
				// search for the EXPORT_TAG and extract the symbols
				int size=GetFileSize(hFile,NULL);
				for (int i=0;i<size-len;i++)
					if (CmpTag(data+i)) {
						i+=len;
						const char *text=data+i;
						while (data[i]!=0 && data[i]!=' ' && data[i]!='/' && i<size)
							i++;
						std::string name(text,data+i);
						// add the symbols to a sorted set
						g_Names.insert(name);
					}
				UnmapViewOfFile(data);
			}
			CloseHandle(hFileMap);
		}
		CloseHandle(hFile);
	}
	return !err;
}

int main( int argc, char *argv[] )
{
	if (argc<3) {
		printf("defmaker: Not enough command line parameters.\n");
		printf("Usage: defmaker <def file> <libfiles>\n");
		return 1;
	}
	for (int i=2;i<argc;i++) {
		printf("!defmaker: Parsing library %s.\n",argv[i]);
		if (!ParseLIB(argv[i])) {
			printf("defmaker: Failed to parse library %s.\n",argv[i]);
			return 1;
		}
	}

	FILE *def=fopen(argv[1],"wt");
	if (!def) {
		printf("defmaker: Failed to open %s for writing.\n",argv[1]);
		return 1;
	}

	fprintf(def,"EXPORTS\n");
	for (std::set<std::string,StrNCmp>::iterator it=g_Names.begin();it!=g_Names.end();++it) {
		std::string name=*it;
		int len=name.size();
		if (len>5 && name[len-5]==',') name[len-5]=' '; // converts ",DATA" to " DATA"
		fprintf(def,"\t%s\n",name.c_str());
	}
	fclose(def);
	printf("defmaker: File %s was created successfully.\n",argv[1]);
	return 0;
}
