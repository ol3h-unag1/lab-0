#include <iostream>

#include <string>
#include <vector>

#include <map>

#include <utility>
#include <functional>

#include <algorithm>
#include <random>

#include <cassert>

#include "my_setup.hpp"

// // // // // // // // // // // // // 

std::size_t const C_DEFAULT_HANDSIZE = 6;

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

template< template < typename > typename Container, typename Card >
struct DeckType
{
public:
    using ContainerType = Container< Card >;
    using CardType = Card;

    using Type = DeckType< Container, Card >;
};

using DTA = DeckType< std::vector, Card< Value, Suit > >;

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
     // std::cout << "CoutDeck" << " " << d.size() << std::endl;
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
    void CoutPlayerHand( Player const&  );
    
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

    void RemoveCard( DTA::CardType card )
    {
        RemoveCardImpl( card );
    }

    void SetHand( DTA::ContainerType&& hand )
    {
        hand_ = std::move( hand );
    }

    std::size_t HandSize() const { return hand_.size(); }
    
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

const char* PlayerID2Str( Player::ID pk )
{
    switch( pk )
    {
    case Player::ID::Human: return "Human";
    case Player::ID::Computer:  return "Computer";
    
    default:
        return nullptr;
    }
    return nullptr;
}

void CoutPlayerHand( Player const& p )
{
    CoutDeck( p.hand_, false );
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

DTA::CardType GetAbsSmallestCard( DTA::ContainerType const& ); 
DTA::CardType G_GRAB_COMPUTER_SMALLEST_CARD() 
{ 
    auto card = G_GET_COMPUTER()->SelectCard( GetAbsSmallestCard );
    G_GET_COMPUTER()->RemoveCard( card );
    return card;
}

void Attack();
void G_TURN_TRANSITION()
{
    std::swap( G_GAME_INFO.ATTACKER_ID, G_GAME_INFO.DEFENDER_ID );
    Attack();
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

DTA::CardType GetSmallestValueGard( DTA::ContainerType const& d )
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

DTA::CardType GetSmallestTrump( DTA::ContainerType const& d )
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

DTA::CardType GetAbsSmallestCard( DTA::ContainerType const& d )
{
    static DTA::CardType const invalid{ Value::Invalid, Suit::Invalid };
    DTA::CardType const* smallest = &invalid;

    for( auto const& card : d )
    {
        if( IsValid( *smallest ) == false )
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

// // // // // // // // // // // // // 
// // //
// //
//
void SetAttackerDefenderRoles( Player const& a, Player const& b )
{
    // // // error handling 
    if( IsValid( G_GAME_INFO.TRUMP ) == false )
    {
        std::cout << "Invalid game info!";
        return;
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
    
    PlayerMapped2Card left2smallestTrump( &a, a.SelectCard( GetSmallestTrump ) ); // left right doesnt mean nothing, just to distinguish
    PlayerMapped2Card right2smallestTrump( &b, b.SelectCard( GetSmallestTrump ) );

    std::cout << PlayerID2Str( left2smallestTrump.player->GetID() ) << " smallest trump: " << Card2Str( left2smallestTrump.card ) << std::endl;
    std::cout << PlayerID2Str( right2smallestTrump.player->GetID() )  << " smallest trump: " << Card2Str( right2smallestTrump.card ) << std::endl;

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
    else if ( IsValid( left2smallestTrump.card ) && IsValid( right2smallestTrump.card ) ) // both have trumps, selecting one with smallest value
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
        PlayerMapped2Card left2smallestCard( &a, a.SelectCard( GetSmallestValueGard ) );
        PlayerMapped2Card right2smallestCard( &b, b.SelectCard( GetSmallestValueGard ) );
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
        std::cout << "Can't select first attacker!" << std::endl;
        return;
    }

    if( defender == nullptr )
    {
        std::cout << "Can't select first attacker!" << std::endl;
        return;
    }

    G_GAME_INFO.ATTACKER_ID = attacker->GetID();
    G_GAME_INFO.DEFENDER_ID = defender->GetID();
}

void ___ASSERT_ATT_DEF_INVARIANT___()
{
    assert( ( "ATTACKER/DEFENDER INFO CORRUPTED",
        IsValid( G_GAME_INFO.ATTACKER_ID ) && IsValid( G_GAME_INFO.DEFENDER_ID ) && G_GAME_INFO.ATTACKER_ID  != G_GAME_INFO.DEFENDER_ID ) );
}

// // // // // // // // // // // // // 
DTA::CardType Defend( DTA::CardType );

void ComputerAttack( DTA::CardType attacker )
{
    std::cout << "Computer attacks with: " << Card2Str( attacker ) << std::endl;

    struct BEATEN
    {
        DTA::CardType ATTACKER;
        DTA::CardType DEFENDER;
    };
    std::vector< BEATEN > beaten;

    // // // // // // // // / // // // // 

    auto defeater = Defend( attacker );
    if( IsValid( defeater ) == false ) // can't defeat attacker
    {
        auto& deck = G_GAME_INFO.DECK;
        if( deck.empty() )
        {
            std::cout << "Computer won!" << std::endl;
            return;
        }

        while( deck.size() && G_GET_COMPUTER()->HandSize() < C_DEFAULT_HANDSIZE )
        {
            G_GET_COMPUTER()->AddCard( deck.back() );
            deck.pop_back();
        }

        ComputerAttack( G_GRAB_COMPUTER_SMALLEST_CARD() );
    }
    else // trying to find matching cards in computer hand to proceed attack
    {
        beaten.push_back( { attacker, defeater } );
        const auto intersection = [&beaten]( DTA::CardType card )
        {
            for( auto& ad : beaten )
            {
                if( ad.ATTACKER.GetValue() == card.GetValue() || 
                    ad.DEFENDER.GetValue() == card.GetValue() )
                {
                    return true;
                }
            }

            return false;
        };

        if( auto similarForPlanting = G_GET_COMPUTER()->SelectCards( intersection ); similarForPlanting.empty() == false )
        {
            for( auto const& card : similarForPlanting )
            {
                auto defendResult = Defend( card );
                while( IsValid( defendResult) )
                {
					// we need to find conditions to be able to planlt cards if defender can't beat em
					break;
                };
            }
        }
    }

    // // // // // // // // / // // // // 
    std::cout << "All cards beated! End of the turn!" << std::endl;

    G_TURN_TRANSITION();

    // // // // // // // // / // // // // 
}

void HumanAttack()
{
    std::cout << "Human attacks! " << std::endl;
    // Defend( card );
}

void Attack()
{
    ___ASSERT_ATT_DEF_INVARIANT___();
    switch( std::addressof(G_GAME_INFO.PLAYERS[ G_GAME_INFO.ATTACKER_ID ])->GetID() )
    {
    case Player::ID::Human: 
        HumanAttack();
        break;

    case Player::ID::Computer: 
        ComputerAttack( G_GRAB_COMPUTER_SMALLEST_CARD() );
        break;
    }
}

// // // // // // // // // // // // // 

DTA::CardType ComputerDefend( DTA::CardType card )
{
    std::cout << "\tComputer defends from: " << Card2Str( card ) << "(WiP)" << std::endl;
    return G_INVALID_CARD();
}

DTA::CardType HumanDefend( DTA::CardType attacker )
{
    std::cout << "\n\tHuman defends from: " << Card2Str( attacker ) << std::endl;
    Player& human = *G_GET_HUMAN();
    const auto selector = [attacker]( DTA::CardType const& c )
    {
        if( c.GetSuit() == attacker.GetSuit() )
        {
            return c.GetValue() > attacker.GetValue();
        }

        if( c.GetSuit() == G_GET_TRUMP() )
        {
            return true;
        }

        return false;
    };

    std::cout << "\tHuman hand: " << std::endl;
    auto preTab = []( std::ostream& os ) -> std::ostream&
    {
        return os << "\t";
    };
    CoutDeck( human.SelectCards( []( DTA::CardType const& ) { return true; } ), false, preTab );

    if( auto defenderCandidats = human.SelectCards( selector ); defenderCandidats.empty() == false )
    {
        std::cout << "\tHuman can beat attacker with following card(s):" << std::endl;
        std::size_t counter = 0u;
        auto numOut = [&counter]( std::ostream& os ) -> std::ostream&
        {
            return os << " [" << counter++ << "]";
        };

        CoutDeck( defenderCandidats, false, preTab, numOut );

        std::size_t total = counter - 1;
        std::size_t number = total + 1;
        while( number > total )
        {
            std::cout << "Enter number between[ 0 : " << total << " ]" << std::endl;
            std::cin >> number;
        }

        assert( ("SELECTED CARD INDEX IS BIGGER THEN DEFENDERS VECTOR SIZE", number < defenderCandidats.size()) );
        auto defender = defenderCandidats[ number ];
        human.RemoveCard( defender );

        std::cout << "Human beated " << Card2Str( attacker ) << " with his " << Card2Str( defender ) << std::endl;

        return defender;
    }
    else
    {
        std::cout << "\tHuman can't beat attacker! " << Card2Str( attacker ) << " added to Human hand." << std::endl;
        G_GET_HUMAN()->AddCard( attacker );
        return G_INVALID_CARD();
    }

    return G_INVALID_CARD();
}

DTA::CardType Defend( DTA::CardType attacker )
{
    ___ASSERT_ATT_DEF_INVARIANT___();

    switch( std::addressof( G_GAME_INFO.PLAYERS[ G_GAME_INFO.DEFENDER_ID ] )->GetID() )
    {
    case Player::ID::Human: return HumanDefend( attacker );
    case Player::ID::Computer: return ComputerDefend( attacker );
    }

    return G_INVALID_CARD();
}

// // // // // // // // // // // // // 

void MakeFirstTurn( Player const& a, Player const& b )
{
    // // // error handling 
    if( IsValid( a ) == false )
    {
        std::cout << "Invalid first player-A";
        return;
    }
    if( IsValid( b ) == false )
    {
        std::cout << "Invalid second player-B";
        return;
    }
    // // //

    SetAttackerDefenderRoles( a, b );
    if( auto* attacker = std::addressof( G_GAME_INFO.PLAYERS[ G_GAME_INFO.ATTACKER_ID ] ) )
    {
        std::cout << "Attacker is: " << PlayerID2Str( attacker->GetID() ) << std::endl;
        Attack();
    }
    else
    {
        std::cout << "Can't attack!" << std::endl;
    }
}

// // // // // // // // // // // // // 

int main()
{
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


    CoutDeck( deck, true);

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

    std::cout << "We've made turn!" << std::endl;
    MakeFirstTurn( human, computer );
    

    // std::cout << "Human's smallest card in a hand: " << Card2Str( human.SelectCard( GetAbsSmallestCard ) ) << std::endl;
    // std::cout << "Computers's smallest card a hand: " << Card2Str( computer.SelectCard( GetAbsSmallestCard )  ) << std::endl;

    return 0;
}