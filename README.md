# 自定义提醒 CustomReminder
- qt version: 6.5.2 MSVC-2019-64bit
- 使用到的库: [WinToast](https://github.com/mohabouje/WinToast)
- 参考: [JsonConfigManager](https://github.com/c-can-do-it/jsonconfigmanager)
---

## 功能如下:
- 根据预设的内容进行倒计时,倒计时结束后发出Toast通知
- 预设中可自定义的内容如下:
  - 提醒标题
  - 提醒内容
  - 距离发出提醒的时间
  - 声音 (默认 短信 邮件 闹钟)
  - 播放方式 (默认 循环 静音)
  - 重复 (仅一次 始终)
    - 间隔时间 (0~60分钟)(若选择始终重复)

 ## 写在最后
- 项目初期就遇到各种报错，再加上没有使用Qt Creator而是使用了VS+Qt，BUFF叠满了，出现的问题一个接着一个
- 这是我作为qt初学者的第一个的项目，总历时3周，如有改进之处还请指出XD
