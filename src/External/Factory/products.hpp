#ifndef PRODUCTS_HPP
#define PRODUCTS_HPP

#include<iostream>
#include<type_traits>
#include<QString>
#include"External/Factory/productdef.hpp"

template <typename ProductsList>
requires (std::is_enum_v<ProductsList>)
class ProductsConfiguration{
    /*requires
    (std::is_same_v<ProductDef, FactoryProductDefinition> and
     std::is_enum_v<ProductDef::Type> and
     std::is_same_v<std::underlying_type_t<typename ProductDef::Type>, uint> and
     FCT_toUnderlying<typename ProductDef::Type>(ProductDef::Type::FCT_Begin) == 0 and*/
     //FCT_toUnderlying<ProductDef::Type>(ProductDef::Type::FCT_End) - FCT_toUnderlying<ProductDef::Type>(ProductDef::Type::FCT_Begin) > 0/* and
     //(ProductDef::Type::FCT_End - ProductDef::Type::FCT_Begin) >= 0*/)

    // Products Definition
    using ProductDefinition = typename  ProductDefinition<ProductsList>::Definition;

    public:
    using ProductBase = ProductDefinition;
    using ListOfBases = QList<ProductBase*>;

    protected:
    ProductsConfiguration() = delete;
    ProductsConfiguration(const ProductsConfiguration& ) = delete;

    template<ProductsList ProductType>
    requires (ProductType >= ProductsList::FCT_Begin and ProductType < ProductsList::FCT_End)
    class ImplementationData{
        public:
        static constexpr ProductsList type(){return ProductType;}
        struct Properties;
        class Methods;
    };

    private:
    template<ProductsList ProductType>
    //requires std::is_base_of_v<Properties, Methods>
    using IS_PROPERTIES_BASE_OF_METHODS = void;
    public:

    template<ProductsList ProductType>
    class Implementation : public ImplementationData<ProductType>::Methods{};

    // Interface
    template<ProductsList ProductType>
    requires (ProductType >= ProductsList::FCT_Begin and ProductType < ProductsList::FCT_End)
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

    template<ProductsList ProductType>
    class Interface : protected InterfaceData<ProductType>::Methods{};

};

#endif // PRODUCTS_HPP
