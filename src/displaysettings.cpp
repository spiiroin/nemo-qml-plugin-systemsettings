/*
 * Copyright (C) 2013 Jolla Ltd. <pekka.vuorela@jollamobile.com>
 *
 * You may use this file under the terms of the BSD license as follows:
 *
 * "Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Nemo Mobile nor the names of its contributors
 *     may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 */

#include <mce/dbus-names.h>
#include <mce/mode-names.h>
#include "mceiface.h"
#include "displaysettings.h"
#include <MGConfItem>
#include <QDebug>

static const char *MceDisplayBrightness = "/system/osso/dsm/display/display_brightness";
static const char *MceDisplayDimTimeout = "/system/osso/dsm/display/display_dim_timeout";
static const char *MceDisplayBlankTimeout = "/system/osso/dsm/display/display_blank_timeout";
static const char *MceDisplayInhibitMode = "/system/osso/dsm/display/inhibit_blank_mode";
static const char *MceDisplayUseAdaptiveDimming = "/system/osso/dsm/display/use_adaptive_display_dimming";
static const char *MceDisplayUseLowPowerMode = "/system/osso/dsm/display/use_low_power_mode";
static const char *MceDisplayUseAmbientLightSensor = "/system/osso/dsm/display/als_enabled";
static const char *MceDisplayAutoBrightnessEnabled = "/system/osso/dsm/display/als_autobrightness";
static const char *MceDoubleTapMode = "/system/osso/dsm/doubletap/mode";
static const char *MceLidSensorEnabled = "/system/osso/dsm/locks/lid_sensor_enabled";
static const char *MceLidSensorFilteringEnabled = "/system/osso/dsm/locks/filter_lid_with_als";

DisplaySettings::DisplaySettings(QObject *parent)
    : QObject(parent)
{
    m_orientationLock = new MGConfItem("/lipstick/orientationLock", this);
    connect(m_orientationLock, SIGNAL(valueChanged()), SIGNAL(orientationLockChanged()));

    m_mceIface = new ComNokiaMceRequestInterface(MCE_SERVICE, MCE_REQUEST_PATH, QDBusConnection::systemBus(), this);
    QDBusPendingReply<QDBusVariant> result = m_mceIface->get_config(QDBusObjectPath(MceDisplayBrightness));
    result.waitForFinished();
    m_brightness = result.value().variant().toInt();

    result = m_mceIface->get_config(QDBusObjectPath(MceDisplayDimTimeout));
    result.waitForFinished();
    m_dimTimeout = result.value().variant().toInt();

    result = m_mceIface->get_config(QDBusObjectPath(MceDisplayBlankTimeout));
    result.waitForFinished();
    m_blankTimeout = result.value().variant().toInt();

    result = m_mceIface->get_config(QDBusObjectPath(MceDisplayInhibitMode));
    result.waitForFinished();
    m_inhibitMode = static_cast<InhibitMode>(result.value().variant().toInt());

    result = m_mceIface->get_config(QDBusObjectPath(MceDisplayUseAdaptiveDimming));
    result.waitForFinished();
    m_adaptiveDimmingEnabled = result.value().variant().toBool();

    result = m_mceIface->get_config(QDBusObjectPath(MceDisplayUseLowPowerMode));
    result.waitForFinished();
    m_lowPowerModeEnabled = result.value().variant().toBool();

    result = m_mceIface->get_config(QDBusObjectPath(MceDisplayUseAmbientLightSensor));
    result.waitForFinished();
    m_ambientLightSensorEnabled = result.value().variant().toBool();

    result = m_mceIface->get_config(QDBusObjectPath(MceDisplayAutoBrightnessEnabled));
    result.waitForFinished();
    m_autoBrightnessEnabled = result.value().variant().toBool();

    result = m_mceIface->get_config(QDBusObjectPath(MceDoubleTapMode));
    result.waitForFinished();
    m_doubleTapMode = result.value().variant().toInt();

    result = m_mceIface->get_config(QDBusObjectPath(MceLidSensorEnabled));
    result.waitForFinished();
    m_lidSensorEnabled = result.value().variant().toBool();

    result = m_mceIface->get_config(QDBusObjectPath(MceLidSensorFilteringEnabled));
    result.waitForFinished();
    m_lidSensorFilteringEnabled = result.value().variant().toBool();

    m_mceSignalIface = new ComNokiaMceSignalInterface(MCE_SERVICE, MCE_SIGNAL_PATH, QDBusConnection::systemBus(), this);
    connect(m_mceSignalIface, SIGNAL(config_change_ind(QString,QDBusVariant)), this, SLOT(configChange(QString,QDBusVariant)));
}

