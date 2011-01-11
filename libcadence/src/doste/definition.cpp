/*
 * src/dvm/definition.cpp
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

#include <cadence/doste/definition.h>
#include <cadence/doste/event.h>
#include <cadence/doste/modifiers.h>
#include <string.h>
#include <cadence/doste/processor.h>
//#include <doste/ecounter.h>
#include <cadence/messages.h>
#include <cadence/memory.h>
#include "context.h"

using namespace cadence;
using namespace cadence::doste;

Definition Definition::operator()(const OID &o) {
	//Get the current size and append this object
	//This is an unsafe GET.
	m_def.set(m_size++, o, true);
	//Save the NEW size
	m_def.set(Size, m_size, true);
	
	//Otherwise the GET size on the next call is wrong.
	//Processor::processAll();
	return *this;
}

#include <iostream>

OID Definition::parseExpression(Context *ctx, Buffer *def, int &index, bool fdef) {
	OID res = def->get(index++);
	OID temp, temp2;
	int modi;
	bool nodep = false;

	Event *evt2;
	Event *evt = NEW Event(Event::GET, Null);

	if (res == modifiers::Create) res = OID::create();
	else if (res == modifiers::BeginSub) res = parseExpression(ctx,def,index, fdef);

	while (index < m_size) {
		temp = def->get(index);

		if (res == This) res = ctx->object();
		else if (res == Key) res = ctx->key();

		if (temp == modifiers::BeginSub) temp = parseExpression(ctx,def,++index, fdef);

		if (temp.isModifier()) {
			modi = temp.d();
		
			switch (modi) {
			case modifiers::ENDSUB:
				//index++;
				if (res == This) res = ctx->object();
				else if (res == Key) res = ctx->key();
				delete evt;
				return res;
	
			/*case modifiers::SET:
				temp = def->get(++index);
				if (temp == modifiers::BeginSub) temp = parseExpression(ctx,def,index);
				temp2 = def->get(++index);
				if (temp2 == modifiers::BeginSub) temp2 = parseExpression(ctx,def,index);
				evt2 = NEW Event(Event::SET, res);
				evt2->param<0>(temp);		//Get key from definition
				evt2->param<1>(temp2);		//Get value from definition
				evt2->send(Event::FLAG_FREE);
				break;
	
			case modifiers::DEFINE:
				temp = def->get(++index);
				if (temp == modifiers::BeginSub) temp = parseExpression(ctx,def,index);
				temp2 = def->get(++index);
				if (temp2 == modifiers::BeginSub) temp2 = parseExpression(ctx,def,index);
				evt2 = NEW Event(Event::DEFINE, res);
				evt2->param<0>(temp);		//Get key from definition
				evt2->param<1>(temp2);		//Get definition object from definition
				evt2->param<2>(Null);
				evt2->send(Event::FLAG_FREE);
				break;
	
			case modifiers::DEFINEFUNC:
				temp = def->get(++index);
				if (temp == modifiers::BeginSub) temp = parseExpression(ctx,def,index);
				temp2 = def->get(++index);
				if (temp2 == modifiers::BeginSub) temp2 = parseExpression(ctx,def,index);
				evt2 = NEW Event(Event::DEFINE_FUNC, res);
				evt2->param<0>(temp);		//Get key from definition
				evt2->param<1>(temp2);		//Get function definition from definition
				evt2->send(Event::FLAG_FREE);
				break;*/
	
			//case modifiers::CREATE:	
			//	res = OID::create();
			//	break;
	
			case modifiers::UNION:
				temp = def->get(++index);
				if (temp == modifiers::BeginSub) temp = parseExpression(ctx,def,index, fdef);
				temp.copy(res);
				break;
				
			/*case modifiers::GETDEF:
				evt->type(Event::GETDEF);
				evt->dest(res);
				evt->param<0>(temp);
				evt->send();
				res = evt->result();
				break;
				
			case modifiers::GETFLAGS:
				evt->type(Event::GETFLAGS);
				evt->dest(res);
				evt->param<0>(temp);
				evt->send();
				res = evt->result();
				break;*/
	
			case modifiers::SEQ:
				//Replace current object with next object in definition
				res = def->get(++index);
				if (res == modifiers::BeginSub) res = parseExpression(ctx,def,index, fdef);
				break;
	
			/*case modifiers::NODEP:
				//Toggle the nodep to add or not add dependencies automatically
				nodep = !nodep;
				break;*/
	
			case modifiers::COMPARE:
				//Compare the current object with the next and put boolean
				//result as the current object.
				temp = def->get(++index);
				if (temp == modifiers::BeginSub) temp = parseExpression(ctx,def,index, fdef);
				res = (res == temp);
				break;
	
			default: break;
			}
		} else {
			evt->type(Event::GET);
			evt->dest(res);
			evt->param<0>(temp);
			evt->send();
			res = evt->result();
	
			//Should a dependency be added.
			if (!nodep) {
				if (!fdef)
					evt2 = NEW Event(Event::ADDDEP, evt->dest());
				else
					evt2 = NEW Event(Event::ADD_IS_DEP, evt->dest());
				evt2->param<0>(temp);
				evt2->param<1>(ctx->object());
				evt2->param<2>(ctx->key());
				evt2->send(Event::FLAG_FREE);
			}
		}

		//Move to next element in the definition.
		index++;
	}

	delete evt;

	if (res == This) res = ctx->object();
	else if (res == Key) res = ctx->key();
	return res;
}

