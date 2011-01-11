/*
 * includes/cadence/doste/oid.h
 * 
 * Object IDentifier class with C++ wrappers for object access.
 * Copyright (C) 2008 Nicolas Pope
 *
 * Author(s) : Nicolas Pope
 * Date      : 9/11/2007
 * Home Page : http://www.cadence.co.uk
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
 
#ifndef _cadence_DVM_OID_H_
#define _cadence_DVM_OID_H_

#include <cadence/dll.h>
#include <cadence/doste/type_traits.h>

//Because this file is included everywhere...
//#define NEW new(__FILE__, __LINE__)

#include <iostream>

/*inline void *operator new(size_t size, const char *filename, int line) {
	std::cout << "Allocate: " << size << " "<< filename << ":" << line << "\n";
	return malloc(size);
}*/
 
namespace cadence {
	class Object;

	namespace doste {

		class Buffer;
		class Definition;
		class OIDAccessor;
	
		/**
		 * Object Identifier. A 128bit number that uniquely identifies an
		 * object within the DVM. This class provides wrapper methods for
		 * accessing and changing attributes within the object referred to
		 * by this OID. These wrappers generate the required events.
		 * @author Nicolas Pope
		 */
		#ifdef _MSC_VER
		#pragma pack(1)
		#endif
		class DVMIMPORT OID {
			public:
			/** Null constructor. */
			OID() : m_hl(0), m_ll(0) {};
			/** Manually specify the OID components when constructing */
			OID(unsigned int a, unsigned int b, unsigned int c, unsigned int d)
				: m_b(b), m_a(a), m_d(d), m_c(c) {};
			/** Create an OID from an integer */
			OID(int v)
				: m_b(1), m_a(0), m_ll(v) {};
			/** Create an OID from an unsigned integer */
			OID(unsigned int v)
				: m_b(1), m_a(0), m_ll(v) {};
			/** Create an OID from a float */
			OID(float v)
				: m_b(2), m_a(0) {
				m_dbl = (double)v;
			};
			/** Create an OID from a boolean */
			OID(bool v)
				: m_b(0), m_a(0), m_d((v)?1:2), m_c(0) {};
			/** Create an OID from a name. This name should not contain white space or special characters. */
			OID(const char *v);
			/** Create an OID from a 64bit long long integer. */
			OID(long long v)
				: m_b(1), m_a(0) {
				m_ll = v;	
			};
			/** Create an OID from a 64bit double */
			OID(double v)
				: m_b(2), m_a(0) {
				m_dbl = v;
			};
			/** Create an OID from an ascii character. */
			OID(char v)
				: m_b(3), m_a(0), m_d(v), m_c(0) {};
			/** Create an OID from a 32 or 64bit pointer. */
			OID(void *v)
				: m_b(4), m_a(0) {
				#ifdef X86_64
				m_ll = (long long)v;
				#else
				m_ll = (long long)(int)v;
				#endif

			};
			OID(const OIDAccessor &a);
			
			~OID() {};
			
			/**
			 * Will map a specific OID to a label.
			 * @param o OID to map to.
			 * @param l The label.
			 */
			static void mapLabel(const OID &o, const char*l);
			
			/**
			 * Enables you to cast an OID to an object pointer. The object you are
			 * casting to must be a DOSTE-enabled object, inherits from Object.
			 */
			template <typename T>
			operator T*() const { return get_pointer<T>(cadence::doste::is_pod<T>()); };
	
			template <typename T>
			inline T *get_pointer(true_type t) const {
				if (isPointer()) {
					#ifndef X86_64
					return (T*)(int)m_ll;
					#else
					return (T*)m_ll;
					#endif
				} else {
					return 0;
				}
			}
			template <typename T>
			inline T *get_pointer(false_type) const;
			
