#include "CustomReminder.h"

#include <QDebug>
#include <QMessageBox>
#include <QWindow>
#include <QFontDatabase>
#include "wintoastlib.h"
using namespace WinToastLib;

CustomReminder::CustomReminder(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CustomReminder())
{
    ui->setupUi(this);
    initUi();
    
}

CustomReminder::~CustomReminder()
{
    delete ui;
}

void CustomReminder::initUi()
{
    this->setWindowTitle("自定义提醒");
    this->setFixedSize(400, 300);
    this->setWindowIcon(QIcon(":/image/Application.ico"));

    m_configManager = new ConfigManager;
    m_timer = new QTimer(this);
    m_timerMiddle = new QTimer(this);
    m_trayMenu = new QMenu(this);
    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setContextMenu(m_trayMenu);
    m_trayIcon->setIcon(QIcon(":/image/Application.ico"));
    m_trayIcon->setToolTip("自定义提醒");
    m_trayIcon->show();
    m_presetToUpdate = m_configManager->m_sectionList.at(0);

    homeAction = new QAction(this);
    presetsAction = new QAction(this);
    settingsAction = new QAction(this);
    recoverAction = new QAction(this);
    exitAction = new QAction(this);

    homeAction = ui->mainToolBar->addAction("主页");
    presetsAction = ui->mainToolBar->addAction("预设");
    settingsAction = ui->mainToolBar->addAction("设置");
    recoverAction = m_trayMenu->addAction("还原窗口");
    exitAction = m_trayMenu->addAction("退出");

    ui->mainToolBar->setMovable(false);
    ui->stackedWidget->setCurrentIndex(0);
    ui->HourSpinBox->setMaximum(23);
    ui->MinuteSpinBox->setMaximum(59);
    ui->SecondSpinBox->setMaximum(59);
    ui->HourSpinBox->setWrapping(true);
    ui->MinuteSpinBox->setWrapping(true);
    ui->SecondSpinBox->setWrapping(true);

    int fontId = QFontDatabase::addApplicationFont(":/font/VerticalSans.ttf");
    QString font = QFontDatabase::applicationFontFamilies(fontId).at(0);
    QFont f(font, 52);
    ui->TimerLabel->setFont(f);

    updateUi();

    WinToast::instance()->setAppName(L"CustomReminder");
    WinToast::instance()->setAppUserModelId(WinToast::configureAUMI(L"ARCHIVE", L"CUSTOMREMINDER", L"CustomReminder", L"240214Ver1_2"));
    if (!WinToast::instance()->initialize()) {
        qDebug() << "Error, your system in not compatible!";
    }

    connect(homeAction, &QAction::triggered, this, &CustomReminder::homeActionSlot);
    connect(presetsAction, &QAction::triggered, this, &CustomReminder::presetsActionSlot);
    connect(settingsAction, &QAction::triggered, this, &CustomReminder::settingsActionSlot);
    connect(exitAction, &QAction::triggered, this, &CustomReminder::exitActionSlot);
    connect(recoverAction, &QAction::triggered, this, &CustomReminder::recoverActionSlot);

    connect(ui->StartButton, &QPushButton::clicked, this, &CustomReminder::startButtonSlot);
    connect(ui->NewButton, &QPushButton::clicked, this, &CustomReminder::newButtonSlot);
    connect(ui->DeleteButton, &QPushButton::clicked, this, &CustomReminder::deleteButtonSlot);
    connect(ui->SaveButton, &QPushButton::clicked, this, &CustomReminder::saveButtonSlot);
    connect(ui->TestButton, &QPushButton::clicked, this, &CustomReminder::testButtonSlot);

    connect(ui->ContinueButton, &QPushButton::clicked, this, &CustomReminder::continueButtonSlot);
    connect(ui->StopButton, &QPushButton::clicked, this, &CustomReminder::stopButtonSlot);
    connect(ui->EndButton, &QPushButton::clicked, this, &CustomReminder::endButtonSlot);

    connect(ui->StartComboBox, &QComboBox::textActivated, this, &CustomReminder::startActivatedSlot);
    connect(ui->PresetsComboBox, &QComboBox::textActivated, this, &CustomReminder::presetsActivatedSlot);
    connect(ui->RepeatComboBox, &QComboBox::currentTextChanged, this, &CustomReminder::repeatActivatedSlot);

    connect(ui->TitleLineEdit, &QLineEdit::textEdited, this, &CustomReminder::changeSlot);
    connect(ui->TextLineEdit, &QLineEdit::textEdited, this, &CustomReminder::changeSlot);
    connect(ui->HourSpinBox, &QSpinBox::textChanged, this, &CustomReminder::changeSlot);
    connect(ui->MinuteSpinBox, &QSpinBox::textChanged, this, &CustomReminder::changeSlot);
    connect(ui->SecondSpinBox, &QSpinBox::textChanged, this, &CustomReminder::changeSlot);
    connect(ui->SoundComboBox, &QComboBox::textActivated, this, &CustomReminder::changeSlot);
    connect(ui->PlayModeComboBox, &QComboBox::textActivated, this, &CustomReminder::changeSlot);
    connect(ui->RepeatComboBox, &QComboBox::textActivated, this, &CustomReminder::changeSlot);
    connect(ui->MiddleTimeComboBox, &QComboBox::textActivated, this, &CustomReminder::changeSlot);

    connect(m_timer, &QTimer::timeout, this, &CustomReminder::timerOutSlot);
    connect(m_timerMiddle, &QTimer::timeout, this, &CustomReminder::timerMiddleOutSlot);

    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &CustomReminder::trayClickedSlot);
    connect(ui->TopCheckBox, &QCheckBox::stateChanged, this, &CustomReminder::topCheckSlot);
}

