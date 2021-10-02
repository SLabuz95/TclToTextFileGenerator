#ifndef FACTORY_HPP
#define FACTORY_HPP

#include<iostream>
#include<type_traits>
#include<QString>



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
        return static_cast<EnumType>(type + 1);    // return old value
    }

// Factory Definitions
    template <class ProductsEnumType>
    requires std::is_enum_v<ProductsEnumType>
    class FactoryProductDefinition{
    public:
        using Type = ProductsEnumType;
        class ProductCommonInterface;
        class ProductCommonImplementation;

        static_assert (std::is_base_of_v<ProductCommonImplementation , ProductCommonInterface>, "ProductCommonInterface must derive from ProductCommonImplementation");
    };


    template <typename ProductsEnumType>
    class Factory{
        /*requires
        (std::is_same_v<ProductDef, FactoryProductDefinition> and
         std::is_enum_v<ProductDef::Type> and
         std::is_same_v<std::underlying_type_t<typename ProductDef::Type>, uint> and
         FCT_toUnderlying<typename ProductDef::Type>(ProductDef::Type::FCT_Begin) == 0 and*/
         //FCT_toUnderlying<ProductDef::Type>(ProductDef::Type::FCT_End) - FCT_toUnderlying<ProductDef::Type>(ProductDef::Type::FCT_Begin) > 0/* and
         //(ProductDef::Type::FCT_End - ProductDef::Type::FCT_Begin) >= 0*/)

        Factory() = delete;
        Factory(const Factory& ) = delete;

        using ProductDefinition = FactoryProductDefinition<ProductsEnumType>;

       class FactoryCommonInterface : public ProductDefinition::BaseProductInterface{
        public:
            using Type = typename ProductDefinition::Type;

            using Interface = typename ProductDefinition::ProductInterface;
            template<typename ...Args>
            FactoryCommonInterface(Args... args) : ProductDefinition::ProductInterface(args...){}
            virtual ~FactoryCommonInterface(){}
            inline virtual Type type()const = 0;
        };
       using ProductInterface = FactoryCommonInterface<ProductDefinition>;

        public:

        template<ProductsEnumType ProductType>
        //requires (ProductType >= ProductsEnumType::FCT_Begin and ProductType < ProductsEnumType::FCT_End)
        class ProductBase : public ProductInterface{
            using Interface = ProductInterface;
        public:
            ProductBase() : ProductInterface(){}
            template<typename ...Args>
            ProductBase(Args... args) : ProductInterface(args...){}
            virtual ~ProductBase(){}
            //ProductBase(ProductInterface&& interface) : ProductInterface(interface){}
            inline typename Interface::Type type()const override final{return ProductType;}
        };

        using ProductBasePtr = ProductInterface*;
        using ProductBaseRef = ProductInterface&;
        using ListOfBases = QList<ProductBasePtr>;

        template<ProductsEnumType type>
        //requires (type >= ProductsEnumType::FCT_Begin and type < ProductsEnumType::FCT_End)
        class Product;

        private:
        inline static constexpr std::underlying_type_t<ProductsEnumType> toUnderlyng(ProductsEnumType value){
            return static_cast<std::underlying_type_t<ProductsEnumType>>(value);
        }

        inline static constexpr std::underlying_type_t<ProductsEnumType> numbOfProducts(){return toUnderlyng(ProductsEnumType::FCT_End) - toUnderlyng(ProductsEnumType::FCT_Begin);}
        using CreateFunction = ProductInterface *const (Factory::*)();
        using CreateFunctionTable = CreateFunction[numbOfProducts() /*toUnderlyng(ProductsEnumType::FCT_End) - toUnderlyng(ProductsEnumType::FCT_Begin)*/];

        public:
        template<ProductsEnumType productType>
        static ProductBase<ProductsEnumType>* create(){
            return Product<productType>();
        }


        private:
        static constexpr CreateFunction initCreateFunctionTable(){
            for(ProductsEnumType productType = ProductsEnumType::FCT_Begin; productType < ProductsEnumType::FCT_End; productType++){
                static_assert (std::is_base_of_v<ProductBase<productType>, Product<productType>>, "Invalid product base");
                createFunctionTable[toUnderlyng(productType)] = &Factory::create<productType>;
            }

            return createFunctionTable[0];
        }

        constexpr static CreateFunctionTable createFunctionTable = {initCreateFunctionTable()};

    };
//};


#endif // FACTORY_HPP