			/**
			 * Cast an OID to an integer. This will automatically convert from
			 * other types where possible. Floats, characters and booleans will
			 * be converted.
			 * @return Integer if possible, otherwise 0.
			 */
			operator int() const {
				if (isLongLong()) {
					return (int)m_ll;
				} else if (isDouble()) {
					return (int)m_dbl;
				} else if (isChar()) {
					return m_d;
				} else if (isBuffer()) {
					return m_d;
				} else if (isBool()) {
					return (m_d == 1) ? 1 : 0;
				} else {
					return 0;
				}
			};
			/**
			 * Cast an OID to a float. This will automatically convert integers
			 * and booleans.
			 * @return A floating point number or 0.0
			 */
			operator float() const {
				if (isDouble()) {
					return (float)m_dbl;
				} else if (isLongLong()) {
					return (float)m_ll;
				} else if (isBool()) {
					return (m_d == 1) ? 1.0f : 0.0f;
				} else {
					return 0.0f;
				}
			};
			/**
			 * Cast an OID to a double. This automatically converts integers and booleans.
			 * @return A double or 0.0.
			 */
			operator double() const {
				if (isDouble()) {
					return m_dbl;
				} else if (isLongLong()) {
					return (double)m_ll;
				} else if (isBool()) {
					return (m_d == 1) ? 1.0 : 0.0;
				} else {
					return 0.0;
				}
			};
			/**
			 * Cast an OID to a long long integer. This automatically converts floats,
			 * characters and booleans.
			 * @return An integer if possible, otherwise 0.
			 */
			operator long long() const {
				if (isLongLong()) {
					return m_ll;
				} else if (isDouble()) {
					return (long long)m_dbl;
				} else if (isChar()) {
					return m_d;
				} else if (isBool()) {
					return (m_d == 1) ? 1 : 0;
				} else {
					return 0;
				}
			};
			/**
			 * Cast an OID to an ascii character. It automatically converts
			 * integers.
			 * @return A character or 0.
			 */
			operator char() const {
				if (isChar()) {
					return m_d;
				} else if (isLongLong()) {
					return m_d;
				} else if (isDouble()) {
					return 0;
				} else {
					return 0;
				}
			};
			
			/**
			 * Cast an OID to a boolean. Everything is true except false, 0, 0.0, Null and a null character.
			 * @return A boolean value.
			 */
			operator bool() const {
				if (isBool()) {
					return (m_d == 1) ? true : false;
				} else {
					return (m_d > 0) ? true : false;
				}
			};
	
			/**
			 * This operator will return a new definition object when used on an OID.
			 * An example might be cadence::root("time").
			 * @see Definition
			 * @return A new definition.
			 */
			Definition operator()(const OID &o) const;
	
			/**
			 * Array style access to the contents of the object this OID represents.
			 * You can also use assignment on this to change values in the database.
			 * @see OIDAccessor
			 * @param o The OID to use as the attribute name.
			 * @return An OIDAccessor but this can be cast to an OID to get the value.
			 */
			template <typename T>
			inline OIDAccessor operator[](const T &o) const;
	
			/**
			 * Get an Object pointer from a specified attribute.
			 * @param o The attribute name.
			 * @return Object pointer or NULL.
			 */
			template <typename T>
			inline T *get(const OID &o) const;

			//This is implemented in object.
			template <typename T>
			inline void set(const OID &key, T *value, bool async=false) const;

			/**
			 * Get the value of an attribute within this object.
			 * @param o Attribute name.
			 * @return Value of this attribute
			 */
			OID get(const OID &o) const;
			/**
			 * Change the value of an attribute within this object.
			 * @param key The attribute name
			 * @param value New value to change it to.
			 * @param async Synchronous or asynchronous. False (default) means block until done.
			 */
			void set(const OID &key, const OID &value, bool async=false) const;
			/**
			 * Set the definition for a specified attribute.
			 * @param key The attribute name
			 * @param def The definition object
			 * @param async Synchronous or asynchronous. False (default) means block until done.
			 * @see Definition
			 */
			void definefuture(const OID &key, const OID &def, bool async=false) const;
			/**
			 * Set the definition for a specified attribute. This version does not evaluate
			 * the definition to determine the initial value but instead uses the value you specify.
			 * @param key The attribute name
			 * @param def The definition object
			 * @param init Initialise the value to this
			 * @param async Synchronous or asynchronous. False (default) means block until done.
			 * @see Definition
			 */
			void definefuture(const OID &key, const OID &def, const OID &init, bool async=false) const;
			/**
			 * Set a function definition for a specified attribute. Note that this overrides any
			 * normal definition (and vice versa). A function definition evaluates on read instead
			 * of using dependency maintenance.
			 * @param key The attribute name
			 * @param func The function definition object
			 * @param async Synchronous or asynchronous. False (default) means block until done.
			 * @see Definition
			 */
			void define(const OID &key, const OID &func, bool async=false) const;
			/**
			 * Get the current definition for an attribute in this object.
			 * @param key Attribute name.
			 * @return Definition object.
			 * @see Definition
			 */
			OID definition(const OID &key) const;
			/**
			 * Return the flags for an attribute. This tells you what kind of definition it is
			 * and various other meta information. Look at OID::FLAG_ constants.
			 * @param key The attribute name.
			 * @return 32bit flags.
			 */
			int flags(const OID &key) const;

