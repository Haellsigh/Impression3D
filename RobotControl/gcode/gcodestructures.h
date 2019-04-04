#ifndef GCODESTRUCTURES_H
#define GCODESTRUCTURES_H

#include <QHash>

namespace gcode {

enum FieldType {
    ExtrudeLength,
    Feedrate,
    Standard,
    RepRap,
    Parameter,
    X,
    Y,
    Z
};

/**
 * @brief Represents a gcode field
 *
 * A field is either:
 *  - a command and the data
 *  - a parameter and the data
 */
struct Field {
    FieldType field;
    double data;
};

/**
 * @brief Represents a gcode block (line)
 *
 * A block is a list of fields.
 * It must have a command in order to be valid (Either Standard or RepRap).
 *
 * The member variable isValid is set to true when there is at least
 * one command.
 */
struct Block {
private:
    QPair<FieldType, double> command;
    QHash<FieldType, double> parameters;

    bool typeIsCommand(FieldType type)
    {
        return type == Standard || type == RepRap;
    }

public:
    bool isValid = false;

    void setField(const FieldType type, const double data)
    {
        if (typeIsCommand(type)) {
            isValid        = true;
            command.first  = type;
            command.second = data;
        } else {
            parameters.insert(type, data);
        }
    }

    bool hasParameter(const FieldType type) const
    {
        return parameters.contains(type);
    }

    double getParameter(const FieldType type) const
    {
        return parameters.value(type, 0);
    }
};

}

#endif // GCODESTRUCTURES_H
