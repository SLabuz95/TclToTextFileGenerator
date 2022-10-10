#include"attributesController.hpp"

using Attribute = AttributesController::Attributes::mapped_type;

const AttributesController::Attributes AttributesController::PredefinedAttributes{
    {"PROCESS_MODEL_STATUS", Attribute()},
    {"FILE_PREFIX", Attribute()},
    {"FILE_POSTFIX", Attribute()},
    {"FILE_NAME", Attribute()},
    {"DIR_NAME", Attribute()},
};

AttributesController::AttributesController(Attributes& attributes)
    : attributes_(PredefinedAttributes)
{
    attributes_.insert(attributes);
}

