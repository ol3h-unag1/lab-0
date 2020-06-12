#pragma once

template< typename... Ts >
class Test
{

};

template< typename T, typename U >
class Test< T, U >
{

};
