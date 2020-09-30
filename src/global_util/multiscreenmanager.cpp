#include "multiscreenmanager.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>
#include <QDesktopWidget>

/** zzz
 * @brief Construct a new Multi Screen Manager:: Multi Screen Manager object
 * 管理多屏信息（增减屏幕时，需要调用这个类中的方法）
 * @param parent
 */
MultiScreenManager::MultiScreenManager(QObject *parent)
    : QObject(parent)
    , m_registerFunction(nullptr)
    , m_raiseContentFrameTimer(new QTimer(this))
{
    connect(qApp, &QGuiApplication::screenAdded, this, &MultiScreenManager::onScreenAdded, Qt::QueuedConnection);
    connect(qApp, &QGuiApplication::screenRemoved, this, &MultiScreenManager::onScreenRemoved, Qt::QueuedConnection);

    // 在sw平台存在复制模式显示问题，使用延迟来置顶一个Frame
    m_raiseContentFrameTimer->setInterval(50);
    m_raiseContentFrameTimer->setSingleShot(true);

    connect(m_raiseContentFrameTimer, &QTimer::timeout, this, &MultiScreenManager::raiseContentFrame);
}

/** zzz
 * @brief 注册多屏入口函数
 *
 * @param function 匿名函数的函数指针
 */
void MultiScreenManager::register_for_mutil_screen(std::function<QWidget *(QScreen *)> function)
{
    m_registerFunction = function;

    // update all screen
    for (QScreen *screen : qApp->screens()) {
        onScreenAdded(screen);
    }
}

/** zzz
 * @brief 启动定时器，超时后调用显示锁屏界面的内容
 *
 */
void MultiScreenManager::startRaiseContentFrame()
{
    m_raiseContentFrameTimer->start();
}

/** zzz
 * @brief 在多屏情况下，屏幕个数增加时需要调用这个函数，将对应的屏幕信息添加进QMap中
 *
 * @param screen
 */
void MultiScreenManager::onScreenAdded(QScreen *screen)
{
    if (!m_registerFunction) {
        return;
    }

    m_frames[screen] = m_registerFunction(screen);

    startRaiseContentFrame();
}

/** zzz
 * @brief 在多屏情况下，屏幕个数减少时需要调用这个函数，将对应的屏幕信息从QMap中删除
 *
 * @param screen
 */
void MultiScreenManager::onScreenRemoved(QScreen *screen)
{
    if (!m_registerFunction) {
        return;
    }

    m_frames[screen]->deleteLater();
    m_frames.remove(screen);

    startRaiseContentFrame();
}

/** zzz
 * @brief 遍历存储屏幕信息的QMap，找到当前状态为可显示的屏幕，并在对应屏幕上显示界面中的内容（锁屏和登录界面的输入密码框、关机界面的一排按钮）
 *
 */
void MultiScreenManager::raiseContentFrame()
{
    for (auto it = m_frames.constBegin(); it != m_frames.constEnd(); ++it) {
        if (it.value()->property("contentVisible").toBool()) {
            it.value()->raise();
            return;
        }
    }
}
