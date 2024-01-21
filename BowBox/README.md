# Bow Box - Expressive Violin Synthesizer

*This project was made for the 2023 Synthmas Contest. No code was used from any other projects.*

![IMG_0485](https://github.com/GeorgeTheSlayer/BowBox/assets/31327906/d44d419b-ee28-43bc-ac2c-f9ed7c3f1381)

Inspired by the Hitbox design, the Bow Box is a six voice Physically Modeled Violin Synthesizer capable of playing 6 voices at the same time. The eight touch plates change the amplitude of the playing voices on each side, allowing the user to create swells and change dynamics. The six arcade buttons give a very tactile feel, letting the user select which tones shall be played. 

The algorithm was custom made. It uses tuned waveguides and modal synthesis to create the desired sound. It needs minimal parts and has a simple schematic. The enclosure is easily 3d printable and customizable. 

## Project Goals

- Create a Synth that is extremely simple and intuitive to use.
- Create this as I would a normal open source project. 
- Be in guidelines for the Synthmas contest.

The guidelines for the contest are available to here. Tl;dr: Create an open source synthesizer/effect using the Daisy and the MPR121 touch sensor with a demo video.


## Ideation

This idea came about from playing Street Fighter 6 the past couple of months. The idea being, what if I could take a lever-less fight stick (a controller made for fighting games) and turn it into a musical instrument. 

At first I thought that it could maybe be a grove box or looper of some sort. The idea being that the user would have to input commands in order to get certain functions from the synthesizer. I made many quick sketches of this idea, but ultimately decided that it would be:

1. To much to implement in 2 weeks
2. 2. It goes against what makes these controllers so good in the first place. 

See fight sticks are good for one thing, controlling one character/object/sound. As soon as you have to switch sounds or change certain parameters they feel horrible to use. So I instead decided to make it control one instrument really well. For this I decided the Violin would be a really good instrument to try and emulate. **I could use the touch plates to let the user control how hard the instrument is being played and the arcade buttons to control which voice is being played.**  

## CAD Files
First we nee to make an enclosure and get the parts for this project. I got this cheapest touch sensors and arcade buttons off of amazon. Then I got to work on finalizing the cad design. 

I decided that 6 voices would be more than enough, then it will comfortably fit in your hand and you can easily press your thumbs against the touch sensors. Using a template I found online I used the common specs found for movement buttons of a leverless controller. I then recreated a basic front panel in OpenScad. 

I decided against creating a full box for the first version since I just wanted to worry about the code and getting it done on time. However, If I ever make another version I will for sure have a custom PCB that is a lot thinner. 

I made the touch sensitive parts square. That way it is easily ascertainable that these plates are different than the normal buttons. 

## Schematic 

The schematic for this is reality straight forward. The Six buttons are all wired into their own channel. And the touch sensors are communicating with the Seed through i2c. Finally all the audio comes out of the main audio outputs. 

For the sake of the current project, keeping everything simple is best. It is most important to make a basic working version of my idea rather than to have one part of my idea be perfect. 

## Python

Oh boy this was hard. 

I think 99% of all my time on this project was spent on the python. I spent a lot of time trying to get a very clean sound but I was unable to. Although I have made a lot of effects/synthesizers, I have never made a physically modeled instrument. I spent a lot of time looking through Julius Orion Smith III's research and it is incredible! Truthfully if I could I would love to be able to keep working on this project and make it better.

## CPP

This part was fairly straight forward. Port over my python code, into CPP. There was some code I ended up not using. Like some code for some filters and some code to emulate a bow on a string. But other than that I am really happy with how it sounds.

# I am available to work. Please contact at holland@sersen.net 