void CustomReminder::updateUi()
{
    ui->HourSpinBox->blockSignals(true);
    ui->MinuteSpinBox->blockSignals(true);
    ui->SecondSpinBox->blockSignals(true);

    //清空数据
    ui->StartComboBox->clear();
    ui->PresetsComboBox->clear();
    ui->SoundComboBox->clear();
    ui->PlayModeComboBox->clear();
    ui->RepeatComboBox->clear();
    ui->MiddleTimeComboBox->clear();
    ui->TitleLineEdit->clear();
    ui->TextLineEdit->clear();
    ui->HourSpinBox->setValue(0);
    ui->MinuteSpinBox->setValue(0);
    ui->SecondSpinBox->setValue(0);

    //导入数据
    ui->StartComboBox->addItems(m_configManager->m_sectionList);
    ui->PresetsComboBox->addItems(m_configManager->m_sectionList);
    ui->SoundComboBox->addItem("默认", WinToastTemplate::AudioSystemFile::DefaultSound);
    ui->SoundComboBox->addItem("短信", WinToastTemplate::AudioSystemFile::SMS);
    ui->SoundComboBox->addItem("邮件", WinToastTemplate::AudioSystemFile::Mail);
    ui->SoundComboBox->addItem("闹钟", WinToastTemplate::AudioSystemFile::Alarm);
    ui->PlayModeComboBox->addItem("默认", WinToastTemplate::AudioOption::Default);
    ui->PlayModeComboBox->addItem("循环", WinToastTemplate::AudioOption::Loop);
    ui->PlayModeComboBox->addItem("静音", WinToastTemplate::AudioOption::Silent);
    ui->RepeatComboBox->addItem("始终");
    ui->RepeatComboBox->addItem("仅一次");
    ui->MiddleTimeComboBox->addItem("0");
    ui->MiddleTimeComboBox->addItem("5");
    ui->MiddleTimeComboBox->addItem("10");
    ui->MiddleTimeComboBox->addItem("15");
    ui->MiddleTimeComboBox->addItem("30");
    ui->MiddleTimeComboBox->addItem("60");

    //显示待更新的数据
    ui->StartComboBox->setCurrentText(m_presetToUpdate);
    ui->PresetsComboBox->setCurrentText(m_presetToUpdate);
    ui->TitleLineEdit->setText(m_configManager->getValue(m_presetToUpdate, "标题"));
    ui->TextLineEdit->setText(m_configManager->getValue(m_presetToUpdate, "文本"));
    ui->HourSpinBox->setValue(m_configManager->getValue(m_presetToUpdate, "时").toInt());
    ui->MinuteSpinBox->setValue(m_configManager->getValue(m_presetToUpdate, "分").toInt());
    ui->SecondSpinBox->setValue(m_configManager->getValue(m_presetToUpdate, "秒").toInt());
    ui->SoundComboBox->setCurrentText(m_configManager->getValue(m_presetToUpdate, "声音"));
    ui->PlayModeComboBox->setCurrentText(m_configManager->getValue(m_presetToUpdate, "播放模式"));
    ui->RepeatComboBox->setCurrentText(m_configManager->getValue(m_presetToUpdate, "重复"));
    if (ui->RepeatComboBox->currentText() == "始终") {
        ui->MiddleTimeComboBox->setCurrentText(m_configManager->getValue(m_presetToUpdate, "间隔时间"));
    }
    else {
        ui->MiddleTimeLabel->hide();
        ui->MiddleMinuteLabel->hide();
        ui->MiddleTimeComboBox->hide();
    }

    ui->HourSpinBox->blockSignals(false);
    ui->MinuteSpinBox->blockSignals(false);
    ui->SecondSpinBox->blockSignals(false);
}

