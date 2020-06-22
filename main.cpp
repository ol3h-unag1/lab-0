#include <iostream>

#include <string>
#include <vector>

#include <map>
#include <set>

#include <utility>
#include <functional>

#include <algorithm>
#include <random>

#include <cassert>


// // // // // // // // // // // // // 

namespace MySetup
{
    std::vector< std::string > traceCollector;
#define __TRACE_COLLECTOR__ traceCollector
}

#include "my_setup.hpp"

// // // // // // // // // // // // // 

std::size_t const C_DEFAULT_HANDSIZE = 6;

class Player;
void G__ENDTURN( bool swap );

// // // // // // // // // // // // // 

template< typename Value, typename Suit >
class Card
{
public:
    using ValueType = Value;
    using SuitType = Suit;

public:
    template< typename V, typename S >
    Card( V value, S suit )
        : _value( std::forward< V >( value ) )
        , _suit( std::forward< S >( suit ) )
        , _hash( std::hash< std::string >{}( Value2Str( value ) + Suit2Str( suit ) ) )
    {}

    template< typename TT, typename UU >
    friend
        bool operator==( Card< TT, UU > const& left, Card< TT, UU > const& right )
    {
        return left._hash == right._hash;
    }

    template< typename TT, typename UU >
    friend
        bool operator!=( Card< TT, UU > const& left, Card< TT, UU > const& right )
    {
        return ! (left._hash == right._hash );
    }

public:
    Value GetValue() const
    {
        return _value;
    }

    Suit GetSuit() const
    {
        return _suit;
    }

private:
    Value   _value;
    Suit    _suit;

    std::size_t _hash;
};

enum class Value
{
    Two,
    Three,
    Four,
    Five,
    Six,
    Seven,
    Eight,
    Nine,
    Ten,
    Jack,
    Queen,
    King,
    Ace,
    
    Invalid
};

std::string Value2Str( Value v )
{
    switch( v )
    {
    case Value::Ace: return "Ace";
    case Value::Two: return "Two";
    case Value::Three: return "Three";
    case Value::Four: return "Four";
    case Value::Five: return "Five";
    case Value::Six: return "Six";
    case Value::Seven: return "Seven";
    case Value::Eight: return "Eight";
    case Value::Nine: return "Nine";
    case Value::Ten: return "Ten";
    case Value::Jack: return "Jack";
    case Value::Queen: return "Queen";
    case Value::King: return "King";

    default:
        return "";
    }

    return "";
}

enum class Suit
{
    Hearts,     // ♥
    Spades,     // ♠
    Dimonds,    // ♦
    Clubs,      // ♣
    
    Invalid
};

std::string Suit2Str( Suit s )
{
    switch( s )
    {
    case Suit::Hearts: return "Hearts";
    case Suit::Spades: return "Spades";
    case Suit::Dimonds: return "Dimonds";
    case Suit::Clubs: return "Clubs";
   
    default:
        return "";
    }


    return "";
}

template< typename Card >
std::string Card2Str( Card const& c )
{
    std::string result;
    if( auto value = Value2Str( c.GetValue() ); value.empty() == false )
    {
        result += value + " of ";
    }
    else
    {
        result += "INCORECT VALUE of ";
    }

    if( auto suit = Suit2Str( c.GetSuit() ); suit.empty() == false )
    {
        result += suit;
    }
    else
    {
        result += "INCORRECT SUIT";
    }

    return result;
}

// // // // // // // // // // // // // 

template< typename Card >
struct AttackerDefenderPair
{
    AttackerDefenderPair( Card attacker, Card defender )
        : Attacker( attacker )
        , Defender( defender )
    {}

    Card Attacker;
    Card Defender;
};

using ADPair = AttackerDefenderPair< Card< Value, Suit > >;

// // // // // // // // // // // // // 
template< template < typename > typename Container, typename Card >
struct DeckType
{
public:
    using ContainerType = Container< Card >;
    using CardType = Card;

    using Type = DeckType< Container, Card >;
};

using DTA = DeckType< std::vector, Card< Value, Suit > >;
using CT = DTA::CardType;

// // // // // // // // // // // // // 

