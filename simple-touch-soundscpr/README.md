# Plugdata and daisy seed  + mpr121 touch sensor

**This is a copy from the Notion page I made, I've got to update the formatting and images and links from there - so for now best go to notion until this is fixed**

go here for now: https://jonwtr.notion.site/Plugdata-and-daisy-seed-mpr121-touch-sensor-41be6a24dc0b4dc4bdd2fffbe4763dee

---

<aside>
💡 I’m not the ‘actual’ author of these solutions, nor do I fully understand everything, I just collected available info by searching and asking around forums and a lot of help from ‘Dreamer’ on Discord.
Feel free to comment, copy, alter etc. … Or reach out to me on e.g. Discord @jonwtr
This is a live document - a copy lives on github - and might change over time.

</aside>

This page contains a lot of text and info, there’s a [TLDR;](https://www.notion.so/Plugdata-and-daisy-seed-mpr121-touch-sensor-41be6a24dc0b4dc4bdd2fffbe4763dee?pvs=21) if you want to just *quickly* see how to get Plugdata and the MPR121 sensor up and running.

## Synthux Synthmas 2023 Challenge

The bulk of this work was made building up to [Synthux academy’s Synthmas 2023 challenge](https://www.synthux.academy/events/synthmas-2023).

[2023 Synthmas Contest](https://www.synthux.academy/events/synthmas-2023)

**The challenge:** build an instrument / sound fx with a Daisy seed and a MPR121 touchcontroller.

I used the Simple Touch by Synthux but this was optional.

In short the **goal of Synthux** was to create a pool of examples from which the community can benefit.

**My goal**, even before the challenge was to start using Plugdata as a way to realize my project of building my concept Soundscpr, a musical, texture, sound-scaping device shaped like a landscape. 

I’ve got  that ‘end goal’ Soundscpr in mind and have already built these: a noise box and a custom top on a ShieldXL (monome norns clone by Denki Oto).

![IMG20231101210418-01.jpeg](https://prod-files-secure.s3.us-west-2.amazonaws.com/183357c7-1e01-4c65-a869-200d788b649c/ac9b342b-a382-4504-b874-f9eb06c30977/IMG20231101210418-01.jpeg)

This is a 3D sketch where I hope to land, it’s designed to fit on top of a Synthux Simple board.:

![soundscpr_evee-3D.jpg](https://prod-files-secure.s3.us-west-2.amazonaws.com/183357c7-1e01-4c65-a869-200d788b649c/d16fe03d-520e-4c1b-965e-8dd6be7daf38/soundscpr_evee-3D.jpg)

Overlay on the Synthux Simple PCB:

![landscpr simple drawing top.jpg](https://prod-files-secure.s3.us-west-2.amazonaws.com/183357c7-1e01-4c65-a869-200d788b649c/83e5addb-9f44-4d83-90d3-403259a9fa92/landscpr_simple_drawing_top.jpg)

This Soundscpr would have three main parts; The rock (with a noisebox piezo function), The brain hill (reverb and filtering) and the H2O part containing a touch keypad.

One of the elements I wanted to use was the MPR121, so this challenge was an excellent stepping stone and an well timed opportunity to learn.

---

# My submission:

## Simple delay/ looper/ drone/ fx/ basic aeolean tuned synth/ soundscpr - Built with plugdata

### About my Plugdata Patch and instrument fx box:

The Plugdata patch I’ve build contains:

![Simpletouch_delaydronefx_soundscpr_layout.jpg](https://prod-files-secure.s3.us-west-2.amazonaws.com/183357c7-1e01-4c65-a869-200d788b649c/e8163d12-015e-4b7d-a6e9-fe1ca61336e9/Simpletouch_delaydronefx_soundscpr_layout.jpg)

- **Stereo audio input**
    - Left fader controls a dry signal that bypasses the delay
    - toggling the touch pads on top (the Tou-ch logo, pads 10 and 11) will send only the audio input to delay and trigger an LED
        - LED = red means recording only for the audio input,
            - (keypads have a separate feedback knob 4)
        - Turning it of allows to still keep listening to the live input via the left fader.
- S**tereo ping-pong delay**
    - **Knob 1: Time** up to 5000ms (left and right have a difference to create a moving stereo pinpong)
    - **Knob 2: Feedback** (up to almost infinite looping)
    - **Knob 3: Delay volume**
- **Touch keypad**
    - **with an aeolean scale** mapped to **knob 5**
        - note this isn’t locked, the knob will simply go from 0 to 440hz.
            - via simple calculations sending the osc~ through frequency to midi, adding notes, then back to frequency (using mtof and ftom blocks)
    - **Pads 3 - 9** are basic osc~ with a quick attack and slower release
    - **Switch 1 and 2**: droning osc (with noise) to the pitch of knob 5
        - Stop or add **noise** on top of the drones with **pads 0 and 2**
        
        | Pad | 4 | 5 | 6 | 7 | 8 | 9 | switch 1 | switch 2 |
        | --- | --- | --- | --- | --- | --- | --- | --- | --- |
        | Semitones | +0 | +1 | +3 | +5 | +6 | +8 | - 12 | - 24 |

Download link to patch:

[Soundscpr_alpha_plugdatampr121-demo.pd](https://prod-files-secure.s3.us-west-2.amazonaws.com/183357c7-1e01-4c65-a869-200d788b649c/ce28ea26-5e6e-4a6d-a15d-8c5a65d20a4d/Soundscpr_alpha_plugdatampr121-demo.pd)

Download link to custom json:

[simpletouch_mpr121_knobs_sliders.json](https://prod-files-secure.s3.us-west-2.amazonaws.com/183357c7-1e01-4c65-a869-200d788b649c/57da48a3-daf1-4570-b6c0-97a37a0643fb/simpletouch_mpr121_knobs_sliders.json)

Read TLDR; for quick instructions

---

<aside>
💡 By no means is this anything more than a proof of concept for me, I’m not a proficient musician, I’m a noob at programming, etc … Though I’m proud I’ve learned so much, and I’m ever so grateful to the community around al these tools enabling this journey.

</aside>

---

---

---

---

What follows beneath this is my documentation, which grew into a full blown guide on how every step went. There’s a short TLDR; (Too long, didn't read) here, if you’re not as noob as me ;-)

## TLDR;

Getting plugdata and MPR121 up and running

- Install Plugdata
- Choose the compile option in the menu
- Download the toolchain
- Edit `plugdata\Toolchain\usr\bin\Heavy\json2daisy\resources\component_defs.json`
    - delete the part referring to the pins in the “map_init” **line 558**, leave the line “pin” as is:
    - delete: `{name}_config.scl = som.GetPin({pin_scl});\n    {name}_config.sda = som.GetPin({pin_sda});\n`
- Create a custom json file with all the components connected to the Daisy ([download below](https://www.notion.so/Plugdata-and-daisy-seed-mpr121-touch-sensor-41be6a24dc0b4dc4bdd2fffbe4763dee?pvs=21))
- Start patching
    - Use this block in the patch `r mpr121_driver_ch0 @hv_param`
    - to refer to touch pads 0 - 11 change `_ch0` , `_ch…` up to `_ch11`
    - To use the knobs and faders and switch use the names you’ve added in the custom json file
        - e.g. `r toggle1left @hv_param` or `r knob1@hv_param`
- Go to the compiler menu and pick the right options and flash!
    - note I’m using a Custom Linker, but you should check first if the small, big or huge modes work for you, more [on the use of memory below](https://www.notion.so/Plugdata-and-daisy-seed-mpr121-touch-sensor-41be6a24dc0b4dc4bdd2fffbe4763dee?pvs=21).

![Untitled](https://prod-files-secure.s3.us-west-2.amazonaws.com/183357c7-1e01-4c65-a869-200d788b649c/c62f8951-c1d5-4924-bf65-4cc3dad890c4/Untitled.png)

---

# Intro - about this page and my learning process

<aside>
💡 **These are my notes on getting the touch sensor mpr121 talking to Daisy seed through Plugdata**

</aside>

I had some hurdles getting daisy to speak to custom knobs and the touch sensor.

I’m a newcomer to Plugdata, coding and daisy - though I’ve been fumbling with Arduino for some years, I mostly just copy paste until it works. Fake it ‘till you make it! 🔍📋😅

## SynthUX Academy - Simple Touch

Specifically the code here is ready for Simple Touch Synth Kit, but can be adapted to fit your own custom setup going from Plugdata to Daisy. Specifically the documentation here is intended to help me remember the workflow in the future. 

[Simple Touch Synth Kit](https://www.synthux.academy/shop/touch-kit)

### Custom json for addressing knobs and touchpads

To use the touchpads via Plugdata you’ll need a description of your board and how the pins are connected. This custom json file is discussed below. 

Note there's also a guide over on the daisy forum. It goes over the same basics of setting up a custom json. Though without addressing the mpr121 part specifically.

https://forum.electro-smith.com/t/quick-guide-on-setting-up-a-custom-json-file-for-pd2dsy-oopsy/4021 

https://forum.electro-smith.com/t/quick-guide-on-setting-up-a-custom-json-file-for-pd2dsy-oopsy/4021

### Memory allocations and regions for e.g. using delayread

Specifically I started out with wanting to experiment with **delays**, and the `delread` block quickly fills up the memory of the Daisy when you want longer delays than 1 or 2 seconds. That’s why I ended up using a custom linker to make us of makes use of SDRAM. Delays make use of this larger available memory. Though I’ve yet to learn more about how memory allocation actually flows, this seems like a good thing to know.

On the bottom of this page I’ve got some extra info on where to find more on using these modes and the way the bootloader works.

Open each section to follow my findings and how I got the mpr121 to work in plugdata

---

---

---

## About visually / modular programming

If you don’t know Pure Data, Plugdata, Pd, flavours, modular, Visual programming. What are these? Read this

<aside>
💡 If you now what Plugdata is and how it’s different from Pure Data > skip this part, I’ve written this with noobs like myself in mind.

</aside>

- Pure Data goes back many years and was made as a tool to help artists create (interactive) sound. It uses a visual blocks structure, comparable to e.g. Scratch, a tool used to teach (kids) programming, you link pieces of code together where each block has a specific function in the whole.
    
    ![Untitled](https://prod-files-secure.s3.us-west-2.amazonaws.com/183357c7-1e01-4c65-a869-200d788b649c/c1f0b436-9793-4592-ab78-be86734b53b2/Untitled.png)
    
    - Pure Data - “flavours”
        
        > **Pure Data** (or just "Pd") is an open source visual programming language for multimedia. Pure Data is developed by Miller Puckette since 1996 and you can find it on [his official website](https://msp.ucsd.edu/) along with the official documentation and other related resources. This is the official "Pd" or "main distribution" and it is also known as "Pd Vanilla", but there are other forks or "flavours".
        > 
        
        source: intro on [puredata.info](http://puredata.info) 
        
- Plugdata is a flavour that builds upon the vanilla Pure Data, but adds a more user friendly interface, implements the tools we can use to program the Daisy microcontroller, makes use of more visual elements, can become a VST plugin, …
    - Plugdata
        
        > plugdata is a free/open-source visual programming environment based on pure-data. It is available for a wide range of operating systems, and can be used both as a standalone app, or as a VST3, LV2, CLAP or AU plugin.
        > 
        
        > plugdata allows you to create and manipulate audio systems using visual elements, rather than writing code. Think of it as building with virtual blocks – simply connect them together to design your unique audio setups. It's a user-friendly and intuitive way to experiment with audio and programming.
        > 
        
        source: [plugdata.org](http://plugdata.org) 
        
        ![Untitled](https://prod-files-secure.s3.us-west-2.amazonaws.com/183357c7-1e01-4c65-a869-200d788b649c/4f81c328-3752-408d-bb06-dd6e33731895/Untitled.png)
        

### The modular approach

- There are many tools out there that use this approach to programming or building electronic stuff, be it analog or digital. In 3D software like e.g. Blender they’re called nodes, Touchdesigner and [Cables.gl](http://Cables.gl) are both examples of tools that heavily rely on the approach of connecting blocks with virtual cables.
- You could go far with this analogy as all things are a whole of parts, yet for sake of this document let’s just keep the connecting of different parts as the main concept.
- Coming from the music world you might be familiar with modular Eurorack synths. Maybe you’ve even used synths or kits that use Daisy. Or maybe you’ve seen the virtual eurorack systems like VCV rack or Cherry Audio Voltage Modular and many others ([see some listed in this article on attackmagazine.](https://www.attackmagazine.com/reviews/the-best/ten-of-the-best-modular-software/) )
    - VCV a virtual cable spaghetti maker:
    
    https://youtu.be/NPErDUlwSlg?list=PL7NZKetd80yrCyyQ2-1J8fad420gL8MAf
    
- Seeing you’ve stumbled upon this page, you’re probably already interested or knowledgeable on the concept of using code and libraries. I have been tinkering with Arduino microcontrollers before and most code I’ve used relied heavily on the work of others, not only the builders of the hardware and it’s software, but also people who’ve made pieces of code often called libraries that you refer to in your own piece of code. Or sometimes it’s just pieces you could copy paste at the bottom etc. …
    - So you can use links or refer to these pre-made pieces of functions and tools inside your own code that get compiled or run in the background.
        - E.g. case in point of this page referring to Daisy seed and the mpr121: you link to the mpr121 touch sensor, and in pure data all you need to do is refer to the touch points, without actually knowing how it works.
        - The necessary code to talk to the sensor, the code to read the sensor, and the implementation into plugdata is all being done through various tools, libraries.
        - As an end user we can just put a block that says “knob1”, etc.
    - Pure Data can be seen as a similar thing of being a modular programming approach you write names of functions or tools; e.g. an oscillator, you don’t know how the oscillator is itself in turn made of smaller components. You just put a block that says give me an oscillator with the frequency 440.
        - We don’t always have to know how to make paint, brushes and canvas to be a proficient painter

### Let’s get started, full length explanations on getting MPR121 working;

# **Install Plugdata**

- download plugdata from [plugdata.org](https://plugdata.org/download.html) see the install instructions there as well
- After installing the software you need to install the toolchain
    - Install the toolchain the first time from plugdata by going to “compile” in the menu
    - upon clicking that menu item you should be prompted to first install this toolchain, this takes a while nut is a one time need, later you’ll need to find the installed folder to edit a file (see below)

---

# **Workflow: from plugdata to Daisy**

To **get code from plugdata into daisy** we need to understand the workflow

<aside>
💡 Not all existing patches work as is; they get converted; Many of the fancy visual stuff is useless for Daisy, many patches rely on stuff that isn’t supported, and Daisy also has limited CPU power compared to a computer.

</aside>

- Patches get converted from pure data to C/C++ code
- The conversion is done by hvcc https://github.com/Wasted-Audio/hvcc
- some pd stuff is not supported, there is a [list with all (un)supported items](https://github.com/Wasted-Audio/hvcc/blob/develop/docs/09.supported_vanilla_objects.md)
- e.g. Plug Data “else” is not (yet) able to convert anything using this, library
- plugdata makes it ‘easy’ when you use ‘compiled mode’ by showing a warning what is or isn’t supported
- Next to the need to use your own custom json, there’s a predefined json file. It’s a similar looking file, but is sort of the ‘mother’ and contains the code that your custom json board files use as a sort of template in the compiling process.
    - These predefined json files are part of the toolchain and are ‘hard’ to find. Normally you wouldn’t need to edit these, but below I explain further what to edit for the mpr121.

---

# **To compile we need a custom json file**

- To address knobs or buttons etc. connected to daisy pins we define a list in a custom json file;
- This json file needs to be selected in the compile window under target board
- It seems I had to reselect the json file in previous versions when I saved changes in the file each time again before compiling for Plugdata to see these changes. So I now have the custom to reselect it each time I’ve changed something inside the json. Newer versions fixed this, once selected it keeps using the last one.
- The custom json file will be used in the compile screen where we select it as our custom board under “Target board”.
- more on the use of the patch size options below this document: [Extra info on using different memory options](https://www.notion.so/Extra-info-on-using-different-memory-options-dd221e0823cd4e589fb10dc6ee43e682?pvs=21)

![Untitled](https://prod-files-secure.s3.us-west-2.amazonaws.com/183357c7-1e01-4c65-a869-200d788b649c/bec7b194-1c1d-4ebc-be49-61e580e104e6/Untitled.png)

---

# **Edit a part of the toolchain json** components definition file

To get a working mpr121 **edit a part of the toolchain json** components definition file / might get fixed

- This is not ‘normal’ and is supposed to work without the edit;
- this bug has been reported: **[Bug: mpr121 pin setup incorrect](https://github.com/electro-smith/json2daisy/issues/18)**

https://github.com/electro-smith/json2daisy/issues/18

- currently the file in question is causing an error referring to wrong pins
    - mpr121 touch sensor is supported, however seems to give a pin error:
    - `Error c2daisy: 'pin_scl’`
- We need to edit the file that contains the mpr121 code and is located in the installed toolchain:
    - edit code in the json file `component_defs.json`
    - on my windows machine it’s here: `C:\Users\gebruiker\Documents\plugdata\Toolchain\usr\bin\Heavy\json2daisy\resources\component_defs.json`
- Change the first line `“map_init”`:
    - Note I’ve added a few extra lines to easily see where it’s located

```json
Original code:
--------------
"Mpr121": {
		"map_init": "daisy::Mpr121I2CTransport::Config {name}_config;\n    {name}_config.periph = {periph};\n    {name}_config.speed = {speed};\n    {name}_config.scl = som.GetPin({pin_scl});\n    {name}_config.sda = som.GetPin({pin_sda});\n    {name}_config.dev_addr = {address};\n    daisy::Mpr121I2C::Config {name}_main_conf;\n    {name}_main_conf.transport_config = {name}_config;\n    {name}_main_conf.touch_threshold = {touch_threshold};\n    {name}_main_conf.release_threshold = {release_threshold};\n    {name}.Init({name}_main_conf);",
		"typename": "daisy::Mpr121I2C",
		"direction": "in",
		"pin": "scl,sda",
```

delete the part referring to the pins in the “map_init” **line 558**, leave the line “pin” as is:

delete: `{name}_config.scl = som.GetPin({pin_scl});\n    {name}_config.sda = som.GetPin({pin_sda});\n`    

```json
Altered code:
-------------
"Mpr121": {
		"map_init": "daisy::Mpr121I2CTransport::Config {name}_config;\n    {name}_config.periph = {periph};\n    {name}_config.speed = {speed};\n     {name}_config.dev_addr = {address};\n    daisy::Mpr121I2C::Config {name}_main_conf;\n    {name}_main_conf.transport_config = {name}_config;\n    {name}_main_conf.touch_threshold = {touch_threshold};\n    {name}_main_conf.release_threshold = {release_threshold};\n    {name}.Init({name}_main_conf);",
		"typename": "daisy::Mpr121I2C",
		"direction": "in",
		"pin": "scl,sda",
```

---

# json for the “custom” daisy board

This was made to fit the Daisy seed into a Simple Synthux touch board, this can be altered to fit other setups or the larger simple board

This code is the basics:

```json
{
    "name": "soundscprDaisy",
    "som": "seed",
    "audio": {
      "channels": 2
    },

    "parents": {
      "mpr121_driver": {
        "component": "Mpr121"
        }
    }

}
```

The full code below wasn’t working at first because of the use of **Caps in the custom component name**. I was using `faderLeft` in the json file, which prompted this error when compiling in Plugdata: `Error c2daisy: Unknown parameter "faderLeft”`

Don’t use caps in the names. JUST DON’T 😆

<aside>
💡 This is the custom json code for the Synthux Simple Touch with use of the knobs, faders, switches and the mpr121:

</aside>

```json
{
   "name": "soundscprDaisy",
   "som": "seed",
   "audio": {
     "channels": 2
   },

   "parents": {
     "mpr121_driver": {
       "component": "Mpr121"
       }
   },

   "components": {
       "faderleft": {
           "component": "AnalogControl",
           "pin": 21
       },
       "faderright": {
           "component": "AnalogControl",
           "pin": 22
       },
       "knob0": {
           "component": "AnalogControl",
           "pin": 15
       },
       "knob1": {
           "component": "AnalogControl",
           "pin": 16
       },
       "knob2": {
           "component": "AnalogControl",
           "pin": 17
       },
       "knob3": {
           "component": "AnalogControl",
           "pin": 18
         },
       "knob4": {
           "component": "AnalogControl",
           "pin": 19
       },
       "knob5": {
           "component": "AnalogControl",
           "pin": 20
       },
       "toggle1left": {
           "component": "Switch",
           "pin": 8
       },
       "toggle1right": {
           "component": "Switch",
           "pin": 9
       },
       "toggle2up": {
           "component": "Switch",
           "pin": 7
       },
       "toggle2down": {
           "component": "Switch",
           "pin": 6
       },
       "recled": {
           "component": "Led",
           "pin": 23
       }

   }
}
```

---

# Pins **table overview** for Simple Touch

I’m still often confused by the pin numbering used to program daisy. So I’ve made a table with an overview.

Link to the spreadsheet with a complete overview:

https://docs.google.com/spreadsheets/d/1xtg_s1tk8tm-6qNkBLFc6V1L_Mpmu-PCOvv7qEyr9mU/edit?usp=sharing

![Untitled](https://prod-files-secure.s3.us-west-2.amazonaws.com/183357c7-1e01-4c65-a869-200d788b649c/8ddbb4ad-6ecc-48e6-8c66-fe2fc44c2955/Untitled.png)

Synthux is using the ‘S’ number as a way to refer to the connections on their PCB. This is convenient for them to allow easy mounting of the Daisy on the back of the Simple PCB.

| Daisy seed pin numbers on the board | Daisy pin names digital | Daisy pin names analog | Pin names Synthux | Pin numbers for Simple Touch to use in Plugdata | https://www.synthux.academy/shop/simple-touch-pcb Description | Custom https://www.notion.so/Plugdata-and-daisy-seed-mpr121-touch-sensor-41be6a24dc0b4dc4bdd2fffbe4763dee?pvs=21 |
| --- | --- | --- | --- | --- | --- | --- |
| 7 | D6 |  | S07 | 6 | switch vert down | toggle2down |
| 8 | D7 |  | S08 | 7 | switch vert up | toggle2up |
| 9 | D8 |  | S09 | 8 | switch hor left | toggle1left |
| 10 | D9 |  | S10 | 9 | switch hor right | toggle1right |
|  |  |  |  |  |  |  |
| 20 | A GND | A GND | A GND |  | A GND |  |
| 21 | 3V3 Analog | 3V3 Analog | 3V3 Analog |  | 3V3 Analog |  |
| 22 | D15 | A0 | S30 | 15 | pot 0 bottom left | knob0 |
| 23 | D16 | A1 | S31 | 16 | pot 1 top left | knob1 |
| 24 | D17 | A2 | S32 | 17 | pot 2 top | knob2 |
| 25 | D18 | A3 | S33 | 18 | pot 3 top | knob3 |
| 26 | D19 | A4 | S34 | 19 | pot 4 top right | knob4 |
| 27 | D20 | A5 | S35 | 20 | pot 5 bottom right | knob5 |
| 28 | D21 | A6 | S36 | 21 | fader left | faderleft |
| 29 | D22 | A7 | S37 | 22 | fader right | faderright |
| 30 | D23 | A8 |  | 23 |  |  |
| 31 | D24 | A9 |  |  |  |  |

![Untitled](https://prod-files-secure.s3.us-west-2.amazonaws.com/183357c7-1e01-4c65-a869-200d788b649c/13e3ccca-f47e-4c9a-8aef-60ee7551fab6/Untitled.png)

---

# Example patch 1 (minimal) in Plugdata

- Use this block in the patch `r mpr121_driver_ch0 @hv_param`
- to refer to touch pads 0 - 11 change `_ch0` , `_ch…` up to `_ch11`
- The output of this block is a 0 or 1 state. So while being touched it sends a 1
- Copy-paste this code into a new plugdata patch

```lua
#X obj 214 502 tgl 25 0 empty empty empty 17 7 0 10 #191919 #e1e1e1 #e1e1e1 0 1;
#X obj 361 637 dac~;
#X obj 137 538 *~;
#X obj 137 502 osc~ 440;
#X obj 435 501 tgl 25 0 empty empty empty 17 7 0 10 #191919 #e1e1e1 #e1e1e1 0 1;
#X obj 361 538 *~;
#X obj 361 501 osc~ 220;
#X obj 666 502 tgl 25 0 empty empty empty 17 7 0 10 #191919 #e1e1e1 #e1e1e1 0 1;
#X obj 589 538 *~;
#X obj 589 502 osc~ 660;
#X obj 134 465 r mpr121_driver_ch0 @hv_param;
#X obj 361 465 r mpr121_driver_ch1 @hv_param;
#X obj 586 465 r mpr121_driver_ch2 @hv_param;
#X connect 0 0 2 1;
#X connect 2 0 1 0;
#X connect 2 0 1 1;
#X connect 3 0 2 0;
#X connect 4 0 5 1;
#X connect 5 0 1 0;
#X connect 5 0 1 1;
#X connect 6 0 5 0;
#X connect 7 0 8 1;
#X connect 8 0 1 0;
#X connect 8 0 1 1;
#X connect 9 0 8 0;
#X connect 10 0 0 0;
#X connect 11 0 4 0;
#X connect 12 0 7 0;
```

or download:

[mpr121tester.pd](https://prod-files-secure.s3.us-west-2.amazonaws.com/183357c7-1e01-4c65-a869-200d788b649c/92ff56e3-1260-43aa-abcd-447732e6ea25/mpr121tester.pd)

![Untitled](https://prod-files-secure.s3.us-west-2.amazonaws.com/183357c7-1e01-4c65-a869-200d788b649c/3f914668-61cf-470b-967d-1c9aa20f918f/Untitled.png)

## Smoothing and performance practices:

And also, as I’ve learned from ‘dreamer’ on the Daisy discord, it’s good practice to avoid using graphical elements like sliders etc, because those are ignored when converted, while building a patch they can be helpful in testing code before uploading, but they add bulk to the code wich can make your files unnecessarily big. Rather make use of `trigger float bang symbol list anything pointer` or the shorthand `t f b s l a p` 

Also it’s good practice to keep as much as you can in the Signal ~ domain.

And smoothing are easing down the input from sensors or knobs can be beneficial for performance.

Here’s a patch with some fading, less harsh then the sudden on/off sounds of the example above. In example 2 below there’s the same tactics for using knobs.

```json
#X obj 1177 1426 r mpr121_driver_ch8 @hv_param;
#X msg 1238 1564 1 300;
#X obj 1177 1599 line~;
#X obj 1079 1589 osc~ 440;
#X obj 1079 1662 *~;
#X obj 1079 1699 dac~;
#X obj 1099 1625 *~ 0.5;
#X msg 1177 1564 0 300;
#X obj 1177 1495 change;
#X obj 1177 1530 sel 0 1;
#X obj 1177 1460 tgl 21 0 empty empty empty 17 7 0 10 #191919 #e1e1e1 #e1e1e1 0 1;
#X connect 0 0 10 0;
#X connect 1 0 2 0;
#X connect 2 0 6 0;
#X connect 3 0 4 0;
#X connect 4 0 5 0;
#X connect 4 0 5 1;
#X connect 6 0 4 1;
#X connect 7 0 2 0;
#X connect 8 0 9 0;
#X connect 9 0 7 0;
#X connect 9 1 1 0;
#X connect 10 0 8 0;
```

![Untitled](https://prod-files-secure.s3.us-west-2.amazonaws.com/183357c7-1e01-4c65-a869-200d788b649c/af610949-cc31-4cab-a0b9-3a437b89f532/Untitled.png)

[smoothfadingcurve.pd](https://prod-files-secure.s3.us-west-2.amazonaws.com/183357c7-1e01-4c65-a869-200d788b649c/572d636a-a91e-437f-bb86-8824bc02b00f/smoothfadingcurve.pd)

---

# Example patch 2 (all options of Simple Touch)

Larger patch with examples of how to use all Simple Touch options: faders, knobs, switches and touchpads.

I’m actively working on this patch and have submitted it to [Synthux’s Synthmas challenge](https://www.synthux.academy/events/synthmas-2023) ( january 2024) so it gets added to the pool of examples. 

- [x]  Todo: copy this documentation into a github repo, and post link here.
- [ ]  Edit formatting on Github
- [ ]  Reupload readme to Github

The delay in this patch is a slightly altered version of the file petal_test.pd found the pd2dsy examples on Github:

[](https://github.com/electro-smith/pd2dsy/blob/master/examples/PdFiles/petal_test.pd)

- The sound you input in adc~ is run into a delay effect with a max buffer of 5 seconds.
- Knobs 1, 2, 3: Delay time, delay feedback, delay volume
- The two switches turn on and off two osc~ at a steady 110 and 220 hz.
- Touchpads 4, 6 also trigger an osc~ but their pitch is controlled by knob 0 and 5, going from 0 to 440 hz.
- Knob 4 sends the touchpad sounds into the delay feedback loop. When you turn the feedback back down you can play the pads without those getting send into the delay.
- Fader left controls the volume of the two switches
- Fader right controls the volume of the two touchpads
- Using the feeback knobs to maximum will create an endless looper.
- In this delay, changing the delay time will also be recorded into the feedback loop, effectively changing pitch.

![Untitled](https://prod-files-secure.s3.us-west-2.amazonaws.com/183357c7-1e01-4c65-a869-200d788b649c/90e4d5ca-f1e3-4a63-acd2-cb51404f3f59/Untitled.png)

### Copy this code for easy pasting in Plugdata (ctrl/cmd + c here, and paste in a patch in Plugdata)

```json
#X obj 126 119 cnv 15 316 761 empty empty adc\ Delay\ with\ extra\ feedback\ in\ from\ touchpads 20 12 0 14 #5d5d5d #e1e1e1 0;
#X obj 459 119 cnv 15 505 605 empty empty Switches\ with\ 220\ pitch\ and\ 110\ pitch\ osc 20 12 0 14 #3c3c3c #e1e1e1 0;
#X obj 984 119 cnv 15 593 605 empty empty Touch\ pads\ 4\ and\ 6\,\ osc\ pitch\ controlled\ \ by\ knobs\ 0\ and\ 5 20 12 0 14 #3c3c3c #e1e1e1 0;
#X obj 1285 194 cnv 15 275 296 empty empty empty 20 12 0 14 #878787 #e1e1e1 0;
#X obj 1000 194 cnv 15 275 296 empty empty empty 20 12 0 14 #878787 #e1e1e1 0;
#X obj 717 194 cnv 15 237 296 empty empty empty 20 12 0 14 #696969 #e1e1e1 0;
#X obj 472 194 cnv 15 237 296 empty empty empty 20 12 0 14 #696969 #e1e1e1 0;
#X obj 493 205 r toggle1left @hv_param;
#X obj 538 233 r toggle1right @hv_param;
#X obj 738 205 r toggle2up @hv_param;
#X obj 776 233 r toggle2down @hv_param;
#X obj 472 939 dac~;
#X obj 493 382 *~ 0.5;
#X obj 479 463 *~;
#X obj 479 429 osc~ 220;
#X obj 493 358 line~;
#X msg 493 263 1;
#X msg 539 263 0;
#X obj 493 300 t \$1 \$2;
#X msg 493 328 \$1 500;
#X msg 555 328 \$1 500;
#X obj 737 382 *~ 0.5;
#X obj 731 463 *~;
#X obj 737 358 line~;
#X obj 737 300 t \$1 \$2;
#X msg 737 328 \$1 500;
#X msg 799 328 \$1 500;
#X msg 778 263 1;
#X msg 738 263 0;
#X obj 731 429 osc~ 110;
#X obj 622 516 +~;
#X obj 1029 382 *~ 0.5;
#X obj 1029 263 change;
#X obj 1029 358 line~;
#X obj 1029 300 sel 0 1;
#X msg 1080 328 1 20;
#X msg 1029 328 0 10;
#X msg 1029 233 \$1 20;
#X obj 1021 465 *~;
#X obj 555 691 *~;
#X msg 622 586 \$1 20;
#X obj 622 614 line~, f 7;
#X obj 622 642 *~ 0.5;
#X obj 622 559 r faderleft @hv_param;
#X obj 1029 205 r mpr121_driver_ch6 @hv_param;
#X msg 1148 586 \$1 20;
#X obj 1148 614 line~, f 7;
#X obj 1148 642 *~ 0.5;
#X obj 1036 691 *~;
#X obj 1148 559 r faderright @hv_param;
#X msg 1136 328 \$1 20;
#X obj 1136 400 * 440;
#X obj 1136 300 r knob5 @hv_param;
#X obj 1136 429 osc~ 440;
#X obj 1136 358 line;
#X obj 1315 382 *~ 0.5;
#X obj 1315 265 change;
#X obj 1315 358 line~;
#X obj 1315 300 sel 0 1;
#X msg 1366 328 1 20;
#X msg 1315 328 0 10;
#X msg 1315 235 \$1 20;
#X obj 1307 467 *~;
#X msg 1422 328 \$1 20;
#X obj 1422 400 * 440;
#X obj 1422 429 osc~ 440;
#X obj 1422 358 line;
#X obj 1422 300 r knob0 @hv_param;
#X obj 1315 207 r mpr121_driver_ch4 @hv_param;
#X obj 1148 516 +~;
#X obj 135 205 adc~;
#X obj 135 697 *~;
#X obj 220 771 + 1;
#X obj 220 745 * -1;
#X obj 135 850 +~;
#X obj 166 794 *~;
#X obj 164 529 line~;
#X obj 301 620 s~ feedback;
#X obj 301 594 *~;
#X obj 193 256 +~;
#X obj 204 218 r~ feedback;
#X obj 164 473 t f 5;
#X obj 164 501 pack f f;
#X obj 220 691 r knob3 @hv_param;
#X text 135 170 from petal_test;
#X obj 164 418 r knob1 @hv_param;
#X obj 193 309 delwrite~ delay1 5000;
#X obj 300 502 r knob2 @hv_param;
#X msg 300 528 \$1 20;
#X obj 300 557 line~, f 7;
#X obj 164 445 * 4999;
#X msg 278 722 \$1 20, f 7;
#X obj 278 752 line, f 7;
#X obj 313 218 r~ touchfeedback;
#X obj 1321 678 s~ touchfeedback;
#X obj 302 263 +~;
#X obj 1321 651 *~;
#X msg 1320 583 \$1 20;
#X obj 1320 612 line~, f 7;
#X obj 1320 557 r knob4 @hv_param;
#X obj 164 557 delread4~ delay1;
#X connect 7 0 16 0;
#X connect 8 0 17 0;
#X connect 9 0 28 0;
#X connect 10 0 27 0;
#X connect 12 0 13 1;
#X connect 13 0 30 0;
#X connect 14 0 13 0;
#X connect 15 0 12 0;
#X connect 16 0 18 0;
#X connect 17 0 18 0;
#X connect 18 0 19 0;
#X connect 18 1 20 0;
#X connect 19 0 15 0;
#X connect 20 0 15 0;
#X connect 21 0 22 1;
#X connect 22 0 30 1;
#X connect 23 0 21 0;
#X connect 24 0 25 0;
#X connect 24 1 26 0;
#X connect 25 0 23 0;
#X connect 26 0 23 0;
#X connect 27 0 24 0;
#X connect 28 0 24 0;
#X connect 29 0 22 0;
#X connect 30 0 39 0;
#X connect 31 0 38 1;
#X connect 32 0 34 0;
#X connect 33 0 31 0;
#X connect 34 0 36 0;
#X connect 34 1 35 0;
#X connect 35 0 33 0;
#X connect 36 0 33 0;
#X connect 37 0 32 0;
#X connect 38 0 69 0;
#X connect 39 0 11 0;
#X connect 39 0 11 1;
#X connect 40 0 41 0;
#X connect 41 0 42 0;
#X connect 42 0 39 1;
#X connect 43 0 40 0;
#X connect 44 0 37 0;
#X connect 45 0 46 0;
#X connect 46 0 47 0;
#X connect 47 0 48 1;
#X connect 48 0 11 0;
#X connect 48 0 11 1;
#X connect 48 0 96 0;
#X connect 49 0 45 0;
#X connect 50 0 54 0;
#X connect 51 0 53 0;
#X connect 52 0 50 0;
#X connect 53 0 38 0;
#X connect 54 0 51 0;
#X connect 55 0 62 1;
#X connect 56 0 58 0;
#X connect 57 0 55 0;
#X connect 58 0 60 0;
#X connect 58 1 59 0;
#X connect 59 0 57 0;
#X connect 60 0 57 0;
#X connect 61 0 56 0;
#X connect 62 0 69 1;
#X connect 63 0 66 0;
#X connect 64 0 65 0;
#X connect 65 0 62 0;
#X connect 66 0 64 0;
#X connect 67 0 63 0;
#X connect 68 0 61 0;
#X connect 69 0 48 0;
#X connect 70 0 71 0;
#X connect 70 0 79 0;
#X connect 71 0 74 0;
#X connect 72 0 71 1;
#X connect 73 0 72 0;
#X connect 74 0 11 0;
#X connect 74 0 11 1;
#X connect 75 0 74 1;
#X connect 76 0 100 0;
#X connect 78 0 77 0;
#X connect 79 0 95 0;
#X connect 80 0 79 1;
#X connect 81 0 82 0;
#X connect 81 1 82 1;
#X connect 82 0 76 0;
#X connect 83 0 91 0;
#X connect 85 0 90 0;
#X connect 87 0 88 0;
#X connect 88 0 89 0;
#X connect 89 0 78 1;
#X connect 90 0 81 0;
#X connect 91 0 92 0;
#X connect 92 0 73 0;
#X connect 92 0 75 1;
#X connect 93 0 95 1;
#X connect 95 0 86 0;
#X connect 96 0 94 0;
#X connect 97 0 98 0;
#X connect 98 0 96 1;
#X connect 99 0 97 0;
#X connect 100 0 75 0;
#X connect 100 0 78 0;
```

Open the toggle above this to reveal the code to copy past or download the file:

[SimpleTouch_Plugdata_demo_drone_delay_01.pd](https://prod-files-secure.s3.us-west-2.amazonaws.com/183357c7-1e01-4c65-a869-200d788b649c/69937c06-a08e-47a0-a32b-5abf4ca29c18/SimpleTouch_Plugdata_demo_drone_delay_01.pd)

---

# Extra info on MPR121 messages from pd2dsy

| MPR121 | --- | Returns whether channel 0 is touched according to the threshold. |
| --- | --- | --- |
| MPR121 | _ch0 ... _ch11 | Returns a 1 if the given channel is touched according to the threshold, otherwise 0. |
| MPR121 | _ch0_raw ... _ch11_raw | Returns the raw sensor data for the given channel as a 10-bit unsigned integer. |

info from: https://github.com/electro-smith/pd2dsy/tree/master

---

# Extra info on using different memory options

The Daisy has a few options for uploading your compiled patch. 

More info on the subjects ram,  rom SDRAM, size, speed, … : 

[GitHub - electro-smith/pd2dsy: Utility for converting Pure Data (Vanilla) patches to Daisy projects.](https://github.com/electro-smith/pd2dsy/tree/master?tab=readme-ov-file#--ram)

And this site 

[libDaisy: Getting Started - External SDRAM](https://electro-smith.github.io/libDaisy/md_doc_2md_2__a6___getting-_started-_external-_s_d_r_a_m.html)

To be able to use this larger memory I’m using the custom linker **[sram_linker_sdram.lds](https://github.com/electro-smith/pd2dsy/blob/master/util/sram_linker_sdram.lds)** found at the pd2dsy Github: 

[](https://github.com/electro-smith/pd2dsy/tree/master/util)

I saved the file on my computer and in the compile window choose this file with option ‘custom linker’ under ‘**Patch size**’ 

- Also pick the corresponding ‘**App type**’ option below that, I didn’t see it at first that I could scroll down !

![Untitled](https://prod-files-secure.s3.us-west-2.amazonaws.com/183357c7-1e01-4c65-a869-200d788b649c/f470110c-369a-46c9-95f1-45dc40fc15ff/Untitled.png)

---

- Sources / resources