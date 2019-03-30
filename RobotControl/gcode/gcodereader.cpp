#include "gcodereader.h"

#include <QFile>

#include <QDebug>

namespace gcode {

Reader::Reader()
{
    decodeFile("test.gcode");
}

QVector<Command> Reader::decodeFile(QString filepath)
{
    auto lines = readFile(filepath);

    QVector<Command> commands;

    int i = 0;

    for (auto line : lines) {
        if (i > 10) {
            return commands;
        }
        // Remove comments
        if (line.contains(';')) {
            // Remove the line if it begins with ;
            if (line.front() == ';') {
                continue;
            }
            // Remove the commented part of the line
            else {
                line = line.left(line.indexOf(';') + 1);
            }
        }

        qDebug() << line;

        // Split at each whitespace group
        QStringList fields = line.split(QRegExp("\\s+"));

        // Process each field
        for (auto field : fields) {
            switch (field.front().toUpper().toLatin1()) {
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

        i++;
    }

    return commands;
}

QStringList Reader::readFile(QString filepath)
{
    QFile file(filepath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit error(tr("Can't open file %1").arg(filepath));
    }

    QStringList contents;

    while (!file.atEnd()) {
        contents.append(file.readLine());
    }

    return contents;
}

} // namespace gcode
