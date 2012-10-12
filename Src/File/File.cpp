//--------------------------------------------------------------------------------
/// @author Maxime GRIOT
//--------------------------------------------------------------------------------

#include <File/File.h>
#include <direct.h>

namespace Framework
{
	namespace File
	{
		File::File(const char* file)
		{
			setFileName(std::string(file));
		}

		void File::setFileName(std::string file)
		{
			if (file.find(":") != file.npos || file.substr(0,2) == "\\\\")
				fileName = file;
			else
			{
				char szPath[MAX_PATH_SIZE+1];
				_getcwd(szPath, MAX_PATH_SIZE);
				std::string pathName = szPath;
				while(file.substr(0,2) == "..")
				{
					file.erase(0,3);
					if (pathName.find("\\") == file.npos) throw std::exception("Can't access File.");
					pathName.erase(pathName.find_last_of("\\"));
				}
				fileName = pathName + "\\" + file;
			}
		}

		std::string File::GetFileName()
		{
			if (fileName.find("\\") == fileName.npos) throw std::runtime_error ("Invalid Path.");
			return fileName.substr(fileName.find_last_of("\\")+1);
		}

		const std::string& File::GetFullPathName()
		{
			return fileName;
		}

		std::string File::GetExtension()
		{
			std::string file = GetFileName();
			if (file.find(".") != file.npos) throw std::runtime_error ("Can't find extension.");
			return file.substr(file.find(".")+1);
		}
	}
}
