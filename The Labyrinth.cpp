#include <iostream>
#include <vector>
#include <list>
#include <unordered_map>

/**This program takes a constant input of a map of a maze
 * and Kirk's current location. It then gives an output for navagating
 * the maze to the control room and back to the origin.
 **/

struct Coordinate
{
    size_t y, x;
        
    inline Coordinate( const size_t &input_y, const size_t &input_x )
    {
        y = input_y;
        x = input_x;
    }
};

std::unordered_map<std::string,const Coordinate*> orientation = { { "DOWN", new Coordinate( 1, 0 ) },   // These directions are used through out the program
                                                                { "RIGHT", new Coordinate( 0, 1 ) },    // This makes them into a iterable sequence
                                                                { "UP", new Coordinate( -1, 0 ) },
                                                                { "LEFT", new Coordinate( 0, -1 ) } };

std::list<Coordinate> distance_set( std::vector<std::vector<int>> &dist_map, const size_t &location_Y, const size_t &location_X,
        const size_t &target_Y, const size_t &target_X, const size_t avoid_Y = NULL, const size_t avoid_X = NULL, const bool exploring = false )
{ // This method returns a list of coordinates that are a path to a destination
    { // this block skips the first and last of the vectors
        std::vector<std::vector<int>>::iterator High_iterator = dist_map.begin(), High_end = dist_map.end();
        std::vector<int>::iterator Low_iterator, Low_end;
        --High_end;
    
        for( ++High_iterator; High_iterator != High_end; ++High_iterator )
        {
            Low_iterator = High_iterator->begin(), Low_end = High_iterator->end();
            --Low_end;
            
            for( ++Low_iterator; Low_iterator != Low_end; ++Low_iterator )
            {
                if( *Low_iterator > 1 )
                {
                    *Low_iterator = 1; // taversible locations set to a default
                }
            }
        }
    }
    
    dist_map[ location_Y ][ location_X ] = 2; // coordinates to go to
    std::list<Coordinate> pathfinding_queue { Coordinate( location_Y, location_X ) };

    for( std::list<Coordinate>::iterator queue_iterator = pathfinding_queue.begin() ; queue_iterator != pathfinding_queue.end(); ++queue_iterator )
    { // populate the queue to make the distance map
        for( const std::pair<std::string,const Coordinate*> &direction_modifier: orientation )
        {
            if( !( queue_iterator->y + direction_modifier.second->y == avoid_Y && queue_iterator->x + direction_modifier.second->x == avoid_X ) )
            {
                if( dist_map[ queue_iterator->y + direction_modifier.second->y ][ queue_iterator->x + direction_modifier.second->x ] == 1 )
                { // if location has not been set
                    dist_map[ queue_iterator->y + direction_modifier.second->y ][ queue_iterator->x + direction_modifier.second->x ]
                                = 1 + dist_map[ queue_iterator->y ][ queue_iterator->x ]; // assign distance
                
                    if( queue_iterator->y + direction_modifier.second->y == target_Y && queue_iterator->x + direction_modifier.second->x == target_X )
                    {  // stop looking and set up the path to follow
                        std::list<Coordinate>::iterator return_iterator = queue_iterator;
                        
                        for( ++queue_iterator; queue_iterator != pathfinding_queue.end(); queue_iterator = pathfinding_queue.erase( queue_iterator ) );
                        // clear everything after the queue_iterator

                        queue_iterator = return_iterator;
                        
                        for( --queue_iterator; queue_iterator != pathfinding_queue.begin(); --queue_iterator )
                        { // remove coordinates that are not along the path
                            int difference_y = return_iterator->y - queue_iterator->y,
                                difference_x = return_iterator->x - queue_iterator->x;
                            
                            if( !( dist_map[ return_iterator->y ][ return_iterator->x ] - dist_map[ queue_iterator->y ][ queue_iterator->x ] == 1 &&
                                ( !difference_y && ( difference_x == -1 || difference_x == 1 ) ) || ( !difference_x && ( difference_y == -1 || difference_y == 1 ) ) ) )
                            { // if the next coordinates isnt closer or adjacent to the current coordinates
                                return_iterator = queue_iterator = pathfinding_queue.erase( queue_iterator );
                            }
                            else
                            {
                                --return_iterator;
                            }
                        }
                                
                        return pathfinding_queue; // queue now only contains the path to follow
                    }
                
                    pathfinding_queue.emplace_back( Coordinate( queue_iterator->y + direction_modifier.second->y,
                                                            queue_iterator->x + direction_modifier.second->x ) ); // add to queue
                }
                else if( exploring && dist_map[ queue_iterator->y + direction_modifier.second->y ][ queue_iterator->x + direction_modifier.second->x ] == 0 )
                { // if location is not explored
                    return distance_set( dist_map, queue_iterator->y, queue_iterator->x, location_Y, location_X, avoid_Y, avoid_X ); // plots a path to the new target
                }
            }
        }
    }
    
    if( exploring ) // shouldn't ever reach this command if exploring
    {
        std::cerr << "No Path" << std::endl;
    }
    
    pathfinding_queue.clear();
    return pathfinding_queue;
}

