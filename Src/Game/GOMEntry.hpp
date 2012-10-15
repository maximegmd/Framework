#pragma once

#include <string>
#include <functional>

namespace Game
{
	class IGOMEntry
	{
	public:

		enum
		{
			kDirty = 1 << 0
		};

		virtual void Update() = 0;
		virtual void Synchronize() = 0;
		bool IsDirty();

		virtual std::string Serialize() const = 0;
		virtual void Deserialize(const std::string&) = 0;

		void SetDirty(bool);

	protected:

		template <class T>
		friend struct Verifier;

	private:

		unsigned char flags;
	};

	template <class Model>
	class GOMEntry : public IGOMEntry
	{
	public:

		GOMEntry(Model* m) 
			: model(m)
		{
		}

	protected:

		Model* model;
	};


}