#ifndef _WGD_LOADER_H_
#define _WGD_LOADER_H_

#include <cadence/file.h>
#include <cadence/dstring.h>
#include <cadence/messages.h>
#include <vector>

#define LOADER(k)	static Loader* construct(cadence::File *file){ return new k(file); };
#define BASE_LOADER(k)	static const char *type() { return #k; }; static std::vector<LoaderStruct> s_map;
#define IMPLEMENT_LOADER(k)	std::vector<wgd::Loader::LoaderStruct> k::s_map;

//dllexport warning
#ifdef _MSC_VER
#pragma warning ( disable : 4275 )
#endif

namespace wgd {
	class Loader {
		public:

		//Example macro.
		//LOADER(Model3DS);

		Loader(cadence::File *f) : m_file(f) {}

		struct LoaderStruct {
			void *construct;
			void *validate;
		};

		cadence::File *file() const { return m_file; }

		//virtual void load(Resource *r, cadence::File *f) { std::cout << "Idiot\n"; }

		/**
		 * Detects resource file type and attempts to return the relevant loader.
		 * The template type specified is the base loader for that resource type.
		 */
		template <typename T>
		static T *create(cadence::File *file) {
			if (!file->open(cadence::File::READ)) {
				//ERROR GOES HERE.
				cadence::Error(0, cadence::dstring("The file '") + file->filename() + "' does not exist or cannot be opened\n");
				return 0;
			}

			for (unsigned int i=0; i<T::s_map.size(); i++) {
				if (((bool (*)(cadence::File*))T::s_map[i].validate)(file)) {
					file->close();
					return (T*)((Loader *(*)(cadence::File*))T::s_map[i].construct)(file);
				}
			}
			file->close();
			cadence::Error(0, cadence::dstring("I cannot create a ") + T::type() + " from the file '" + file->filename() + "'\n");
			return 0;
		}

		/**
		 * Register a loader for a specific file type. T is the actual loader type.
		 */
		template <typename T>
		static void regFileType() {
			LoaderStruct s;
			s.construct = (void*)T::construct;
			s.validate = (void*)T::validate;
			T::s_map.push_back(s);
		};

		static bool validate(cadence::File *) {
			cadence::DMsg msg(cadence::DMsg::WARNING);
			msg << "Please implement a 'static bool validate(File*)' method.\n";
			return false;
		}

		static Loader *construct(cadence::File *f) {
			cadence::DMsg msg(cadence::DMsg::WARNING);
			msg << "You forgot to use the LOADER macro.\n";
			return 0;
		}

		private:
		cadence::File *m_file;
	};
};

#endif
