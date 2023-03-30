#include "utils.h"

cparser::Section::Section(const std::string& name, const cparser::Element& elem) : cparser::Element(name, cparser::elementType::SECTION) {}

cparser::Section::Section(const cparser::Element& elem) : cparser::Element("GLOBAL-SECTION", cparser::elementType::SECTION) {}

std::pair<std::string, std::string> cparser::Section::NameSeparation(const std::string& str) {
    std::stringstream ss(str);

    std::string sect;
    getline(ss, sect, '.');
    std::string key;
    ss >> key;
    return {sect, key};
}

bool cparser::Section::valid() const {
    return isValid();
}

bool cparser::Section::InSection(const std::string& key) const {
    return map_elements_.find(key) != map_elements_.end();
}

cparser::Section* cparser::Section::AddSection(const std::string& str) {
    if (str.find('.') == std::string::npos) {
        if (map_elements_.find(str) != map_elements_.end()) {
            return (Section*) (map_elements_.at(str));
        }
        auto* sect = new Section();
        sect->setSection();
        map_elements_.insert({str, sect});
        return sect;
    } else {
        std::pair<std::string, std::string> key = NameSeparation(str);
        return this->AddSection(key.first)->AddSection(key.second);
    }
}

cparser::Element& cparser::Section::Get(const std::string& str) const {
    if (str.find('.') == std::string::npos) {
        return dynamic_cast<Element&>(*(map_elements_.at(str)));
    }

    std::pair<std::string, std::string> key = NameSeparation(str);
    return dynamic_cast<cparser::Section&>(this->Get(key.first)).Get(key.second);
}

void cparser::Section::AddArgument(const std::string& str, cparser::Element* el) {
    map_elements_.insert({str, el});
}

cparser::Section& cparser::Section::operator=(const cparser::Section& rhs) {
    this->~Section();

    for (auto& elem : rhs.map_elements_) {

        if (elem.second->getElementType() == cparser::elementType::SECTION) {
            cparser::Element* sect = new Section();
            sect = elem.second;
            this->map_elements_[elem.first] = sect;

        }

        Element temp = *elem.second;
        std::string name = elem.first;

        this->map_elements_[name] = &temp;
    }
    return *this;
}

cparser::Section::~Section() {

    std::vector<std::string> keys;
    for (const auto& elem : map_elements_) {
        keys.emplace_back(elem.first);
    }

    for (std::string& elem : keys) {
        if (map_elements_[elem]->getElementType() == cparser::elementType::SECTION) {
            dynamic_cast<Section*>(map_elements_[elem])->~Section();
        } else {
            if (dynamic_cast<OMFLvariable::Variable&>(*map_elements_[elem]).IsArray()) {
                dynamic_cast<OMFLvariable::ArrayValue&>(*map_elements_[elem]).~ArrayValue();
            }
            delete map_elements_[elem];
            map_elements_.erase(elem);
        }
    }
}

OMFLvariable::Variable::Variable(const std::string& name, OMFLvariable::variableType variableType) : cparser::Element(name, cparser::VARIABLE) {
    variableType_ = variableType;
}

OMFLvariable::Variable::Variable(OMFLvariable::variableType variableType) : cparser::Element("", cparser::VARIABLE) {
    variableType_ = variableType;
}

OMFLvariable::variableType OMFLvariable::Variable::getVariableType() const {
    return variableType_;
}

void OMFLvariable::Variable::setType(OMFLvariable::variableType type) {
    variableType_ = type;
}

OMFLvariable::Variable& OMFLvariable::Variable::operator=(OMFLvariable::Variable& rhs) {
    this->~Variable();

    if (dynamic_cast<cparser::Element&>(*this).isVariable()) {
        switch (dynamic_cast<Variable&>(*this).getVariableType()) {
            case variableType::INT:
                dynamic_cast<IntValue&>(*this) = dynamic_cast<IntValue&>(rhs);
                break;
            case variableType::FLOAT:
                dynamic_cast<FloatValue&>(*this) = dynamic_cast<FloatValue&>(rhs);
                break;
            case variableType::STRING:
                dynamic_cast<StringValue&>(*this) = dynamic_cast<StringValue&>(rhs);
                break;
            case variableType::BOOL:
                dynamic_cast<BoolValue&>(*this) = dynamic_cast<BoolValue&>(rhs);
                break;
            case variableType::ARRAY:
                dynamic_cast<ArrayValue&>(*this) = dynamic_cast<ArrayValue&>(rhs);
                break;
            case variableType::INVALID:
                break;
        }
    }
    return *this;
}

