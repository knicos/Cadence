/*
 * src/dasm.cpp
 * 
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

#include "dasm.h"
#include <cadence/messages.h>
#include <cadence/token.h>
#include <cadence/doste/modifiers.h>
#include <cadence/doste/doste.h>
#include <cadence/doste/definition.h>
#include <cadence/memory.h>
#include <string.h>

using namespace cadence;
using namespace cadence::doste;
using namespace cadence::token;

char errbuf[100];

DASM::DASM() {
	if (get("variables") == Null) set("variables", OID::create());
	//registerEvents();
}

DASM::DASM(const OID &obj)
 : Notation(obj) {
	if (get("variables") == Null) set("variables", OID::create());
	//registerEvents();
}

DASM::~DASM() {

}

bool DASM::statement(const OID &base) {
	//Parse Number
	//Parse alphanumeric
	//Parse =
	//Parse is
	//Parse func
	//Parse new
	//Parse (
	//Parse [
	//Parse {
	//Parse #
	//Parse !
	//Parse "
	//Parse '
	//Parse 
	
	OID temp = get("root");
	set("root", base);
	
	WhiteSpace ws;
	OID def;
	int flags;
	char *buf = NEW char[2000];
	parseValue(ws); m_lines += ws.count;
	
	if (parse(Keyword("%list"))) {
		parseValue(ws); m_lines += ws.count;
		parseSubExpr(m_result);
		for (OID::iterator i=m_result.begin(); i!=m_result.end(); i++) {
			(*i).toString(buf,50);
			std::cout << "\t" << buf << " = ";
			m_result.get(*i).toString(buf,50);
			std::cout << buf;
			def = m_result.definition(*i);
			if (def != Null) {
				flags = m_result.flags(*i);
				if (flags & OID::FLAG_FUNCTION)
					std::cout << " is { ";
				else
					std::cout << " := { ";
				buf[0] = 0;
				Definition(def).toString(buf,2000);
				std::cout << buf << "}\n";
			} else {
				std::cout << "\n";
			}
		}
	} else {
		if (!parseSubExpr(m_result)) {
			//error("Failed to complete script.");
		}
	}
	
	//DMsg msg(DMsg::INFO);
	//msg << "  " << m_result << "\n";
	set("result", m_result);
	set("root", temp);
	
	delete [] buf;
	return false;
}

bool DASM::parseString(OID &cur) {
	int i=0;
	cur = OID::create();
	AnyChar p;
	while (parseValue(p) && (p.value != '"')) {
		if (p.value == '\\') {
			parseValue(p);
			switch (p.value) {
			case 'n': cur[i++] = '\n'; break;
			case 't': cur[i++] = '\t'; break;
			case '\\': cur[i++] = '\\'; break;
			default: cur[i++] = p.value;
			}
		} else {
			cur[i++] = p.value;
		}
	}
	cur[Size] = i;
	return true;
}

bool DASM::parseObject(OID &cur) {
	Integer<10> integer;
	Float real;
	AlphaNumericX alpha;
	
	if (parseValue(real)) cur = real.value;
	else if (parseValue(integer)) cur = integer.value;
	else if (parse(Char<1>('\''))) {
		//Now parse next char
		char c;
		stream->read(c);

		if (c == '\\') {
			stream->read(c);
			switch (c) {
			case 'n': cur = '\n'; break;
			case 't': cur = '\t'; break;
			default: cur = c;
			}
		} else {
			cur = c;
		}
		parse(Char<1>('\''));
	} else if (parse(Char<1>('*'))) cur = Multiply;
	else if (parse(Char<1>('$'))) cur = Key;
	else if (parse(Char<1>('-'))) cur = Subtract;
	else if (parse(Char<1>('/'))) cur = Divide;
	else if (parse(Char<1>('+'))) cur = Add;
	else if (parse(Char<1>('>'))) cur = Greater;
	//else if (parse(Char<1>('>'))) cur = Greater;
	else if (parse(Char<1>('|'))) cur = Or;
	else if (parse(Char<1>('&'))) cur = And;
	else if (parse(Char<1>('?'))) cur = Question;
	else if (parse(Char<1>('%'))) cur = Percent;
	else if (parse(Char<1>('!'))) cur = modifiers::NoDep;
	else if (parse(Char<1>('@'))) {
		//This is a variable.
		if (parseValue(alpha)) {
			cur = variables().get(alpha.value);
			if (cur == Null) {
				error("Undefined context variable");
			}
		} else {
			cur = At;
		}
	}
	//else if (parse(Keyword(".."))) cur = Parent;
	else if (parse(Char<1>('.'))) cur = This;
	//else if (parse(Keyword("new"))) {
	//	OID temp = OID::create();
	//	temp.set(This, cur);
	//	cur = temp;
	else if (parseValue(alpha)) cur = OID(alpha.value);
	else if (parse(Char<1>('~'))) cur = modifiers::NoDep;
	else if (parse(Char<1>(';'))) cur = modifiers::Seq;
	else if (parse(Keyword("=="))) cur = modifiers::Compare;
	else if (parse(Char<1>('"'))) parseString(cur);
	//else if (parse(Char<1>('('))) return parseSubExpr(cur);
	else if (parse(Char<1>('<'))) {
		if (parseValue(integer)) {
			cur.m_a = (unsigned int)integer.value;
			parse(Char<1>(':'));
			parseValue(integer);
			cur.m_b = (unsigned int)integer.value;
			parse(Char<1>(':'));
			parseValue(integer);
			cur.m_c = (unsigned int)integer.value;
			parse(Char<1>(':'));
			parseValue(integer);
			cur.m_d = (unsigned int)integer.value;
			parse(Char<1>('>'));
		} else {
			cur = Less;
		}
	}// else if (parse(Char<1>('<'))) cur = Less;
	else {
		return false;
	}
	
	return true;
}

bool DASM::parseIf(OID &context, int &i, OID &cur, bool imed) {
	//Now parse condition
	OID condition;
	WhiteSpace ws;

	int oldi = i;
	i += 2;
	int zero = 0;

	parseValue(ws); m_lines += ws.count;
	if (parse(Char<1>('('))) {
		if (imed) parseSubExpr(condition);
		else {
			cur.set(i++, modifiers::BeginSub);
			parseDefinition(context, cur, i, true);
			cur.set(i++, modifiers::EndSub);
		}
		if (!parse(Char<1>(')'))) {
			error("Missing ) in if statement");
			return false;
		}
	} else {
		error("An if must be followed by some condition.");
		return false;
	}
	//Parse true part
	OID truepart;
	bool abstrue = false;
	parseValue(ws); m_lines += ws.count;
	if (parse(Char<1>('{'))) {
		zero = 0;
		truepart = OID::create();
		if (!parseDefinition(context, truepart, zero, true)) return false;
		if (!parse(Char<1>('}'))) {
			error("Missing } in if statement");
			return false;
		}
	} else {
		if (!parseObject(truepart)) {
			error("An object or definition must be given in an if statement.");
			return false;
		}
		abstrue = true;
	}

	zero = 0;

	//We may now have an else
	parseValue(ws); m_lines += ws.count;
	OID elsepart = Null;
	bool abselse = false;
	if (parse(Keyword("else"))) {
		parseValue(ws); m_lines += ws.count;
		if (parse(Char<1>('{'))) {
			zero = 0;
			elsepart = OID::create();
			if (!parseDefinition(context, elsepart, zero, true)) return false;
			if (!parse(Char<1>('}'))) {
				error("Missing } in if statement");
				return false;
			}
		} else if (parse(Keyword("if"))) {
			zero = 0;
			elsepart = OID::create();
			if (!parseIf(context, zero, elsepart, imed)) return false;
			elsepart.set("size", ((int)elsepart.get("size")) + 1);
		} else {
			if (!parseObject(elsepart)) {
				error("An object or definition must be given in an if statement.");
				return false;
			}
			abselse = true;
		}
	}

	//Now actually construct the if.
	OID ifobj = OID::create();
	ifobj.set("type", OID("if"));
	ifobj.set(This, context);
	if (abstrue) ifobj.set(True, truepart, true);
	else ifobj.define(True, truepart, true);
	if (abselse) ifobj.set(Key, elsepart, false);
	else ifobj.define(Key, elsepart, false);

	if (imed) {
		cur = ifobj.get(condition);
	} else {
		cur.set(oldi, ifobj, true);
		cur.flags(oldi++, OID::FLAG_DEEP);
		cur.set(oldi++, cadence::doste::modifiers::NoDep, true);
		//cur.function(i++, condition, false);
	}
	return true;
}

bool DASM::parseSelect(OID &context, int &i, OID &cur, bool imed) {
	OID selector;
	OID selobj = OID::create();
	OID caseval;
	OID rhsval;
	WhiteSpace ws;

	int oldi = i;
	int zero = 0;
	i += 2;

	//Parse the selector value
	parseValue(ws); m_lines += ws.count;
	if (parse(Char<1>('('))) {
		if (imed) parseSubExpr(selector);
		else {
			cur.set(i++, modifiers::BeginSub);
			parseDefinition(context, cur, i, true);
			cur.set(i++, modifiers::EndSub);
			
		}
		if (!parse(Char<1>(')'))) {
			error("Missing ) in select");
			return false;
		}
	} else {
		error("A select must be followed by some definition.");
		return false;
	}

	//Now parse starting brace
	parseValue(ws); m_lines += ws.count;
	if (!parse(Char<1>('{'))) {
		error("Missing '{' in select.");
		return false;
	}

	//Until a closing brace is found, parse cases
	while (!stream->eof()) {
		parseValue(ws); m_lines += ws.count;
		if (parse(Char<1>('}'))) break;
		else {
			//Parse an object (or expression)
			if (!parseObject(caseval)) {
				error("Missing case value in select statement.");
				return false;
			}

			//Parse a :
			if (!parse(Char<1>(':'))) {
				error("Missing a ':' after a select case value.");
				return false;
			}
			parseValue(ws); m_lines += ws.count;

			//Parse object, expression or definition
			if (parse(Char<1>('('))) {
				//TODO
			} else if (parse(Char<1>('{'))) {
				zero = 0;
				rhsval = OID::create();
				if (!parseDefinition(context, rhsval, zero, true)) return false;
				if (!parse(Char<1>('}'))) {
					error("Missing } in select case");
					return false;
				}
				selobj.define(caseval,rhsval,false);
			} else if (parseObject(rhsval)) {
				selobj.set(caseval, rhsval,false);
			} else {
				error("The right-hand-side of a select case must be an expression, definition or value.");
				return false;
			}
		}
	}

	if (imed) {
		cur = selobj.get(selector);
	} else {
		cur.set(oldi++, selobj, true);
		cur.set(oldi++, cadence::doste::modifiers::NoDep, true);
	}

	return true;
}

bool DASM::parseDefinition(OID &context, OID &cur, int &i, bool real, bool ncontext, bool endnl) {
	AlphaNumeric alpha;
	WhiteSpace ws;
	bool deep = false;
	int zero = 0;

	//if (i == 0) cur = OID::create();
	OID key, value;
	
	parseValue(ws); m_lines += ws.count;
	while (parse(Block<>('#','\n'))) { parseValue(ws); m_lines += ws.count+1; };

	//if (ncontext) {
	//	cur.set(i++, modifiers::NoContext);
	//}

	//If statements
	if (parse(Keyword("if"))) {
		if (!parseIf(context, i, cur)) return false;
	} else if (parse(Keyword("select"))) {
		if (!parseSelect(context, i, cur)) return false;
	} else if (parse(Keyword("%deep"))) {
			deep = true;
	} else {
	
		//Read the base object
		if (!parseObject(value)) {
			error("Unable to parse definition");
			return false;
		}
		cur.set(i++, value);
	}
	
	//Now read more until something special is encountered.
	while (!stream->eof()) {
		parseValue(ws);
		if ((ws.count > 0) && endnl) {
			cur.set(Size, i);
			//stream->seek(-1, Stream::CUR);
			m_lines += ws.count;
			return true;
		}
		m_lines += ws.count;
		while (parse(Block<>('#','\n'))) {
			parseValue(ws);
			if ((ws.count > 0) && endnl) {
				cur.set(Size, i);
				//stream->seek(-1, Stream::CUR);
				m_lines += ws.count+1;
				return true;
			}
			m_lines += ws.count+1;
		};
		
		if (parse(Keyword("if"))) {
			if (!parseIf(context, i, cur)) return false;
		} else if (parse(Keyword("select"))) {
			if (!parseSelect(context, i, cur)) return false;
		/*} else if (parse(Keyword("is"))) {
			cur.set(i, cur.get(i-1));
			cur.set(i-1, modifiers::Define);
			i++;
		} else if (parse(Keyword("func"))) {
			cur.set(i, cur.get(i-1));
			cur.set(i-1, modifiers::DefineFunc);
			i++;
		} else if (parse(Keyword("new"))) {
			cur.set(i++, modifiers::Create);
		//} else if (parse(Keyword("clone"))) {
		//	cur.set(i++, modifiers::Clone);
		} else if (parse(Keyword("%deep"))) {
			deep = true; */
		} else if (parse(Keyword("=="))) {
			cur.set(i++, modifiers::Compare);
		} else if (parse(Char<1>('='))) {
			cur.set(i, cur.get(i-1));
			cur.set(i-1, modifiers::Set);
			i++;
		//} else if (parse(Keyword(".."))) {
		//	cur.set(i++, Parent);
		} else if (parse(Char<1>('('))) {
			if (real) {
				//if (!parseDefinition(cur, i, true, true)) return false;
				//cur.function(i++, value);
				cur.set(i++, modifiers::BeginSub);
				if (!parseDefinition(context, cur,i,true)) return false;
				if (!parse(Char<1>(')'))) {
					error("Missing )");
					return false;
				}
				cur.set(i++, modifiers::EndSub);
			} else {
				if (!parseSubExpr(value)) return false;
				cur.set(i++, value);
				if (!parse(Char<1>(')'))) {
					error("Missing )");
					return false;
				}
			}
		/* } else if (parse(Char<1>('{'))) {
			//if (real && !dot) {
				zero = 0;
				value = OID::create();
				if (!parseDefinition(value, zero, true)) return false;
				if (!parse(Char<1>('}'))) {
					error("Missing }");
					return false;
				}
				cur.set(i++, value);
			//} else {
			//	dot = false;
			//	if (!parseSubExpr(value)) return false;
			//	cur.set(i++, value);
			//}
		//} else if (real && parse(Char<1>(')'))) {
		//	cur.set(i++, modifiers::EndSub); */
		} else if (parse(Char<1>(')')) || parse(Char<1>(']')) || parse(Char<1>('}'))) {
			//cur = cur.get(key);
			//cur.set(i++,key);
			cur.set(Size, i);
			stream->seek(-1, Stream::CUR);
			return true;
		}else if (parseObject(value)) {
			//DMsg msg(DMsg::DEBUG);

			//if (!first) {
				cur.set(i++, value);
			//} else {
			//	first = false;
			//}
			//key = OID(value);
		} else if (!parse(Char<1>(0))) {
			error("I could not understand a statement on this line (def)");
			return false;
		}
	}
	
	//cur.set(i++,key);
	cur.set(Size, i);
	
	return true;
}