int DisplaySettings::brightness() const
{
    return m_brightness;
}

void DisplaySettings::setBrightness(int value)
{
    if (m_brightness != value) {
        m_brightness = value;
        m_mceIface->set_config(QDBusObjectPath(MceDisplayBrightness), QDBusVariant(value));
        emit brightnessChanged();
    }
}

int DisplaySettings::maximumBrightness()
{
    QDBusPendingReply<QDBusVariant> result = m_mceIface->get_config(QDBusObjectPath("/system/osso/dsm/display/max_display_brightness_levels"));
    result.waitForFinished();

    return result.value().variant().toInt();
}

int DisplaySettings::dimTimeout() const
{
    return m_dimTimeout;
}

void DisplaySettings::setDimTimeout(int value)
{
    if (m_dimTimeout != value) {
        m_dimTimeout = value;
        m_mceIface->set_config(QDBusObjectPath(MceDisplayDimTimeout), QDBusVariant(value));
        emit dimTimeoutChanged();
    }
}

int DisplaySettings::blankTimeout() const
{
    return m_blankTimeout;
}

void DisplaySettings::setBlankTimeout(int value)
{
    if (m_blankTimeout != value) {
        m_blankTimeout = value;
        m_mceIface->set_config(QDBusObjectPath(MceDisplayBlankTimeout), QDBusVariant(value));
        emit blankTimeoutChanged();
    }
}

DisplaySettings::InhibitMode DisplaySettings::inhibitMode() const
{
    return m_inhibitMode;
}

void DisplaySettings::setInhibitMode(InhibitMode mode)
{
    if (m_inhibitMode != mode) {
        m_inhibitMode = mode;
        m_mceIface->set_config(QDBusObjectPath(MceDisplayInhibitMode), QDBusVariant(static_cast<int>(mode)));
        emit inhibitModeChanged();
    }
}

bool DisplaySettings::adaptiveDimmingEnabled() const
{
    return m_adaptiveDimmingEnabled;
}

void DisplaySettings::setAdaptiveDimmingEnabled(bool enabled)
{
    if (m_adaptiveDimmingEnabled != enabled) {
        m_adaptiveDimmingEnabled = enabled;
        m_mceIface->set_config(QDBusObjectPath(MceDisplayUseAdaptiveDimming), QDBusVariant(enabled));
        emit adaptiveDimmingEnabledChanged();
    }
}

bool DisplaySettings::lowPowerModeEnabled() const
{
    return m_lowPowerModeEnabled;
}

void DisplaySettings::setLowPowerModeEnabled(bool enabled)
{
    if (m_lowPowerModeEnabled != enabled) {
        m_lowPowerModeEnabled = enabled;
        m_mceIface->set_config(QDBusObjectPath(MceDisplayUseLowPowerMode), QDBusVariant(enabled));
        emit lowPowerModeEnabledChanged();
    }
}

bool DisplaySettings::ambientLightSensorEnabled() const
{
    return m_ambientLightSensorEnabled;
}

void DisplaySettings::setAmbientLightSensorEnabled(bool enabled)
{
    if (m_ambientLightSensorEnabled != enabled) {
        m_ambientLightSensorEnabled = enabled;
        m_mceIface->set_config(QDBusObjectPath(MceDisplayUseAmbientLightSensor), QDBusVariant(enabled));
        emit ambientLightSensorEnabledChanged();
    }
}

bool DisplaySettings::autoBrightnessEnabled() const
{
    return m_autoBrightnessEnabled;
}

void DisplaySettings::setAutoBrightnessEnabled(bool enabled)
{
    if (m_autoBrightnessEnabled != enabled) {
        m_autoBrightnessEnabled = enabled;
        m_mceIface->set_config(QDBusObjectPath(MceDisplayAutoBrightnessEnabled), QDBusVariant(enabled));
        emit autoBrightnessEnabledChanged();
    }
}

