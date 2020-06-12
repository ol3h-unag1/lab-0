//
//
// For type_name template func many thanks to
// Howard Hinnant (original implementation)
// and
// 康桓瑋 (magic numbers removal)
//
// originally posted on:
// https://stackoverflow.com/questions/81870/is-it-possible-to-print-a-variables-type-in-standard-c/56766138#56766138
//
//
//

#ifndef TYPE_NAME_HPP
#define TYPE_NAME_HPP

#include <string_view>
#include <iostream>
#include <utility>

template <typename T>
constexpr std::string_view 
type_name()
{
    std::string_view name, prefix, suffix;
#ifdef __clang__
    name = __PRETTY_FUNCTION__;
    prefix = "std::string_view type_name() [T = ";
    suffix = "]";
#elif defined(__GNUC__)
    name = __PRETTY_FUNCTION__;
    prefix = "constexpr std::string_view type_name() [with T = ";
    suffix = "; std::string_view = std::basic_string_view<char>]";
#elif defined(_MSC_VER)
    name = __FUNCSIG__;
    prefix = "class std::basic_string_view<char,struct std::char_traits<char> > __cdecl type_name<";
    suffix = ">(void)";
#endif
    name.remove_prefix(prefix.size());
    name.remove_suffix(suffix.size());
    return name;
}

template< typename Arg, typename... Args >
void printTemplateTypeAndArgumentTypeUR( Arg&& arg, Args&&... args )
{
	int counter = 0;
	std::cout << counter << " | template type: " << type_name< Arg >() 
						 << " | argument type: " << type_name< decltype( ( std::forward< Arg >( arg ) ) )>() 
						 << " | argument value: " << arg; 
	
	( ( ++counter, std::cout << std::endl << counter << " | template type: " << type_name< Arg >() 
												     << " | argument type: " << type_name< decltype( ( std::forward< Args >( args ) ) )>() 
												     << " | argument value: " << args ), ... );
	std::cout << std::endl;
}

template< typename Arg, typename... Args >
void printTemplateTypeAndArgumentTypeBV( Arg arg, Args... args )
{
	int counter = 0;
	std::cout << counter << " | template type: " << type_name< Arg >() 
						 << " | argument type: " << type_name< decltype( ( std::forward< Arg >( arg ) ) )>() 
						 << " | argument value: " << arg; 
	
	( ( ++counter, std::cout << std::endl << counter << " | template type: " << type_name< Arg >() 
												     << " | argument type: " << type_name< decltype( ( std::forward< Args >( args ) ) )>() 
												     << " | argument value: " << args ), ... );
	std::cout << std::endl;
}

#define PRINT_DECLARED_TYPE( x ) std::cout << type_name< decltype( x )>() << std::endl;
#define PRINT_EXPRESSION_TYPE( x ) std::cout << type_name< decltype( ( x ) )>() << std::endl;

// template< typename T >
// void printTemplateTypeAndArgumentTypeUR( T&& t )
// {
// 	std::cout << __PRETTY_FUNCTION__ << std::endl;
//     std::cout << type_name< T >() << std::endl;
//     std::cout << type_name< decltype( ( std::forward< T >( t ) ) )>() << std::endl;
//     std::cout << std::endl;
// }

// template< typename T >
// void printTemplateTypeAndArgumentTypeLVR( T& t )
// {
// 	std::cout << __PRETTY_FUNCTION__ << std::endl;
//     std::cout << type_name< T >() << std::endl;
//     std::cout << type_name< decltype( ( std::forward< T >( t ) ) )>() << std::endl;
//     std::cout << std::endl;
// }

// template< typename T >
// void printTemplateTypeAndArgumentTypeBV( T t )
// {
// 	std::cout << __PRETTY_FUNCTION__ << std::endl;
//     std::cout << type_name< T >() << std::endl;
//     std::cout << type_name< decltype( ( std::forward< T >( t ) ) )>() << std::endl;
//     std::cout << std::endl;
// }



#endif // #ifndef TYPE_NAME_HPP