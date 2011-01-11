/*
 * includes/doste/object.h
 * 
 * An base class wrapper to map C++ objects to DOSTE objects.
 * Copyright (C) 2008 Nicolas Pope
 *
 * Author(s) : Nicolas Pope
 * Date      : 2008
 * Home Page : http://www.doste.co.uk
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */


#ifndef _doste_OBJECT_H_
#define _doste_OBJECT_H_

#include <cadence/dmsg.h>
#include <cadence/doste/oid.h>
//#include <doste/agent.h>
#include <cadence/map.h>
#include <cadence/list.h>
#include <cadence/doste/type_traits.h>

#define OBJECT_BASE(j)		static cadence::doste::OID type() { return #j; }; \
				static cadence::Object* construct(const cadence::doste::OID &o){ return new j( o ); }; \
				virtual bool isa(const cadence::doste::OID &t) { if (type() == t) return true; else return false; };

#define OBJECT(k,j)	static cadence::doste::OID type() { return #j; }; \
			static cadence::Object* construct(const cadence::doste::OID &o){ return new j( o ); }; \
			virtual bool isa(const cadence::doste::OID &t) { if (type() == t) return true; else return k::isa(t); }; 

			/*void operator delete(void *p) { free(p); }
			void *operator new(size_t s, bool nop, bool nop2) { return malloc(s); } \
			void *operator new(size_t s) { OID b = doste::OID::create(); b.set("type", #j); return Object::convert<j>(b); } \
			void *operator new(size_t s, const doste::OID &b) { return Object::convert<j>(b); }*/

#define VOBJECT(k,i)	static cadence::doste::OID type() { return #i; }; \
			virtual bool isa(const cadence::doste::OID &t) { if (type() == t) return true; else return k::isa(t); } \
			
//#undef THIS
//#define THIS static_cast<doste::dvm::OID>(*this)

#ifdef WIN32
#define PROPERTY_R(T, A)	cadence::make_pointer< T, cadence::doste::is_pod<T>::value >::type __stdcall A() const { return get_value<T>(#A, cadence::doste::is_pod<T>()); };
#define PROPERTY_RF(T, A, B)	cadence::make_pointer< T, cadence::doste::is_pod<T>::value >::type __stdcall A() const { return get_value<T>(B, cadence::doste::is_pod<T>()); };
#else
#define PROPERTY_R(T, A)	cadence::make_pointer< T, cadence::doste::is_pod<T>::value >::type A() const { return get_value<T>(#A, cadence::doste::is_pod<T>()); };
#define PROPERTY_RF(T, A, B)	cadence::make_pointer< T, cadence::doste::is_pod<T>::value >::type A() const { return get_value<T>(B, cadence::doste::is_pod<T>()); };
#endif

#define PROPERTY_W(T, A)	void A(const cadence::make_pointer< T, cadence::doste::is_pod<T>::value >::type &v) { set_value<T>(#A, v, cadence::doste::is_pod<T>()); };
#define PROPERTY_WF(T, A, B)	void A(const cadence::make_pointer< T, cadence::doste::is_pod<T>::value >::type &v) { set_value<T>(B, v, cadence::doste::is_pod<T>()); };

#define PROPERTY_RW(T, A)	PROPERTY_R(T,A); PROPERTY_W(T,A)
#define PROPERTY_RWF(T,A,B)	PROPERTY_RF(T,A,B); PROPERTY_WF(T,A,B)
#define PROPERTY(T,A)		PROPERTY_RW(T,A)
#define PROPERTY_F(T,A,B)	PROPERTY_RWF(T,A,B)

namespace cadence {

	/*
	 * Templates to optionally make a type into a pointer at compile time.
	 * Used by PROP_GET and PROP_SET.
	 */
	template <typename T, bool X>
	struct make_pointer { typedef T type; };
	template <typename T>
	struct make_pointer<T, false> { typedef T* type; };

	/**
	 * A base class that can be inherited by any class that wishes to represent
	 * a type of object stored within the database. It provides a means of
	 * constructing an object automatically from a database object and a means
	 * of easily accessing data in the database.<br><br>
	 * Anything that does inherit from the class requires a public OBJECT macro
	 * to be added to the class definition. This takes two arguments, first the
	 * name of the class it inherits from and second its own class name. In
	 * addition to this the constructor must take an OID parameter which is
	 * passed down to Object.<br><br>
	 * Before the system can construct objects of your type you must register it
	 * with Object::registerType&lt;T&gt;();<br><br>
	 * Generally you would want to inherit Agent instead which provides extra
	 * functionality in the form of event handling.
	*/
	class XARAIMPORT Object : public cadence::doste::OID {
		public:
		
		OBJECT_BASE(Object);
		
		/**
		 * Make a new object with a new OID. This may be called directly because type
		 * does not matter in this case. Make sure that you set the type attribute in
		 * you top most constructor.
		 */
		Object() : OID(OID::create()) {
			s_map.add(*this, (void*)this);
			s_list.addFront(this);
			noremove = false;
			//std::cout << "MAKING C++ OBJECT: " << d() << "\n";
		};
		