void CustomReminder::titleChange()
{
    if (isChanged) {
        this->setWindowTitle("自定义提醒*");
    }
    else {
        this->setWindowTitle("自定义提醒");
    }
}

void CustomReminder::selectPreset(const QString& preset)
{
    ui->HourSpinBox->blockSignals(true);
    ui->MinuteSpinBox->blockSignals(true);
    ui->SecondSpinBox->blockSignals(true);

    ui->TitleLineEdit->setText(m_configManager->getValue(preset, "标题"));
    ui->TextLineEdit->setText(m_configManager->getValue(preset, "文本"));
    ui->HourSpinBox->setValue(m_configManager->getValue(preset, "时").toInt());
    ui->MinuteSpinBox->setValue(m_configManager->getValue(preset, "分").toInt());
    ui->SecondSpinBox->setValue(m_configManager->getValue(preset, "秒").toInt());
    ui->SoundComboBox->setCurrentText(m_configManager->getValue(preset, "声音"));
    ui->PlayModeComboBox->setCurrentText(m_configManager->getValue(preset, "播放模式"));
    ui->RepeatComboBox->setCurrentText(m_configManager->getValue(preset, "重复"));

    ui->HourSpinBox->blockSignals(false);
    ui->MinuteSpinBox->blockSignals(false);
    ui->SecondSpinBox->blockSignals(false);

    m_presetToUpdate = preset;
}

class CustomHandler : public IWinToastHandler {
public:
    void toastActivated() const {
        std::wcout << L"The user clicked in this toast" << std::endl;
    }

    void toastActivated(int actionIndex) const {
        std::wcout << L"The user clicked on button #" << actionIndex << L" in this toast" << std::endl;
    }

    void toastFailed() const {
        std::wcout << L"Error showing current toast" << std::endl;
    }
    void toastDismissed(WinToastDismissalReason state) const {
        switch (state) {
        case UserCanceled:
            std::wcout << L"The user dismissed this toast" << std::endl;
            break;
        case ApplicationHidden:
            std::wcout << L"The application hid the toast using ToastNotifier.hide()" << std::endl;
            break;
        case TimedOut:
            std::wcout << L"The toast has timed out" << std::endl;
            break;
        default:
            std::wcout << L"Toast not activated" << std::endl;
            break;
        }
    }
};

void CustomReminder::showNotification(const QString& title, const QString& text)
{
    WinToastTemplate toast = WinToastTemplate(WinToastTemplate::Text02);
    toast.setTextField(title.toStdWString(), WinToastTemplate::FirstLine);
    toast.setTextField(text.toStdWString(), WinToastTemplate::SecondLine);
    toast.setAudioPath(static_cast<WinToastTemplate::AudioSystemFile>(ui->SoundComboBox->currentData().toInt()));
    toast.setAudioOption(static_cast<WinToastTemplate::AudioOption>(ui->PlayModeComboBox->currentData().toInt()));

    if (WinToast::instance()->showToast(toast, new CustomHandler()) < 0) {
        QMessageBox::warning(this, "Error", "Could not launch your toast notification!");
    }
}

void CustomReminder::homeActionSlot()
{
	if (isChanged) {
		QMessageBox::warning(this, "警告", "还没有保存!");
		return;
	}
	ui->stackedWidget->setCurrentIndex(0);
}
void CustomReminder::presetsActionSlot()
{
	ui->stackedWidget->setCurrentIndex(1);
}
void CustomReminder::settingsActionSlot()
{
	ui->stackedWidget->setCurrentIndex(2);
}
void CustomReminder::exitActionSlot()
{
	exit(0);
}
void CustomReminder::recoverActionSlot()
{
	this->showNormal();
}

