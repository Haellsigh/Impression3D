#include "gcodereader.h"

#include <QFile>

#include <QDebug>

namespace gcode {

Reader::Reader()
{
}

QVector<Block> Reader::decodeFile(QString filepath)
{
    auto lines = readFile(filepath);

    QVector<Block> blocks;

    int iLine = 1;
    for (QString line : lines) {
        // Process comments
        if (line.contains(';')) {
            // Remove the line if it begins with ;
            if (line.at(0) == ';') { // Go to next line
                iLine++;
                continue;
            }
            // Remove the commented part of the line
            else {
                line.truncate(line.indexOf(';'));
            }
        }

        // Split at each whitespace group
        QStringList fields = line.split(QRegExp("\\s+"));

        auto block = parseFields(fields, iLine);

        if (block.isValid)
            blocks.append(block);

        iLine++;
    }

    // Display some statistics
    qDebug().noquote() << "G-Code fields statistics:";
    auto it = m_cmdCounts.begin();
    while (it != m_cmdCounts.end()) {
        qDebug().noquote() << it.key()
                           << ":" << 100. * it.value() / m_totals << "%";
        it++;
    }

    return blocks;
}

QStringList Reader::readFile(QString filepath)
{
    QFile file(filepath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning().noquote() << tr("Can't open file %1").arg(filepath);
        return QStringList();
    }

    QStringList contents;

    while (!file.atEnd()) {
        contents.append(file.readLine().trimmed());
    }

    return contents;
}

Block Reader::parseFields(QStringList fields, int iLine)
{
    Block block;

    int iField = 0;
    for (auto field : fields) {
        if (!field.isEmpty()) {
            char command = field.at(0).toUpper().toLatin1();
            double data  = field.mid(1).toDouble();

            m_cmdCounts[command] += 1;

            /**
             * @todo Stop assuming that every line is correct
             * Check for example if we get a G0 or G1, we need to have at least
             * one of those: X, Y, Z, E, F
             */
            switch (command) {
            // Commands
            case 'G': { // Standard command
                block.setField(Standard, data);
            } break;
            case 'M': { // RepRap command
                block.setField(RepRap, data);
            } break;

            // Parameters
            case 'E': { // Extrude length
                block.setField(ExtrudeLength, data);
            } break;
            case 'F': { // Feedrate in mm/min
                block.setField(Feedrate, data);
            } break;
            case 'S': { // Command parameter
                block.setField(Parameter, data);
            } break;
            case 'X': { // X coordinate
                block.setField(X, data);
            } break;
            case 'Y': { // Y coordinate
                block.setField(Y, data);
            } break;
            case 'Z': { // Z coordinate
                block.setField(Z, data);
            } break;
            }

            // If the first field in the line isn't a command
            if (iField == 0) {
                if (!block.isValid) {
                    qWarning().noquote()
                        << "GCode error at line"
                        << iLine
                        << ": block not starting with a command (G or M)";
                    return Block();
                }
            }

            // Increment field number
            iField++;
        }
    }

    if (block.isValid)
        m_totals++;

    return block;
}

} // namespace gcode
