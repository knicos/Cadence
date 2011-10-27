#include "syntax.h"
#include <iostream>
#include <cadence/doste/oid.h>
#include <cadence/doste/doste.h>

using namespace cadence;
using namespace cadence::doste;

Syntax::Syntax(QTextDocument *textEdit, QListWidget *kw)
	: QSyntaxHighlighter(textEdit)
{
	m_keylist = kw;
	
	font_command.setFont(QFont("Courier New", 10, QFont::Bold));
	font_object.setFont(font_command.font());
	font_comment.setFont(QFont("Courier New", 10));
	font_normal.setFont(font_comment.font());
	font_oid.setFont(font_object.font());
	font_string.setFont(font_normal.font());
	font_number.setFont(font_normal.font());
	font_char.setFont(font_number.font());
	font_op.setFont(font_command.font());
	font_ivar.setFont(font_command.font());

	font_command.setForeground(QColor(39,104,26));
	font_object.setForeground(QColor(38,140,223));
	font_comment.setForeground(QColor(130,130,130));
	font_normal.setForeground(QColor("black"));
	font_oid.setForeground(QColor("red"));
	font_string.setForeground(QColor("red"));
	font_number.setForeground(QColor("blue"));
	font_char.setForeground(QColor(255,25,241));
	font_op.setForeground(QColor("black"));
	font_ivar.setForeground(QColor(255,25,241));
}

Syntax::~Syntax()
{

}

