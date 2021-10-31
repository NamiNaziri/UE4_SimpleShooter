# UE4_SimpleShooter

This is actually my expansion of the shooter project from [Unreal Engine C++ Developer: Learn C++ and Make Video Games ](https://www.udemy.com/course/unrealcourse/)
I have tried to learn about locomotion systems.


## Animation Graph


### Animation Graph
![AnimGraph](https://user-images.githubusercontent.com/49837425/139589945-6dafa030-9a2f-40f1-b68a-009db3c03e06.png)


This is a picture of the animation graph in this project. The root state machine consits of the base locomotion of the Player character. The output pose is connected to a defautSlot. This slot is used for AnimMontages that use default slot. Finally, this pose is cached to be used in different sections.

The next part is the Upper Body Blend. In this part we use the cached pose with another slot labled "UpperBody" and this two poses will be the input for layered blend per bone node with can mask the animation to play in specific part of the character skeleton (here we want to be upper body). So the "UpperBody" slots will be used for upperBody animations such as reload animation. In this part the pose will get cached too.

After that, we have aim offset blend space used for offset the weapon of the player character. since this offset has to be applied when the character is aiming, a blend poses by bool is used to only offset when the character in Aim fight state.

In the final part, we have a inertialization node which is used for inertialization blending.
More information about this blending technique can be found [here](https://www.youtube.com/watch?v=BYyv4KTegJI)


### Ground Root State Machine
![GroundRoot](https://user-images.githubusercontent.com/49837425/139591089-69709acf-82f0-4399-8439-0b50e148caaf.png)

The root state machine split into Alive and Death state. In the alive state we have Locomotion and In the we see the grounded state which can be seen in the picutre too.

As can be seen from the picture, We have three states.

### Standing Ground Movement Sate Machine

Here we have the main movement functionality. 

Walk/Jog process is actually consists of three parts. Start, Loop And Stop. The start and stop animations use rootmotion but in the loop section the movement is controled using the code and the capsule.

The Pivote state is used for fast direction changes.

The Stop State on the top left is used to control the micro inputs(The process in which the player move the joystick slightly) 

### Walk/Jog Loop
![WalkLoop](https://user-images.githubusercontent.com/49837425/139592174-959a275c-ad95-48bb-b0e2-f958d150a89b.png)


As can be seen from the picture, We have three blend spaces which corresponds to the fight state.
and their values are controled by the character angle and speed.