			void flags(const OID &key, int flags, bool async=false) const;
			
			/**
			 * Make an attribute within this object dependent upon something.
			 * @param dest The object to depend upon
			 * @param attrib The attribute to depend upon
			 * @param key The attribute in this object that has the dependency.
			 */
			void dependency(const OID &dest, const OID &attrib, const OID &key);
	
			/**
			 * Copy all attributes in this object into the specified object.
			 * @param n Destination.
			 */
			OID copy(const OID &n);
			void destroy();
			//void merge(const OID &obj);

			static const unsigned char FLAG_FUNCTION = 0x01; /**< The definition is a function definition */
			static const unsigned char FLAG_OUT_OF_DATE = 0x40; /**< Evaluate on next use */
			static const unsigned char FLAG_DEFINITION = 0x04; /**< Not used */
			static const unsigned char FLAG_PRIVATE = 0x08; /**< Must have private read/write permission */
			static const unsigned char FLAG_HIDDEN = 0x10; /**< By default hide this attribute */
			static const unsigned char FLAG_DEEP = 0x20;
			static const unsigned char FLAG_GARBAGE = 0x02;
			
			class iterator;
			
			/**
			 * Start iterating over elements within this object.
			 * @return A new iterator.
			 */
			iterator begin() const;
			/**
			 * An iterator that corresponds to the end of elements.
			 * @return A null of finished iterator. Compare with this to stop iterating.
			 */
			iterator end() const;
			
			/** Compare two OIDs. Does a 128bit comparison */
			friend inline bool operator==(const OID&, const OID&);
			/** Compare two OIDs. Does a 128bit comparison */
			friend inline bool operator!=(const OID&, const OID&);
			/** Compares all 128bits */
			friend inline bool operator<(const OID&, const OID&);
			/** Compares all 128bits */
			friend inline bool operator>(const OID&, const OID&);
			friend inline bool operator<=(const OID&, const OID&);
			friend inline bool operator>=(const OID&, const OID&);
			
			friend inline OID operator&(const OID&, const OID&);
			
			/**
			 * Add one to this OID. 128bit number increment.
			 * @return The new OID with 1 added.
			 */
			const OID &operator++() {
				if (m_d == 0xFFFFFFFF) {
					m_d = 0;
					if (m_c == 0xFFFFFFFF) {
						m_c = 0;
						if (m_b == 0xFFFFFFFF) {
							m_b = 0;
							m_a+=1;
						} else {
							m_b += 1;
						}
					} else {
						m_c += 1;
					}
				} else {
					m_d += 1;
				}
				
				return *this;
			};
			
			/**
			 * Add one to this OID. 128bit number increment.
			 * @return The OID before incrementing.
			 */
			OID operator++(int) {
				OID temp = *this;
				
				if (m_d == 0xFFFFFFFF) {
					m_d = 0;
					if (m_c == 0xFFFFFFFF) {
						m_c = 0;
						if (m_b == 0xFFFFFFFF) {
							m_b = 0;
							m_a+=1;
						} else {
							m_b += 1;
						}
					} else {
						m_c += 1;
					}
				} else {
					m_d += 1;
				}
				
				return temp;
			};
			//OID operator--();
			//OID operator--(int);
			