void Syntax::highlightBlock ( const QString & text )
{
	unsigned int pos = 0;
	unsigned int beg = 0;
	bool istring;
	unsigned int i;
	const char *text2 = (const char*)text.toAscii();
	
	OID cur;
	bool start = true;
	
	//std::cout << "SYNTAX: " << text2 << "\n";

	char buffer[1000];
	buffer[0] = 0;

	while (pos < text.length())
	{
		//Remove all white space from the buffer, reguardless of whether its in peek mode.
		beg = pos;
		while ((pos < text.length()) && (text2[pos] == ' ' || text2[pos] == '\n' || text2[pos] == '\r' || text2[pos] == '\t'))
		{
			pos++;
		}
		if (beg != pos) setFormat(beg,pos-beg, font_normal);

		if (pos == text.length()) return;
		if (text2[pos] == 0) return;
		
		//std::cout << pos << ":" << text.length() << " = " << (int)text2[pos] << "\n";
	
		switch (text2[pos])
		{
			case COMMENT:	beg = pos;
							pos++;
							while (text2[pos] != '\n' && text2[pos] != (char)0) {			//Remove comment until new line
								pos++;
								if (pos == text.length()) break;
							}
							setFormat(beg, pos-beg, font_comment);
							break;
			case CMD:		beg = pos;
							pos++;
							i = 0;
							while ((pos < text.length()) && text2[pos] != ' ' && text2[pos] != '\t' && text2[pos] != (char)0)
							{
								buffer[i] = text2[pos];
								pos++;
								i++;
								if (pos == text.length()) break;
							}
							buffer[i] = 0;
							if (strcmp(buffer, "include") == 0)
								setFormat(beg, pos-beg, font_command);
							else if (strcmp(buffer, "list") == 0)
								setFormat(beg, pos-beg, font_command);
							else if (strcmp(buffer, "debug") == 0)
								setFormat(beg, pos-beg, font_command);
							else if (strcmp(buffer, "query") == 0)
								setFormat(beg, pos-beg, font_command);
							else if (strcmp(buffer, "using") == 0)
								setFormat(beg, pos-beg, font_command);
							else if (strcmp(buffer, "def") == 0)
								setFormat(beg, pos-beg, font_command);
							else if (strcmp(buffer, "print") == 0)
								setFormat(beg, pos-beg, font_command);
							else if (strcmp(buffer, "hide") == 0)
								setFormat(beg, pos-beg, font_command);
							else
								setFormat(beg, pos-beg, font_normal);
							break;
			case IVAR:		beg = pos;
							pos++;
							i = 0;
							while ((pos < text.length()) && text2[pos] != '(' && text2[pos] != ' ' && text2[pos] != '\t' && text2[pos] != (char)0)
							{
								buffer[i] = text2[pos];
								pos++;
								if (pos == text.length()) break;
								i++;
							}
							buffer[i] = 0;
							if (start) {
								//cur = root["notations"]["dasm"]["variables"][buffer];
							}
							start = false;
							setFormat(beg, pos-beg, font_ivar);
							break;
			case TERM:		break;
			case TLP:
			case RP:
			case LO:
			case RO:
			case RS:
			case LS:		pos++; break;
			case LC:		beg = pos;
						pos++;
						start = true;
						break;
			case RC:		pos++;
							setFormat(beg, pos-beg, font_oid);
							break;
			case ASSIGN:	pos++;
							break;
			case SEP:		pos++;
							break;
			case '.':		pos++;
						if (start) {
							//cur = root;
							start = false;
						}
						break;
			case END:		pos++;
							break;
	
			default:		beg = pos;
							i = 0;
							istring = false;
							while (pos < text.length() && (istring || (text2[pos] != ' ' && text2[pos] != '\t' && text2[pos] != (char)0 && text2[pos] != '\n' && text2[pos] != '(' && text2[pos] != ')' && text2[pos] != ';' && text2[pos] != ',' && text2[pos] != '<' && text2[pos] != '>')))
							{
								if (text2[pos] == '"') {
									istring = !istring;
								}
								buffer[i] = text2[pos];
								pos++;
								i++;
							}
							buffer[i] = 0;
							if (strcmp(buffer, "this") == 0) {
								setFormat(beg, pos-beg, font_object);
								if (start) {
									//cur = This;
								} else {
									if (text2[pos] != 0) {
										//cur = cur.get(OID(buffer));
									}
								}
							} else if (strcmp(buffer, "true") == 0) {
								setFormat(beg, pos-beg, font_object);
								if (start) {
									//cur = True;
								} else {
									if (text2[pos] != 0) {
										//cur = cur.get(OID(buffer));
									}
								}
							} else if (strcmp(buffer, "false") == 0) {
								setFormat(beg, pos-beg, font_object);
								if (start) {
									//cur = False;
								} else {
									if (text2[pos] != 0) {
										//cur = cur.get(OID(buffer));
									}
								}
							} else if (strcmp(buffer, "null") == 0) {
								setFormat(beg, pos-beg, font_object);
								if (start) {
									//cur = Null;
								} else {
									if (text2[pos] != 0) {
										//cur = cur.get(OID(buffer));
									}
								}
							} else if (strcmp(buffer, "is") == 0)
								setFormat(beg, pos-beg, font_op);
							else if (strcmp(buffer, "if") == 0)
								setFormat(beg, pos-beg, font_op);
							else if (strcmp(buffer, "new") == 0)
								setFormat(beg, pos-beg, font_op);
							else if (strcmp(buffer, "union") == 0)
								setFormat(beg, pos-beg, font_op);
							else if (strcmp(buffer, "else") == 0)
								setFormat(beg, pos-beg, font_op);
							else if (strcmp(buffer, "$") == 0)
								setFormat(beg, pos-beg, font_object);
							else
							{
								//Check for numbers, characters and strings
								if (buffer[0] == '"')
									setFormat(beg, pos-beg, font_string);
								else if (buffer[0] == '0' || buffer[0] == '1' || buffer[0] == '2' || buffer[0] == '3' || buffer[0] == '4' || buffer[0] == '5' || buffer[0] == '6' || buffer[0] == '7' || buffer[0] == '8' || buffer[0] == '9' || buffer[0] == '-') {
									setFormat(beg, pos-beg, font_number);
									if (start) {
										//cur = OID(buffer);
									} else {
										if (text2[pos] != 0) {
											//cur = cur.get(OID(buffer));
										}
									}
								} else if (buffer[0] == '\'') {
									setFormat(beg, pos-beg, font_char);
									if (start) {
										//cur = OID(buffer);
									} else {
										if (text2[pos] != 0) {
											//cur = cur.get(OID(buffer));
										}
									}
								} else {
									setFormat(beg, pos-beg, font_normal);
									if (start) {
										//cur = OID(buffer);
									} else {
										if (text2[pos] != 0) {
											//cur = cur.get(OID(buffer));
										}
									}
									
								}
							}
							break;
		}
	}
	
	//char buf2[100];
	//int slen = strlen(buffer);
	
	/*if (m_keylist != 0) {
		QListWidgetItem *citem;
		while (citem = m_keylist->takeItem(0))
			delete citem;
		
		for (OID::iterator i=cur.begin(); i!=cur.end(); i++) {
			(*i).toString(buf2,100);
			if (strncmp(buf2, buffer, slen) == 0)
				m_keylist->addItem(buf2);
		}
	}*/
}