		/**
		 * Make an object from an object in the database. This object should be
		 * of the correct type and so it is best if you use the convert() function
		 * instead of directly using this constructor.
		 * @param obj The object id for the database object.
		 */
		Object(const cadence::doste::OID &obj)
		 : OID(obj) {
		 	s_map.add(*this, (void*)this);
			s_list.addFront(this);
			noremove = false;
			//std::cout << "MAKING C++ OBJECT: " << d() << "\n";
		 };
		 
		virtual ~Object() {
			if (!noremove) {
				s_list.removeAll(this);
				s_map.remove(*this);
			}
		};

		void setID(const OID &o) { m_a = o.a(); m_b = o.b(); m_c = o.c(); m_d = o.d(); };

		/** @return The OID for this C++ object */
		operator OID() {
			return static_cast<cadence::doste::OID>(*this);
		};
		
		/**
		 * Get an object pointer for an object contained within this one.
		 * @param k The attribute name where the object is.
		 * @return Pointer to C++ object or null.
		 */
		template <typename T>
		T *getObject(const OID &k) const { return Object::convert<T>(get(k)); };

		/**
		 * Register a C++ class as a valid object type. This allows objects of
		 * this type to be constructed automatically by the convert() function.
		 * The template parameter is your type.
		 */
		template <typename T>
		inline static void registerType();
		
		/**
		 * Convert a DOSTE OID into a C++ object pointer of the correct type.
		 * If the conversion cannot be performed then NULL is returned and an
		 * error is generated telling you why.
		 * @param o The database object.
		 * @return C++ pointer for object or NULL.
		 */
		template <typename T>
		inline static T *convert(const OID &o);

		/**
		 * Delete all C++ objects that have ever been converted.
		 */
		static void destroyAll() {
			//Iterate over map and delete objects.
			for (cadence::List<Object*>::iterator i=s_list.begin(); i!=s_list.end(); i++) {
				(*i)->noremove = true;
				delete *i;
			}
			s_list.clear();
			s_map.clear();
		};

		protected:
		/*
		 * Template selector to choose how a value or object should be
		 * retrieved from the database. Is it a primitive type or an Object?
		 */
		template <typename T>
		inline T get_value(const OID &k, doste::true_type) const { return (T)((OID)(*this)[k]); };
		template <typename T>
		inline T *get_value(const OID &k, doste::false_type) const { return getObject<T>(k); };

		template <typename T>
		inline void set_reuse(const OID &k, const T &v, doste::true_type) {
			doste::OID o = get(k);
			if (o == doste::Null) {
				o = doste::OID::create();
				set(k, o);
			}
			v.fillObject(o);
		}
		template <typename T>
		inline void set_reuse(const OID &k, const T &v, doste::false_type) { set(k,v,false); }

		template <typename T>
		inline void set_value(const OID &k, const T &v, doste::true_type) { set_reuse(k,v, doste::is_reuseable<T>()); };
		template <typename T>
		inline void set_value(const OID &k, const T *v, doste::false_type) { set(k,*v, false); };
		
		private:
		bool noremove;

		static cadence::Map s_map;
		static cadence::Map s_reg;
		static cadence::List<Object*> s_list;
	};
};

template <typename T>
T *cadence::Object::convert(const OID &o) {
	//not an object
	if(o==doste::Null) return 0;
	
	//Lookup C++ pointer in map
	Object *obj = (Object*)(void*)s_map[o];
	
	if (obj != 0) {
		//Check RTTI for valid type.
		if (obj->isa(T::type())) return (T*)obj;
		else {
			DMsg msg(DMsg::WARNING);
			msg << o << " is not a " << T::type() << "\n";
			return 0;
		}
	} else {
		//Haxy solution to automatically set type on first use.
		doste::OID ttype = o.get(doste::Type);
		if (ttype == doste::Null) {
			o.set(doste::Type, T::type(), true);
			ttype = T::type();
		}
		
		
		//If not exist then construct as correct object type
		if (s_reg[ttype] != doste::Null) {
			obj = ((Object *(*)(const OID &))(void*)(s_reg[ttype]))(o);
			//Check RTTI again
			if (o.get(doste::Type)==doste::Null || obj->isa(T::type())) return (T*)obj;
			else {
				DMsg msg(DMsg::WARNING);
				msg << o << " is not a " << T::type() << "\n";
				return 0;
			}
		} else {
			//Give a warning about unregistered type
			if (o.get(doste::Type) != doste::Null) {
				DMsg msg(DMsg::WARNING);
				msg << "What is a " << ttype << "\n";
			}
			return 0;
		}
	}
}

template <typename T>
void cadence::Object::registerType() {
	s_reg.add(T::type(), (void*)T::construct);
}


template <typename T>
T *cadence::doste::OID::get(const OID &o) const {
	return cadence::Object::convert<T>(*this);
}

template <typename T>
void cadence::doste::OID::set(const OID &key, T *value, bool async) const {
	set(key, *value, async);
}

namespace cadence {
	namespace doste {
		template <>
		inline void OID::set<void>(const OID &key, void *value, bool async) const {
			set(key, OID(value), async);
		}
	};
};

template <typename T>
T *cadence::doste::OID::get_pointer(false_type) const {
	return cadence::Object::convert<T>(*this);
}

#endif