OID Definition::evaluate(const OID &obj, const OID &key, bool fdef) {
	
	//Need to know the current processor.
	Processor *cproc;
	cproc = Processor::getThisProcessor();
	Context *context, *oldctx;

	oldctx = cproc->context();
	context = NEW Context(*this, obj, key);
	cproc->context(context);
	
	//OID skey;
	//OID sobject;
	//bool restore_context = false;

	Event *evt2 = NEW Event(Event::GET_RANGE, m_def);
	evt2->param<0>(0);
	evt2->param<1>(m_size);
	evt2->send();
	OID boid = evt2->result();
	Buffer *def = Buffer::lookup(boid);
	delete evt2;
	if (def == 0) {
		Error(0, "An invalid definition was found, could not evaluate");
		return Null;
	}

	//Get the first object and use as initial base.
	int i=0;
	OID res; // = def->get(0);
	
	//Do we need to save the context
	//if (res != modifiers::NoContext) {
		//skey = context->selfKey();
		//sobject = context->selfObject();
		//context->selfKey(key);
		//context->selfObject(obj);
		//restore_context = true;
	//} else {
	//	res = def->get(++i);
	//}

	res = parseExpression(context, def, i, fdef);

	
	//if (restore_context) {
	//	context->selfObject(sobject);
	//	context->selfKey(skey);
	//}

	cproc->context(oldctx);
	delete context;

	Buffer::free(boid);

	return res;
}

void Definition::toString(char *buf, int max) const {
	toString(buf, max, 0);
}

void Definition::toString(char *buf, int max, int indent) const {
	int index=0;
	OID temp;
	int modi;
	char buf2[50];
	Event *evt2 = NEW Event(Event::GET_RANGE, m_def);
	evt2->param<0>(0);
	evt2->param<1>(m_size);
	evt2->send();
	OID boid = evt2->result();
	OID ifobj = Null;
	Buffer *def = Buffer::lookup(boid);
	delete evt2;
	if (def == 0) {
		buf[0] = 0;
		return;
	}
	
	for (int i=0; i<indent; i++) strcat(buf, "\t");
	
	while (index < m_size) {
		temp = def->get(index++);
		
		if (temp.isModifier()) {
			modi = temp.d();
		
			switch (modi) {
			case modifiers::ENDSUB:
				strcat(buf, ")");
				break;
			case modifiers::BEGINSUB:
				strcat(buf, "(");
				break;
			case modifiers::UNION:
				strcat(buf, "union ");
				break;
			case modifiers::COMPARE:
				strcat(buf, "== ");
				break;
			default: break;
			}
		} else {
			if (temp == This) {
				strcat(buf, ".");
			} else if (index == 1 && temp.get("type") == OID("if")) {
				ifobj = temp;
				strcat(buf, "if ");
				//buildif(buf, temp);
			} else {
				temp.toString(buf2, 50);
				strcat(buf, buf2);
				strcat(buf, " ");
			}
		}
	}
	
	if (ifobj != Null) buildif(buf, ifobj, indent);

	Buffer::free(boid);
}

void Definition::buildif(char *buf, OID ifobj, int indent) const {
	strcat(buf, " {\n");
	Definition d1(ifobj.definition("true"));
	Definition d2(ifobj.definition(Key));
	d1.toString(buf+strlen(buf), 1000, indent+1);
	strcat(buf, "\n");
	for (int i=0; i<indent; i++) strcat(buf, "\t");
	strcat(buf, "} else {\n");
	d2.toString(buf+strlen(buf), 1000, indent+1);
	strcat(buf, "\n");
	for (int i=0; i<indent; i++) strcat(buf, "\t");
	strcat(buf, "}");
}