//========================== EDEN =============================


EdenSyntax::EdenSyntax(QTextDocument *textEdit)
	: QSyntaxHighlighter(textEdit)
{
	font_notation.setFont(QFont("Courier New", 10, QFont::Bold));
	font_comment.setFont(QFont("Courier New", 10));
	font_normal.setFont(font_comment.font());
	font_string.setFont(font_normal.font());
	font_number.setFont(font_normal.font());
	font_char.setFont(font_number.font());
	font_op.setFont(font_notation.font());
	font_type.setFont(font_normal.font());
	font_function.setFont(font_normal.font());

	font_notation.setForeground(QColor(39,104,26));
	font_comment.setForeground(QColor(130,130,130));
	font_normal.setForeground(QColor("black"));
	font_string.setForeground(QColor("red"));
	font_number.setForeground(QColor("blue"));
	font_char.setForeground(QColor(255,25,241));
	font_op.setForeground(QColor("black"));
	font_type.setForeground(QColor(126,0,0));
	font_function.setForeground(QColor(0,105,186));
}

EdenSyntax::~EdenSyntax()
{

}

void EdenSyntax::highlightBlock ( const QString & text )
{
	unsigned int pos = 0;
	unsigned int beg = 0;
	bool istring;
	unsigned int i;

	char buffer[1000];

	while (pos < text.length())
	{
		//Remove all white space from the buffer, reguardless of whether its in peek mode.
		while ((pos < text.length()) && (text[pos] == ' ' || text[pos] == '\n' || text[pos] == '\r' || text[pos] == '\t'))
		{
			pos++;
		}

		if (pos == text.length()) return;
	
		switch (text[pos].toAscii())
		{
			case '#':			beg = pos;
							pos++;
							while (text[pos] != '\n' && text[pos] != (char)0) {			//Remove comment until new line
								pos++;
								if (pos == text.length()) break;
							}
							setFormat(beg, pos-beg, font_comment);
							break;
			case '%':			beg = pos;
							pos++;
							i = 0;
							while ((pos < text.length()) && text[pos] != ' ' && text[pos] != '\t' && text[pos] != (char)0)
							{
								buffer[i] = text[pos].toAscii();
								pos++;
								i++;
								if (pos == text.length()) break;
							}
							buffer[i] = 0;
							if (strcmp(buffer, "eden") == 0)
								setFormat(beg, pos-beg, font_notation);
							else if (strcmp(buffer, "donald") == 0)
								setFormat(beg, pos-beg, font_notation);
							else if (strcmp(buffer, "scout") == 0)
								setFormat(beg, pos-beg, font_notation);
							else if (strcmp(buffer, "aop") == 0)
								setFormat(beg, pos-beg, font_notation);
							else if (strcmp(buffer, "eddi") == 0)
								setFormat(beg, pos-beg, font_notation);
							else if (strcmp(buffer, "dasm") == 0)
								setFormat(beg, pos-beg, font_notation);
							else if (strcmp(buffer, "cadence") == 0)
								setFormat(beg, pos-beg, font_notation);
							else if (strcmp(buffer, "sasami") == 0)
								setFormat(beg, pos-beg, font_notation);
							else
								setFormat(beg, pos-beg, font_normal);
							break;
			
			case '/':	beg = pos; pos++;
					if (pos < text.length() && text[pos] == '*') {
						pos++;
						while (pos < text.length()-1 && !(text[pos] == '*' && text[pos+1] == '/')) {
							pos++;
						}
						setFormat(beg,pos-beg, font_comment);
					} else break;
			case TERM:		break;
			case TLP:
			case RP:
			case LO:
			case RO:
			case RS:
			case RC:
			case LC:
			case LS:		pos++; break;
			case ASSIGN:	pos++;
							break;
			case SEP:		pos++;
							break;
			case END:		pos++;
							break;
	
			default:		beg = pos;
							i = 0;
							istring = false;
							while (pos < text.length() && (istring || (text[pos] != ' ' && text[pos] != '\t' && text[pos] != (char)0 && text[pos] != '\n' && text[pos] != '(' && text[pos] != ')' && text[pos] != ';' && text[pos] != ',' && text[pos] != '<' && text[pos] != '>')))
							{
								if (text[pos] == '"')
									istring = !istring;
								buffer[i] = text[pos].toAscii();
								pos++;
								i++;
							}
							buffer[i] = 0;
							if (strcmp(buffer, "is") == 0)
								setFormat(beg, pos-beg, font_op);
							else if (strcmp(buffer, "proc") == 0)
								setFormat(beg, pos-beg, font_op);
							else if (strcmp(buffer, "func") == 0)
								setFormat(beg, pos-beg, font_op);
							else if (strcmp(buffer, "if") == 0)
								setFormat(beg, pos-beg, font_op);
							else if (strcmp(buffer, "else") == 0)
								setFormat(beg, pos-beg, font_op);
							else if (strcmp(buffer, "switch") == 0)
								setFormat(beg, pos-beg, font_op);
							else if (strcmp(buffer, "case") == 0)
								setFormat(beg, pos-beg, font_op);
							else if (strcmp(buffer, "break") == 0)
								setFormat(beg, pos-beg, font_op);
							else if (strcmp(buffer, "default") == 0)
								setFormat(beg, pos-beg, font_op);
							else if (strcmp(buffer, "continue") == 0)
								setFormat(beg, pos-beg, font_op);
							else if (strcmp(buffer, "for") == 0)
								setFormat(beg, pos-beg, font_op);
							else if (strcmp(buffer, "while") == 0)
								setFormat(beg, pos-beg, font_op);
							else if (strcmp(buffer, "return") == 0)
								setFormat(beg, pos-beg, font_op);
							else if (strcmp(buffer, "auto") == 0)
								setFormat(beg, pos-beg, font_type);
							else if (strcmp(buffer, "para") == 0)
								setFormat(beg, pos-beg, font_type);
							else if (strcmp(buffer, "execute") == 0)
								setFormat(beg, pos-beg, font_function);
							else if (strcmp(buffer, "writeln") == 0)
								setFormat(beg, pos-beg, font_function);
							else if (strcmp(buffer, "write") == 0)
								setFormat(beg, pos-beg, font_function);
							else if (strcmp(buffer, "type") == 0)
								setFormat(beg, pos-beg, font_function);
							else if (strcmp(buffer, "include") == 0)
								setFormat(beg, pos-beg, font_function);
							else if (strcmp(buffer, "cd") == 0)
								setFormat(beg, pos-beg, font_function);
							else if (strcmp(buffer, "pwd") == 0)
								setFormat(beg, pos-beg, font_function);
							else
							{
								//Check for numbers, characters and strings
								if (buffer[0] == '"')
									setFormat(beg, pos-beg, font_string);
								else if (buffer[0] == '0' || buffer[0] == '1' || buffer[0] == '2' || buffer[0] == '3' || buffer[0] == '4' || buffer[0] == '5' || buffer[0] == '6' || buffer[0] == '7' || buffer[0] == '8' || buffer[0] == '9' || buffer[0] == '-')
									setFormat(beg, pos-beg, font_number);
								else if (buffer[0] == '\'')
									setFormat(beg, pos-beg, font_char);
								else
									setFormat(beg, pos-beg, font_normal);
							}
							break;
		}
	}
}
