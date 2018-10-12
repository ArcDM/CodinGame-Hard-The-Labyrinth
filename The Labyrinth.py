import sys

"""
    This program takes a constant input of a map of a maze
    and Kirk's current location. It then gives an output for
    navagating the maze to the control room and back to the origin.
"""

def distance_set( input_Y, input_X, target, avoid = None, exploring = False ):
# This method returns a list of coordinates that are a path to a destination
    for y in range( 1, height - 1 ):
        for x in range( 1, width - 1 ):
            if distance_map[ y ][ x ] > 0:
                distance_map[ y ][ x ] = 1 # default distance
    
    distance_map[ input_Y ][ input_X ] = 2 # coordinates to go to
    pathfinding_queue = [ ( input_Y, input_X ) ]

    for location_y, location_x in pathfinding_queue: # populate the queue to make the distance map
        for modifier_y, modifier_x in ( ( 1, 0 ), ( 0, 1 ), ( -1, 0 ), ( 0, -1 ) ):
            if ( location_y + modifier_y, location_x + modifier_x ) != avoid:
                if distance_map[ location_y + modifier_y ][ location_x + modifier_x ] == 1: # if location has not been set
                    distance_map[ location_y + modifier_y ][ location_x + modifier_x ] = 1 + distance_map[ location_y ][ location_x ]
                    
                    if ( location_y + modifier_y, location_x + modifier_x ) == target: # stop looking and set up the path to follow
                        pathfinding_index = pathfinding_queue.index( ( location_y, location_x ) )
                        pathfinding_queue = pathfinding_queue[ : pathfinding_index + 1 ]
                        
                        while pathfinding_index > 1:
                            if tuple( high - low for high, low in zip( pathfinding_queue[ pathfinding_index ], pathfinding_queue[ pathfinding_index - 1 ] ) ) not in ( ( 1, 0 ), ( 0, 1 ), ( -1, 0 ), ( 0, -1 ) ) :
                                pathfinding_queue.pop( pathfinding_index - 1 )
                            
                            pathfinding_index -= 1
                            
                        return pathfinding_queue # queue now only contains the path to follow
                    
                    pathfinding_queue.append( ( location_y + modifier_y, location_x + modifier_x ) ) # add to queue
                elif exploring and not distance_map[ location_y + modifier_y ][ location_x + modifier_x ]: # if location is not explored
                        return distance_set( location_y, location_x, ( input_Y, input_X ), avoid ) # plots a path to the current target avoiding the control room
                    
    if exploring:
        print( "No Path", file = sys.stderr ) # shouldn't ever reach this command
    
    return None
        
height, width, alarm_duration = ( int( input_data ) for input_data in input().split() )
kirk_Y, kirk_X = ( int( input_data ) for input_data in input().split() ) # row and column where Kirk is located

beginning_Y, beginning_X = ( kirk_Y, kirk_X )  # where kirk started off at
target = None # location of the control room
path = None
plan = None
alarm = False # if the alarm has been triggered
directions = { "DOWN": ( 1, 0 ), "RIGHT": ( 0, 1 ), "UP": ( -1, 0 ), "LEFT": ( 0, -1 ) } #{direction: (y, x)}

distance_map = [ [ 0 ] * width for count in range( height ) ]