template< typename Container, typename Values, typename Suits >
void PackDeck( Container& deck, Values&& values, Suits&& suits )
{
    for( auto const& suit : suits )
    {
        for( auto const& value : values )
        {
            deck.emplace_back( std::move( value ), std::move( suit ) );
        }
    }
}

struct EmptyOstreamModifier
{
    std::ostream& operator()( std::ostream& os )
    {
        return os;
    }
} emptyOsMod;

void CoutPreCardPost( std::string pre, CT card, std::string post )
{
    std::cout << pre;
    std::cout << Card2Str( card );
    std::cout << post;
}

void CoutPreDeckPost( std::string pre, DTA::ContainerType const& deck, std::string post )
{
    for( auto& card : deck )
    {
        CoutPreCardPost( pre, card, post );
    }
}

template< typename Card, typename PreCard = EmptyOstreamModifier, typename PostCard = EmptyOstreamModifier>
void CoutCard( Card&& c, PreCard&& pre = std::move( emptyOsMod ), PostCard&& post = std::move( emptyOsMod ) )
{
    pre( std::cout );
    std::cout << Card2Str( c );
    post( std::cout );
}

template< typename Deck, typename PreCard = EmptyOstreamModifier, typename PostCard = EmptyOstreamModifier >
void CoutDeck( Deck const& d, bool ordered = false, PreCard&& pre = std::move( emptyOsMod ), PostCard&& post = std::move( emptyOsMod ) )
{
     bool needEndingNewLine = true;

     std::size_t i = 0u;
     for( auto const& card : d )
     {
         CoutCard( card, std::forward< PreCard >( pre ), std::forward< PostCard >( post ) );
         std::cout << std::endl;

         if( ordered )
         {
             // placing separator line after each suit values listed, except end
             if( ( i + 1 ) < d.size() )
             {
                 if( !( d[ i ].GetSuit() == d[ i + 1 ].GetSuit() ) )
                 {
                     std::cout << " - - - - - - - - - - - - - - - -" << std::endl;
                 }
             }
             else
             {
                 std::cout << std::endl;
                 needEndingNewLine = false;
             }
         }

         ++i;
     }

     if( needEndingNewLine )
     {
         std::cout << std::endl;
     }
}

 template< typename Deck, typename RandomEngine = std::default_random_engine, typename RandomDevice = std::random_device >
 void ShuffleDeck( Deck& d )
 {
     std::shuffle( std::begin( d ), std::end( d ), std::default_random_engine( std::random_device{}() ) );
 }

// // // // // // // // // // // // // 
 DTA::CardType G_INVALID_CARD();

 /// <summary>
/// PLAYER CLASS
/// </summary>
 class Player
 {
 public:
     enum class ID
     {
         Human,
         Computer,

         Invalid
     };

 private:
     Player( const Player& ) = delete;

 private:
     friend
         void CoutPlayerHand( Player const&, bool );

     friend
         DTA::CardType G_GRAB_SMALLEST_CARD( Player& player );

 public:
     Player()
         : id_( Player::ID::Invalid )
     {}

     explicit Player( ID id )
         : id_( id )
     {}

     Player( Player&& source ) noexcept
         : hand_( std::move( source.hand_ ) )
         , id_( source.id_ )
     {
         source.hand_.clear();
         source.id_ = Player::ID::Invalid;
     }

 public:
     Player::ID GetID() const { return id_; }
     // std::size_t HandSize() const { return hand_.size(); } 

     void AddCard( DTA::CardType card )
     {
         hand_.push_back( card );
         SortIml();
     }

     template< typename F >
     DTA::CardType SelectCard( F&& f ) const
     {
         return f( hand_ );
     }

     template< typename F >
     DTA::ContainerType SelectCards( F&& f ) const
     {
         DTA::ContainerType satified;
         for( const auto& card : hand_ )
         {
             if( f( card ) == true )
             {
                 satified.push_back( card );
             }
         }

         return satified;
     }

     template< typename F >
     DTA::CardType GrabCard( F&& f )
     {
         auto card = SelectCard( std::forward< F >( f ) );
         RemoveCardImpl( card );
         return card;
     }

     DTA::CardType GrabCard( DTA::CardType card )
     {
         RemoveCardImpl( card );
         return card;
     }

     void SetHand( DTA::ContainerType&& hand )
     {
         if( hand_.empty() )
         {
             hand_ = std::move( hand );
         }
         else
         {
             __ASSERT_MSG__( false, "HAND IS NOT EMPTY" );
         }

         SortIml();
     }

     std::size_t HandSize() const { return hand_.size(); }

     DTA::CardType operator[]( std::size_t i )
     {
         if( i < hand_.size() )
         {
             return hand_[ i ];
         }

         return G_INVALID_CARD();
     }

private:
    void RemoveCardImpl( DTA::CardType card )
    {
        SortIml();
        hand_.erase( std::remove( hand_.begin(), hand_.end(), card ), hand_.end() );
    }
    void SortIml()
    {
        std::sort( hand_.begin(), hand_.end(), []( DTA::CardType const& left, DTA::CardType const& right )
            {
                return left.GetValue() < right.GetValue();
            } );
    }

private:
   DTA::ContainerType hand_;
   ID id_;
};

