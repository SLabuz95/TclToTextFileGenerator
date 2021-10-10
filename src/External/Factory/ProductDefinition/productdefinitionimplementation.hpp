#ifndef PRODUCTDEFINITIONIMPLEMENTATION_HPP
#define PRODUCTDEFINITIONIMPLEMENTATION_HPP

#include<type_traits>

template<class ProductsList>
requires (std::is_enum_v<ProductsList>)
class ProductDefinition {
    public:
    // Implementation
    class ImplementationData {
    public:
        class Properties;
        class Methods;

        //static_assert (  std::is_base_of_v<Properties, Methods>, "ERROR");
    };


    template<class Properties, class Methods>
    //requires std::is_base_of_v<Properties, Methods>
    class ImplementationPriv : protected Methods{};

    using Implementation = ImplementationPriv<typename  ImplementationData::Properties, typename ImplementationData::Methods>;

    // Interface
    class InterfaceData {
    public:
        class Methods;

        //static_assert (  std::is_base_of_v<Properties, Methods>, "ERROR");
    };


    template<class Implementation, class Methods>
    //requires std::is_base_of_v<Implementation, Methods>
    class InterfacePriv : protected Methods{};

    using Interface = InterfacePriv<Implementation, typename InterfaceData::Methods>;

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

AnimalsDefinition animalDefinition;



#endif // PRODUCTDEFINITIONIMPLEMENTATION_HPP
