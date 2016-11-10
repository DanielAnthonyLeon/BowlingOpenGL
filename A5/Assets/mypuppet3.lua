-- Transform the whole body
rootnode = gr.node('root')
rootnode:rotate('y', -20.0)
rootnode:scale( 0.25, 0.25, 0.25 )
rootnode:translate(0.0, 0.0, -1.0)

-- Standard colours
red = gr.material({1.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
blue = gr.material({0.0, 0.0, 1.0}, {0.1, 0.1, 0.1}, 10)
green = gr.material({0.0, 1.0, 0.0}, {0.1, 0.1, 0.1}, 10)
white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10)
black = gr.material({0.0, 0.0, 0.0}, {1.0, 1.0, 1.0}, 30)

-- Extra colours
orange1 = gr.material({0.917, 0.266, 0.180}, {0.1, 0.1, 0.1}, 10)
yellow1 = gr.material({0.913, 0.929, 0.149}, {0.1, 0.1, 0.1}, 10)
pink1 = gr.material({0.929, 0.149, 0.866}, {0.1, 0.1, 0.1}, 10)
green1 = gr.material({0.149, 0.929, 0.596}, {0.1, 0.1, 0.1}, 10)
blue1 = gr.material({0.149, 0.749, 0.929}, {0.1, 0.1, 0.1}, 10)
purple1 = gr.material({0.474, 0.149, 0.929}, {0.1, 0.1, 0.1}, 10)
blue2 = gr.material({0.149, 0.929, 0.913}, {0.1, 0.1, 0.1}, 10)

--torso 
torso = gr.mesh('sphere', 'torso')
rootnode:add_child(torso)
torso:set_material(white)
torso_scale = {0.4, 0.75, 0.4}
torso:scale(torso_scale[1], torso_scale[2], torso_scale[3])

-- shoulders
shoulders = gr.mesh('sphere', 'shoulders')
torso:add_child(shoulders)
shoulders:set_material(blue)
shoulders:scale(1.0/torso_scale[1], 1.0/torso_scale[2], 1.0/torso_scale[3])
shoulders_scale = {0.8, 0.2, 0.2}
shoulders:scale(shoulders_scale[1], shoulders_scale[2], shoulders_scale[3])
shoulders_translate = {0.0, 0.8, 0.0}
shoulders:translate(shoulders_translate[1], shoulders_translate[2], shoulders_translate[3])

-- neck
neck = gr.mesh('sphere', 'neck')
torso:add_child(neck)
neck:set_material(green)
neck:scale(1.0/torso_scale[1], 1.0/torso_scale[2], 1.0/torso_scale[3])
neck_scale = {0.15, 0.3, 0.15}
neck:scale(neck_scale[1], neck_scale[2], neck_scale[3])
neck_translate = {0.0, 1.0, 0.0}
neck:translate(neck_translate[1], neck_translate[2], neck_translate[3])

neck_joint = gr.joint('neck_joint', {-45, 0,45}, {-45, 0, 45})
torso:add_child(neck_joint)
neck_joint:scale(1.0/torso_scale[1], 1.0/torso_scale[2], 1.0/torso_scale[3])
neck_joint_translate = {0.0, 1.5, 0.0}
neck_joint:translate(neck_joint_translate[1], neck_joint_translate[2], neck_joint_translate[3])

head = gr.mesh('sphere', 'head')
neck_joint:add_child(head)
head:set_material(red)
head_scale = {0.3, 0.4, 0.3}
head:scale(head_scale[1], head_scale[2], head_scale[3])
head_translate = {0.0, 0.2, 0.0}
head:translate(head_translate[1], head_translate[2], head_translate[3])

-- upper arms
upperArms_scale = {0.2, 0.4, 0.2}
upperArms_translate = {0.8, -0.6, 0.0}

leftUpperArm_joint = gr.joint('leftUpperArm_joint', {-120, 0, 0}, {0, 0, 0})
shoulders:add_child(leftUpperArm_joint)
leftUpperArm_joint:scale(1.0/shoulders_scale[1], 1.0/shoulders_scale[2], 1.0/shoulders_scale[3])
leftUpperArm_joint:translate(upperArms_translate[1]/2, upperArms_translate[2]/2, upperArms_translate[3]/2)

leftUpperArm = gr.mesh('sphere', 'leftUpperArm')
leftUpperArm_joint:add_child(leftUpperArm)
leftUpperArm:set_material(orange1)
leftUpperArm:scale(upperArms_scale[1], upperArms_scale[2], upperArms_scale[3])
leftUpperArm:translate(upperArms_translate[1]/2, upperArms_translate[2]/2, upperArms_translate[3]/2)

rightUpperArm_joint = gr.joint('rightUpperArm_joint', {-120, 0, 0}, {0, 0, 0})
shoulders:add_child(rightUpperArm_joint)
rightUpperArm_joint:scale(1.0/shoulders_scale[1], 1.0/shoulders_scale[2], 1.0/shoulders_scale[3])
rightUpperArm_joint:translate(-upperArms_translate[1]/2, upperArms_translate[2]/2, upperArms_translate[3]/2)

rightUpperArm = gr.mesh('sphere', 'leftUpperArm')
rightUpperArm_joint:add_child(rightUpperArm)
rightUpperArm:set_material(orange1)
rightUpperArm:scale(upperArms_scale[1], upperArms_scale[2], upperArms_scale[3])
rightUpperArm:translate(-upperArms_translate[1]/2, upperArms_translate[2]/2, upperArms_translate[3]/2)

-- lower arms
lowerArms_scale = {0.15, 0.35, 0.15}
lowerArms_translate = {0.0, -0.9, 0.0}

leftLowerArm_joint = gr.joint('leftLowerArm_joint', {-120, 0, 0}, {0, 0, 0})
leftUpperArm:add_child(leftLowerArm_joint)
leftLowerArm_joint:scale(1.0/upperArms_scale[1], 1.0/upperArms_scale[2], 1.0/upperArms_scale[3])
leftLowerArm_joint:translate(lowerArms_translate[1]/2, lowerArms_translate[2]/2, lowerArms_translate[3]/2)

leftLowerArm = gr.mesh('sphere', 'leftLowerArm')
leftLowerArm_joint:add_child(leftLowerArm)
leftLowerArm:set_material(pink1)
leftLowerArm:scale(lowerArms_scale[1], lowerArms_scale[2], lowerArms_scale[3])
leftLowerArm:translate(lowerArms_translate[1]/2, lowerArms_translate[2]/2, lowerArms_translate[3]/2)

rightLowerArm_joint = gr.joint('rightLowerArm_joint', {-120, 0, 0}, {0, 0, 0})
rightUpperArm:add_child(rightLowerArm_joint)
rightLowerArm_joint:scale(1.0/upperArms_scale[1], 1.0/upperArms_scale[2], 1.0/upperArms_scale[3])
rightLowerArm_joint:translate(lowerArms_translate[1]/2, lowerArms_translate[2]/2, lowerArms_translate[3]/2)

rightLowerArm = gr.mesh('sphere', 'rightLowerArm')
rightLowerArm_joint:add_child(rightLowerArm)
rightLowerArm:set_material(pink1)
rightLowerArm:scale(lowerArms_scale[1], lowerArms_scale[2], lowerArms_scale[3])
rightLowerArm:translate(lowerArms_translate[1]/2, lowerArms_translate[2]/2, lowerArms_translate[3]/2)

-- hands
hands_scale = {0.1, 0.1, 0.1}
hands_translate = {0.0, -0.5, 0.0}

leftHand_joint = gr.joint('leftHand_joint', {-45, 0, 45}, {0, 0, 0})
leftLowerArm:add_child(leftHand_joint)
leftHand_joint:scale(1.0/lowerArms_scale[1], 1.0/lowerArms_scale[2], 1.0/lowerArms_scale[3])
leftHand_joint:translate(hands_translate[1], hands_translate[2], hands_translate[3])

leftHand = gr.mesh('sphere', 'leftHand')
leftHand_joint:add_child(leftHand)
leftHand:set_material(black)
leftHand:scale(hands_scale[1], hands_scale[2], hands_scale[3])
leftHand:translate(hands_translate[1]/2, hands_translate[2]/2, hands_translate[3]/2)

rightHand_joint = gr.joint('right_joint', {-45, 0, 45}, {0, 0, 0})
rightLowerArm:add_child(rightHand_joint)
rightHand_joint:scale(1.0/lowerArms_scale[1], 1.0/lowerArms_scale[2], 1.0/lowerArms_scale[3])
rightHand_joint:translate(hands_translate[1], hands_translate[2], hands_translate[3])

rightHand = gr.mesh('sphere', 'rightHand')
rightHand_joint:add_child(rightHand)
rightHand:set_material(black)
rightHand:scale(hands_scale[1], hands_scale[2], hands_scale[3])
rightHand:translate(hands_translate[1]/2, hands_translate[2]/2, hands_translate[3]/2)

-- hips
hips = gr.mesh('sphere', 'hips')
torso:add_child(hips)
hips:set_material(green1)
hips:scale(1.0/torso_scale[1], 1.0/torso_scale[2], 1.0/torso_scale[3])
hips_scale = {0.5, 0.2, 0.5}
hips:scale(hips_scale[1], hips_scale[2], hips_scale[3])
hips_translate = {0.0, -1.1, 0.0}
hips:translate(hips_translate[1], hips_translate[2], hips_translate[3])

-- thighs
thighs_scale = {0.2, 0.4, 0.2}

leftThigh_joint = gr.joint('leftThigh_joint', {-90, 0, 45}, {0, 0, 0})
hips:add_child(leftThigh_joint)
leftThigh_joint:scale(1.0/hips_scale[1], 1.0/hips_scale[2], 1.0/hips_scale[3])
leftThigh_joint:translate(0.9, -0.3, 0.0)

leftThigh = gr.mesh('sphere', 'leftThigh')
leftThigh_joint:add_child(leftThigh)
leftThigh:set_material(blue1)
leftThigh:scale(thighs_scale[1], thighs_scale[2], thighs_scale[3])
leftThigh:translate(0.0, -0.3, 0.0)

rightThigh_joint = gr.joint('rightThigh_joint', {-90, 0, 45}, {0, 0, 0})
hips:add_child(rightThigh_joint)
rightThigh_joint:scale(1.0/hips_scale[1], 1.0/hips_scale[2], 1.0/hips_scale[3])
rightThigh_joint:translate(-0.9, -0.3, 0.0)

rightThigh = gr.mesh('sphere', 'rightThigh')
rightThigh_joint:add_child(rightThigh)
rightThigh:set_material(blue1)
rightThigh:scale(thighs_scale[1], thighs_scale[2], thighs_scale[3])
rightThigh:translate(0.0, -0.3, 0.0)

-- calves
calves_scale = {0.15, 0.3, 0.15}
calves_translate = {0.0, -1.3, 0.0}

leftCalf_joint = gr.joint('leftCalf_joint', {0, 0, 90}, {0, 0, 0})
leftThigh:add_child(leftCalf_joint)
leftCalf_joint:scale(1.0/thighs_scale[1], 1.0/thighs_scale[2], 1.0/thighs_scale[3])
leftCalf_joint:translate(0.0, -0.4, 0.0)

leftCalf = gr.mesh('sphere', 'leftCalf')
leftCalf_joint:add_child(leftCalf)
leftCalf:set_material(purple1)
leftCalf:scale(calves_scale[1], calves_scale[2], calves_scale[3])
leftCalf:translate(0.0, -0.4, 0.0)

rightCalf_joint = gr.joint('rightCalf_joint', {0, 0, 90}, {0, 0, 0})
rightThigh:add_child(rightCalf_joint)
rightCalf_joint:scale(1.0/thighs_scale[1], 1.0/thighs_scale[2], 1.0/thighs_scale[3])
rightCalf_joint:translate(0.0, -0.4, 0.0)

rightCalf = gr.mesh('sphere', 'rightCalf')
rightCalf_joint:add_child(rightCalf)
rightCalf:set_material(purple1)
rightCalf:scale(calves_scale[1], calves_scale[2], calves_scale[3])
rightCalf:translate(0.0, -0.4, 0.0)

-- feet
feet_scale = {0.2, 0.2, 0.2}

leftFoot_joint = gr.joint('leftFoot_joint', {-45, 0, 45}, {0, 0, 0})
leftCalf:add_child(leftFoot_joint)
leftFoot_joint:scale(1.0/calves_scale[1], 1.0/calves_scale[2], 1.0/calves_scale[3])
leftFoot_joint:translate(0.0, -0.6, 0.0)

leftFoot = gr.mesh('sphere', 'leftFoot_joint')
leftFoot_joint:add_child(leftFoot)
leftFoot:set_material(blue2)
leftFoot:scale(feet_scale[1], feet_scale[2], feet_scale[3])
leftFoot:translate(0.0, -0.1, 0.0)

rightFoot_joint = gr.joint('rightFoot_joint', {-45, 0, 45}, {0, 0, 0})
rightCalf:add_child(rightFoot_joint)
rightFoot_joint:scale(1.0/calves_scale[1], 1.0/calves_scale[2], 1.0/calves_scale[3])
rightFoot_joint:translate(0.0, -0.6, 0.0)

rightFoot = gr.mesh('sphere', 'rightFoot_joint')
rightFoot_joint:add_child(rightFoot)
rightFoot:set_material(blue2)
rightFoot:scale(feet_scale[1], feet_scale[2], feet_scale[3])
rightFoot:translate(0.0, -0.1, 0.0)


return rootnode

