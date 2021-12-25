# UE4_SimpleShooter

This is actually my expansion of the shooter project from [Unreal Engine C++ Developer: Learn C++ and Make Video Games ](https://www.udemy.com/course/unrealcourse/)
I have tried to learn about locomotion systems.


## Animation


### Animation Graph
![AnimGraph](https://user-images.githubusercontent.com/49837425/139589945-6dafa030-9a2f-40f1-b68a-009db3c03e06.png)


This is a picture of the animation graph in this project. The root state machine consists of the base locomotion of the Player character. The output pose is connected to a defaultSlot. This slot is used for AnimMontages that use the default slot. Finally, this pose is cached to be used in different sections.

The next part is the Upper Body Blend. In this part, we use the cached pose with another slot labeled "UpperBody". These two poses will be the input for layered blend per bone node, which can mask the animation to play a specific part of the character skeleton (here, we want to be the upper body). So the "UpperBody" slots will be used for UpperBody animations such as reload animation. In this part, the pose will get cached too.

After that, we have aim offset blend space used to offset the weapon of the player character. Since this offset has to be applied when the character is aiming, a blend poses by bool is used only to offset when the character is in Aim fight state.

In the final part, we have an inertialization node used for inertialization blending.
More information about this blending technique can be found [here](https://www.youtube.com/watch?v=BYyv4KTegJI)


### Ground Root State Machine
![GroundRoot](https://user-images.githubusercontent.com/49837425/139591089-69709acf-82f0-4399-8439-0b50e148caaf.png)

The root state machine is split into Alive and Death states. In the alive state, we have Locomotion, and In that, we see the grounded state, which can be seen in the picture.

As can be seen from the picture, We have three states.

### Standing Ground Movement Sate Machine


Here we have the main movement functionality. 
![StandingGroundMovement](https://user-images.githubusercontent.com/49837425/139866637-6056bc91-8554-4c4e-b4ae-f616f87bbe98.png)

The walk/Jog process actually consists of three parts. Start, Loop And Stop. The start and stop animations use rootmotion, but the movement is controlled using the code and the capsule in the loop section.

The Pivot state is used for fast direction changes.

The Stop State on the top left is used to control the micro inputs(The process in which the player move the joystick slightly) 

### Walk/Jog Loop
![WalkLoop](https://user-images.githubusercontent.com/49837425/139592174-959a275c-ad95-48bb-b0e2-f958d150a89b.png)


As can be seen from the picture, We have three blend spaces corresponding to the fight state.
And their values are controlled by the character angle and speed.



## Gameplay


### Movement States

<p align="center">
  <img src="https://user-images.githubusercontent.com/49837425/139868468-dc89203a-8e26-4ce6-9ca8-30c03283ebee.png" />
</p>


### Fight States

<p align="center">
  <img src="https://user-images.githubusercontent.com/49837425/139868482-c3606e6c-e809-463b-92db-e16574230ec7.png" />
</p>

### Cover States

<p align="center">
  <img src="https://user-images.githubusercontent.com/49837425/139868566-4a4c0a50-d227-47b2-9e19-9586e524413b.png" />
</p>



### Gun



The gun class in this project has several attributes that a designer can change. 
- The Ammo in the Gun 
- Max Ammo per magazine
- Range
- Damage Amount Per Hit
- Reload Time
- Fire Rate
- Single Fire or Automatic
- Auto Reload
- Muzzle Flash Particle
- Impact Effect Particle
- Muzzle Sound
- Impact Sound
- Fire Recoil Animation
- Reload Animation


<p align="center">
  <img src="https://user-images.githubusercontent.com/49837425/139873287-789300af-72f1-4e2e-8860-a359ea6f276e.png" />
</p>


The player can carry two guns at the same time. The player can change the gun using the Q on the keyboard. He/She can grab the gun from the ground or drop it. Each weapon has a specific amount of ammo, which the designers can alter.

This can be seen in the following video. On the top left, We show the ammo capacity as debug text. As can be seen, while the blue gun has the maximum amount of 10 bullets per magazine, the black rifle has the maximum amount of 30 bullets per magazine.



https://user-images.githubusercontent.com/49837425/139878315-60fa49c6-89d8-4125-978f-04076a1875d3.mp4


### Gun Fire

The gun uses line trace single by channel to find the hitted actor. The impact particle and the sound will be played on the hit position. If the hitted actor is the enemy, the damage will be applied. 


https://user-images.githubusercontent.com/49837425/139878440-2acabc49-a7b7-47f0-8b08-75f1a4e6fcb8.mp4




### Reload Functionality

Reloading gun is consists of 2 animations notify states. 

- ReloadStart: The beginning of this state will be the player removing the empty mag for the gun. In this stage, The mag attached to the gun will get hidden. A new instance of the empty mag will be created and attached to the reload socket on the character's hand. At the end of this state, after simulating physics, the instantiated mag will get detached from the character's hand.
- ReloadEnd: This state starts when the character reaches his back to grab the new mag. Here we again instantiate a new mag and attach it to the character's hand. This state ends when the character embeds the mag into the gun. Here we unhide the hidden magazine and destroy the instantiated mag.

https://user-images.githubusercontent.com/49837425/139878485-b33a23c5-b9c8-40e5-a27b-97dcf3811a37.mp4




