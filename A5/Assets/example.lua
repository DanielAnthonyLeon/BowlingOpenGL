rootnode = gr.node('root')

red = gr.material({1.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
blue = gr.material({0.0, 0.0, 1.0}, {0.1, 0.1, 0.1}, 10)
green = gr.material({0.0, 1.0, 0.0}, {0.1, 0.1, 0.1}, 10)
white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10)

inchesInFoot = 12.0

BowlingLaneWidthInInches = 42.0
BowlingLaneLengthInFeet = 60.0

BowlingLane = gr.mesh('plane2', 'BowlingLane')
rootnode:add_child(BowlingLane)
BowlingLane:set_material(red)
-- A bowling lane is 42 inches wide and 60 feet long, with the length of being measured from the foul line to the head pin
BowlingLane:scale(BowlingLaneWidthInInches/inchesInFoot, 3.0, BowlingLaneLengthInFeet)
BowlingLane:translate(0.0, -2.8, -65.0)

return rootnode
