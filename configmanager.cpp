#include "configmanager.h"
#include <QDebug>

ConfigManager::ConfigManager()
{
	initConfig();
}

ConfigManager::~ConfigManager()
{

}

void ConfigManager::initConfig()
{
	/*
	 * 1.如果未在文件目录下找到config 新建一个根对象 填入数据 保存到m_content 保存到config
	 * 2.已经找到config 加载到m_content
	*/
	QFileInfo fileinfo(m_configFilePath);
	if (fileinfo.isFile()) {
		qDebug() << "已在目录下找到config";

		loadConfig();
	}
	else {
		qDebug() << "未在目录下找到config";

		setValue("久坐", "标题", "久坐提醒小帮手");
		setValue("久坐", "文本", "休息一下~");
		setValue("久坐", "时", "0");
		setValue("久坐", "分", "30");
		setValue("久坐", "秒", "0");
		setValue("久坐", "声音", "默认");
		setValue("久坐", "播放模式", "默认");
		setValue("久坐", "重复", "始终");
		setValue("久坐", "间隔时间", "5");

		setValue("喝水", "标题", "喝水提醒小帮手");
		setValue("喝水", "文本", "该喝水了~");
		setValue("喝水", "时", "1");
		setValue("喝水", "分", "0");
		setValue("喝水", "秒", "0");
		setValue("喝水", "声音", "默认");
		setValue("喝水", "播放模式", "循环");
		setValue("喝水", "重复", "始终");
		setValue("喝水", "间隔时间", "5");

		saveConfig();
	}
}

void ConfigManager::saveConfig()
{
	//m_content -> config
	qDebug() << "正在将m_content保存到config";
	QFile file(m_configFilePath);
	if (!file.open(QIODevice::WriteOnly)) {
		qDebug() << "无法打开文件!";
		return;
	}

	QTextStream fileStream(&file);
	fileStream << m_content;
	file.flush();
	file.close();
}

void ConfigManager::loadConfig()
{
	//config -> m_content
	qDebug() << "正在将config导入到m_content";
	QFile file(m_configFilePath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qDebug() << "无法打开文件!";
		return;
	}

	QByteArray bytes = file.readAll();
	//qDebug() << "写入前的bytes=" << bytes.data();
	qDebug() << "从config写入前的m_content=" << m_content;
	m_content = bytes;
	qDebug() << "从config写入后的m_content=" << m_content;
	file.close();

	//m_content -> m_sectionList
	QJsonObject rootObj = getRootobj();
	m_sectionList = rootObj.keys();
}

void ConfigManager::setValue(const QString& section, const QString& key, const QString& value)
{
	/* 目的: 向根对象填入数据 保存到m_content
	 * 从根对象中查询是否存在section
	 * 1.存在 填入数据 保存到m_content
	 * 2.不存在 创建section对象 填入数据 保存到m_content
	*/
	qDebug() << "正在写入数据: " << section << "," << key << "," << value;
	QJsonObject rootObj = getRootobj();
	if (!rootObj.contains(section)) {
		//不存在 创建一个sectionObj
		QJsonObject obj;
		rootObj[section] = obj;

		//把section添加到section列表
		if (!m_sectionList.contains(section)) {
			m_sectionList.append(section);
		}
	}
	//存在
	QJsonObject sectionObj = rootObj[section].toObject();
	sectionObj[key] = value;

	rootObj[section] = sectionObj;
	QJsonDocument document;
	document.setObject(rootObj);
	m_content = document.toJson();
}

QString ConfigManager::getValue(const QString& section, const QString& key)
{
	QString returnValue;
	QJsonObject rootObj = getRootobj();
	if (rootObj.contains(section)) {
		QJsonObject sectionObj = rootObj[section].toObject();
		if (sectionObj.contains(key)) {
			returnValue = sectionObj[key].toString();
		}
	}
	return returnValue;
}

void ConfigManager::removeKey(const QString& section, const QString& key)
{
	QJsonObject rootObj = getRootobj();
	if (rootObj.contains(section)) {
		QJsonObject sectionObj = rootObj[section].toObject();
		if (sectionObj.contains(key)) {
			sectionObj.remove(key);
		}

		rootObj[section] = sectionObj;
		QJsonDocument document;
		document.setObject(rootObj);
		m_content = document.toJson();
	}
}

void ConfigManager::removeSection(const QString& section)
{
	QJsonObject rootObj = getRootobj();
	if (rootObj.contains(section)) {
		rootObj.remove(section);
		m_sectionList.removeOne(section);
	}
	QJsonDocument document;
	document.setObject(rootObj);
	m_content = document.toJson();
}

QJsonObject ConfigManager::getRootobj()
{
	QJsonObject returnObj;
	QJsonParseError jsonError;
	QJsonDocument document = QJsonDocument::fromJson(m_content.toStdString().c_str(), &jsonError);

	if ((!document.isNull()) && (jsonError.error == QJsonParseError::NoError)) {
		if (document.isObject()) {
			//如果m_content中有内容 返回根对象
			returnObj = document.object();
		}
	}
	else {
		//如果m_content中无内容 创建一个根对象 放入m_content 返回根对象
		QJsonDocument doc;
		QJsonObject obj;
		doc.setObject(obj);
		m_content = doc.toJson();

		returnObj = obj;
	}
	return returnObj;
}