			/**
			 * Numerically add two OIDs together. Useful for adding OID::local() to
			 * some standard base.
			 */
			friend inline OID operator+(const OID &, const OID &);
			//friend OID operator-(const OID &, int);
			//const OID &operator+=(int);
			//const OID &operator-=(int); 
			
			//Stream operators.
			
			/**
			 * Convert this OID into a human readable ascii string. All number OIDs are
			 * converted to number strings. Label OIDs are looked up for the actually label.
			 * Other OIDs are printed out as 4 32bit numbers.
			 * @param buf The char buffer to fill with the result.
			 * @param max The size of the buffer.
			 */
			void toString(char *buf, int max) const;
			
			/** @return True is this OID represents an integer (32bit or 64bit). */
			bool isInt() const {
				return (m_a == 0 && m_b == 1) ? true : false;
			};
			/** @return True is this OID represents a float (32bit or 64bit). */
			bool isFloat() const {
				return (m_a == 0 && m_b == 2) ? true : false;
			};
			/** @return True is this OID represents a float (32bit or 64bit). */
			bool isDouble() const {
				return (m_a == 0 && m_b == 2) ? true : false;
			};
			/** @return True is this OID represents an integer (32bit or 64bit). */
			bool isLongLong() const {
				return (m_a == 0 && m_b == 1) ? true : false;
			};
			/** @return True is this OID represents a character (ascii or unicode). */
			bool isChar() const {
				return (m_a == 0 && m_b == 3) ? true : false;
			};
			/** @return True is this OID represents a label. */
			bool isName() const {
				return (m_a == 0 && m_b == 5) ? true : false;
			};
			/** @return True is this OID represents a boolean. */
			bool isBool() const {
				return (isSpecial() && m_c == 0 && (m_d == 1 || m_d == 2)) ? true : false;
			};
			/** @return True is this OID represents a pointer (32bit or 64bit). */
			bool isPointer() const {
				return (m_a == 0 && m_b == 4) ? true : false;
			};
			bool isCharPtr() const {
				return (m_a == 0 && m_b == 10) ? true : false;
			};
			/** @return True is this OID represents a special Buffer object. */
			bool isBuffer() const {
				return (m_a == 0 && m_b == 14) ? true : false;
			};
			bool isIntPtr() const {
				return (m_a == 0 && m_b == 11) ? true : false;
			};
			bool isFloatPtr() const {
				return (m_a == 0 && m_b == 12) ? true : false;
			};
			bool isOIDPtr() const {
				return (m_a == 0 && m_b == 13) ? true : false;
			};
			/** @return True is this OID corresponds to an agent. */
			bool isAgent() const {
				return (m_a > 0 && m_c == 15) ? true : false;
			};
			/** @return True is this OID represents a virtual machine instruction. */
			bool isVirtual() const {
				return (m_a == 0 && m_b == 100) ? true : false;
			};
			/** @return True is this OID is any type of reserved OID (numbers, specials, pointers, buffers...). */
			bool isReserved() const {
				return (m_a == 0) ? true : false;
			};
			/** @return True is this OID represents a definition modifier. */
			bool isModifier() const {
				return (m_hl == 0 && m_c == 1) ? true : false;
			};
			/** @return True is this OID represents a special OID (true,false,this,null,all,key,...). */
			bool isSpecial() const {
				return (m_a == 0 && m_b == 0) ? true : false;
			};
			
			/** @return First 32bit component */
			unsigned int a() const { return m_a; };
			/** @return Second 32bit component */
			unsigned int b() const { return m_b; };
			/** @return Third 32bit component */
			unsigned int c() const { return m_c; };
			/** @return Fourth 32bit component */
			unsigned int d() const { return m_d; };
	
			/**
			  * Each machine has a base local OID of the form '<*:0:0:0>'. This sets this
			  * machines local OID and should only be called during initialisation.
			  * @param o The base OID for this machine.
			  */
			static void local(const OID &o) { s_local = o; };
			/** @return This machines local base OID */
			static OID local() { return s_local; };
			/** @return A new OID for this machine */
			static OID create();
			
