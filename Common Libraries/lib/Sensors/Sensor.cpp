#include "Sensor.h"

namespace sensors {

Sensor::Sensor(SensorType type, SensorMetric metric, SensorDataType dataType, String unit) {
  this->type = type;
  this->metric = metric;
  this->dataType = dataType;
  this->unit = unit;
}

SensorType Sensor::getSensorType() { return type; }

SensorMetric Sensor::getSensorMetric() { return metric; }

SensorDataType Sensor::getSensorDataType() { return dataType; }

void Sensor::setActive(boolean active) { this->active = active; }

boolean Sensor::isActive() { return active; }

String Sensor::toString() { return toString(""); }

String Sensor::toString(String prefix) {
  String type;
  String metric;
  String dataType;

  switch (this->type) {
    case ACTIVE:
      type = "ACTIVE";
      break;
    case PASSIVE:
      type = "PASSIVE";
      break;
  }

  switch (this->metric) {
    case TEMPERATURE:
      metric = "TEMPERATURE";
      break;
    case HUMIDITY:
      metric = "HUMIDITY";
      break;
    case HEAT_INDEX:
      metric = "HEAT_INDEX";
      break;
    case MOVEMENT:
      metric = "MOVEMENT";
      break;
    case TIME:
      metric = "TIME";
      break;
    case LIGHT:
      metric = "LIGHT";
      break;
    case BUTTON:
      metric = "BUTTON";
      break;
  }

  String s = prefix;

  if (prefix != "") {
    s += " ";
  }

  s += metric + " (" + type + ")";

  while (s.length() < 35) {
    s += " ";
  }

  s += ": ";

  if (active && unit == "") {
    s += getValue();
  } else if (active && unit != "") {
    s += getValue() + " " + unit;
  } else {
    s += " -- DISABLED --";
  }

  return s;
}

}  // namespace sensors