std::string PlayerID2Str( Player::ID pk )
{
    switch( pk )
    {
    case Player::ID::Human: return "Human";
    case Player::ID::Computer:  return "Computer";
    
    default:
        return "";
    }
    return "";
}

void CoutDeckNumered( DTA::ContainerType const& deck )
{
    auto preTab = []( std::ostream& os ) -> std::ostream&
    {
        return os << "\t";
    };
    std::size_t counter = 0u;
    auto numOut = [&counter]( std::ostream& os ) -> std::ostream&
    {
        return os << " [" << counter++ << "]";
    };
    CoutDeck( deck, false, preTab, numOut );
}

void CoutPlayerHand( Player const& p, bool numered = false )
{
    if( numered )
    {
        CoutDeckNumered( p.hand_ );
    }
    else
    {
        CoutDeck( p.hand_ );
    }
}

// // // // // // // // // // // // // 

/// <summary>
/// GAME INFO TABLE
/// </summary>
struct GameInfo
{
    GameInfo()
    {
        PLAYERS.emplace( Player::ID::Human , Player( Player::ID::Human ) );
        PLAYERS.emplace( Player::ID::Computer, Player( Player::ID::Computer ) );
    }

    DTA::CardType::SuitType TRUMP = DTA::CardType::SuitType::Invalid;

    Player::ID ATTACKER_ID = Player::ID::Invalid;
    Player::ID DEFENDER_ID = Player::ID::Invalid;

    DTA::ContainerType DECK;
    std::vector< ADPair > WASTE;

    std::map< Player::ID, Player > PLAYERS;

} G_GAME_INFO;

Player* G_GET_HUMAN() 
{
    return std::addressof( G_GAME_INFO.PLAYERS[ Player::ID::Human ] );
}

Player* G_GET_COMPUTER()
{
    return std::addressof( G_GAME_INFO.PLAYERS[ Player::ID::Computer ] );
}

DTA::CardType::SuitType G_GET_TRUMP()
{
    return G_GAME_INFO.TRUMP;
}

DTA::CardType G_INVALID_CARD() { return { Value::Invalid, Suit::Invalid }; }

DTA::CardType GetAbsMinCard( DTA::ContainerType const& );

template< typename DeckType >
DTA::CardType G_GRAB_SMALLEST_CARD( DeckType&& deck )
{
    if( deck.size() )
    {
        auto smallest = GetAbsMinCard( deck );
        deck.erase( std::remove( deck.begin(), deck.end(), smallest ), deck.end() );
        return smallest;
    }

    return G_INVALID_CARD();
}

DTA::CardType G_GRAB_SMALLEST_CARD( Player& player )
{
    return G_GRAB_SMALLEST_CARD( player.hand_ );
}

Player* G_GET_ATTACKER()
{
    return std::addressof( G_GAME_INFO.PLAYERS[ G_GAME_INFO.ATTACKER_ID ] );
}

Player* G_GET_DEFENDER()
{
    return std::addressof( G_GAME_INFO.PLAYERS[ G_GAME_INFO.DEFENDER_ID ] );
}

