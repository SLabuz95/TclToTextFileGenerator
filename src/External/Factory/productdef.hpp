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

    class Definition : public Interface{
    public:
        using ProductsList = ProductsList_;
        inline static constexpr ProductsList fromUnderlying(std::underlying_type_t<ProductsList> value){
            return static_cast<ProductsList>(value);
        }
        inline static constexpr std::underlying_type_t<ProductsList> toUnderlyng(ProductsList value){
            return static_cast<std::underlying_type_t<ProductsList>>(value);
        }
        inline static constexpr std::underlying_type_t<ProductsList> numbOfProducts(){return toUnderlyng(ProductsList::FCT_End) - toUnderlyng(ProductsList::FCT_Begin);}

        virtual constexpr ProductsList type()const = 0;
    };

};



#endif // PRODUCTDEF_HPP
