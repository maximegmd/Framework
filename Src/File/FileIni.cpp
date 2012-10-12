//--------------------------------------------------------------------------------
/// @author Maxime GRIOT
//--------------------------------------------------------------------------------

#include <File/FileIni.h>

namespace Framework
{
	namespace File
	{
		FileIni::FileIni() : File() {}

		FileIni::FileIni(std::string file) : File (file.c_str())
		{
			m_in.open(fileName.c_str(),std::ios::in);
			std::string message = std::string("Can't open file ") + fileName;
			if (!m_in.is_open()) throw std::runtime_error(message.c_str());
		}

		FileIni::~FileIni()
		{
			if (m_in.is_open()) m_in.close();
		}

		void FileIni::setFile(const std::string& file)
		{
			setFileName(file);
			if (m_in.is_open()) m_in.close();
			m_in.open(fileName.c_str(),std::ios::in);
			if (!m_in.is_open())
			{
				std::string message = std::string("Can't open file ") + fileName;
				throw std::runtime_error(message.c_str());
			}
		}

		std::string FileIni::GetVal(const std::string& section, const std::string champ)
		{
			std::string buffer;
			std::string currentSection;
			std::string currentChamp;
			m_in.seekg(0,std::ios::beg);
			while (std::getline(m_in,buffer))
			{
				int posDeb = buffer.find('[');
				int posEnd = buffer.find(']');
				if(posDeb >= 0 && posEnd > posDeb)
				{
					currentSection = buffer.substr(posDeb+1,posEnd-1);
				}
				if (currentSection == section)
				{
					int posEgal = buffer.find('=');
					if(posEgal >= 0)
					{
						currentChamp = buffer.substr(0,posEgal);

						if (currentChamp == champ)
							return buffer.substr(posEgal+1,buffer.length()-posEgal-1);
					}
				}
			}
			// No value found :
			std::string message= std::string("Value not defined for the section [")+section+"],Key="+champ;
			throw std::runtime_error(message.c_str());
		}

		int FileIni::GetInt(const std::string& section, const std::string champ)
		{
			return atoi(GetVal(section,champ).c_str());
		}

		bool FileIni::GetBool(const std::string& section, const std::string champ)
		{
			return (GetVal(section,champ)==std::string("True")||GetVal(section,champ)==std::string("true"))?true:false;
		}
	}
}