template< typename T >
bool IsValid( T t )
{
    return t != T::Invalid;
}

bool IsValid( Player const& p )
{
    return p.GetID() != Player::ID::Invalid;
}

bool IsValid( DTA::CardType const& c )
{
    return IsValid( c.GetValue() ) && 
           IsValid( c.GetSuit() );
}

// // // // // // // // // // // // // 

// GLOBAL FUNCTORS

DTA::ContainerType GetHand( DTA::ContainerType& deck, std::size_t const handSize = C_DEFAULT_HANDSIZE )
{
    if( handSize < deck.size() ) // deck is full enough to fill the hand
    {
        // 
        // The copy-initialization of the returned entity is sequenced before the destruction of 
        // temporaries at the end of the full-expression established by the operand of the return 
        // statement, which, in turn, is sequenced before the destruction of local variables (6.6)
        // of the block enclosing the return statement.
        //
        struct Remover
        {
            DTA::ContainerType& deck_;
            Remover( DTA::ContainerType& deck )
                : deck_( deck )
            {}

            ~Remover()
            {
                for( std::size_t i = C_DEFAULT_HANDSIZE; i > 0; --i )
                {
                    deck_.pop_back();
                }
            }
        } d( deck );

        return DTA::ContainerType{ std::rbegin( deck ), std::rbegin( deck ) + handSize };
    }

    return DTA::ContainerType();
}

DTA::CardType GetMinValueGard( DTA::ContainerType const& d )
{
    static DTA::CardType const invalid{ Value::Invalid, Suit::Invalid };
    DTA::CardType const* smallestValueCard = &invalid;

    for( auto const& c : d )
    {
        if( IsValid( *smallestValueCard ) == false )
        {
            smallestValueCard = &c;
            continue;
        }

        if( smallestValueCard->GetValue() > c.GetValue() )
        {
            smallestValueCard = &c;
        }
    }

    return *smallestValueCard;
}

DTA::CardType GetMinTrump( DTA::ContainerType const& d )
{
    static DTA::CardType const invalid{ Value::Invalid, Suit::Invalid };
    DTA::CardType const* smallestTrump = &invalid;

    for( auto const& c : d )
    {
        if( c.GetSuit() != G_GAME_INFO.TRUMP )
        {
            continue;
        }

        if( IsValid( *smallestTrump ) == false )
        {
            smallestTrump = &c;
            continue;
        }

        if( smallestTrump->GetValue() > c.GetValue() )
        {
            smallestTrump = &c;
        }
    }

    return *smallestTrump;
}

DTA::CardType GetAbsMinCard( DTA::ContainerType const& d )
{
    static DTA::CardType const invalid{ Value::Invalid, Suit::Invalid };
    DTA::CardType const* smallest = &invalid;

    for( auto const& card : d )
    {
        // init of the cycle
        if( IsValid( *smallest ) == false ) // initialization
        {
            smallest = &card;
            continue;
        }     
  
        // same suits (different values)
        if( smallest->GetSuit() == card.GetSuit() )
        {
            smallest = ( smallest->GetValue() > card.GetValue() ) ? &card : smallest ; // chosing card with smallest value
        }
        else // different suits
        {
            if( smallest->GetValue() == card.GetValue() ) // different suits, same values
            {
                smallest = ( smallest->GetSuit() == G_GAME_INFO.TRUMP  ) ? &card : smallest;
            }
            else if( smallest->GetValue() > card.GetValue() )  // different suits, current smallest is bigger then newly checked card
            {
                smallest = ( card.GetSuit() == G_GAME_INFO.TRUMP ) ? smallest : &card;
            }
            else // different suits, current smallest is smaller then newly checked card
            {
                smallest = ( smallest->GetSuit() == G_GAME_INFO.TRUMP ) ? &card : smallest;
            }
        }

    }

    return *smallest;
}

bool CanAbsBeatMe( DTA::CardType me, DTA::CardType tested )
{
    if( me.GetSuit() == tested.GetSuit() )
    {
        return me.GetValue() < tested.GetValue();
    }

    if( tested.GetSuit() == G_GET_TRUMP() )
    {
        __ASSERT_MSG__( me.GetSuit() != G_GET_TRUMP(), "CanAbsBeatMe:: Same suits branch should be checked before!" );
        return true;
    }

    return false;
}

