#ifndef GCODEREADER_H
#define GCODEREADER_H

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

    QVector<Command> decodeFile(QString filepath);

signals:
    void error(QString errorMessage);

private:
    QStringList readFile(QString filepath);

    // Parse methods
private:
    Command parseFields(QStringList fields);
};

} // namespace gcode

#endif // GCODEREADER_H
