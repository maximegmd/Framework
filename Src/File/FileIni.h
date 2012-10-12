//--------------------------------------------------------------------------------
/// @author Maxime GRIOT
//--------------------------------------------------------------------------------

#pragma once


#include <File/File.h>
#include <map>

namespace Framework
{
	namespace File
	{
		//--------------------------------------------------------------------//
		/// @class FileIni
		/// @brief Ini reader class
		//--------------------------------------------------------------------//
		class FileIni : public File
		{
		public :
			//--------------------------------------------------------------------//
			// Default constructor
			//--------------------------------------------------------------------//
			FileIni();

			//--------------------------------------------------------------------//
			/// @brief Construct from a filename.
			/// @param file   the name of the file
			//--------------------------------------------------------------------//
			FileIni(std::string file);

			//--------------------------------------------------------------------//
			/// @brief Default desctuctor
			//--------------------------------------------------------------------//
			~FileIni();

			//--------------------------------------------------------------------//
			/// @brief Open a new file and close the old one
			/// @param file   The name of the new file to open
			//--------------------------------------------------------------------//
			void setFile(const std::string& file);

			//--------------------------------------------------------------------//
			/// @brief Get the value in the file
			/// @param section    The name of the section
			/// @param champ      The key of the value needed
			//--------------------------------------------------------------------//
			std::string GetVal(const std::string& section, const std::string champ);

			//--------------------------------------------------------------------//
			/// @brief Get a number in the file
			/// @param section    The name of the section
			/// @param champ      The key of the value needed
			//--------------------------------------------------------------------//
			int GetInt(const std::string& section, const std::string champ);

			//--------------------------------------------------------------------//
			/// @brief Get a boolean value in the file
			/// @param section    The name of the section
			/// @param champ      The key of the value needed
			//--------------------------------------------------------------------//
			bool GetBool(const std::string& section, const std::string champ);

		protected :
			//--------------------------------------------------------------------//
			/// The opened file
			//--------------------------------------------------------------------//
			std::ifstream m_in;
		};
	}
}