void CustomReminder::startButtonSlot()
{
	QString preset = ui->StartComboBox->currentText();
	int h = m_configManager->getValue(preset, "时").toInt();
	int m = m_configManager->getValue(preset, "分").toInt();
	int s = m_configManager->getValue(preset, "秒").toInt();
	int sec = h * 3600 + m * 60 + s;

	m_time.setHMS(0, 0, 0);
	m_timeGo.setHMS(0, 0, 0);
	m_time = m_time.addSecs(sec);

	ui->mainToolBar->hide();
	ui->stackedWidget->setCurrentIndex(3);
	ui->TimerLabel->setText(m_time.toString("hh:mm:ss"));
	ui->ContinueButton->setEnabled(false);
	m_timer->start(1000);
	qDebug() << "开始预设倒计时: " << m_time.toString("hh:mm:ss");

	if (ui->RepeatComboBox->currentText() == "始终") {
		//始终重复
		QString minute = ui->MiddleTimeComboBox->currentText();
		qDebug() << "间隔时间: " << minute << "分钟";
		int second = minute.toInt() * 60;
		int h = second / 3600;
		int m = (second - (h * 3600)) / 60;

		m_timeMiddle.setHMS(h, m, 0);
	}
}
void CustomReminder::newButtonSlot()
{
	isChanged = true;
	titleChange();

	ui->PresetsComboBox->setEditable(true);
	ui->NewButton->setEnabled(false);
	ui->DeleteButton->setEnabled(false);
	ui->TestButton->setEnabled(false);
	//////////////////////////////////////////////隐藏选择预设的按钮

	ui->HourSpinBox->blockSignals(true);
	ui->MinuteSpinBox->blockSignals(true);
	ui->SecondSpinBox->blockSignals(true);

	ui->PresetsComboBox->clearEditText();
	ui->TitleLineEdit->clear();
	ui->TextLineEdit->clear();
	ui->HourSpinBox->setValue(0);
	ui->MinuteSpinBox->setValue(0);
	ui->SecondSpinBox->setValue(0);
	ui->SoundComboBox->setCurrentText("默认");
	ui->PlayModeComboBox->setCurrentText("默认");
	ui->RepeatComboBox->setCurrentText("始终");
	ui->MiddleTimeComboBox->setCurrentText("5");
	ui->PresetsComboBox->lineEdit()->setPlaceholderText("请输入预设名");
	ui->TitleLineEdit->setPlaceholderText("请输入标题");
	ui->TextLineEdit->setPlaceholderText("请输入文本");

	ui->HourSpinBox->blockSignals(false);
	ui->MinuteSpinBox->blockSignals(false);
	ui->SecondSpinBox->blockSignals(false);
}
void CustomReminder::deleteButtonSlot()
{
	QString preset = ui->PresetsComboBox->currentText();
	if (QMessageBox::Ok != QMessageBox::question(this, "删除预设", "你确定的要删除预设⌈" + preset + "⌋吗?\n按下\"OK\"后,删除将立刻生效!", QMessageBox::Ok | QMessageBox::Cancel)) {
		return;
	}

	isChanged = false;
	titleChange();

	m_configManager->removeSection(preset);
	m_configManager->saveConfig();
	m_presetToUpdate = m_configManager->m_sectionList.at(0);
	updateUi();
}
void CustomReminder::saveButtonSlot()
{
	if (ui->PresetsComboBox->isEditable()) {
		//正在进行新建预设操作
		if (ui->PresetsComboBox->currentText().isEmpty() || ui->TitleLineEdit->text().isEmpty() || ui->TextLineEdit->text().isEmpty()) {
			QMessageBox::critical(this, "错误", "填入的数据不合理");
			return;
		}
		if (ui->HourSpinBox->value() == 0 && ui->MinuteSpinBox->value() == 0 && ui->SecondSpinBox->value() == 0) {
			QMessageBox::critical(this, "错误", "时间必须大于0秒!");
			return;
		}
		if (m_configManager->m_sectionList.contains(ui->PresetsComboBox->currentText())) {
			QMessageBox::critical(this, "错误", "已存在同名预设!");
			return;
		}
	}
	else {
		//正在修改已存在的预设
		if (m_configManager->getValue(ui->PresetsComboBox->currentText(), "重复") == "始终" && ui->RepeatComboBox->currentText() == "仅一次") {
			//若“重复”由“始终”变为“仅一次”，则删除间隔时间的config内容
			m_configManager->removeKey(ui->PresetsComboBox->currentText(), "间隔时间");
			m_configManager->saveConfig();
		}
	}

	isChanged = false;
	titleChange();

	ui->HourSpinBox->blockSignals(true);
	ui->MinuteSpinBox->blockSignals(true);
	ui->SecondSpinBox->blockSignals(true);

	m_presetToUpdate = ui->PresetsComboBox->currentText();
	m_configManager->setValue(m_presetToUpdate, "标题", ui->TitleLineEdit->text());
	m_configManager->setValue(m_presetToUpdate, "文本", ui->TextLineEdit->text());
	m_configManager->setValue(m_presetToUpdate, "时", ui->HourSpinBox->text());
	m_configManager->setValue(m_presetToUpdate, "分", ui->MinuteSpinBox->text());
	m_configManager->setValue(m_presetToUpdate, "秒", ui->SecondSpinBox->text());
	m_configManager->setValue(m_presetToUpdate, "声音", ui->SoundComboBox->currentText());
	m_configManager->setValue(m_presetToUpdate, "播放模式", ui->PlayModeComboBox->currentText());
	m_configManager->setValue(m_presetToUpdate, "重复", ui->RepeatComboBox->currentText());
	if (ui->RepeatComboBox->currentText() == "始终") {
		m_configManager->setValue(m_presetToUpdate, "间隔时间", ui->MiddleTimeComboBox->currentText());
	}
	m_configManager->saveConfig();

	ui->PresetsComboBox->setEditable(false);
	ui->NewButton->setEnabled(true);
	ui->DeleteButton->setEnabled(true);
	ui->TestButton->setEnabled(true);
	ui->HourSpinBox->blockSignals(false);
	ui->MinuteSpinBox->blockSignals(false);
	ui->SecondSpinBox->blockSignals(false);

	updateUi();
}
void CustomReminder::testButtonSlot()
{
	showNotification(ui->TitleLineEdit->text(), ui->TextLineEdit->text());
}

