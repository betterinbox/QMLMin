/* jsmin.c
   2008-08-03
   Ported to C++ by Adrien Chey <adrien@betterinbox.com>

Copyright (c) 2002 Douglas Crockford  (www.crockford.com)

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

The Software shall be used for Good, not Evil.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "jsmin.h"

#include <stdlib.h>
#include <stdio.h>

JsMin::JsMin():
	m_theLookahead(EOF),
	m_charPos(0)
{
}

QByteArray JsMin::minify(const QByteArray &data)
{
	m_data = data;
	jsmin();
	return m_output;
}

/* isAlphanum -- return true if the character is a letter, digit, underscore,
		dollar sign, or non-ASCII character.
*/

int JsMin::isAlphanum(int c)
{
	return ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') ||
		(c >= 'A' && c <= 'Z') || c == '_' || c == '$' || c == '\\' ||
		c > 126);
}


int JsMin::getDataChar()
{
	if (m_charPos >= m_data.length()) {
		return EOF;
	}
	int res = m_data.at(m_charPos);
	m_charPos++;
	return res;
}

/* get -- return the next character from stdin. Watch out for lookahead. If
		the character is a control character, translate it to a space or
		linefeed.
*/
int JsMin::get()
{
	int c = m_theLookahead;
	m_theLookahead = EOF;
	if (c == EOF) {
		c = getDataChar();
	}
	if (c >= ' ' || c == '\n' || c == EOF) {
		return c;
	}
	if (c == '\r') {
		return '\n';
	}
	return ' ';
}


/* peek -- get the next character without getting it.
*/

int JsMin::peek()
{
	m_theLookahead = get();
	return m_theLookahead;
}


/* next -- get the next character, excluding comments. peek() is used to see
		if a '/' is followed by a '/' or '*'.
*/

int JsMin::next()
{
	int c = get();
	if  (c == '/') {
		switch (peek()) {
		case '/':
			for (;;) {
				c = get();
				if (c <= '\n') {
					return c;
				}
			}
		case '*':
			get();
			for (;;) {
				switch (get()) {
				case '*':
					if (peek() == '/') {
						get();
						return ' ';
					}
					break;
				case EOF:
					fprintf(stderr, "Error: JSMIN Unterminated comment.\n");
					exit(1);
				}
			}
		default:
			return c;
		}
	}
	return c;
}


/* action -- do something! What you do is determined by the argument:
		1   Output A. Copy B to A. Get the next B.
		2   Copy B to A. Get the next B. (Delete A).
		3   Get the next B. (Delete B).
   action treats a string as a single character. Wow!
   action recognizes a regular expression if it is preceded by ( or , or =.
*/

void JsMin::putDataChar(int dataChar)
{
	m_output.append(dataChar);
}

void JsMin::action(int d)
{
	switch (d) {
	case 1:
		putDataChar(m_theA);
	case 2:
		 m_theA =  m_theB;
		if ( m_theA == '\'' ||  m_theA == '"') {
			for (;;) {
				putDataChar(m_theA);
				 m_theA = get();
				if ( m_theA ==  m_theB) {
					break;
				}
				if ( m_theA == '\\') {
					putDataChar(m_theA);
					 m_theA = get();
				}
				if ( m_theA == EOF) {
					fprintf(stderr, "Error: JSMIN unterminated string literal.");
					exit(1);
				}
			}
		}
	case 3:
		 m_theB = next();
		if ( m_theB == '/' && ( m_theA == '(' ||  m_theA == ',' ||  m_theA == '=' ||
							 m_theA == ':' ||  m_theA == '[' ||  m_theA == '!' ||
							 m_theA == '&' ||  m_theA == '|' ||  m_theA == '?' ||
							 m_theA == '{' ||  m_theA == '}' ||  m_theA == ';' ||
							 m_theA == '\n')) {
			putDataChar(m_theA);
			putDataChar(m_theB);
			for (;;) {
				 m_theA = get();
				if ( m_theA == '/') {
					break;
				}
				if ( m_theA =='\\') {
					putDataChar(m_theA);
					 m_theA = get();
				}
				if ( m_theA == EOF) {
					fprintf(stderr,
"Error: JSMIN unterminated Regular Expression literal.\n");
					exit(1);
				}
				putDataChar(m_theA);
			}
			 m_theB = next();
		}
	}
}


/* jsmin -- Copy the input to the output, deleting the characters which are
		insignificant to JavaScript. Comments will be removed. Tabs will be
		replaced with spaces. Carriage returns will be replaced with linefeeds.
		Most spaces and linefeeds will be removed.
*/

void JsMin::jsmin()
{
	m_theA = '\n';
	action(3);
	while ( m_theA != EOF) {
		switch ( m_theA) {
		case ' ':
			if (isAlphanum( m_theB)) {
				action(1);
			} else {
				action(2);
			}
			break;
		case '\n':
			switch ( m_theB) {
			case '{':
			case '[':
			case '(':
			case '+':
			case '-':
				action(1);
				break;
			case ' ':
				action(3);
				break;
			default:
				if (isAlphanum( m_theB)) {
					action(1);
				} else {
					action(2);
				}
			}
			break;
		default:
			switch ( m_theB) {
			case ' ':
				if (isAlphanum( m_theA)) {
					action(1);
					break;
				}
				action(3);
				break;
			case '\n':
				switch ( m_theA) {
				case '}':
				case ']':
				case ')':
				case '+':
				case '-':
				case '"':
				case '\'':
					action(1);
					break;
				default:
					if (isAlphanum( m_theA)) {
						action(1);
					} else {
						action(3);
					}
				}
				break;
			default:
				action(1);
				break;
			}
		}
	}
}
