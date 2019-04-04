#ifndef GCODEREADER_H
#define GCODEREADER_H

#include <QMap>
#include <QObject>
#include <QVector>

#include "gcodestructures.h"

namespace gcode {

/**
 * @brief The GCodeReader class opens .gcode files and decodes the commands
 *
 * @note Reference: https://reprap.org/wiki/G-code
 */
class Reader : public QObject {
    Q_OBJECT

public:
    Reader();

    QVector<Block> decodeFile(QString filepath);

private:
    QStringList readFile(QString filepath);

    // Parser
private:
    Block parseFields(QStringList fields, int iLine);

private:
    QMap<char, int> m_cmdCounts;
    int m_totals = 0;
};

} // namespace gcode

#endif // GCODEREADER_H