#undef This

bool DASM::parseSubExpr(OID &cur) {

	AlphaNumeric alpha;
	WhiteSpace ws;
	bool first = true;
	int lop = 0;
	int i;
	bool deep = false;
	int zero = 0;
	
	parseValue(ws); m_lines += ws.count;
	while (parse(Block<>('#','\n'))) { parseValue(ws); m_lines += ws.count+1; };
	
	OID key, value;
	
	if (parse(Keyword("%include"))) {
		AnyChar p;
		char *buf = NEW char[1000];
		parseValue(ws); m_lines += ws.count;
		if (parse(Char<1>('"'))) {
			int i=0;
			while (parseValue(p) && (p.value != '"')) {
				if (p.value == '\\') {
					parseValue(p);
					switch (p.value) {
					case 'n': buf[i++] = '\n'; break;
					case 't': buf[i++] = '\t'; break;
					case '\\': buf[i++] = '\\'; break;
					default: buf[i++] = p.value;
					}
				} else {
					buf[i++] = p.value;
				}
			}
			buf[i] = 0;
			
			run(buf, cadence::doste::root);
			delete [] buf;
			cur = This;
		} else {
			error("Expecting filename after %include");
			delete [] buf;
			return false;
		}
	} else if (parse(Char<1>('@'))) {
		if (!parseValue(alpha)) {
			error("Expected a name after the variable identifier '@'");
			return false;
		}
		//Now must check for equals
		parseValue(ws); m_lines += ws.count;
		if (parse(Char<1>('='))) {
			parseValue(ws); m_lines += ws.count;
			if (parse(Char<1>('('))) {
				if (!parseSubExpr(cur)) return false;
				if (!parse(Char<1>(')'))) {
					error("Missing )");
					return false;
				}
			} else {
				if (!parseObject(cur)) {
					error("Variable must be followed by expression or object");
					return false;
				}
			}
			variables().set(alpha.value, cur);
			//cur = Null;
			//parse(Char<1>(';'));
		} else {
			cur = variables().get(alpha.value);
			if (cur == Null) {
				error("Undefined context variable");
				return false;
			}
		}
	} else if (parse(Char<1>('('))) {
		if (!parseSubExpr(cur)) return false;
		if (!parse(Char<1>(')'))) {
			error("Missing )");
			return false;
		}
	} else if (parse(Keyword("if"))) {
		if (!parseIf(cur, i, cur, true)) return false;
	} else if (parse(Keyword("select"))) {
		if (!parseSelect(cur, i, cur, true)) return false;
	
	} else if (parse(Keyword("%deep"))) {
		deep = true;
	} else {
		//Read the base object
		if (!parseObject(cur)) {
			//error("An expression must start with an object, if, select, expression, a % tag or @ variable");
			return true;
		}
	}
	
	if (cur == This) cur = get("root"); //doste::dvm::root;
	if (cur == modifiers::Create) cur = OID::create();
	
	//Now read more until something special is encountered.
	while (!stream->eof()) {
		parseValue(ws); m_lines += ws.count;
		while (parse(Block<>('#','\n'))) { parseValue(ws); m_lines += ws.count+1; };
		
		if (parse(Char<1>('='))) {
			if (parse(Char<1>('='))) {
				//Comparison
				if (!parseSubExpr(value)) return false;
				if (cur == value) cur = True; else cur = False;
				continue;
			} else {
			lop = 0;
			parseValue(ws); m_lines += ws.count;
			if (parse(Char<1>('{'))) {
				zero = 0;
				value = OID::create();
				if (!parseDefinition(cur, value, zero, true)) return false;
				if (!parse(Char<1>('}'))) {
					error("Missing } in assignment");
					return false;
				}
				cur.set(key, value);
				continue;
			} else if (parse(Char<1>('['))) {
				zero = 0;
				value = OID::create();
				if (!parseDefinition(cur, value, zero, false)) return false;
				if (!parse(Char<1>(']'))) {
					error("Missing ] in assignment");
					return false;
				}
				cur.set(key, value);
				continue;
			} else if (parseObject(value)) {
				cur.set(key, value);
				continue;
			} else if (parse(Char<1>('('))) {
				if (!parseSubExpr(value)) return false;
				if (!parse(Char<1>(')'))) {
					error("Missing ) in assignment");
					return false;
				}
				cur.set(key, value);
				//NOTE: Set parent upon any direct assignment... IS THIS WHAT WE WANT?
				value.set(This, cur);
				continue;
			} else if (parse(Char<1>('='))) {
				cur.set(key, modifiers::Compare);
			} else {
				error("A '(' or '[' is expected after an 'is'.");
				return false;
			}
			}
		} else if (parse(Char<1>('('))) {
			if (!parseSubExpr(value)) return false;
			if (!first && (lop == 1)) {
				cur = cur.get(key);
			} else {
				first = false;
			}
			lop = 1;
			key = value;

			if (deep) {
				cur.flags(key, OID::FLAG_DEEP);
				deep = false;
			}
			if (!parse(Char<1>(')'))) {
				error("Missing )");
				return false;
			}
		} else if (parse(Keyword("union"))) {
			if (lop == 1) cur = cur.get(key);
			lop = 0;
			parseValue(ws); m_lines += ws.count;
			if (parse(Char<1>('{'))) {
				zero = 0;
				value = OID::create();
				if (!parseDefinition(cur, value, zero, true)) return false;
				if (!parse(Char<1>('}'))) {
					error("Missing } in union");
					return false;
				}
			} else if (parse(Char<1>('('))) {
				if (!parseSubExpr(value)) return false;
				if (!parse(Char<1>(')'))) {
					error("Missing ) in union");
					return false;
				}
			} else {
				error("A '(' or '[' is expected after an 'is'.");
				return false;
			}

			value.copy(cur);
			//Now sync it.
			//Processor::processRemaining();
			continue;
		} else if (parse(Keyword(":="))) {
			lop = 0;
			parseValue(ws); m_lines += ws.count;
			if (parse(Char<1>('{'))) {
				zero = 0;
				value = OID::create();
				if (!parseDefinition(cur, value, zero, true)) return false;
				if (!parse(Char<1>('}'))) {
					error("Missing } in future definition");
					return false;
				}
				cur.definefuture(key, value);
				continue;
			} else {
				zero = 0;
				value = OID::create();
				if (!parseDefinition(cur, value, zero, true, true)) return false;
				cur.definefuture(key, value);
				continue;
			}
		} else if (parse(Keyword("is"))) {
			lop = 0;
			parseValue(ws); m_lines += ws.count;
			if (parse(Char<1>('{'))) {
				zero = 0;
				value = OID::create();
				if (!parseDefinition(cur, value, zero, true)) return false;
				if (!parse(Char<1>('}'))) {
					error("Missing } in definition");
					return false;
				}
				cur.define(key, value);
				continue;
			} else {
				zero = 0;
				value = OID::create();
				if (!parseDefinition(cur, value, zero, true, true)) return false;
				cur.define(key, value);
				continue;
			}
		} else if (parse(Char<1>(';'))) {
			if (lop == 1) cur = cur.get(key);
			lop = 0;

			parseValue(ws); m_lines += ws.count;
			while (parse(Block<>('#','\n'))) { parseValue(ws); m_lines += ws.count+1; };
			if (stream->eof()) break;
			
			
			if (parse(Keyword("%include"))) {
				AnyChar p;
				char *buf = NEW char[1000];
				parseValue(ws); m_lines += ws.count;
				if (parse(Char<1>('"'))) {
					int i=0;
					while (parseValue(p) && (p.value != '"')) {
						if (p.value == '\\') {
							parseValue(p);
							switch (p.value) {
							case 'n': buf[i++] = '\n'; break;
							case 't': buf[i++] = '\t'; break;
							case '\\': buf[i++] = '\\'; break;
							default: buf[i++] = p.value;
							}
						} else {
							buf[i++] = p.value;
						}
					}
					buf[i] = 0;
					
					run(buf, cadence::doste::root);
					delete [] buf;
				} else {
					error("Expecting filename after %include");
					delete [] buf;
					return false;
				}
				
				cur = This;
			} else if (parse(Char<1>('@'))) {
				if (!parseValue(alpha)) {
					error("Expected a name after the variable identifier '@'");
					return false;
				}
				//Now must check for equals
				parseValue(ws); m_lines += ws.count;
				if (parse(Char<1>('='))) {
					parseValue(ws); m_lines += ws.count;
					if (parse(Char<1>('('))) {
						if (!parseSubExpr(cur)) return false;
						if (!parse(Char<1>(')'))) {
							error("Missing )");
							return false;
						}
					} else {
						if (!parseObject(cur)) {
							error("Variable must be followed by expression or object");
							return false;
						}
					}
					variables().set(alpha.value, cur);
					//cur = Null;
					//parse(Char<1>(';'));
				} else {
					cur = variables().get(alpha.value);
					if (cur == Null) {
						error("Possibly undefined context variable");
						return false;
					}
				}
			} else if (parse(Char<1>('('))) {
				if (!parseSubExpr(cur)) return false;
				if (!parse(Char<1>(')'))) {
					error("Missing )");
					return false;
				}
			} else if (parse(Keyword("if"))) {
				if (!parseIf(cur, i, cur, true)) return false;
			} else if (parse(Keyword("select"))) {
				if (!parseSelect(cur, i, cur, true)) return false;
			} else {
				//Read the base object
				if (!parseObject(cur)) {
					error("A ; must be followed by an object, expression, if or select");
					return false;
				}
			}
			if (cur == This) cur = get("root"); //cadence::doste::root;
			if (cur == modifiers::Create) cur = OID::create();
		} else if (parse(Char<1>(')'))) {
			if (lop == 1) cur = cur.get(key);
			stream->seek(-1,Stream::CUR);
			return true;
		} else if (parse(Keyword("%deep"))) {
			deep = true;
		} else if (parseObject(value)) {
			if (!first && (lop == 1)) {
				cur = cur.get(key);
				if (cur == Null) {
					error("Undefined expression");
					return false;
				}
			} else {
				first = false;
			}
			lop = 1;
			key = value;

			if (deep) {
				cur.flags(key, OID::FLAG_DEEP);
				deep = false;
			}
		} else if (!parse(Char<1>(0))) {
			error("I could not understand a statement on this line");
			return false;
		}
	}
	
	if (lop == 1) cur = cur.get(key);
	
	return true;
}

void DASM::error(const char *message) {
	char msg[2000];
	strcpy(msg, m_source);
	strcat(msg, ": line ");
	OID(m_lines+1).toString(&msg[strlen(msg)], 500);
	strcat(msg, ": ");
	strcat(msg,message);
	Error(Error::SYNTAX, msg);
}

void DASM::warning(const char *message) {
	char msg[2000];
	strcpy(msg, m_source);
	strcat(msg, ": line ");
	OID(m_lines+1).toString(&msg[strlen(msg)], 500);
	strcat(msg, ": ");
	strcat(msg,message);
	Warning(Warning::SYNTAX, msg);
}
