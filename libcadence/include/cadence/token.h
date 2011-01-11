/*
 * includes/doste/token.h
 * 
 * Generic tokens for token parsing scripts.
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


#ifndef _doste_TOKEN_H_
#define _doste_TOKEN_H_

#include <stdlib.h>
#include <cadence/stream.h>

namespace cadence {
	
	/**
	 * A collection of tokens for parsing text files. These are used in conjunction
	 * with the Stream class which can be a file. If a token fails to parse then it
	 * returns false and does not modify the stream position.
	 */
	namespace token {
	
		/**
		 * Removes any white space from the stream. This includes spaces, tabs and new line characters.
		 */
		struct WhiteSpace {
			
			bool parse(Stream &s) const {
				count = 0;
				while (!s.eof() && (s.peek() == ' ' || s.peek() == '\t' || s.peek() == '\n' || s.peek() == '\r')) {
					if (s.peek() == '\n') count++;
					s.seek(1);
				}
				return true;
			};

			mutable int count;
		};
		
		/**
		 * Parses a specific sequence of characters
		 */
		template <int Size=1>
		struct Char {
			Char(const char *c) : m_c(c) {};
			
			bool parse(Stream &s) const {

				for (int i=0; i<Size; i++) {
					if ((s.peek(i) != m_c[i])) return false;
				}
				s.seek(Size);
				return true;
			};
			
			private:
			const char *m_c;
		};

		/**
		 * Parses a single specified character.
		 */
		template <>
		struct Char<1> {
			Char(char c) : m_c(c) {};
			
			bool parse(Stream &s) const {
				if (s.peek() == m_c) {
					s.seek(1);
					return true;
				} else {
					return false;
				}
			};
			
			private:
			char m_c;
		};

		/**
		 * Reads any single character from the stream. Use the value property to access the character read.
		 */
		struct AnyChar {

			bool parse(Stream &s) const {
				if (s.eof()) return false;
				value = s.read<char>();
				return true;
			};
			
			mutable char value;
		};
		
		/**
		 * Parse a specified keyword from the input.
		 */
		struct Keyword {
			Keyword(const char *c) : m_c(c) {};
			
			bool parse(Stream &s) const {
				int i=0;
				int p = s.tell();
				while (m_c[i] != 0) {
					if (s.read<char>() != m_c[i]) {
						s.seek(p, Stream::BEG);
						return false;
					}
					i++;
				}

				char next = s.peek();
				if ((next >= 'a' && next <= 'z') || (next >= 'A' && next <='Z') || (next >= '0' && next <= '9') || next == '_') {
					s.seek(p, Stream::BEG);
					return false;
				}
				//s.seek(i);
				return true;
			};
			
			private:
			const char *m_c;
		};

		/**
		 * Read any alpha-numeric string. It will parse a-z, A-Z, 0-9 and _ but will
		 * stop on anything else. Returns false only if no string exists. The value read
		 * can be accessed in the value property.
		 */
		struct AlphaNumeric {
			AlphaNumeric() : value(0) {};
			~AlphaNumeric() { if (value != 0) delete [] value; };
			
			bool parse(Stream &s) const {
				int i=0;
				while (true) {
					//if (s.eof()) break;
					char c = s.peek(i);
					if (	!( (c >= 'a' && c <= 'z')
						|| (c >= 'A' && c <= 'Z')
						|| (c >= '0' && c <= '9')
						|| (c == '_')) ) break;
					i++;
				}

				//s.seek(p, Stream::BEG);
				if (i == 0) return false;

				if (value != 0) delete [] value;
				value = new char[i+1];
				for (int j=0; j<i; j++) value[j] = s.read<char>();
				value[i] = 0;
				return true;
			};
			
			mutable char *value;
		};

		/**
		 * Same as AlphaNumeric except it includes the characters % . ? @.
		 */
		struct AlphaNumericX {
			AlphaNumericX() : value(0) {};
			~AlphaNumericX() { if (value != 0) delete [] value; };
			
			bool parse(Stream &s) const {
				int i=0;

				while (true) {
					//if (s.eof()) break;
					char c = s.peek(i);
					if (	!( (c >= 'a' && c <= 'z')
						|| (c >= 'A' && c <= 'Z')
						|| (c >= '0' && c <= '9')
						|| (c == '_')
						|| (c == '%')
						|| (c == '.')
						|| (c == '?')
						|| (c == '@')) ) break;
					i++;
				}

				//s.seek(p, Stream::BEG);
				if (i == 0) return false;

				if (value != 0) delete [] value;
				value = new char[i+1];
				for (int j=0; j<i; j++) value[j] = s.read<char>();
				value[i] = 0;
				return true;
			};
			
			mutable char *value;
		};
		
		/**
		 * Parse a floating point number from a string. Use the value property to get the number parsed.
		 */
		struct Float {
			bool parse(Stream &s) const {
				char temp[20];
				int i=0;
				
				//Allow for negative.
				if (!s.eof() && s.peek(i) == '-') {
					temp[i] = s.peek(i);
					i++;
				}
					
				if (s.eof() || s.peek(i) < '0' || s.peek(i) > '9') return false;
					
				//Read integer part
				while (!s.eof() && (s.peek(i) >= '0' && s.peek(i) <= '9')) {
					temp[i] = s.peek(i);
					i++;
				}
				
				//Read decimal
				if (!s.eof() && !(s.peek(i) == '.' || s.peek(i) == 'e')) return false;
				temp[i] = s.peek(i);
				i++;
				
				if (s.eof() || ((s.peek(i) < '0' || s.peek(i) > '9') && s.peek(i) != 'e' && s.peek(i) != '-')) return false;
				
				while (!s.eof() && ((s.peek(i) >= '0' && s.peek(i) <= '9') || (s.peek(i) == 'e') || (s.peek(i) == '-'))) {
					temp[i] = s.peek(i);
					i++;
				}
				
				s.seek(i);
				temp[i] = 0;
				value = (float)atof(temp);
				return true;
			};
			
			mutable float value;
		};

		/**
		 * Read an integer from the stream. You can specify the base as a template parameter. Valid bases include 2,10 and 16.
		 * The value can be accessed in the value property.
		 */
		template <int Base>
		struct Integer {
			bool parse(Stream &s) const {
				//An invalid integer base.
				return false;
			}
		};

		template <>
		struct Integer<10> {

			bool parse(Stream &s) const {
			
				//Need to allow for negative
				bool negative = false;
				if ((s.peek() == '-')) {
					negative = true;
					s.seek(1);
				}
			
				if (s.peek() < '0' || s.peek() > '9') {
					if (negative) s.seek(-1);
					return false;
				}

				value = s.read<char>() - '0';

				int factor = 10;
				while (s.peek() >= '0' && s.peek() <= '9') {
					value = value * factor;
					value += s.read<char>() - (int)'0';
				}

				if (negative) value = -value;

				return true;
			}

			mutable long long value;
		};

		template <>
		struct Integer<2> {

			bool parse(Stream &s) const {
				if (s.peek() != '1') return false;

				s.seek(1);
				value = 1;

				while (s.peek() == '0' || s.peek() == '1') {
					value = value << 1;
					value += s.read<char>() - (int)'0';
				}

				return true;
			}

			mutable long long value;
		};

		template <>
		struct Integer<16> {

			bool parse(Stream &s) const {
				value = 0;
				int i = 0;

				while ((s.peek() >= '0' && s.peek() <= '9') || (s.peek() >= 'A' && s.peek() <= 'F') || (s.peek() >= 'a' && s.peek() <= 'f')) {
					i++;
					value = value << 4;
					switch(s.read<char>()) {
					case '0':	break;
					case '1':	value += 1; break;
					case '2':	value += 2; break;
					case '3':	value += 3; break;
					case '4':	value += 4; break;
					case '5':	value += 5; break;
					case '6':	value += 6; break;
					case '7':	value += 7; break;
					case '8':	value += 8; break;
					case '9':	value += 9; break;
					case 'a':
					case 'A':	value += 10; break;
					case 'b':
					case 'B':	value += 11; break;
					case 'c':
					case 'C':	value += 12; break;
					case 'd':
					case 'D':	value += 13; break;
					case 'e':
					case 'E':	value += 14; break;
					case 'f':
					case 'F':	value += 15; break;
					default: break;
					}
				}

				if (i == 0) return false;
				return true;
			}

			mutable long long value;
		};
		
		/**
		 * Read a block of text until a specified character is reached. If two characters are
		 * given then the first character must be matched first. An example usage would be to parse
		 * a string in quotes.
		 */
		template <int Size=-1>
		struct Block {
			Block(char b) : m_a(0), m_b(b) {};
			Block(char a, char b) : m_a(a), m_b(b) {};
			
			bool parse(Stream &s) const {
				if (((s.peek() == m_a) || (m_a == 0)) && !s.eof()) {
					if (m_a != 0) s.seek(1);
					int i=0;
					while (!s.eof() && s.peek() != m_b) {
						value[i++] = s.read<char>();
					}
					s.seek(1);
					value[i] = 0;
					return true;
				} else {
					return false;
				}
			};
			
			mutable char value[500];
			
			private:
			char m_a;
			char m_b;
		};
		
		template <>
		struct Block<1> {
			Block(char a, char b) : m_a(a), m_b(b) {};
			
			bool parse(Stream &s) const {
				if (s.peek() == m_a) {
					value = s.peek(1);
					if (s.peek(2) == m_b) {
						s.seek(3);
						return true;
					} else {
						return false;
					}
				} else {
					return false;
				}
			};
			
			mutable char value;
			
			private:
			char m_a;
			char m_b;
		};
		
		template <>
		struct Block<2> {
			Block(char a, char b) : m_a(a), m_b(b) {};
			
			bool parse(Stream &s) const {
				if (s.peek() == m_a) {
					value[0] = s.peek(1);
					value[1] = s.peek(2);
					if (s.peek(3) == m_b) {
						s.seek(4);
						return true;
					} else {
						return false;
					}
				} else {
					return false;
				}
			};
			
			mutable char value[2];
			
			private:
			char m_a;
			char m_b;
		};
	};
};

#endif
