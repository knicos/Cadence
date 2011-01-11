#ifndef _SYNTAX_
#define _SYNTAX_

#include <QSyntaxHighlighter>
#include <QListWidget>

enum Token {TLP='(', RP=')', LS='{', RS='}', LC='[', RC=']', COMMENT='#', LO='<', RO='>', ASSIGN='=',
				STRING='"', CHARSTART='\'', OIDSEP=':', END=';', TERM=0, OBJECT='O', SEP=',', CMD='%', IVAR='@'};

class Syntax : public QSyntaxHighlighter
{
	public:
		Syntax(QTextDocument *textEdit, QListWidget *kw=0);
		~Syntax();

		void highlightBlock ( const QString & text );

		QTextCharFormat font_normal;
		QTextCharFormat font_comment;
		QTextCharFormat font_string;
		QTextCharFormat font_command;
		QTextCharFormat font_number;
		QTextCharFormat font_object;
		QTextCharFormat font_oid;
		QTextCharFormat font_char;
		QTextCharFormat font_op;
		QTextCharFormat font_ivar;
		
	private:
		QListWidget *m_keylist;
};


class EdenSyntax : public QSyntaxHighlighter
{
	public:
		EdenSyntax(QTextDocument *textEdit);
		~EdenSyntax();

		void highlightBlock ( const QString & text );

		QTextCharFormat font_normal;
		QTextCharFormat font_comment;
		QTextCharFormat font_string;
		QTextCharFormat font_function;
		QTextCharFormat font_number;
		QTextCharFormat font_char;
		QTextCharFormat font_op;
		QTextCharFormat font_type;
		QTextCharFormat font_notation;
};

#endif
