#ifndef PRODUCTDEF_HPP
#define PRODUCTDEF_HPP

#include<type_traits>
#include"External/Factory/products.hpp"

template<class ProductsList_>
requires (std::is_enum_v<ProductsList_>)
class ProductDefinition {
    public:
    using ProductsList = ProductsList_;
    // Implementation
    class ImplementationData {
    public:
        struct Properties;
        class Methods;

        //static_assert (  std::is_base_of_v<Properties, Methods>, "ERROR");
    };


    private:
    template<class Properties, class Methods>
    //requires std::is_base_of_v<Properties, Methods>
    using IS_PROPERTIES_BASE_OF_METHODS = void;
    public:

    class Implementation : protected ImplementationData::Methods{};


    // Interface
    class InterfaceData {
    public:
        class Methods;

        //static_assert (  std::is_base_of_v<Properties, Methods>, "ERROR");
    };

    private:
    template<class Implementation, class Methods>
    //requires std::is_base_of_v<Properties, Methods>
    using IS_Methods_BASE_OF_Implementation = void;
    public:

    class Interface : protected InterfaceData::Methods{};

    class Definition : public Interface{};

};

enum class Animals{
    Cat,
    Dog
};

using AnimalData = ProductDefinition<Animals>::ImplementationData;

template<>
class ProductDefinition<Animals>::ImplementationData::Properties{
protected:
    int age = 0;
};

template<>
class ProductDefinition<Animals>::ImplementationData::Methods : protected ProductDefinition<Animals>::ImplementationData::Properties{
    void calculateAge(){}
};

using AnimalInterfaceData = ProductDefinition<Animals>::InterfaceData;

template<>
class AnimalInterfaceData::Methods : public ProductDefinition<Animals>::Implementation{
    const int& ageView()const{return age;}
};

using AnimalsDefinition = ProductDefinition<Animals>::Definition;



#endif // PRODUCTDEF_HPP