int DisplaySettings::doubleTapMode() const
{
    return m_doubleTapMode;
}

void DisplaySettings::setDoubleTapMode(int mode)
{
    if (m_doubleTapMode != mode) {
        m_doubleTapMode = mode;
        m_mceIface->set_config(QDBusObjectPath(MceDoubleTapMode), QDBusVariant(mode));
        emit doubleTapModeChanged();
    }
}

QVariant DisplaySettings::orientationLock() const
{
    return m_orientationLock->value("dynamic");
}

void DisplaySettings::setOrientationLock(const QVariant &orientationLock)
{
    m_orientationLock->set(orientationLock);
}

bool DisplaySettings::lidSensorEnabled() const
{
    return m_lidSensorEnabled;
}

void DisplaySettings::setLidSensorEnabled(bool enabled)
{
    if (m_lidSensorEnabled != enabled) {
        m_lidSensorEnabled = enabled;
        m_mceIface->set_config(QDBusObjectPath(MceLidSensorEnabled), QDBusVariant(enabled));
        emit lidSensorEnabledChanged();
    }
}

bool DisplaySettings::lidSensorFilteringEnabled() const
{
    return m_lidSensorFilteringEnabled;
}

void DisplaySettings::setLidSensorFilteringEnabled(bool enabled)
{
    if (m_lidSensorFilteringEnabled != enabled) {
        m_lidSensorFilteringEnabled = enabled;
        m_mceIface->set_config(QDBusObjectPath(MceLidSensorFilteringEnabled), QDBusVariant(enabled));
        emit lidSensorFilteringEnabledChanged();
    }
}

void DisplaySettings::configChange(const QString &key, const QDBusVariant &value)
{
    if (key == MceDisplayBrightness) {
        int val = value.variant().toInt();
        if (val != m_brightness) {
            m_brightness = val;
            emit brightnessChanged();
        }
    } else if (key == MceDisplayDimTimeout) {
        int val = value.variant().toInt();
        if (val != m_dimTimeout) {
            m_dimTimeout = val;
            emit dimTimeoutChanged();
        }
    } else if (key == MceDisplayBlankTimeout) {
        int val = value.variant().toInt();
        if (val != m_blankTimeout) {
            m_blankTimeout = val;
            emit blankTimeoutChanged();
        }
    } else if (key == MceDisplayInhibitMode) {
        InhibitMode val = static_cast<InhibitMode>(value.variant().toInt());
        if (val != m_inhibitMode) {
            m_inhibitMode = val;
            emit inhibitModeChanged();
        }
    } else if (key == MceDisplayUseAdaptiveDimming) {
        bool val = value.variant().toBool();
        if (val != m_adaptiveDimmingEnabled) {
            m_adaptiveDimmingEnabled = val;
            emit adaptiveDimmingEnabledChanged();
        }
    } else if (key == MceDisplayUseLowPowerMode) {
        bool val = value.variant().toBool();
        if (val != m_lowPowerModeEnabled) {
            m_lowPowerModeEnabled = val;
            emit lowPowerModeEnabledChanged();
        }
    } else if (key == MceDisplayUseAmbientLightSensor) {
        bool val = value.variant().toBool();
        if (val != m_ambientLightSensorEnabled) {
            m_ambientLightSensorEnabled = val;
            emit ambientLightSensorEnabledChanged();
        }
    } else if (key == MceDisplayAutoBrightnessEnabled) {
        bool val = value.variant().toBool();
        if (val != m_autoBrightnessEnabled) {
            m_autoBrightnessEnabled = val;
            emit autoBrightnessEnabledChanged();
        }
    } else if (key == MceDoubleTapMode) {
        int val = value.variant().toInt();
        if (val != m_doubleTapMode) {
            m_doubleTapMode = val;
            emit doubleTapModeChanged();
        }
    } else if (key == MceLidSensorEnabled) {
        bool val = value.variant().toBool();
        if (val != m_lidSensorEnabled) {
            m_lidSensorEnabled = val;
            emit lidSensorEnabledChanged();
        }
    } else if (key == MceLidSensorFilteringEnabled) {
        bool val = value.variant().toBool();
        if (val != m_lidSensorFilteringEnabled) {
            m_lidSensorFilteringEnabled = val;
            emit lidSensorFilteringEnabledChanged();
        }
    }
}
