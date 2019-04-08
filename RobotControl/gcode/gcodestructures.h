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
 * A block is one command and zero or more parameters.
 * A block is valid if there is at least one command.
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
    bool hasParameters() const
    {
        return !parameters.isEmpty();
    }

    double getParameter(const ParameterType type) const
    {
        return parameters.value(type, 0);
    }
};

}

#endif // GCODESTRUCTURES_H
