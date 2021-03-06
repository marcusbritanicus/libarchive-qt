/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <NBStandardPaths.hpp>
#include <QProcess>
#include <QUrl>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>
#if QT_VERSION <= QT_VERSION_CHECK(4, 7, 0)  // i do not sure but for qt 4.7 and 4.6 we need this include
#include <QHash>
#endif
// LOCAL HACK #include <private/qfilesystemengine_p.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef QT_NO_STANDARDPATHS

QString QStandardPaths::writableLocation(StandardLocation type)
{
    switch (type) {
    case HomeLocation:
        return QDir::homePath();
    case TempLocation:
        return QDir::tempPath();
    case CacheLocation:
    {
        // http://standards.freedesktop.org/basedir-spec/basedir-spec-0.6.html
        QString xdgCacheHome = QLatin1String(qgetenv("XDG_CACHE_HOME"));
        if (xdgCacheHome.isEmpty())
            xdgCacheHome = QDir::homePath() + QLatin1String("/.cache");
        if (!QCoreApplication::organizationName().isEmpty())
            xdgCacheHome += QLatin1Char('/') + QCoreApplication::organizationName();
        if (!QCoreApplication::applicationName().isEmpty())
            xdgCacheHome += QLatin1Char('/') + QCoreApplication::applicationName();
        return xdgCacheHome;
    }
    case DataLocation:
    case GenericDataLocation:
    {
        QString xdgDataHome = QLatin1String(qgetenv("XDG_DATA_HOME"));
        if (xdgDataHome.isEmpty())
            xdgDataHome = QDir::homePath() + QLatin1String("/.local/share");
        if (type == QStandardPaths::DataLocation) {
            if (!QCoreApplication::organizationName().isEmpty())
                xdgDataHome += QLatin1Char('/') + QCoreApplication::organizationName();
            if (!QCoreApplication::applicationName().isEmpty())
                xdgDataHome += QLatin1Char('/') + QCoreApplication::applicationName();
        }
        return xdgDataHome;
    }
    case ConfigLocation:
    {
        // http://standards.freedesktop.org/basedir-spec/latest/
        QString xdgConfigHome = QFile::decodeName(qgetenv("XDG_CONFIG_HOME"));
        if (xdgConfigHome.isEmpty())
            xdgConfigHome = QDir::homePath() + QLatin1String("/.config");
        return xdgConfigHome;
    }
    case RuntimeLocation:
    {
        const uid_t myUid = geteuid();
        // http://standards.freedesktop.org/basedir-spec/latest/
        QString xdgRuntimeDir = QFile::decodeName(qgetenv("XDG_RUNTIME_DIR"));
        if (xdgRuntimeDir.isEmpty()) {
            const QString userName = QString(); // LOCAL HACK QFileSystemEngine::resolveUserName(myUid);
            xdgRuntimeDir = QDir::tempPath() + QLatin1String("/runtime-") + userName;
            QDir dir(xdgRuntimeDir);
            if (!dir.exists()) {
                if (!QDir().mkdir(xdgRuntimeDir)) {
                    qWarning("QStandardPaths: error creating runtime directory %s: %s", qPrintable(xdgRuntimeDir), qPrintable(qt_error_string(errno)));
                    return QString();
                }
            }
        }
        // "The directory MUST be owned by the user"
        QFileInfo fileInfo(xdgRuntimeDir);
        if (fileInfo.ownerId() != myUid) {
            qWarning("QStandardPaths: wrong ownership on runtime directory %s, %d instead of %d", qPrintable(xdgRuntimeDir),
                     fileInfo.ownerId(), myUid);
            return QString();
        }
        // "and he MUST be the only one having read and write access to it. Its Unix access mode MUST be 0700."
        QFile file(xdgRuntimeDir);
        const QFile::Permissions wantedPerms = QFile::ReadUser | QFile::WriteUser | QFile::ExeUser;
        if (file.permissions() != wantedPerms && !file.setPermissions(wantedPerms)) {
            qWarning("QStandardPaths: wrong permissions on runtime directory %s", qPrintable(xdgRuntimeDir));
            return QString();
        }
        return xdgRuntimeDir;
    }
    default:
        break;
    }

    // http://www.freedesktop.org/wiki/Software/xdg-user-dirs
    QString xdgConfigHome = QLatin1String(qgetenv("XDG_CONFIG_HOME"));
    if (xdgConfigHome.isEmpty())
        xdgConfigHome = QDir::homePath() + QLatin1String("/.config");
    QFile file(xdgConfigHome + QLatin1String("/user-dirs.dirs"));
    if (file.exists() && file.open(QIODevice::ReadOnly)) {
        QHash<QString, QString> lines;
        QTextStream stream(&file);
        // Only look for lines like: XDG_DESKTOP_DIR="$HOME/Desktop"
        QRegExp exp(QLatin1String("^XDG_(.*)_DIR=(.*)$"));
        while (!stream.atEnd()) {
            const QString line = stream.readLine();
            if (exp.indexIn(line) != -1) {
                QStringList lst = exp.capturedTexts();
                QString key = lst.at(1);
                QString value = lst.at(2);
                if (value.length() > 2
                    && value.startsWith(QLatin1Char('\"'))
                    && value.endsWith(QLatin1Char('\"')))
                    value = value.mid(1, value.length() - 2);
                // Store the key and value: "DESKTOP", "$HOME/Desktop"
                lines[key] = value;
            }
        }

        QString key;
        switch (type) {
        case DesktopLocation:
            key = QLatin1String("DESKTOP");
            break;
        case DocumentsLocation:
            key = QLatin1String("DOCUMENTS");
            break;
        case PicturesLocation:
            key = QLatin1String("PICTURES");
            break;
        case MusicLocation:
            key = QLatin1String("MUSIC");
            break;
        case MoviesLocation:
            key = QLatin1String("VIDEOS");
            break;
        default:
            break;
        }
        if (!key.isEmpty() && lines.contains(key)) {
            QString value = lines[key];
            // value can start with $HOME
            if (value.startsWith(QLatin1String("$HOME")))
                value = QDir::homePath() + value.mid(5);
            return value;
        }
    }

    QString path;
    switch (type) {
    case DesktopLocation:
        path = QDir::homePath() + QLatin1String("/Desktop");
        break;
    case DocumentsLocation:
        path = QDir::homePath() + QLatin1String("/Documents");
       break;
    case PicturesLocation:
        path = QDir::homePath() + QLatin1String("/Pictures");
        break;

    case FontsLocation:
        path = QDir::homePath() + QLatin1String("/.fonts");
        break;

    case MusicLocation:
        path = QDir::homePath() + QLatin1String("/Music");
        break;

    case MoviesLocation:
        path = QDir::homePath() + QLatin1String("/Videos");
        break;

    case ApplicationsLocation:
        path = writableLocation(GenericDataLocation) + QLatin1String("/applications");
        break;

    default:
        break;
    }

    return path;
}

