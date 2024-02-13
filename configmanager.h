#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QApplication>
#include <QString>
#include <QStringList>
#include <QFile>
#include <QFileInfo>
#include <QJsonObject>
#include <QJsonDocument>

class ConfigManager
{
private:
	QString m_content;
	QString m_configFilePath = QApplication::applicationDirPath() + "/config.json";
	QJsonObject getRootobj();

public:
	QStringList m_sectionList;

	ConfigManager();
	~ConfigManager();

	void initConfig();
	void saveConfig();
	void loadConfig();
	void setValue(const QString& section, const QString& key, const QString& value);
	QString getValue(const QString& section, const QString& key);
	void removeKey(const QString& section, const QString& key);
	void removeSection(const QString& section);
};

#endif // CONFIGMANAGER_H