main()
{
    size_t alarm_duration,  // number of rounds allowed to excape after the alarm countdown is activated
        kirk_X, kirk_Y,     // row and column where Kirk is located
        height, width,      // number of rows and columns
        target_X = NULL, target_Y = NULL;   // location of the control room

    std::cin >> height >> width >> alarm_duration; std::cin.ignore();
    std::cin >> kirk_Y >> kirk_X; std::cin.ignore(); // put statement here to initialize end_Y and end_X later outside of game loop
    
    std::string path, area_map[ height ];
    std::vector<std::vector<int>> distance_map ( height, std::vector<int> ( width ) );
    std::vector<int> range_y = { -2, -1, 0, 1, 2 },
                    range_x = { -2, -1, 0, 1, 2 }; // used occasionally
    
    int end_X = kirk_X, end_Y = kirk_Y; // where Kirk started off at
    std::list<Coordinate> plan; // a determined path to follow
    bool alarm = false; // if the alarm has been triggered
    
    for( size_t index, fuel = 1200; fuel > 0; --fuel ) // probably dont need to know the fuel
    {
        index = 0;
            
        for( std::string &line: area_map )
        { // update map
            std::cin >> line; std::cin.ignore();
            
            std::cerr << line << " " << index << std::endl; // graphical user output
                
            ++index;
        }
        
        for( index = 0; index < width; ++index )
        {
            std::cerr << index % 10; // helps with reading the map
        }
    
        std::cerr << std::endl;
            
        if( fuel < 1200 )
        { // only check the edges of Kirk's vision        
            if( orientation[ path ]->x )
            {
                range_y = { -2, -1, 0, 1, 2 };
                range_x = { 2 * orientation[ path ]->x };
            }
            else
            {
                range_y = { 2 * orientation[ path ]->y };
                range_x = { -2, -1, 0, 1, 2 };
            }
        }
    
        for( const int &modifier_x: range_x )
        {
            if( kirk_X + modifier_x < width && kirk_X + modifier_x >= 0 )
            {
                for( const int &modifier_y: range_y )
                {
                    if( kirk_Y + modifier_y < height && kirk_Y + modifier_y >= 0 && !distance_map[ kirk_Y + modifier_y ][ kirk_X + modifier_x ] )
                    { // if not evaluated yet
                        if( ( ( std::string ) "CT." ).find( area_map[ kirk_Y + modifier_y ][ kirk_X + modifier_x ] ) == std::string::npos )
                        {
                            distance_map[ kirk_Y + modifier_y ][ kirk_X + modifier_x ] = -1;
                        }
                        else
                        {
                            distance_map[ kirk_Y + modifier_y ][ kirk_X + modifier_x ] = 1;
                
                            if( area_map[ kirk_Y + modifier_y ][ kirk_X + modifier_x ] == 'C' )
                            { // if is the control room
                                target_X = kirk_X + modifier_x;
                                target_Y = kirk_Y + modifier_y;
                            }
                        }
                    }
                }
            }
        }
        
        path = "NULL";
        
        if( kirk_X == target_X && kirk_Y == target_Y )
        { // arrived at control room
            alarm = true;
        }
        
        if( target_Y && !alarm && plan.empty() && !distance_set( distance_map, target_Y, target_X, end_Y, end_X ).empty()
            && distance_map[ end_Y ][ end_X ] - 2 <= alarm_duration )
        { // found the control room, the end is reachable from the control room, and the distance is traversable during the alarm_duration
            plan = distance_set( distance_map, target_Y, target_X, kirk_Y, kirk_X );
        }
        
        if( plan.empty() )
        { // even if distance_map is populated, dont follow it
            if( alarm )
            { // alarm just rang, needs a path to the exit
                plan = distance_set( distance_map, end_Y, end_X, kirk_Y, kirk_X ); // plots a path to the end
                
                if( distance_map[ kirk_Y ][ kirk_X ] < 2 )
                { // error check for triggering the alarm without a valid path, probably impossible
                    std::cerr << "No Exit Path" << std::endl;
                }
                else if( distance_map[ kirk_Y ][ kirk_X ] - 2 > alarm_duration )
                { // error check for triggering the alarm without a short enough path
                    std::cerr << "Wrong Trigger" << std::endl;
                }
            }
            else
            { // revieling the map
                size_t revieled_count = 0; // number of revieled tiles
                
                for( const std::pair<std::string,const Coordinate*> &direction_modifier: orientation )
                {
                    if( distance_map[ kirk_Y + direction_modifier.second->y ][ kirk_X + direction_modifier.second->x ] > 0 &&
                        !( kirk_Y + direction_modifier.second->y == target_Y && kirk_X + direction_modifier.second->x == target_X )  )
                    { // if that direction is traversable
                        if( direction_modifier.second->x )
                        {
                            range_y = { -2, -1, 0, 1, 2 };
                            range_x = { 3 * direction_modifier.second->x };
                        }
                        else
                        {
                            range_y = { 3 * direction_modifier.second->y };
                            range_x = { -2, -1, 0, 1, 2 };
                        }
                    
                        int tile_count = 0; // number of revieled tiles in this direction
                
                        for( const int &search_x: range_x )
                        {
                            if( kirk_X + search_x < width && kirk_X + search_x >= 0 )
                            {
                                for( const int &search_y: range_y )
                                {
                                    if( kirk_Y + search_y < height && kirk_Y + search_y >= 0 && !distance_map[ kirk_Y + search_y ][ kirk_X + search_x ] )
                                    { // if within bounds and is unexplored
                                        ++tile_count;
                                    }
                                }
                            }
                        }
                
                        if( revieled_count < tile_count )
                        { // take the most unexplored and go that direction
                            revieled_count = tile_count;
                            path = direction_modifier.first;
                        }
                    }
                }
                
                if( path == "NULL" )
                { // no immediate direction to go to reviel the map, must map out a new destination
                    plan = distance_set( distance_map, kirk_Y, kirk_X, NULL, NULL, target_Y, target_X, true ); // explore uncharted locations
                }
            }
        }
        
        if( !plan.empty() )
        { // follow the path
            for( const std::pair<std::string,const Coordinate*> &direction_modifier: orientation )
            {
                if( kirk_Y + direction_modifier.second->y == plan.back().y && kirk_X + direction_modifier.second->x == plan.back().x )
                {
                    path = direction_modifier.first;
                    plan.pop_back();
                    break;
                }
            }
        }
        
        std::cout << path << std::endl; // Kirk's next move (UP DOWN LEFT or RIGHT).
        
        std::cin >> kirk_Y >> kirk_X; std::cin.ignore(); // update Kirk's location
        
        if( alarm && kirk_Y == end_Y && kirk_X == end_X )
        {
            break; // finished excape sequence
        }
    }
}