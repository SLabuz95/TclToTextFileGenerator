#ifndef FACTORY_HPP
#define FACTORY_HPP

#include<iostream>
#include<type_traits>
#include<QString>
#include"External/Factory/products.hpp"


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
        using ProductDefinition = typename  ProductDefinition<ProductsList>::Definition;

        public:
        using ProductBase = ProductDefinition;

        protected:
        Factory() = delete;
        Factory(const Factory& ) = delete;
public:
        template<ProductsList ProductType>
        class Product : public ProductsConfiguration<ProductsList>::Interface<ProductType>{};

        using ProductBasePtr = ProductBase*;
        using ProductBaseRef = ProductBase&;
        //using ListOfBases = QList<ProductBase>;

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