void CustomReminder::continueButtonSlot()
{
	qDebug() << "倒计时继续";
	ui->ContinueButton->setEnabled(false);
	ui->StopButton->setEnabled(true);
	int ms;
	QTime nextTime;
	if (timerMiddleRun) {
		if (m_timeGo.addSecs(1).toString("hh:mm:ss") != m_timeMiddle.toString("hh:mm:ss")) {
			m_timeGo = m_timeGo.addSecs(1);
			ms = m_timeGo.msecsTo(m_timeMiddle);
			nextTime = nextTime.fromMSecsSinceStartOfDay(ms);
			ui->TimerLabel->setText(nextTime.toString("hh:mm:ss"));
		}
		m_timerMiddle->start(1000);
	}
	else {
		if (m_timeGo.addSecs(1).toString("hh:mm:ss") != m_time.toString("hh:mm:ss")) {
			m_timeGo = m_timeGo.addSecs(1);
			ms = m_timeGo.msecsTo(m_time);
			nextTime = nextTime.fromMSecsSinceStartOfDay(ms);
			ui->TimerLabel->setText(nextTime.toString("hh:mm:ss"));
		}
		m_timer->start(1000);
	}
}
void CustomReminder::stopButtonSlot()
{
	qDebug() << "倒计时暂停";
	ui->StopButton->setEnabled(false);
	ui->ContinueButton->setEnabled(true);
	m_timerMiddle->stop();
	m_timer->stop();
}
void CustomReminder::endButtonSlot()
{
	m_timer->stop();
	m_timerMiddle->stop();
	qDebug() << "倒计时结束";
	ui->TimerLabel->setText("归零状态");
	ui->stackedWidget->setCurrentIndex(0);
	ui->mainToolBar->show();
}