for fuel in reversed( range( 1200 ) ): # probably dont need to know the fuel
    area_map = [ input() for count in range( height ) ] # map input
    
    print( '\n'.join( line + " " + str( count ) for count, line in enumerate( area_map ) ), file = sys.stderr ) # graphical user output
    print( *( number%10 for number in range( width ) ), sep = '', file = sys.stderr ) # helps with reading the map
    
    if fuel == 1199: # only executes the first round, needed a map input
        evaluate_y, evaluate_x = [ -2, -1, 0, 1, 2 ], [ -2, -1, 0, 1, 2 ] # check Kirk's full range of vision
    else: # find the control room, checking only the edges of Kirk's vision
        modifier_y, modifier_x = directions[ path ]
        
        if modifier_x:
            evaluate_y, evaluate_x = [ -2, -1, 0, 1, 2 ], [ 2 * modifier_x ]
        else:
            evaluate_y, evaluate_x = [ 2 * modifier_y ], [ -2, -1, 0, 1, 2 ]

    for modifier_x in filter( lambda x: kirk_X + x < width and kirk_X + x >= 0, evaluate_x ): # if modifier is in bounds
        for modifier_y in filter( lambda y: kirk_Y + y < height and kirk_Y + y >= 0, evaluate_y ): # if modifier is in bounds
            if not distance_map[ kirk_Y + modifier_y ][ kirk_X + modifier_x ]: # if not evaluated yet
                if area_map[ kirk_Y + modifier_y ][ kirk_X + modifier_x ] in 'CT.':
                    distance_map[ kirk_Y + modifier_y ][ kirk_X + modifier_x ] = 1
                    
                    if area_map[ kirk_Y + modifier_y ][ kirk_X + modifier_x ] is 'C': # if is the control room
                        target = ( kirk_Y + modifier_y, kirk_X + modifier_x )
                else:
                    distance_map[ kirk_Y + modifier_y ][ kirk_X + modifier_x ] = -1
    
    path = None # reseting directional path
    
    if ( kirk_Y, kirk_X ) == target: # arrived at control room
        alarm = True
    
    if target and not alarm and not plan and distance_set( *target, ( beginning_Y, beginning_X ) ) and distance_map[ beginning_Y ][ beginning_X ] - 2 <= alarm_duration: # assess if finishing is possible
        plan = distance_set( *target, ( kirk_Y, kirk_X ) ) # if the end is reachable from the control room and the distance is traversable during the alarm_duration
    
    if not plan: # even if distance is set, dont follow it
        if alarm: # alarm just rang, needs a path to the exit
            plan = distance_set( beginning_Y, beginning_X, ( kirk_Y, kirk_X ) ) # plots a path to the end

            if distance_map[ kirk_Y ][ kirk_X ] <= 1: # error check for triggering the alarm without a valid path, probably impossible
                print( "No Exit Path", file = sys.stderr )
            
            elif distance_map[ kirk_Y ][ kirk_X ] - 2 > alarm_duration: # error check for triggering the alarm without a short enough path
                print( "Wrong Trigger", file = sys.stderr )
        
        else: # still revieling the map
            revieled_count = 0 # number of revieled tiles
            
            for orientation, ( modifier_y, modifier_x ) in directions.items():
                if ( kirk_Y + modifier_y , kirk_X + modifier_x ) != target and distance_map[ kirk_Y + modifier_y ][ kirk_X + modifier_x ] > 0: # if direction is traversable
                    if modifier_x:
                        range_y = ( -2, -1, 0, 1, 2 )
                        range_x = [ 3 * modifier_x ]
                    else:
                        range_y = [ 3 * modifier_y ]
                        range_x = ( -2, -1, 0, 1, 2 )

                    tile_count = 0 # number of revieled tiles in this direction
                
                    for search_x in filter( lambda x: kirk_X + x < width and kirk_X + x >= 0, range_x ): # if modifier is in bounds
                        for search_y in filter( lambda y: kirk_Y + y < height and kirk_Y + y >= 0, range_y ): # if modifier is in bounds
                            if not distance_map[ kirk_Y + search_y ][ kirk_X + search_x ]:
                                tile_count += 1 # if within bounds and is unexplored
                
                    if revieled_count < tile_count: # take the most unexplored and go that direction
                        revieled_count = tile_count
                        path = orientation
                        
            if not path: # no immediate direction to go to reviel the map, must map out a new destination
                plan = distance_set( kirk_Y, kirk_X, None, target, True ) # explore uncharted locations
    
    if plan: # follow the path
        path = plan.pop()
        
        for orientation, ( modifier_y, modifier_x ) in directions.items():
            if ( kirk_Y + modifier_y, kirk_X + modifier_x ) == path:
                path = orientation
                break
            
    print( path ) # Kirk's next move (DOWN RIGHT UP or LEFT).
    
    kirk_Y, kirk_X = ( int( input_data ) for input_data in input().split() ) # update Kirk's location

    if alarm and ( kirk_Y, kirk_X ) == ( beginning_Y, beginning_X ):
        break # finished excape sequence