// // // // // // // // // // // // // 

// // // // // // // // // // // // // 
// // //
// //
//
bool SetAttackerDefenderRoles( Player const& a, Player const& b )
{
    // // // error handling 
    if( IsValid( G_GAME_INFO.TRUMP ) == false )
    {
        __COUT_FUNC_TRACE__( "Invalid game info!" );
        return false;
    }
    // // //

    std::cout << "First turn. Smallest trumps: " << std::endl;
    struct PlayerMapped2Card
    {
        PlayerMapped2Card( Player const* const p, DTA::CardType const& c )
            : player( p )
            , card( c )
        {}

        Player const* const player;
        DTA::CardType const card;
    };   // definition of 2 variables of this struct type
    
    PlayerMapped2Card left2smallestTrump( &a, a.SelectCard( GetMinTrump ) ); // left right doesnt mean nothing, just to distinguish
    PlayerMapped2Card right2smallestTrump( &b, b.SelectCard( GetMinTrump ) );

    std::cout << PlayerID2Str( left2smallestTrump.player->GetID() ) << " smallest trump: " << Card2Str( left2smallestTrump.card ) << std::endl;
    std::cout << PlayerID2Str( right2smallestTrump.player->GetID() ) << " smallest trump: " << Card2Str( right2smallestTrump.card ) << std::endl;
    
    Player const* attacker = nullptr;
    Player const* defender = nullptr;
    if( IsValid( left2smallestTrump.card ) && ( IsValid( right2smallestTrump.card ) == false ) ) // player B doesn't have trumps
{
    attacker = left2smallestTrump.player;
    defender = right2smallestTrump.player;
}
    else if( IsValid( right2smallestTrump.card ) && ( IsValid( left2smallestTrump.card ) == false ) ) // player A doesn't have trumps
{
    attacker = right2smallestTrump.player;
    defender = left2smallestTrump.player;
}
    else if( IsValid( left2smallestTrump.card ) && IsValid( right2smallestTrump.card ) ) // both have trumps, selecting one with smallest value
{
    if( left2smallestTrump.card.GetValue() < right2smallestTrump.card.GetValue() )
    {
        attacker = left2smallestTrump.player;
        defender = right2smallestTrump.player;
    }
    else
    {
        attacker = right2smallestTrump.player;
        defender = left2smallestTrump.player;
    }
}
    else // no one have trumps, detecting by smallest value of card they have ( or player B if smallest values are equal )
{
    PlayerMapped2Card left2smallestCard( &a, a.SelectCard( GetMinValueGard ) );
    PlayerMapped2Card right2smallestCard( &b, b.SelectCard( GetMinValueGard ) );
    if( left2smallestCard.card.GetValue() < right2smallestCard.card.GetValue() )
    {
        attacker = left2smallestCard.player;
        defender = right2smallestCard.player;
    }
    else
    {
        attacker = right2smallestCard.player;
        defender = left2smallestCard.player;
    }

    std::cout << PlayerID2Str( left2smallestCard.player->GetID() ) << " smallest card: " << Card2Str( left2smallestCard.card ) << std::endl;
    std::cout << PlayerID2Str( right2smallestCard.player->GetID() ) << " smallest card: " << Card2Str( right2smallestCard.card ) << std::endl;

}
    
    if( attacker == nullptr )
    {
        __COUT_FUNC_TRACE__( "Can't select first attacker!" );
        return false;
    }
        
    if( defender == nullptr )
    {
        __COUT_FUNC_TRACE__("Can't select first attacker!");
        return false;
    }
        
    G_GAME_INFO.ATTACKER_ID = attacker->GetID();
    G_GAME_INFO.DEFENDER_ID = defender->GetID();

    return true;
}

void ___ASSERT_ATT_DEF_INVARIANT___()
{
    assert( ( "ATTACKER/DEFENDER INFO CORRUPTED",
        IsValid( G_GAME_INFO.ATTACKER_ID ) && IsValid( G_GAME_INFO.DEFENDER_ID ) && G_GAME_INFO.ATTACKER_ID != G_GAME_INFO.DEFENDER_ID ) );
}