OMFLvariable::IntValue::IntValue(const std::string& name, int value) : Variable(name, variableType::INT) {
    value_ = value;
}

OMFLvariable::IntValue& OMFLvariable::IntValue::Get() {
    return *this;
}

int OMFLvariable::IntValue::getValue() const {
    return value_;
}

void OMFLvariable::IntValue::setValue(int value) {
    value_ = value;
}

OMFLvariable::IntValue& OMFLvariable::IntValue::operator=(OMFLvariable::Variable& rhs) {
    *this = rhs;
    return *this;
}

OMFLvariable::FloatValue::FloatValue(const std::string& name, float value) : Variable(name, variableType::FLOAT) {
    value_ = value;
}

OMFLvariable::FloatValue& OMFLvariable::FloatValue::Get() {
    return *this;
}

float OMFLvariable::FloatValue::getValue() const {
    return value_;
}

void OMFLvariable::FloatValue::setValue(float value) {
    value_ = value;
}

OMFLvariable::FloatValue& OMFLvariable::FloatValue::operator=(OMFLvariable::Variable& rhs) {
    *this = rhs;
    return *this;
}

OMFLvariable::StringValue::StringValue(const std::string& name, const std::string& value) : Variable(name, variableType::STRING){
    value_ = value;
}

OMFLvariable::StringValue& OMFLvariable::StringValue::Get() const{
    return const_cast<StringValue&>(*this);
}

std::string OMFLvariable::StringValue::getValue() const {
    return value_;
}

void OMFLvariable::StringValue::setValue(const std::string& value) {
    value_ = value;
}

OMFLvariable::StringValue& OMFLvariable::StringValue::operator=(OMFLvariable::Variable& rhs) {
    *this = rhs;
    return *this;
}

OMFLvariable::BoolValue::BoolValue(const std::string& name, bool value) : Variable(name, variableType::BOOL) {
    value_ = value;
}

OMFLvariable::BoolValue& OMFLvariable::BoolValue::Get() {
    return *this;
}

bool OMFLvariable::BoolValue::getValue() const {
    return value_;
}

void OMFLvariable::BoolValue::setValue(bool value) {
    value_ = value;
}

OMFLvariable::BoolValue& OMFLvariable::BoolValue::operator=(OMFLvariable::Variable& rhs) {
    *this = rhs;
    return *this;
}

OMFLvariable::ArrayValue::ArrayValue(const std::string& name, const std::vector<cparser::Element*>& value) : Variable(name, variableType::ARRAY) {
    value_ = value;
}

OMFLvariable::ArrayValue& OMFLvariable::ArrayValue::Get() {
    return *this;
}

OMFLvariable::ArrayValue::ArrayValue() : Variable("", variableType::ARRAY) {}

bool OMFLvariable::ArrayValue::isEmpty() const {
    return value_.empty();
}

void OMFLvariable::ArrayValue::AddElement(cparser::Element* elem) {
    value_.push_back(elem);
}

cparser::Element& OMFLvariable::ArrayValue::operator[](int index) {
    if (index >= value_.size()) {
        out_of_index[index] = new cparser::Element;
        return *out_of_index[index];
    }
    return *value_.at(index);
}

OMFLvariable::ArrayValue::~ArrayValue() {
    for (auto elem : value_) {
        if (elem->isSection()) {
            dynamic_cast<cparser::Section&>(*elem).~Section();
        } else {
            if (elem->IsArray()) {
                dynamic_cast<OMFLvariable::ArrayValue&>(*elem).~ArrayValue();
            } else {
                delete elem;
            }

        }
    }
}

OMFLvariable::ArrayValue& OMFLvariable::ArrayValue::operator=(OMFLvariable::Variable& rhs) {
    *this = rhs;
    return *this;
}

cparser::Element& cparser::Element::Get(const std::string& name) {
    if (elementType_ == elementType::SECTION) {
        return dynamic_cast<cparser::Section&>(*this).Get(name);
    }
    return dynamic_cast<OMFLvariable::Variable&>(*this).Get(name);
}

