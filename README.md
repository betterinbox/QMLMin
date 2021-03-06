QMLMin Qt RCC Patch
===================

This patch adds the ability to minify QML and JS files before they are embedded in the final executable. The minifying is done with Douglas Crockford's JSMin algorithm (http://www.crockford.com/javascript/jsmin.html), which is very safe and works well with QML files.

Usage
-----

1 - Go to the rcc source directory (under your Qt source tree: src/tools/rcc)

2 - Download the files "jsmin.h" and "jsmin.cpp" and add them to rcc.pri:

	HEADERS += $$PWD/jsmin.h
	SOURCES += $$PWD/jsmin.cpp
	
3 - Include jsmin.h from file rcc.cpp, and add the following 4 lines inside RCCFileInfo::writeDataBlob, after QByteArray data = file.readAll();

	if (m_fileInfo.absoluteFilePath().endsWith(QString::fromUtf8(".js"))
		|| m_fileInfo.absoluteFilePath().endsWith(QString::fromUtf8(".qml"))) {
		data = JsMin().minify(data);
	}

4 - Rebuild rcc.pro, and check that the executable has been correctly installed into the bin/ folder under your Qt tree (together with the qmake executable)