QMLMin Qt RCC Extension
======

Feature addition to QT rcc for minify QML and JS file on the final executable

(test with Qt 4.8.0)
Find your "rcc" directory source folder.
Add the files "jsmin.h" and "jsmin.cpp" to the project (.pri or .pro)


Add the include to jsmin.h to the file rcc.cpp :
	#include <QtCore/QIODevice>
	#include <QtCore/QLocale>
	#include <QtCore/QStack>

	#include <QtXml/QDomDocument>

	#include "jsmin.h"

	QT_BEGIN_NAMESPACE


Again on rcc.cpp, on the definition of RCCFileInfo::writeDataBlob() function, add the 4 lines as follow:
	//find the data to be written
	    QFile file(m_fileInfo.absoluteFilePath());
	    if (!file.open(QFile::ReadOnly)) {
	        *errorMessage = msgOpenReadFailed(m_fileInfo.absoluteFilePath(), file.errorString());
	        return 0;
	    }
	    QByteArray data = file.readAll();

	+	if (m_fileInfo.absoluteFilePath().endsWith(QString::fromUtf8(".js"))
	+			|| m_fileInfo.absoluteFilePath().endsWith(QString::fromUtf8(".qml"))) {
	+		data = JsMin().minify(data);
	+	}

	#ifndef QT_NO_COMPRESS
	    // Check if compression is useful for this file
	    if (m_compressLevel != 0 && data.size() != 0) {


After rebuild the project move the generated ./rcc binary to the bin/ folder with your qMake.