			//private:
			#ifdef X86_64
			union {
			struct {
			#endif
	
			union {
			struct {
			unsigned int m_b;
			unsigned int m_a;
			};
			long long m_hl;
			};
			union {
			struct {
			unsigned int m_d;
			unsigned int m_c;
			};
			double m_dbl;
			long long m_ll;
			};
	
			#ifdef X86_64
			};
			__int128_t m_128;
			};
			#endif
			
			static OID s_local;
		#ifdef _MSC_VER
		};
		#pragma pack() //pop pack value
		#else	
		} __attribute__((packed));
		#endif
		
		/**
		 * A standard C++ iterator for iterating over all elements within an object.
		 * Use the OID begin() method to create a new iterator. An example of how to use
		 * it:<br><br>
		 * <code>for (OID::iterator i=myobj.begin(); i!=myobj.end(); i++) { value = (*i); ... }</code>
		 */
		class DVMIMPORT OID::iterator {
			friend class OID;
			
			public:
			~iterator();
			OID operator*();
			
			iterator &operator++();
			iterator &operator++(int);
			iterator &operator--();
			iterator &operator--(int);
			friend inline bool operator==(const iterator &i1, const iterator &i2);
			friend inline bool operator!=(const iterator &i1, const iterator &i2);
			
			OID object() const { return m_object; }
			
			private:
			cadence::doste::OID m_object;
			cadence::doste::OID m_buf;
			Buffer *m_buffer;
			int m_cur;
			
			iterator(const OID &o, int c);
		};
	
		class DVMIMPORT OIDAccessor {
			friend class OID;
			
			public:
			OIDAccessor(OID obj, OID key)
				: m_obj(obj), m_key(key) {};
			~OIDAccessor() {};
			
			template <typename T>
			operator T() const { return (T)m_obj.get(m_key); }
			//operator OID() const { return m_obj.get(m_key); };
			
			template <typename T>
                        OIDAccessor &operator=(T *p) { m_obj.set(m_key, (OID)*p); return *this; }
			OIDAccessor &operator=(const OID &o) { m_obj.set(m_key, o); return *this; };
			OIDAccessor &operator=(const Definition &d);
			friend bool DVMIMPORT operator==(const OIDAccessor &acc, const OID &o);
			template <typename T>
			OIDAccessor operator[](const T &o) const { return OIDAccessor(m_obj.get(m_key), o); };
			
			void set(const OID &val, bool async=false) { m_obj.set(m_key, val, async); };
			OID get() const { return m_obj.get(m_key); };
	
			private:
			OID m_obj;
			OID m_key;
		};
		
		#undef True
		#undef False
		static const OID Null = OID(0,0,0,0);		/**< A null OID typically means undefined */
		static const OID True = OID(0,0,0,1);		/**< The boolean true OID */
		static const OID False = OID(0,0,0,2);		/**< The boolean false OID */
		static const OID Local = OID(0,0,0,3);		/**< Unused */
		static const OID Global = OID(0,0,0,4);		/**< Unused */
		static const OID This = OID(0,0,0,5);		/**< Refers to the current 'this' object. It will often be substituted for a real OID.*/
		static const OID Infinity = OID(0,0,0,6);	/**< Used for floating pointer operations */
		static const OID All = OID(0,0,0,9);		/**< Refers to all attributes in an object */
		static const OID Size = OID(0,0,0,8);		/**< Used as an attribute label to specify a string or definition list size */
		static const OID Key = OID(0,0,0,9);		/**< In a definition this is substituted for the attribute name of this definition*/
		static const OID Void = OID(0,0,0,10);		/**< Means 'don't care'. Setting this to an attribute always causes notification. */
		static const OID Self = OID(0,0,0,11);		/**< Inside a function definition this access the functions containing object.*/
		static const OID Type = OID(0,0,0,12);		/**< Type of object used for rtti */
		static const OID Parent = OID(0,0,0,13);
		static const OID Add = OID(0,0,0,14);
		static const OID Subtract = OID(0,0,0,15);
		static const OID Divide = OID(0,0,0,16);
		static const OID Multiply = OID(0,0,0,17);
		static const OID Less = OID(0,0,0,18);
		static const OID Greater = OID(0,0,0,19);
		static const OID And = OID(0,0,0,20);
		static const OID Or = OID(0,0,0,21);
		static const OID Not = OID(0,0,0,22);
		static const OID Question = OID(0,0,0,23);
		static const OID Percent = OID(0,0,0,24);
		static const OID At = OID(0,0,0,25);
		static const OID Exclamation = OID(0,0,0,26);