QStringList QStandardPaths::standardLocations(StandardLocation type)
{
    QStringList dirs;
    if (type == ConfigLocation) {
        // http://standards.freedesktop.org/basedir-spec/latest/
        QString xdgConfigDirs = QFile::decodeName(qgetenv("XDG_CONFIG_DIRS"));
        if (xdgConfigDirs.isEmpty())
            dirs.append(QString::fromLatin1("/etc/xdg"));
        else
            dirs = xdgConfigDirs.split(QLatin1Char(':'));
    } else if (type == GenericDataLocation) {
        // http://standards.freedesktop.org/basedir-spec/latest/
        QString xdgConfigDirs = QFile::decodeName(qgetenv("XDG_DATA_DIRS"));
        if (xdgConfigDirs.isEmpty()) {
            dirs.append(QString::fromLatin1("/usr/local/share"));
            dirs.append(QString::fromLatin1("/usr/share"));
        } else
            dirs = xdgConfigDirs.split(QLatin1Char(':'));
    }
    const QString localDir = writableLocation(type);
    dirs.prepend(localDir);
    return dirs;
}

QString QStandardPaths::displayName(StandardLocation type)
{
    Q_UNUSED(type);
    return QString();
}

QT_END_NAMESPACE

#endif // QT_NO_STANDARDPATHS
