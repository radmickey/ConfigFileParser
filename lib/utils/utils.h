#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <cstring>
#include <stdexcept>
#include <sstream>


namespace cparser {
    class Section;
}


namespace cparser {
    enum elementType {
        VARIABLE,
        SECTION
    };

    class Element {
    private:
        elementType elementType_;
        std::string elementName_;
        bool valid_;
    public:
        Element(const std::string& name, elementType type);

        Element();

        virtual ~Element() = default;

        bool isSection();

        bool isVariable();

        void setSection();

        [[nodiscard]] elementType virtual getElementType() const;

        [[nodiscard]] std::string virtual getName() const;

        virtual void setNotValid();

        [[nodiscard]] virtual bool isValid() const;

        virtual Element& Get(const std::string& name);

        virtual int AsInt();

        virtual float AsFloat();

        virtual std::string AsString();

        virtual bool AsBool();

        virtual bool IsInt();

        virtual bool IsFloat();

        virtual bool IsString();

        virtual bool IsBool();

        virtual bool IsArray();

        virtual  int AsIntOrDefault(int value);

        virtual float AsFloatOrDefault(float value);

        virtual std::string AsStringOrDefault(std::string value);

        virtual bool AsBoolOrDefault(bool value);

        virtual cparser::Element& operator[](int index);
    };

}

namespace OMFLvariable {

    enum variableType {
        INT,
        FLOAT,
        STRING,
        BOOL,
        ARRAY,
        INVALID
    };

    class Variable : public cparser::Element {
    private:
        variableType variableType_;
    public:
        Variable(const std::string& name, variableType variableType);

        explicit Variable(variableType variableType);

        Variable() = default;

        virtual Variable& operator=(Variable& rhs) = 0;

        [[nodiscard]] variableType virtual getVariableType() const;

        void setType(variableType type);
    };


    class IntValue final : public Variable {
    private:
        int value_;
    public:
        IntValue(const std::string& name, int value);

        IntValue& operator=(Variable& rhs);

        IntValue& Get();

        [[nodiscard]] int getValue() const;

        void setValue(int value);

    };

    class FloatValue final : public Variable {
    private:
        float value_;
    public:
        FloatValue(const std::string& name, float value);

        FloatValue& operator=(Variable& rhs) override;

        FloatValue& Get();

        [[nodiscard]] float getValue() const;

        void setValue(float value);
    };

    class StringValue final : public Variable {
    private:
        std::string value_;
    public:
        StringValue(const std::string& name, const std::string& value);

        StringValue& operator=(Variable& rhs);

        [[nodiscard]] StringValue& Get() const;


        [[nodiscard]] std::string getValue() const;

        void setValue(const std::string& value);
    };

    class BoolValue final : public Variable {
    private:
        bool value_;
    public:
        BoolValue(const std::string& name, bool value);

        BoolValue& operator=(Variable& rhs);

        BoolValue& Get();

        bool getValue() const;

        void setValue(bool value);

    };


    class ArrayValue final : public Variable {
    private:
        std::vector<cparser::Element*> value_;
        std::unordered_map<int, cparser::Element*> out_of_index;
    public:
        ArrayValue(const std::string& name, const std::vector<cparser::Element*>& value);

        ArrayValue& operator=(Variable& rhs) override;

        ArrayValue();

        ~ArrayValue() override;

        bool isEmpty() const;

        void AddElement(cparser::Element* elem);

        ArrayValue& Get();

        cparser::Element& operator[](int index) override;

    };
}

namespace cparser {
    class Section : public cparser::Element {
    private:
        std::map<std::string, cparser::Element*> map_elements_;
    public:
        Section(const std::string& name, const cparser::Element& elem);

        Section(const cparser::Element& elem);

        Section() = default;

        ~Section();

        cparser::Section& operator=(const cparser::Section& rhs);

        bool valid() const;

        bool InSection(const std::string& key) const;

        Section* AddSection(const std::string& str);

        void AddArgument(const std::string& str, Element* el);

        Element& Get(const std::string& str) const;

    private:

        static std::pair<std::string, std::string> NameSeparation(const std::string& str);

    };


}


