#include <QSplashScreen>
#include "schnapps.h"
#include <QFileInfo>
#include "PythonQt.h"
#include "gui/PythonQtScriptingConsole.h"

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
	
	QSplashScreen* splash = new QSplashScreen(QPixmap(":splash/cgogn/splash.png"));
	if(argc<3)
	{
		splash->show();
		splash->showMessage("Welcome to SCHNApps", Qt::AlignBottom | Qt::AlignCenter);
	}

	// init PythonQt and Python itself
	PythonQt::init();

	QStringList classNames;
	classNames.append("View");
	classNames.append("Camera");
	classNames.append("Plugin");
	classNames.append("MapHandlerGen");
	PythonQt::self()->registerQObjectClassNames(classNames);

	// get a smart pointer to the __main__ module of the Python interpreter
	PythonQtObjectPtr pythonContext = PythonQt::self()->getMainModule();

	PythonQtScriptingConsole* pythonConsole = new PythonQtScriptingConsole(NULL, pythonContext);

	CGoGN::SCHNApps::SCHNApps schnapps(app.applicationDirPath(), pythonContext, *pythonConsole);

	if(argc<3)
	{
		schnapps.show();
	}

	pythonContext.addObject("schnapps", &schnapps);

	if(argc > 1)
	{
		QString filename(argv[1]);
		QFileInfo fi(filename);
		if (fi.exists())
			//pythonContext.evalFile(fi.filePath());
			schnapps.loadPythonScriptFromFile(fi.filePath());
	}

	if(argc<3)
	{
		splash->finish(&schnapps);
	}
	delete splash;

	return app.exec();
}
