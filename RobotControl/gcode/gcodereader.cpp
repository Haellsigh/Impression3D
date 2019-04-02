#include "gcodereader.h"

#include <QFile>

#include <QDebug>

namespace gcode {

Reader::Reader()
{
}

QVector<Command> Reader::decodeFile(QString filepath)
{
    auto lines = readFile(filepath);

    QVector<Command> commands;

    int i = 0;

    for (QString line : lines) {
        // 10 lines limit for testing
        if (i > 10) {
            return commands;
        }

        // Process comments
        if (line.contains(';')) {
            // Remove the line if it begins with ;
            if (line.at(0) == ';') {
                continue;
            }
            // Remove the commented part of the line
            else {
                line.truncate(line.indexOf(';'));
            }
        }

        qDebug().noquote() << line;

        // Split at each whitespace group
        QStringList fields = line.split(QRegExp("\\s+"));

        commands.append(parseFields(fields));

        i++;
    }

    return commands;
}

QStringList Reader::readFile(QString filepath)
{
    QFile file(filepath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning().noquote() << tr("Can't open file %1").arg(filepath);
        emit error(tr("Can't open file %1").arg(filepath));
        return QStringList();
    }

    QStringList contents;

    while (!file.atEnd()) {
        contents.append(file.readLine());
    }

    return contents;
}

Command Reader::parseFields(QStringList fields)
{
    Command cmd;

    for (auto field : fields) {
        if (!field.isEmpty()) {
            char command = field.at(0).toUpper().toLatin1();
            QString data = field.mid(1);

            qDebug().noquote() << "command:" << command;
            qDebug().noquote() << "data:" << data;

            switch (field.at(0).toUpper().toLatin1()) {
            case 'G':
                break;
            case 'M':
                break;
            case 'F':
                break;
            case 'X':
                break;
            case 'Y':
                break;
            case 'Z':
                break;
            case 'E':
                break;
            }
        }
    }

    return cmd;
}

} // namespace gcode