// // // // // // // // // // // // // 

// ATTACKING IMPL

DTA::CardType Defend( DTA::CardType );
void AttackImpl( DTA::CardType attacker );

void ComputerAttack()
{  
    __COUT_FUNC_TRACE__( "" );
    AttackImpl( G_GRAB_SMALLEST_CARD( *G_GET_COMPUTER() ) );
}
 
DTA::CardType ComputerLookingForInterSection( DTA::ContainerType& attackers, DTA::ContainerType& defenders )
{
    auto intersector = [&attackers, &defenders]( CT const& card )
    {
        for( CT const& c : attackers )
        {
            if( c.GetValue() == card.GetValue() )
                return true;
        }

        for( CT const& c : defenders )
        {
            if( c.GetValue() == card.GetValue() )
                return true;
        }

        return false;
    };

    auto smallestCard = G_GRAB_SMALLEST_CARD( G_GET_COMPUTER()->SelectCards( intersector ) );
    return G_GET_COMPUTER()->GrabCard( smallestCard );
}

void AttackPrivateImpl( DTA::ContainerType& attackers, DTA::ContainerType& defenders, bool init );
void AttackImpl( DTA::CardType attacker )
{
    __COUT_FUNC_TRACE__( Card2Str( attacker) );
    std::cout << "\n" << PlayerID2Str( G_GET_ATTACKER()->GetID() ) << " attacks with: " << Card2Str( attacker ) << std::endl;
    
    std::vector< CT > attackers{ attacker };
    if( CT defender = Defend( attackers.front() ); IsValid( defender ) )
    {
        std::vector< CT > defenders;
        defenders.push_back( defender );
        AttackPrivateImpl( attackers, defenders, true );
    }
    else
    {
        G__ENDTURN( false );
    }
}

void AttackPrivateImpl( DTA::ContainerType& attackers, DTA::ContainerType& defenders, bool init )
{
    std::string msg = std::string( " init = " ) += std::string( init ? "true" : "false" );
    __COUT_FUNC_TRACE__( msg );
    std::cout << "\t\tattakers:" << std::endl;
    CoutPreDeckPost( "\t\t ATTACKER: ", attackers, "\n" );
    std::cout << "\n\t\tdefenders:" << std::endl;
    CoutPreDeckPost( "\t\t DEFENDER: ", defenders, "\n" );
    std::cout << std::endl;

    if( init )
        __ASSERT_MSG__( attackers.size() == defenders.size(),
        "AttackPrivateImpl INIT:: DIFFERENT ATTACKERS AND DEFENDERS SIZES" );

    if( init )
    __ASSERT_MSG__( attackers.size() == 1,
        "AttackPrivateImpl INIT:: SOMETHING WRONG WITH SIZES, SIZES SHOULBE BE 1" );

    //   //   //   //   //   //

    // looking for intersection branch
    if( attackers.size() == defenders.size() )
    {
        auto intersect = ComputerLookingForInterSection( attackers, defenders );
        if( IsValid( intersect ) )
        {
            attackers.emplace_back( std::move( intersect ) );
            AttackPrivateImpl( attackers, defenders, false );
        }
        else
        {
            __ASSERT_MSG__( attackers.size() == defenders.size(),
                "AttackPrivateImpl ADDING TO THE WASTE:: DIFFERENT ATTACKERS AND DEFENDERS SIZES" );

            for( std::size_t i = 0u; i < attackers.size(); ++i )
            {
                G_GAME_INFO.WASTE.emplace_back( attackers[ i ], defenders[ i ] );
            }
            attackers.clear();
            defenders.clear();

            G__ENDTURN( true ); // Defender won
        }          
    }
    // looking for defender branch
    else if( attackers.size() - defenders.size() == 1 )
    {       
        if( auto defender = Defend( attackers.back() ); IsValid( defender ) )
        {
            defenders.push_back( defender );
            AttackPrivateImpl( attackers, defenders, false );
        }
        else
        {
            G__ENDTURN( false ); // Attacker won
        }
    }
    else
    {
        __ASSERT_MSG__( false,
            "AttackPrivateImpl SOMETHING WRONG WITH SIZES, SIZES SHOULD DIFFER IN 1" );
    }
}