		static const OID _delete = OID (0,0,0,100);
		static const OID _size = OID(0,0,0,101);
		static const OID _save = OID(0,0,0,102);
		static const OID _clone = OID(0,0,0,103);
		static const OID _keys = OID(0,0,0,104);
		static const OID _allkeys = OID(0,0,0,105);
		static const OID _deep = OID(0,0,0,106);
		
		template <> class is_pod<OID> : public true_type {};
	};
};

template <typename T>
cadence::doste::OIDAccessor cadence::doste::OID::operator[](const T &o) const { return cadence::doste::OIDAccessor(*this, o); };

bool cadence::doste::operator==(const cadence::doste::OID &o1, const cadence::doste::OID &o2) {
	//#ifdef X86_64
	//return (o1.m_128 == o2.m_128);
	//#else
	return (o1.m_hl == o2.m_hl && o1.m_ll == o2.m_ll);
	//#endif
}

bool cadence::doste::operator!=(const cadence::doste::OID &o1, const cadence::doste::OID &o2) {
	//#ifdef X86_64
	//return (o1.m_128 != o2.m_128);
	//#else
	return (o1.m_hl != o2.m_hl || o1.m_ll != o2.m_ll);
	//#endif
}

namespace cadence {
namespace doste{
bool operator<(const cadence::doste::OID &o1, const cadence::doste::OID &o2) {
	//#ifdef X86_64
	//return (o1.m_128 < o2.m_128);
	//#else
	return ((unsigned long long)o1.m_hl < (unsigned long long)o2.m_hl || (o1.m_hl == o2.m_hl && (unsigned long long)o1.m_ll < (unsigned long long)o2.m_ll ));
	//#endif
}

bool operator>(const cadence::doste::OID &o1, const cadence::doste::OID &o2) {
	//#ifdef X86_64
	//return (o1.m_128 > o2.m_128);
	//#else
	return ((unsigned long long)o1.m_hl > (unsigned long long)o2.m_hl || (o1.m_hl == o2.m_hl && (unsigned long long)o1.m_ll > (unsigned long long)o2.m_ll ));
	//#endif
}

bool operator<=(const cadence::doste::OID &o1, const cadence::doste::OID &o2) {
	//#ifdef X86_64
	//return (o1.m_128 <= o2.m_128);
	//#else
	return ((o1 < o2) || (o1 == o2));
	//#endif
}

bool operator>=(const cadence::doste::OID &o1, const cadence::doste::OID &o2) {
	//#ifdef X86_64
	//return (o1.m_128 >= o2.m_128);
	//#else
	return ((o1 > o2) || (o1 == o2));
	//#endif
}

cadence::doste::OID operator&(const cadence::doste::OID &o1, const cadence::doste::OID &o2) {
	return cadence::doste::OID(o1.m_a & o2.m_a, o1.m_b & o2.m_b, o1.m_c & o2.m_c, o1.m_d & o2.m_d);
}

cadence::doste::OID operator+(const cadence::doste::OID &o1, const cadence::doste::OID &o2) {
	return cadence::doste::OID(o1.m_a+o2.m_a,o1.m_b+o2.m_b,o1.m_c+o2.m_c,o1.m_d+o2.m_d);
}

bool operator==(const cadence::doste::OID::iterator &i1, const cadence::doste::OID::iterator &i2) {
	return (i1.m_object == i2.m_object && i1.m_cur == i2.m_cur);
}
bool operator!=(const cadence::doste::OID::iterator &i1, const cadence::doste::OID::iterator &i2) {
	return (i1.m_object != i2.m_object || i1.m_cur != i2.m_cur);
}

}
}


#endif
