#ifndef GCODESTRUCTURES_H
#define GCODESTRUCTURES_H

#include <QHash>

namespace gcode {

enum CommandType {
    Standard,
    RepRap
};

enum ParameterType {
    ExtrudeLength,
    Feedrate,
    Parameter,
    X,
    Y,
    Z
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
    QPair<CommandType, double> command;
    QHash<ParameterType, double> parameters;

public:
    bool isValid = false;

    void setField(const CommandType type, const double data)
    {
        isValid        = true;
        command.first  = type;
        command.second = data;
    }
    void setField(const ParameterType type, const double data)
    {
        parameters.insert(type, data);
    }

    CommandType getCommandType() const { return command.first; }
    double getCommandData() const { return command.second; }

    bool hasParameter(const ParameterType type) const
    {
        return parameters.contains(type);
    }

    double getParameter(const ParameterType type) const
    {
        return parameters.value(type, 0);
    }
};

}

#endif // GCODESTRUCTURES_H