void HumanAttack()
{
    __COUT_FUNC_TRACE__( "" );

    std::cout << "\nHuman can attack with these cards:  " << std::endl;

    auto& human = *G_GET_HUMAN();
    CoutPlayerHand( human, true );

    std::size_t number = human.HandSize();
    std::cin >> number;
    while( !std::cin || number >= human.HandSize() )
    {
        std::cout << "Wronk!!" << std::endl;
        std::cout << "Select number between 0 and " << human.HandSize() << std::endl;
    }

    // // // // // // // //

    AttackImpl( human.GrabCard( human[ number ] ) ) ;
}

void Attack()
{
    __COUT_FUNC_TRACE__("");

    if( G_GET_ATTACKER() == G_GET_HUMAN() )
    {
        HumanAttack();
    }
    else
    {
        ComputerAttack();
    }
}

// // // // // // // // // // // // // 

// DEFENDING IMPL
template< typename Functor >
DTA::CardType GetDefender( DTA::CardType attacker, Functor&& choicer )
{
    auto& defenderPlayer = G_GAME_INFO.PLAYERS[ G_GAME_INFO.DEFENDER_ID ];
    auto canBeatAttacker = [&attacker]( DTA::CardType card )
    {
        return CanAbsBeatMe( attacker, card );
    };

    auto defenderCandidates = defenderPlayer.SelectCards( canBeatAttacker );
    if( defenderCandidates.size() )
    {
        std::cout << "\n\t" << PlayerID2Str( defenderPlayer.GetID() ) << " can use selected cards for defense: " << std::endl;
        CoutDeckNumered( defenderCandidates );

        auto defenderCard = choicer( defenderCandidates );
        return defenderPlayer.GrabCard( defenderCard );
    }

    return G_INVALID_CARD();
}

DTA::CardType ComputerDefend( DTA::CardType attacker )
{
    __ASSERT_MSG__( G_GET_ATTACKER() == G_GET_HUMAN() && G_GET_DEFENDER() == G_GET_COMPUTER(), 
        "ComputerDefend:: ATTACKER / DEFENDER MISMATCH " );

    auto computerChoicer = []( DTA::ContainerType const& candidates )
    {
        auto smallest = GetAbsMinCard( candidates );
        std::cout << "Computer defends with this card: " << Card2Str( smallest ) << std::endl;

        return smallest;
    };

    return GetDefender( attacker, computerChoicer );
}

DTA::CardType HumanDefend( DTA::CardType attacker )
{
    __ASSERT_MSG__( G_GET_ATTACKER() == G_GET_COMPUTER() && G_GET_DEFENDER() == G_GET_HUMAN(), 
        "HumanDefend:: ATTACKER / DEFENDER MISMATCH " );

    auto humanChoicer = []( DTA::ContainerType const& candidates )
    {
        std::size_t number = candidates.size();
        std::cin >> number;
        while( !std::cin || number >= candidates.size() )
        {
            std::cout << "Wronk!!" << std::endl;
            CoutDeckNumered( candidates );
            std::cout << "Enter a number between 0 and " << candidates.size() - 1 << std::endl;

            std::cin >> number;
        }

        return candidates[ number ];
    };

    return GetDefender( attacker, humanChoicer );
}

DTA::CardType Defend( DTA::CardType attacker )
{
    ___ASSERT_ATT_DEF_INVARIANT___();
    __COUT_FUNC_TRACE__( "" );

    if ( G_GET_ATTACKER() == G_GET_HUMAN() )
    {
        return ComputerDefend( attacker );
    }
    else
    {
        return HumanDefend( attacker );
    }

    return G_INVALID_CARD();
}

// // // // // // // // // // // // // 

void MakeTurn()
{
    ___ASSERT_ATT_DEF_INVARIANT___();
    if( G_GET_ATTACKER() )
    {
        std::string const msg = "Attacker is: " + PlayerID2Str( G_GET_ATTACKER()->GetID() );
        __COUT_FUNC_TRACE__( msg );
        Attack();
    }
    else
    {
        __COUT_FUNC_TRACE__( "Can't attack!" );
    }
}

