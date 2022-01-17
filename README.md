# UE4_SimpleShooter

This is an expanded version of the shooter project from the course ["Unreal Engine C++ Developer: Learn C++ and Make Video Games"](https://www.udemy.com/course/unrealcourse/)
 where I tried to learn about locomotion systems.

## Animation


### Animation Graph
![AnimGraph](https://user-images.githubusercontent.com/49837425/139589945-6dafa030-9a2f-40f1-b68a-009db3c03e06.png)


This is a picture of the animation graph used in this project. The root state machine consists of the base locomotion of the Player character. The output pose is connected to a defaultSlot. This slot is used by AnimMontages that use the default slot. Finally, this pose is cached so that it can be used in various sections.

Next comes the Upper Body Blend, in which the cached pose is linked to another slot called "UpperBody". We will use these two poses as the inputs for the layered blend per bone node, which will allow us to mask the animation to play a specific part of the skeleton (here, the upper body). Therefore, the "UpperBody" slots will be used for upper body animations such as reload animation. In this part, the pose will also be cached.

After that, we have an aim offset blend space used to offset the weapon of the player character. Since this offset has to be applied when the character is aiming, a blend poses by bool is used. Therefore, it will only be active when the character is in Aim Fight Fight state.

In the final part, we have an inertialization node used for inertialization blending.
More information about this blending technique can be found [here](https://www.youtube.com/watch?v=BYyv4KTegJI)


### Ground Root State Machine
![GroundRoot](https://user-images.githubusercontent.com/49837425/139591089-69709acf-82f0-4399-8439-0b50e148caaf.png)

Root state machine is split into Alive and Death states. In the alive state, we have locomotion, and in that, we have the grounded state, as shown in the picture.

As can be seen from the picture, We have three states.

### Standing Ground Movement Sate Machine


Here we have the main movement functionality. 
![StandingGroundMovement](https://user-images.githubusercontent.com/49837425/139866637-6056bc91-8554-4c4e-b4ae-f616f87bbe98.png)

Walk/Jog is composed of three parts: Start, Loop, and Stop. The start and stop animations use rootmotion, while in the loop section the movement is controlled by code and the capsule.

The Pivot state is used for fast direction changes.

The Stop State in the top left controls the micro inputs (The process in which the player moves the joystick slightly) 

### Walk/Jog Loop
![WalkLoop](https://user-images.githubusercontent.com/49837425/139592174-959a275c-ad95-48bb-b0e2-f958d150a89b.png)


From the picture, it can be seen that there are three blend spaces corresponding to the fight state.
Their values are determined by the angle and speed of the character.



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


The player can carry two guns at once. Changing guns is done with the Q key on the keyboard. The player can pick up the gun or drop it. Weapons have a specific amount of ammunition, which the designers can change.

The following video shows the ammunition capacity on the top left. As can be seen, the blue gun has a maximum of 10 bullets per magazine, while the black rifle has a maximum of 30 bullets per magazine.



https://user-images.githubusercontent.com/49837425/139878315-60fa49c6-89d8-4125-978f-04076a1875d3.mp4


### Gun Fire

The gun uses line trace to determine the hit actor. The particle and sound are played at the hit position. If the hit actor is the enemy, the damage will be applied. 


https://user-images.githubusercontent.com/49837425/139878440-2acabc49-a7b7-47f0-8b08-75f1a4e6fcb8.mp4




### Reload Functionality

Reloading gun is consists of 2 animations notify states. 

- ReloadStart: This state starts when the player removes an empty magazine from a gun. At this stage, The mag attached to the gun will get hidden. A new instance of the empty magazine will be created and attached to the reload socket on the character's hand. After simulating physics, the mag will get detached from the character's hand at the end of this state.
- ReloadEnd: In this state, we again instantiate a new mag and attach it to the character's hand. After embedding the magazine into the gun, we unhide it and destroy the instantiated magazine.

https://user-images.githubusercontent.com/49837425/139878485-b33a23c5-b9c8-40e5-a27b-97dcf3811a37.mp4