void CustomReminder::startActivatedSlot(const QString& section)
{
	ui->PresetsComboBox->setCurrentText(section);
	selectPreset(section);
}
void CustomReminder::presetsActivatedSlot(const QString& section)
{
	ui->StartComboBox->setCurrentText(section);
	selectPreset(section);
}
void CustomReminder::repeatActivatedSlot(const QString& repeat)
{
	if (repeat == "始终") {
		ui->MiddleTimeLabel->show();
		ui->MiddleTimeComboBox->show();
		ui->MiddleMinuteLabel->show();

		if (m_configManager->getValue(ui->PresetsComboBox->currentText(), "重复") == "仅一次") {
			ui->MiddleTimeComboBox->setCurrentText("5");
		}
	}
	else {
		ui->MiddleTimeLabel->hide();
		ui->MiddleTimeComboBox->hide();
		ui->MiddleMinuteLabel->hide();
	}
}

void CustomReminder::changeSlot(const QString& change)
{
	//正在修改预设的信息
	isChanged = true;
	titleChange();

	qDebug() << "新值: " << change;

}

void CustomReminder::timerOutSlot()
{
	qDebug() << "m_time: " << m_time.toString("hh:mm:ss");
	qDebug() << "m_timeGo: " << m_timeGo.toString("hh:mm:ss");

	m_timeGo = m_timeGo.addSecs(1);
	qDebug() << "m_timeGo+1: " << m_timeGo.toString("hh:mm:ss");
	int msec = m_timeGo.msecsTo(m_time);
	qDebug() << "倒计时剩余秒数: sec=" << msec / 1000;

	if (msec > 0) {
		QTime t = t.fromMSecsSinceStartOfDay(msec);

		ui->TimerLabel->setText(t.toString("hh:mm:ss"));
		qDebug() << "倒计时显示" << t.toString("hh:mm:ss") << "\n";
	}
	else {
		qDebug() << "时间到了!";
		if (ui->RepeatComboBox->currentText() == "仅一次") {
			//仅一次
			showNotification(ui->TitleLineEdit->text(), ui->TextLineEdit->text());
			endButtonSlot();
		}
		else {
			//重复
			m_timeGo.setHMS(0, 0, 0);
			showNotification(ui->TitleLineEdit->text(), ui->TextLineEdit->text());

			if (ui->MiddleTimeComboBox->currentText().toInt() == 0) {
				ui->TimerLabel->setText(m_time.toString("hh:mm:ss"));
			}
			else {
				m_timer->stop();
				//开始间隔倒计时
				ui->TimerLabel->setText(m_timeMiddle.toString("hh:mm:ss"));
				qDebug() << "\n\n开始间隔倒计时: " << m_timeMiddle.toString("hh:mm:ss");
				m_timerMiddle->start(1000);
				timerMiddleRun = true;
			}
		}
	}
}
void CustomReminder::timerMiddleOutSlot()
{
	m_timeGo = m_timeGo.addSecs(1);
	qDebug() << "m_timeGo+1: " << m_timeGo.toString("hh:mm:ss");

	int msec = m_timeGo.msecsTo(m_timeMiddle);
	qDebug() << "倒计时剩余秒数: sec=" << msec / 1000;

	if (msec > 0) {
		QTime t = t.fromMSecsSinceStartOfDay(msec);

		ui->TimerLabel->setText(t.toString("hh:mm:ss"));
		qDebug() << "间隔倒计时显示" << t.toString("hh:mm:ss") << "\n";
	}
	else {
		qDebug() << "间隔倒计时到了,已停止。开始启动倒计时";
		showNotification("时间到了", ui->MiddleTimeComboBox->currentText() + "分钟过去了~");

		m_timerMiddle->stop();

		ui->TimerLabel->setText(m_time.toString("hh:mm:ss"));
		m_timeGo.setHMS(0, 0, 0);
		m_timer->start(1000);
		timerMiddleRun = false;
	}
}

void CustomReminder::trayClickedSlot(QSystemTrayIcon::ActivationReason reason)
{
	if (reason == QSystemTrayIcon::DoubleClick) {
		this->showNormal();
	}
}
void CustomReminder::closeEvent(QCloseEvent* e)
{
	if (ui->MinCheckBox->checkState() == Qt::Checked) {
		this->hide();
		m_trayIcon->showMessage("提示", "已将窗口最小化至托盘", QSystemTrayIcon::MessageIcon::Information);
		e->ignore();
	}
	else {
		exit(0);
	}
}
void CustomReminder::topCheckSlot(int state)
{
	if (state == 2) {
		this->windowHandle()->setFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);
	}
	else {
		this->windowHandle()->setFlags(this->windowFlags());
	}
}