#ifndef FACTORY_HPP
#define FACTORY_HPP

#include<iostream>
#include<type_traits>
#include<QString>
#include"External/Factory/ProductDefinition/productdef.hpp"


//namespace Factory {

// Supporting functions
    template<typename EnumType>
    requires (std::is_enum_v<EnumType>)
    inline static constexpr std::underlying_type_t<EnumType> FCT_toUnderlying(EnumType value){
        return static_cast<std::underlying_type_t<EnumType>>(value);
    }

    template<typename EnumType>
    requires (std::is_enum_v<EnumType>)
    EnumType operator++(EnumType type)
    {
        return static_cast<EnumType>(type+1);    // return old value
    }


// Factory Definitions
    template <typename ProductsList>
    requires (std::is_enum_v<ProductsList>)
    class Factory{
        /*requires
        (std::is_same_v<ProductDef, FactoryProductDefinition> and
         std::is_enum_v<ProductDef::Type> and
         std::is_same_v<std::underlying_type_t<typename ProductDef::Type>, uint> and
         FCT_toUnderlying<typename ProductDef::Type>(ProductDef::Type::FCT_Begin) == 0 and*/
         //FCT_toUnderlying<ProductDef::Type>(ProductDef::Type::FCT_End) - FCT_toUnderlying<ProductDef::Type>(ProductDef::Type::FCT_Begin) > 0/* and
         //(ProductDef::Type::FCT_End - ProductDef::Type::FCT_Begin) >= 0*/)

        // Products Definition
        using ProductDefinition = ProductDefinition<ProductsList>;
        using ProductBase = ProductDefinition;

        Factory() = delete;
        Factory(const Factory& ) = delete;

        template<ProductsList ProductType>
        requires (ProductType >= ProductsList::FCT_Begin and ProductType < ProductsList::FCT_End)
        class ImplementationData{
            public:
            static constexpr ProductsList productType(){return ProductType;}
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
        class Interface : protected InterfaceData::Methods{};

        template<ProductsList ProductType>
        class Product : public Interface<ProductType>{};


        using ProductBasePtr = ProductBase*;
        using ProductBaseRef = ProductBase&;
        using ListOfBases = QList<ProductBase>;

        private:
        inline static constexpr std::underlying_type_t<ProductsList> toUnderlyng(ProductsList value){
            return static_cast<std::underlying_type_t<ProductsList>>(value);
        }

        inline static constexpr std::underlying_type_t<ProductsList> numbOfProducts(){return toUnderlyng(ProductsList::FCT_End) - toUnderlyng(ProductsList::FCT_Begin);}
        using CreateFunction =  ProductBase*const (Factory::*)();
        using CreateFunctionTable = CreateFunction[numbOfProducts()];

        public:
        template<ProductsList productType>
        static ProductBase* create(){
            return new Product<productType>();
        }


        private:
        static constexpr CreateFunction initCreateFunctionTable(){
            for(ProductsList productType = ProductsList::FCT_Begin; productType < ProductsList::FCT_End; productType++){
                //static_assert (std::is_base_of_v<ProductBase<productType>, Product<productType>>, "Invalid product base");
                createFunctionTable[toUnderlyng(productType)] = &Factory::create<productType>;
            }

            return createFunctionTable[0];
        }

        constexpr static CreateFunctionTable createFunctionTable = {initCreateFunctionTable()};

    };
//};


#endif // FACTORY_HPP