int cparser::Element::AsInt() {
    return  dynamic_cast<OMFLvariable::IntValue&>(*this).getValue();
}

float cparser::Element::AsFloat() {
    return dynamic_cast<OMFLvariable::FloatValue&>(*this).getValue();
}

std::string cparser::Element::AsString() {
    return dynamic_cast<OMFLvariable::StringValue&>(*this).getValue();
}

bool cparser::Element::AsBool() {
    return dynamic_cast<OMFLvariable::BoolValue&>(*this).getValue();
}

cparser::Element& cparser::Element::operator[](int index) {
    return (dynamic_cast<OMFLvariable::ArrayValue&>(*this))[index];
}

bool cparser::Element::IsInt() {
    return elementType_ == elementType::VARIABLE && dynamic_cast<OMFLvariable::Variable&>(*this).getVariableType() == OMFLvariable::variableType::INT;;
}

bool cparser::Element::IsFloat() {
    return elementType_ == elementType::VARIABLE && dynamic_cast<OMFLvariable::Variable&>(*this).getVariableType() == OMFLvariable::variableType::FLOAT;
}

bool cparser::Element::IsString() {
    return elementType_ == elementType::VARIABLE && dynamic_cast<OMFLvariable::Variable&>(*this).getVariableType() == OMFLvariable::variableType::STRING;
}

bool cparser::Element::IsBool() {
    return elementType_ == elementType::VARIABLE && dynamic_cast<OMFLvariable::Variable&>(*this).getVariableType() == OMFLvariable::variableType::BOOL;
}

bool cparser::Element::IsArray() {
    return elementType_ == elementType::VARIABLE && dynamic_cast<OMFLvariable::Variable&>(*this).getVariableType() == OMFLvariable::variableType::ARRAY;
}

int cparser::Element::AsIntOrDefault(int value) {
    if (this->elementType_ == cparser::VARIABLE && dynamic_cast<OMFLvariable::Variable&>(*this).getVariableType() == OMFLvariable::variableType::INT) {
        return ((OMFLvariable::IntValue&)*this).getValue();
    }
    *this = OMFLvariable::IntValue(this->getName(), value);
    return value;
}

float cparser::Element::AsFloatOrDefault(float value) {
    if (dynamic_cast<OMFLvariable::Variable&>(*this).getVariableType() == OMFLvariable::variableType::FLOAT) {
        return ((OMFLvariable::FloatValue&)*this).getValue();
    }

    dynamic_cast<OMFLvariable::Variable&>(*this).setType(OMFLvariable::variableType::FLOAT);
    *this = OMFLvariable::FloatValue(this->getName(), value);

    return value;
}

std::string cparser::Element::AsStringOrDefault(std::string value) {
    if (dynamic_cast<OMFLvariable::Variable&>(*this).getVariableType() == OMFLvariable::variableType::STRING) {
        return ((OMFLvariable::StringValue&)*this).getValue();
    }

    dynamic_cast<OMFLvariable::Variable&>(*this).setType(OMFLvariable::variableType::STRING);
    *this = OMFLvariable::StringValue(this->getName(), value);

    return value;
}

bool cparser::Element::AsBoolOrDefault(bool value) {
    if (dynamic_cast<OMFLvariable::Variable&>(*this).getVariableType() == OMFLvariable::variableType::FLOAT) {
        return ((OMFLvariable::BoolValue&)*this).getValue();
    }

    dynamic_cast<OMFLvariable::Variable&>(*this).setType(OMFLvariable::variableType::BOOL);
    *this = OMFLvariable::BoolValue(this->getName(), value);

    return value;
}

void cparser::Element::setSection() {
    elementType_ = elementType::SECTION;
}

cparser::elementType cparser::Element::getElementType() const {
    return elementType_;
}

std::string cparser::Element::getName() const {
    return elementName_;
}

void cparser::Element::setNotValid() {
    valid_ = false;
}

bool cparser::Element::isValid() const {
    return valid_;
}

cparser::Element::Element(const std::string& name, cparser::elementType type) {
    elementName_ = name;
    elementType_ = type;
    valid_ = true;
}

cparser::Element::Element() {
    valid_ = true;
}

bool cparser::Element::isSection() {
    return elementType_ == elementType::SECTION;
}

bool cparser::Element::isVariable() {
    return elementType_ == elementType::VARIABLE;
}
