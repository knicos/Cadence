/*
 * includes/cadence/doste/type_traits.h
 * 
 * DOSTE related type traits for metaprogramming.
 * Copyright (C) 2008 Nicolas Pope
 *
 * Author(s) : Nicolas Pope
 * Date      : 2008
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

#ifndef _cadence_DVM_TYPE_TRAITS_H_
#define _cadence_DVM_TYPE_TRAITS_H_

namespace cadence {
	namespace doste {
		class true_type { public: static const bool value = true; };
		class false_type { public: static const bool value = false; };
	
		//--- is_void ---
		template <typename T> class is_void : public false_type {};
		template <> class is_void<void> : public true_type {};
	
		//--- is_integer ---
		template <typename T> class is_integer : public false_type {};
		template <> class is_integer<char> : public true_type {};
		template <> class is_integer<unsigned char> : public true_type {};
		template <> class is_integer<short> : public true_type {};
		template <> class is_integer<unsigned short> : public true_type {};
		template <> class is_integer<int> : public true_type {};
		template <> class is_integer<unsigned int> : public true_type {};
		template <> class is_integer<long> : public true_type {};
		template <> class is_integer<unsigned long> : public true_type {};
		template <> class is_integer<long long> : public true_type {};
		template <> class is_integer<unsigned long long> : public true_type {};
	
		//--- is_real ---
		template <typename T> class is_real : public false_type {};
		template <> class is_real<float> : public true_type {};
		template <> class is_real<double> : public true_type {};
		template <> class is_real<long double> : public true_type {};
	
		//--- is_bool ---
		template <typename T> class is_bool : public false_type {};
		template <> class is_bool<bool> : public true_type {};
	
		//--- is_pointer ---
		template <typename T> class is_pointer : public false_type {};
		template <typename T> class is_pointer<T*> : public true_type {};
	
		//--- is_reference ---
		template <typename T> class is_reference : public false_type {};
		template <typename T> class is_reference<T&> : public true_type {};
	
		//--- is_const ---
		template <typename T> class is_const : public false_type {};
		template <typename T> class is_const<const T> : public true_type {};
	
		//--- is_unsigned ---
		//template <typename T> class is_unsigned : public false_type {};
		//template <typename T> class is_unsigned<unsigned T> : public true_type {};
	
		//--- is_pod ---
		template <typename T> class is_pod : public false_type {};
		template <> class is_pod<void> : public true_type {};
		template <> class is_pod<char> : public true_type {};
		template <> class is_pod<unsigned char> : public true_type {};
		template <> class is_pod<short> : public true_type {};
		template <> class is_pod<unsigned short> : public true_type {};
		template <> class is_pod<int> : public true_type {};
		template <> class is_pod<unsigned int> : public true_type {};
		template <> class is_pod<long> : public true_type {};
		template <> class is_pod<unsigned long> : public true_type {};
		template <> class is_pod<long long> : public true_type {};
		template <> class is_pod<unsigned long long> : public true_type {};
		template <> class is_pod<float> : public true_type {};
		template <> class is_pod<double> : public true_type {};
		template <> class is_pod<long double> : public true_type {};
		template <> class is_pod<bool> : public true_type {};
		template <typename T> class is_pod<T*> : public true_type {};

		//--- is_reuseable ---
		template <typename T> class is_reuseable : public false_type {};
	
		//--- add_reference ---
		//template <typename T, bool X> struct ref_adder { typedef T& type; };
		//template <typename T> struct ref_adder<T, true> { typedef T type; };
		//template <typename T> struct add_reference { typedef ref_adder< T, is_reference<T>::value >::type type; };
	
		//--- add_const ---
	
		//--- add_pointer ---
	
		//--- bool_constant ---
		template <bool C> class bool_constant : public false_type {};
		template <> class bool_constant<true> : public true_type {};
	
		//--- integer_constant ---
		template <int C> struct integer_constant { static const int value = C; };
	};
};

#endif