// // // // // // // // // // // // // 

bool G__INITIALIZATION()
{
    G_GAME_INFO.WASTE.clear();

    std::cout << "We've created G_GAME_INFO. G_GAME_INFO.PLAYERS inited" << std::endl;
    auto& deck = G_GAME_INFO.DECK;

    std::cout << "We've created a deck: " << std::endl;
    PackDeck( deck, std::vector< Value >{
        Value::Six,
            Value::Seven,
            Value::Eight,
            Value::Ten,
            Value::Jack,
            Value::Queen,
            Value::King,
            Value::Ace
    },
        std::vector< Suit >{
            Suit::Hearts,
                Suit::Spades,
                Suit::Dimonds,
                Suit::Clubs
        } );


    CoutDeck( deck, true );

    std::cout << "We've shuffled the deck: " << std::endl;
    ShuffleDeck( deck );
    CoutDeck( deck );

    std::cout << "We've created players and inited human player: " << std::endl;
    Player& human = *G_GET_HUMAN();
    human.SetHand( GetHand( deck ) );
    CoutPlayerHand( human );
    //std::cout << "Deck after creating human player: " << std::endl;
    //CoutDeck( deck );

    std::cout << "We've inited computer player: " << std::endl;
    Player& computer = *G_GET_COMPUTER();
    computer.SetHand( GetHand( deck ) );
    CoutPlayerHand( computer );
    //std::cout << "Deck after creating computer player: " << std::endl;
    //CoutDeck( deck );

    //if( deck.empty() )
    //{
    //    std::cout << "Error, deck is empty!" << std::endl;
    //    return 1;
    //}

    std::cout << "We've detected trump suit: ";
    G_GAME_INFO.TRUMP = deck.front().GetSuit();
    std::cout << Suit2Str( G_GAME_INFO.TRUMP ) << std::endl << std::endl;

    return SetAttackerDefenderRoles( human, computer );
}

void G__ENDTURN( bool swap )
{
    std::cout << "Next turn" << std::endl;

    std::vector< Player* > needCards{ G_GET_ATTACKER() };

    if( swap )
    {
        needCards.push_back( G_GET_DEFENDER() );
        std::swap( G_GAME_INFO.ATTACKER_ID, G_GAME_INFO.DEFENDER_ID );
    }

    for( auto player : needCards )
    {
        auto& deck = G_GAME_INFO.DECK;
        while( deck.size() && player->HandSize() < C_DEFAULT_HANDSIZE )
        {
            auto card = deck.back(); deck.pop_back();
            player->AddCard( card );
        }
    }
    
    __ASSERT_MSG__( G_GET_HUMAN()->HandSize() || G_GET_COMPUTER()->HandSize() , "EMPTY HANDS BOTH PLAYERS" );

    for( auto& p : G_GAME_INFO.PLAYERS )
    {
        std::cout << PlayerID2Str( p.first ) << " hand after tossing: " << std::endl;
        CoutPlayerHand( p.second, true );
        std::cout << std::endl;
    }

    if( 0 == G_GET_HUMAN()->HandSize() )
    {
        std::cout << "Human won!" << std::endl;
    }
    else if( 0 == G_GET_COMPUTER()->HandSize() )
    {
        std::cout << "Computer won!" << std::endl;
    }
    else
    {
        Attack();
    }
 
}

int main()
{    
    if( G__INITIALIZATION() )
    {
        std::cout << "--- INIT --- PHASE --- COMPLETE !!! ---" << std::endl;
    }
    else
    {
        std::cout << "--- !!! ABORTED --- INIT --- PHASE --- COMPLETE ---" << std::endl;
        return 1;
    }

    std::cout << "We've made turn!" << std::endl;
    MakeTurn();  

    // std::cout << "Human's smallest card in a hand: " << Card2Str( human.SelectCard( GetAbsSmallestCard ) ) << std::endl;
    // std::cout << "Computers's smallest card a hand: " << Card2Str( computer.SelectCard( GetAbsSmallestCard )  ) << std::endl;

    return 0;
}