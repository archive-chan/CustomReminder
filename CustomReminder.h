#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_CustomReminder.h"
#include "configmanager.h"
#include <QTimer>
#include <QTime>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QCloseEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class CustomReminder; };
QT_END_NAMESPACE

class CustomReminder : public QMainWindow
{
    Q_OBJECT

public:
	bool isChanged = false;
	bool timerMiddleRun = false;

	ConfigManager* m_configManager;
	QMenu* m_trayMenu;
	QSystemTrayIcon* m_trayIcon;
	QTimer* m_timer;
	QTimer* m_timerMiddle;
	QTime m_time{ 0,0,0 };
	QTime m_timeMiddle{ 0,0,0 };
	QTime m_timeGo{ 0,0,0 };
	QString m_presetToUpdate;

	QAction* homeAction;
	QAction* presetsAction;
	QAction* settingsAction;
	QAction* recoverAction;
	QAction* exitAction;

public:
    CustomReminder(QWidget *parent = nullptr);
    ~CustomReminder();

    void initUi();
    void updateUi();
	void titleChange();
	void selectPreset(const QString& preset);
	void showNotification(const QString& title, const QString& text);

public slots:
	void homeActionSlot();
	void presetsActionSlot();
	void settingsActionSlot();
	void exitActionSlot();
	void recoverActionSlot();

	void startButtonSlot();
	void newButtonSlot();
	void deleteButtonSlot();
	void saveButtonSlot();
	void testButtonSlot();

	void continueButtonSlot();
	void stopButtonSlot();
	void endButtonSlot();

	void startActivatedSlot(const QString& section);
	void presetsActivatedSlot(const QString& section);
	void repeatActivatedSlot(const QString& repeat);

	void changeSlot(const QString& change);

	void timerOutSlot();
	void timerMiddleOutSlot();

	void trayClickedSlot(QSystemTrayIcon::ActivationReason reason);
	void topCheckSlot(int state);

protected:
	void closeEvent(QCloseEvent* e);

private:
    Ui::CustomReminder *ui;
};
