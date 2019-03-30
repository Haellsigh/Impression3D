#ifndef GCODEREADER_H
#define GCODEREADER_H

#include <QObject>
#include <QVector>

#include "gcodecommands.h"

namespace gcode {

/**
 * @brief The GCodeReader class opens .gcode files and decodes the commands
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
};

} // namespace gcode

#endif // GCODEREADER_H
