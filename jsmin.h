#ifndef JSMIN_H
#define JSMIN_H

#include <QByteArray>

class JsMin
{
public:
	JsMin();
	QByteArray minify(const QByteArray &data);

private:
	void jsmin();
	void action(int d);
	int next();
	int isAlphanum(int c);
	int get();
	int peek();
	int getDataChar();
	void putDataChar(int dataChar);

private:
	int m_theA;
	int m_theB;
	int m_theLookahead;
	QByteArray m_output;
	QByteArray m_data;
	int m_charPos;
};

#endif // JSMIN_H
